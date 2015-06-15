#include "globals.h"
#include "GarrysMod/Lua/Interface.h"

#ifndef UTIL_H
#define UTIL_H

void thfnLoadStream(TChannelThreadArgs ThreadArgs);
void thfnCleanUp();

namespace UTIL
{
	size_t safe_cpy(char *d, char const *s, size_t n);
	bool LoadStream(TChannelThreadArgs& ThreadArgs, bool bNotThreaded);
	unsigned int VersionToDecimal(DWORD iVer);
	unsigned int GetBASSVersionDecimal();
	void ClearLoadingThreads();
	void ClearPendingChannels(lua_State* state);
	const char* DecodeBassError(int iCode);
}

#endif