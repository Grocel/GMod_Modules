#include "globals.h"
#include "GarrysMod/Lua/Interface.h"

#ifndef UTIL_H
#define UTIL_H

#define ISNULLPTR(x) ((x == NULL) || (x == nullptr))

void thfnLoadStream(TChannelThreadArgs ThreadArgs);
void thfnCleanUp();

using namespace GarrysMod::Lua;

namespace UTIL
{
	namespace STRING
	{
		typedef bool (*ForEachSegmentFunc)(string& sFind, unsigned int iCount, void *pUserData);

		void Trim(string& sString);
		bool GetKeyValueFromSeperator(string &sInput, string &sSeperator, string &sKey, string &sValue, bool reverse = false);
		bool RemoveChars(string &sInput, const char* sCharsToRemove);
		unsigned int ForEachSegment(string sInput, string &sSeperator, ForEachSegmentFunc func, void *pUserData);
		bool ToNumber(const char* pString, lua_n &fNumber);
		bool ToNumber(string &sInput, lua_n &fNumber);

		char* safe_getnewcstr(string s);
		size_t safe_cpy(char* d, string s);
		size_t safe_cpy(char* d, string s, size_t n);
		size_t safe_cpy(char* d, char const *s, size_t n);
	}

	bool LoadStream(TChannelThreadArgs& ThreadArgs, bool bNotThreaded);
	unsigned int VersionToDecimal(DWORD iVer);
	unsigned int GetBASSVersionDecimal();
	void ClearLoadingThreads();
	void ClearPendingChannels(ILuaBase* pLUA);
	string DecodeBassError(int iCode);
}

#endif