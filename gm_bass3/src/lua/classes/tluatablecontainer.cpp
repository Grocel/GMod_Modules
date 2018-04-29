#include "tluatablecontainer.hpp"

namespace LUAINTERFACE
{
	map<string, TLuaTableContainer*> TLuaTableContainer::g_mapTablesInstances = map<string, TLuaTableContainer*>();

	TLuaTableContainer::TLuaTableContainer()
	{
		mapFunctions.clear();
		mapEnums.clear();
	}

	TLuaTableContainer::~TLuaTableContainer()
	{
		mapFunctions.clear();
		mapEnums.clear();

		auto it = g_mapTablesInstances.find(sName);
		if (it != g_mapTablesInstances.end())
		{
			g_mapTablesInstances.erase(it);
		}
	}

	void TLuaTableContainer::AddFunction(string name, GarrysMod::Lua::CFunc func)
	{
		lock_guard<mutex> Lock(MutexLock);

		if (name.empty()) throw invalid_argument("invalid name");
		if (!func) throw invalid_argument("invalid function");

		{
			auto it = mapFunctions.find(name);
			if (it != mapFunctions.end())
			{
				mapFunctions.erase(it);
			}
		}
	
		mapFunctions[name] = func;
	}

	void TLuaTableContainer::AddEnum(string name, bass_flag value)
	{
		lock_guard<mutex> Lock(MutexLock);

		if (name.empty()) throw invalid_argument("invalid name");

		{
			auto it = mapEnums.find(name);
			if (it != mapEnums.end())
			{
				mapEnums.erase(it);
			}
		}

		mapEnums[name] = value;
	}

	void TLuaTableContainer::AddEnum(string name, double value)
	{
		this->AddEnum(name, (bass_flag)(floor(value)));
	}

	void TLuaTableContainer::AddEnum(string name, signed int value)
	{
		this->AddEnum(name, (bass_flag)(value));
	}

	void TLuaTableContainer::AddEnum(string name, unsigned int value)
	{
		this->AddEnum(name, (bass_flag)(value));
	}

	void TLuaTableContainer::AddConstant(string name, string value)
	{
		lock_guard<mutex> Lock(MutexLock);

		if (name.empty()) throw invalid_argument("invalid name");

		{
			auto it = mapConstants_str.find(name);
			if (it != mapConstants_str.end())
			{
				mapConstants_str.erase(it);
			}
		}

		mapConstants_str[name] = value;
	}

	void TLuaTableContainer::AddConstant(string name, double value)
	{
		lock_guard<mutex> Lock(MutexLock);

		if (name.empty()) throw invalid_argument("invalid name");

		{
			auto it = mapConstants_n.find(name);
			if (it != mapConstants_n.end())
			{
				mapConstants_n.erase(it);
			}
		}

		mapConstants_n[name] = value;
	}

	void TLuaTableContainer::AddConstant(string name, signed int value)
	{
		this->AddConstant(name, (double)(value));
	}

	void TLuaTableContainer::AddConstant(string name, unsigned int value)
	{
		this->AddConstant(name, (double)(value));
	}

	void TLuaTableContainer::Build(ILuaBase* pLUA, string name)
	{
		lock_guard<mutex> Lock(MutexLock);

		if (name.empty()) throw invalid_argument("invalid table name");

		// Do not add the metatable if already exist
		if (g_mapTablesInstances.find(name) != g_mapTablesInstances.end()) return;

		sName = name;
		int iEnumRef = 0;

		if (!mapEnums.empty()) {
			// _G.<name>.ENUM
			pLUA->CreateTable();
			{
				for (const auto &pair : mapEnums) {
					pLUA->PushNumber(pair.second);
					pLUA->SetField(-2, pair.first.c_str());
				}
			}
			iEnumRef = pLUA->ReferenceCreate();
		}

		// _G.<name>
		pLUA->CreateTable();
		{
			if (iEnumRef) {
				// Enums
				pLUA->PushString("ENUM");
				pLUA->ReferencePush(iEnumRef);
				pLUA->SetTable(-3);
			}

			// Constants
			for (const auto &pair : mapConstants_n) {
				pLUA->PushString(pair.first.c_str());
				pLUA->PushNumber(pair.second);
				pLUA->SetTable(-3);
			}

			for (const auto &pair : mapConstants_str) {
				pLUA->PushString(pair.first.c_str());
				pLUA->PushString(pair.second.c_str());
				pLUA->SetTable(-3);
			}
			
			// Functions
			for (const auto &pair : mapFunctions) {
				auto name = pair.first;
				auto func = pair.second;
				if (!func) continue;

				pLUA->PushString(name.c_str());
				pLUA->PushCFunction(func);
				pLUA->SetTable(-3);
			}
		}
		int iRef = pLUA->ReferenceCreate();

		// Add <name> to _G
		pLUA->PushSpecial(SPECIAL_GLOB);

		pLUA->PushString(name.c_str());
		pLUA->ReferencePush(iRef);
		pLUA->SetTable(-3);

		pLUA->ReferenceFree(iEnumRef);
		pLUA->ReferenceFree(iRef);

		g_mapTablesInstances[name] = this;
	}

	bool TLuaTableContainer::IsValid()
	{
		lock_guard<mutex> Lock(MutexLock);

		if (sName.empty()) return false;
		return true;
	}

	string TLuaTableContainer::GetName() const
	{
		if (sName.empty()) throw logic_error("invalid type name");
		return sName;
	}

	void TLuaTableContainer::Init(ILuaBase* pLUA)
	{
		throw runtime_error("not implemented");
	}

	void TLuaTableContainer::Think(ILuaBase* pLUA)
	{
		throw runtime_error("not implemented");
	}

	void TLuaTableContainer::Dispose(ILuaBase* pLUA)
	{
		throw runtime_error("not implemented");
	}
}