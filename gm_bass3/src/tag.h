#include "globals.h"
#include "GarrysMod/Lua/Interface.h"

#ifndef TAG_H
#define TAG_H

namespace TAG
{
	bool META(const char* pTagData, lua_State* state);
	bool ID3(const char* pTagData, lua_State* state);
	bool HTTP(const char* pTagData, lua_State* state);
	bool ICY(const char* pTagData, lua_State* state);
	bool MF(const char* pTagData, lua_State* state);
	bool MP4(const char* pTagData, lua_State* state);
	bool APE(const char* pTagData, lua_State* state);
	bool OGG(const char* pTagData, lua_State* state);
}

#endif