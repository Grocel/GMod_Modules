#include "tluaobjectcontainer.hpp"

namespace LUAINTERFACE
{
	map<string, TLuaObjectContainer*> TLuaObjectContainer::g_mapObjectInstances = map<string, TLuaObjectContainer*>();

	TLuaObjectContainer::TLuaObjectContainer()
	{
		mapMethods.clear();
		mapMetaMethods.clear();
	}

	TLuaObjectContainer::~TLuaObjectContainer()
	{
		mapMethods.clear();
		mapMetaMethods.clear();

		auto it = g_mapObjectInstances.find(sName);
		if (it != g_mapObjectInstances.end())
		{
			g_mapObjectInstances.erase(it);
		}
	}

	void TLuaObjectContainer::AddMethod(string name, GarrysMod::Lua::CFunc func)
	{
		lock_guard<mutex> Lock(MutexLock);

		if (name.empty()) throw invalid_argument("invalid name");
		if (!func) throw invalid_argument("invalid function");

		{
			auto it = mapMetaMethods.find(name);
			if (it != mapMetaMethods.end())
			{
				mapMetaMethods.erase(it);
			}
		}

		{
			auto it = mapMethods.find(name);
			if (it != mapMethods.end())
			{
				mapMethods.erase(it);
			}
		}

		size_t found = name.find("__", 0);
		if (found != string::npos && found == 0)
		{
			// add as meta methods if the name starts with "__"
			mapMetaMethods[name] = func;
			return;
		}

		mapMethods[name] = func;
	}

	int TLuaObjectContainer::Build(ILuaBase* pLUA, string name)
	{
		lock_guard<mutex> Lock(MutexLock);

		sName = name;
		if (sName.empty()) throw invalid_argument("invalid type name");

		// Do not add the metatable if already exist
		if (g_mapObjectInstances.find(sName) != g_mapObjectInstances.end()) return iTypeID;

		// _R.<object>
		pLUA->CreateTable();
		{
			for (const auto &pair : mapMethods) {
				auto name = pair.first;
				auto func = pair.second;
				if (!func) continue;

				pLUA->PushString(name.c_str());
				pLUA->PushCFunction(func);
				pLUA->SetTable(-3);
			}
		}
		int iTabRef = pLUA->ReferenceCreate();

		// Add <object> to _R
		iTypeID = pLUA->CreateMetaTable(sName.c_str());
		{
			pLUA->PushString("__index");
			pLUA->ReferencePush(iTabRef);
			pLUA->SetTable(-3);

			pLUA->PushString("__newindex");
			pLUA->ReferencePush(iTabRef);
			pLUA->SetTable(-3);

			for (const auto &pair : mapMetaMethods) {
				auto name = pair.first;
				auto func = pair.second;
				if (!func) continue;

				pLUA->PushString(name.c_str());
				pLUA->PushCFunction(func);
				pLUA->SetTable(-3);
			}
		}
		pLUA->Pop();

		pLUA->ReferenceFree(iTabRef);
		g_mapObjectInstances[sName] = this;

		return iTypeID;
	}

	void TLuaObjectContainer::Init(ILuaBase* pLUA)
	{
		throw runtime_error("not implemented");
	}

	void TLuaObjectContainer::Think(ILuaBase* pLUA)
	{
		throw runtime_error("not implemented");
	}

	void TLuaObjectContainer::Dispose(ILuaBase* pLUA)
	{
		throw runtime_error("not implemented");
	}
}