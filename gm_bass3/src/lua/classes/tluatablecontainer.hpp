#include "../lua.hpp"

#ifndef T_LUATABLECONTAINER_H
#define T_LUATABLECONTAINER_H

namespace LUAINTERFACE
{
	class TLuaTableContainer
	{
	private:
		int iRef = 0;
		string sName;

		map<string, GarrysMod::Lua::CFunc> mapFunctions;
		map<string, bass_flag> mapEnums;

		map<string, string> mapConstants_str;
		map<string, double> mapConstants_n;

	protected:
		mutex MutexLock;
		void Build(ILuaBase* pLUA, string name);

	public:
		static map<string, TLuaTableContainer*> g_mapTablesInstances;
		static map<TLuaTableContainer*, string> g_mapTablesInstances_r;

		TLuaTableContainer();
		~TLuaTableContainer();

		void AddFunction(string name, GarrysMod::Lua::CFunc func);

		void AddEnum(string name, bass_flag iEnum);
		void AddEnum(string name, double iEnum);
		void AddEnum(string name, unsigned int iEnum);
		void AddEnum(string name, signed int iEnum);

		void AddConstant(string name, string value);
		void AddConstant(string name, double value);
		void AddConstant(string name, unsigned int value);
		void AddConstant(string name, signed int value);

		bool IsValid();

		string GetName() const;

		virtual void Init(ILuaBase* pLUA);
		virtual void Think(ILuaBase* pLUA);
		virtual void Dispose(ILuaBase* pLUA);
	};
}
#endif