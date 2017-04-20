#include "globals.h"
#include "GarrysMod/Lua/Interface.h"

#ifndef TAG_H
#define TAG_H

using namespace GarrysMod::Lua;

namespace TAG
{
	bool META(const char* pTagData, ILuaBase* pLUA);
	bool ID3(const char* pTagData, ILuaBase* pLUA);
	bool HTTP(const char* pTagData, ILuaBase* pLUA);
	bool ICY(const char* pTagData, ILuaBase* pLUA);
	bool MF(const char* pTagData, ILuaBase* pLUA);
	bool MP4(const char* pTagData, ILuaBase* pLUA);
	bool APE(const char* pTagData, ILuaBase* pLUA);
	bool OGG(const char* pTagData, ILuaBase* pLUA);
}

#endif