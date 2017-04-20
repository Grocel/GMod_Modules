#include "globals.h"
#include "GarrysMod/Lua/Interface.h"
#include "classes/tchannel.h"
#include "lua_functions.h"

// Compiling with Source SDK 2013 for Linux/OSX? Don't forget this:
// #include "steam/steamtypes.h"
// #include "mathlib/vector.h"

#ifndef LUA_H
#define LUA_H

using namespace GarrysMod::Lua;

#define ISNIL(num) (LUA->IsType( num, Type::NIL ) || LUA->IsType( num, Type::NONE ))

namespace LUAINTERFACE
{
	void PushChannel(ILuaBase* pLUA, TChannel* pChannel);
	void DeleteChannel(TChannel *pChannel);

	void PushVector(ILuaBase* pLUA, BASS_3DVECTOR* pVector);

	void SetupRealm(ILuaBase* pLUA);
	void SetupBASSTable(ILuaBase* pLUA);
	void SetupChannelObject(ILuaBase* pLUA);
}

#endif