#include <cstring>
#include <string>
#include <algorithm>

#include "tag.h"
#include "lua.h"
#include "util.h"
#include "classes/tchannel.h"

bool ReadMetaTag(string& sLine, unsigned int iCount, void *pUserData);
bool GenericParseList(const char* pTagData, string& sSeperator, lua_State* state);
bool GenericParseList(const char* pTagData, const char* pSeperator, lua_State* state);
bool GenericParseList(const char* pTagData, char cSeperator, lua_State* state);

bool ReadMetaTag(string& sLine, unsigned int iCount, void *pUserData)
{
	if(pUserData == NULL) return false;
	if(pUserData == nullptr) return false;

	lua_State* state = (lua_State*)(pUserData);
	if(state == NULL) return false;
	if(state == nullptr) return false;

	string sKey = "";
	string sValue = "";
	string sSeperator = "=";

	if(!UTIL::STRING::GetKeyValueFromSeperator(sLine, sSeperator, sKey, sValue)) return false;

	if ( sKey != "" ) {
		if(sKey.front() == '\'') sKey.erase( 0, 1 );
		if(sKey.back() == '\'') sKey.erase( sKey.size() - 1 );
	}

	if ( sValue != "" ) {
		if(sValue.front() == '\'') sValue.erase( 0, 1 );
		if(sValue.back() == '\'') sValue.erase( sValue.size() - 1 );
	}

	UTIL::STRING::Trim(sKey);
	UTIL::STRING::Trim(sValue);
	if(sKey == "") return false;
	if(sValue == "") return false;

	lua_n fValue = 0;
	bool bIsNumber = UTIL::STRING::ToNumber(sValue, fValue);

	LUA->PushString( sKey.c_str() );
	if(bIsNumber) {
		LUA->PushNumber( fValue );
	} else {
		LUA->PushString( sValue.c_str() );
	}
	LUA->SetTable( -3 );

	return true;
}

bool GenericParseList(const char* pTagData, char cSeperator, lua_State* state)
{
	string sSeperator = "";
	if(cSeperator) sSeperator += cSeperator;

	return GenericParseList(pTagData, sSeperator, state);
}

bool GenericParseList(const char* pTagData, const char* pSeperator, lua_State* state)
{
	string sSeperator = "";
	
	if((pSeperator != NULL) && (pSeperator != nullptr))
	{
		sSeperator = string(pSeperator);
	}

	return GenericParseList(pTagData, sSeperator, state);
}

bool GenericParseList(const char* pTagData, string& sSeperator, lua_State* state)
{
	if(pTagData == NULL) return false;
	if(pTagData == nullptr) return false;
	if(state == NULL) return false;
	if(state == nullptr) return false;

	unsigned int i = 0;
	bool bEmpty = true;

	string sKey = "";
	string sValue = "";
	string sData = "";

	lua_n fValue = 0;
	bool bIsNumber = false;

	// Getting a list of null-terminated strings
	while (*pTagData)
	{
		sData = string(pTagData);

		if(UTIL::STRING::GetKeyValueFromSeperator(sData, sSeperator, sKey, sValue))
		{
			if(sValue == "")
			{
				pTagData += sData.length() + 1; // move on to next string
				continue;
			}

			bIsNumber = UTIL::STRING::ToNumber(sValue, fValue);

			LUA->PushString( sKey.c_str() );
			if(bIsNumber) {
				LUA->PushNumber( fValue );
			} else {
				LUA->PushString( sValue.c_str() );
			}
			LUA->SetTable( -3 );
			bEmpty = false;
		}
		else
		{
			if(sData == "")
			{
				pTagData += 1; // move on to next string
				continue;
			}

			i++;
			bIsNumber = UTIL::STRING::ToNumber(sData, fValue);

			LUA->PushNumber( i );
			if(bIsNumber) {
				LUA->PushNumber( fValue );
			} else {
				LUA->PushString( sData.c_str() );
			}
			LUA->SetTable( -3 );
			bEmpty = false;
		}

		pTagData += sData.length() + 1; // move on to next string
	}

	return !bEmpty;
}

namespace TAG
{
	bool META(const char* pTagData, lua_State* state)
	{
		if(pTagData == NULL) return false;
		if(pTagData == nullptr) return false;
		if(state == NULL) return false;
		if(state == nullptr) return false;

		string sSeperator = ";";
		string sData = string(pTagData);

		return UTIL::STRING::ForEachSegment(sData, sSeperator, &ReadMetaTag, state) > 0;
	}

	bool ID3(const char* pTagData, lua_State* state)
	{
		if(pTagData == NULL) return false;
		if(pTagData == nullptr) return false;
		if(state == NULL) return false;
		if(state == nullptr) return false;

		const TAG_ID3* pTagID3 = (const TAG_ID3*)pTagData;
		if(pTagID3 == NULL) return false;
		if(pTagID3 == nullptr) return false;

		lua_n fValue = 0;
		bool bIsNumber = false;

		string sID = string(pTagID3->id, 3);
		string sYear = string(pTagID3->year, 4);
		string sTitle = string(pTagID3->title, 30);
		string sArtist = string(pTagID3->artist, 30);
		string sAlbum = string(pTagID3->album, 30);
		string sComment = string(pTagID3->comment, 30);

		UTIL::STRING::Trim(sID);
		UTIL::STRING::Trim(sYear);
		UTIL::STRING::Trim(sTitle);
		UTIL::STRING::Trim(sArtist);
		UTIL::STRING::Trim(sAlbum);
		UTIL::STRING::Trim(sComment);

		if(sID != "")
		{
			bIsNumber = UTIL::STRING::ToNumber(sID, fValue);

			LUA->PushString( "id" );
			if(bIsNumber) {
				LUA->PushNumber( fValue );
			} else {
				LUA->PushString( sID.c_str() );
			}
			LUA->SetTable( -3 );
		}

		if(sYear != "")
		{
			bIsNumber = UTIL::STRING::ToNumber(sYear, fValue);

			LUA->PushString( "year" );
			if(bIsNumber) {
				LUA->PushNumber( fValue );
			} else {
				LUA->PushString( sYear.c_str()  );
			}
			LUA->SetTable( -3 );
		}

		LUA->PushString( "genre" ); 
		LUA->PushNumber( pTagID3->genre );
		LUA->SetTable( -3 );

		if(sTitle != "")
		{
			LUA->PushString( "title" ); 
			LUA->PushString( sTitle.c_str() );
			LUA->SetTable( -3 );
		}

		if(sArtist != "")
		{
			LUA->PushString( "artist" ); 
			LUA->PushString( sArtist.c_str() );
			LUA->SetTable( -3 );
		}

		if(sAlbum != "")
		{
			LUA->PushString( "album" ); 
			LUA->PushString( sAlbum.c_str() );
			LUA->SetTable( -3 );
		}

		if(sComment != "")
		{
			LUA->PushString( "comment" ); 
			LUA->PushString( sComment.c_str() );
			LUA->SetTable( -3 );
		}

		return true;
	}

	bool HTTP(const char* pTagData, lua_State* state)
	{
		return GenericParseList(pTagData, ':', state);
	}

	bool ICY(const char* pTagData, lua_State* state)
	{
		return GenericParseList(pTagData, "", state);
	}

	bool MF(const char* pTagData, lua_State* state)
	{
		return GenericParseList(pTagData, '=', state);
	}

	bool MP4(const char* pTagData, lua_State* state)
	{
		return GenericParseList(pTagData, '=', state);
	}

	bool APE(const char* pTagData, lua_State* state)
	{
		return GenericParseList(pTagData, '=', state);
	}

	bool OGG(const char* pTagData, lua_State* state)
	{
		return GenericParseList(pTagData, '=', state);
	}
}
