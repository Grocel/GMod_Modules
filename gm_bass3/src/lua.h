#include "globals.h"
#include "GarrysMod/Lua/Interface.h"
#include "classes/tchannel.h"
#include "lua_functions.h"

// Compiling with Source SDK 2013 for Linux/OSX? Don't forget this:
#include "steam/steamtypes.h"
#include "mathlib/vector.h"

#ifndef LUA_H
#define LUA_H

using namespace GarrysMod::Lua;

#define GETCHANNEL(num) (TChannel*)((UserData*)LUA->GetUserdata(num))->data
#define GETVECTOR(num) (Vector*)((UserData*)LUA->GetUserdata(num))->data
#define ISNIL(num) (LUA->IsType( num, Type::NIL ) || LUA->IsType( num, Type::INVALID ))

namespace LUAINTERFACE
{
	void PushChannel(lua_State* state, TChannel* pChannel);
	void DeleteChannel(TChannel *pChannel);

	void PushVector(lua_State* state, BASS_3DVECTOR* pVector);

	void SetupGlobalTable(lua_State* state);
	void SetupBASSTable(lua_State* state);
	void SetupChannelObject(lua_State* state);

	void UnrefGlobalReferences(lua_State* state);
}

#endif