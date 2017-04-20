#include <cstring>
#include <string>
#include <algorithm>

#include "tag.h"
#include "lua.h"
#include "util.h"
#include "classes/tchannel.h"

bool ReadMetaTag(string& sLine, unsigned int iCount, void *pUserData);
bool GenericParseList(const char* pTagData, string& sSeperator, ILuaBase* pLUA);
bool GenericParseList(const char* pTagData, const char* pSeperator, ILuaBase* pLUA);
bool GenericParseList(const char* pTagData, char cSeperator, ILuaBase* pLUA);

bool ReadMetaTag(string& sLine, unsigned int iCount, void *pUserData)
{
	if(ISNULLPTR(pUserData)) return false;

	ILuaBase* pLUA = (ILuaBase*)(pUserData);
	if(ISNULLPTR(pLUA)) return false;

	string sKey = "";
	string sValue = "";
	string sSeperator = "=";

	if(!UTIL::STRING::GetKeyValueFromSeperator(sLine, sSeperator, sKey, sValue)) return false;
	transform(sKey.begin(), sKey.end(), sKey.begin(), ::tolower);

	if(sKey != "")
	{
		if(sKey.front() == '\'') sKey.erase( 0, 1 );
		if(sKey.back() == '\'') sKey.erase( sKey.size() - 1 );
	}

	if(sValue != "")
	{
		if(sValue.front() == '\'') sValue.erase( 0, 1 );
		if(sValue.back() == '\'') sValue.erase( sValue.size() - 1 );
	}

	UTIL::STRING::Trim(sKey);
	UTIL::STRING::Trim(sValue);
	if(sKey == "") return false;
	if(sValue == "") return false;

	lua_n fValue = 0;
	bool bIsNumber = UTIL::STRING::ToNumber(sValue, fValue);

	pLUA->PushString( sKey.c_str() );
	if(bIsNumber) {
		pLUA->PushNumber( fValue );
	} else {
		pLUA->PushString( sValue.c_str() );
	}
	pLUA->SetTable( -3 );

	return true;
}

bool GenericParseList(const char* pTagData, char cSeperator, ILuaBase* pLUA)
{
	string sSeperator = "";
	if(cSeperator) sSeperator += cSeperator;

	return GenericParseList(pTagData, sSeperator, pLUA);
}

bool GenericParseList(const char* pTagData, const char* pSeperator, ILuaBase* pLUA)
{
	string sSeperator = "";
	
	if(!ISNULLPTR(pSeperator))
	{
		sSeperator = string(pSeperator);
	}

	return GenericParseList(pTagData, sSeperator, pLUA);
}

bool GenericParseList(const char* pTagData, string& sSeperator, ILuaBase* pLUA)
{
	if(ISNULLPTR(pTagData)) return false;
	if(ISNULLPTR(pLUA)) return false;

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
			transform(sKey.begin(), sKey.end(), sKey.begin(), ::tolower);
			UTIL::STRING::Trim(sKey);
			UTIL::STRING::Trim(sValue);

			if(sKey == "")
			{
				pTagData += sData.length() + 1; // move on to next string
				continue;
			}

			if(sValue == "")
			{
				pTagData += sData.length() + 1; // move on to next string
				continue;
			}

			pLUA->PushString( sKey.c_str() );
			if(bIsNumber) {
				pLUA->PushNumber( fValue );
			} else {
				pLUA->PushString( sValue.c_str() );
			}
			pLUA->SetTable( -3 );
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

			pLUA->PushNumber( i );
			if(bIsNumber) {
				pLUA->PushNumber( fValue );
			} else {
				pLUA->PushString( sData.c_str() );
			}
			pLUA->SetTable( -3 );
			bEmpty = false;
		}

		pTagData += sData.length() + 1; // move on to next string
	}

	return !bEmpty;
}

namespace TAG
{
	bool META(const char* pTagData, ILuaBase* pLUA)
	{
		if(ISNULLPTR(pTagData)) return false;
		if(ISNULLPTR(pLUA)) return false;

		string sSeperator = ";";
		string sData = string(pTagData);

		return UTIL::STRING::ForEachSegment(sData, sSeperator, &ReadMetaTag, pLUA) > 0;
	}

	bool ID3(const char* pTagData, ILuaBase* pLUA)
	{
		if(ISNULLPTR(pTagData)) return false;
		if(ISNULLPTR(pLUA)) return false;

		const TAG_ID3* pTagID3 = (const TAG_ID3*)pTagData;
		if(ISNULLPTR(pTagID3)) return false;

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

			pLUA->PushString( "id" );
			if(bIsNumber) {
				pLUA->PushNumber( fValue );
			} else {
				pLUA->PushString( sID.c_str() );
			}
			pLUA->SetTable( -3 );
		}

		if(sYear != "")
		{
			bIsNumber = UTIL::STRING::ToNumber(sYear, fValue);

			pLUA->PushString( "year" );
			if(bIsNumber) {
				pLUA->PushNumber( fValue );
			} else {
				pLUA->PushString( sYear.c_str()  );
			}
			pLUA->SetTable( -3 );
		}

		pLUA->PushString( "genre" ); 
		pLUA->PushNumber( pTagID3->genre );
		pLUA->SetTable( -3 );

		if(sTitle != "")
		{
			pLUA->PushString( "title" ); 
			pLUA->PushString( sTitle.c_str() );
			pLUA->SetTable( -3 );
		}

		if(sArtist != "")
		{
			pLUA->PushString( "artist" ); 
			pLUA->PushString( sArtist.c_str() );
			pLUA->SetTable( -3 );
		}

		if(sAlbum != "")
		{
			pLUA->PushString( "album" ); 
			pLUA->PushString( sAlbum.c_str() );
			pLUA->SetTable( -3 );
		}

		if(sComment != "")
		{
			pLUA->PushString( "comment" ); 
			pLUA->PushString( sComment.c_str() );
			pLUA->SetTable( -3 );
		}

		return true;
	}

	bool HTTP(const char* pTagData, ILuaBase* pLUA)
	{
		return GenericParseList(pTagData, ':', pLUA);
	}

	bool ICY(const char* pTagData, ILuaBase* pLUA)
	{
		return GenericParseList(pTagData, "", pLUA);
	}

	bool MF(const char* pTagData, ILuaBase* pLUA)
	{
		return GenericParseList(pTagData, '=', pLUA);
	}

	bool MP4(const char* pTagData, ILuaBase* pLUA)
	{
		return GenericParseList(pTagData, '=', pLUA);
	}

	bool APE(const char* pTagData, ILuaBase* pLUA)
	{
		return GenericParseList(pTagData, '=', pLUA);
	}

	bool OGG(const char* pTagData, ILuaBase* pLUA)
	{
		return GenericParseList(pTagData, '=', pLUA);
	}
}
