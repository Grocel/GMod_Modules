#include "globals.h"
#include "GarrysMod/Lua/Interface.h"

#ifndef UTIL_H
#define UTIL_H

void thfnLoadStream(TChannelThreadArgs ThreadArgs);
void thfnCleanUp();

namespace UTIL
{
	namespace STRING
	{
		typedef bool (*ForEachSegmentFunc)(string& sFind, unsigned int iCount, void *pUserData);

		void Trim(string& sString);
		bool GetKeyValueFromSeperator(string &sInput, string &sSeperator, string &sKey, string &sValue);
		unsigned int ForEachSegment(string sInput, string &sSeperator, ForEachSegmentFunc func, void *pUserData);
		bool ToNumber(const char *pString, lua_n &fNumber);
		bool ToNumber(string &sInput, lua_n &fNumber);
	}
	size_t safe_cpy(char *d, char const *s, size_t n);
	bool LoadStream(TChannelThreadArgs& ThreadArgs, bool bNotThreaded);
	unsigned int VersionToDecimal(DWORD iVer);
	unsigned int GetBASSVersionDecimal();
	void ClearLoadingThreads();
	void ClearPendingChannels(lua_State* state);
	const char* DecodeBassError(int iCode);
}

#endif