#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <thread>
#include <mutex>


#include "util.h"
#include "classes/tchannel.h"
#include "lua.h"

void thfnLoadStream(TChannelThreadArgs ThreadArgs)
{
	if(!g_IntialTickHappend) return;
	if(g_thCleanUp == NULL) return;

	TChannel* pChannel = ThreadArgs.pChannel;
	bool bIsRecycledChannel;

	if(pChannel == NULL)
	{
		bIsRecycledChannel = false;
		pChannel = new TChannel();
	}
	else
	{
		bIsRecycledChannel = true;
		pChannel->AddReference();
	}

	int iError = pChannel->Load(ThreadArgs.sURL, ThreadArgs.bIsOnline, ThreadArgs.eFlags);

	if(!bIsRecycledChannel && iError != BASS_OK)
	{
		LUAINTERFACE::DeleteChannel(pChannel);
		pChannel = NULL;
	}

	if(pChannel != NULL) pChannel->Update(5000);

	TChannelCallbackData* pCallbackData = new TChannelCallbackData();
	pCallbackData->bIsRecycledChannel = bIsRecycledChannel;
	pCallbackData->iCallbackRef = ThreadArgs.iCallbackRef;
	pCallbackData->iError = iError;
	pCallbackData->pChannel = pChannel;
	pCallbackData->bNoPlay = ThreadArgs.bNoPlay;

	g_pListPendingCallbacks->add(pCallbackData);
}

void thfnCleanUp()
{
	unsigned char i = 0;

	while(true)
	{
		if(i > 50)
		{
			UTIL::ClearLoadingThreads();
			i = 0;
		}

		if(g_CLOSING) break;
		i++;

		SLEEP(200);
	}

	UTIL::ClearLoadingThreads();
}

namespace UTIL
{
	size_t safe_cpy(char *d, char const *s, size_t n)
	{
		return snprintf(d, n, "%s", s);
	}

	bool LoadStream(TChannelThreadArgs& ThreadArgs, bool bNotThreaded)
	{
		if(!g_IntialTickHappend) return false;

		// Prevent overflow
		if(g_pListPendingCallbacks->getSize() > 512) return false;

		if(bNotThreaded)
		{
			thfnLoadStream(ThreadArgs);
			return true;
		}

		if(g_thCleanUp == NULL) return false;

		try
		{
			thread* thLoadStream = new thread(thfnLoadStream, ThreadArgs);
			g_pListRunningThreads->add(thLoadStream);

			return true;
		}
		catch(...)
		{
			// Overflow? Clear and try again.
			UTIL::ClearLoadingThreads();

			try
			{
				thread* thLoadStream = new thread(thfnLoadStream, ThreadArgs);
				g_pListRunningThreads->add(thLoadStream);

				return true;
			}
			catch(...)
			{
				Error("GM_BASS3 couldn't create a 'LoadStream' thread for %s!\n", ThreadArgs.sURL);
				return false;
			}
		}
	}

	unsigned int VersionToDecimal(DWORD iVer)
	{
		unsigned char Q1 = (iVer >> 24) & 0xFF;
		unsigned char Q2 = (iVer >> 16) & 0xFF;
		unsigned char Q3 = (iVer >> 8) & 0xFF;
		unsigned char Q4 = iVer & 0xFF;

		if(Q1 > 99) Q1 = 99;
		if(Q2 > 99) Q2 = 99;
		if(Q3 > 99) Q3 = 99;
		if(Q4 > 99) Q4 = 99;

		return 1000000 * Q1 + 10000 * Q2 + 100 * Q3 + Q4;
	}

	unsigned int GetBASSVersionDecimal()
	{
		return VersionToDecimal(BASS_GetVersion());
	}

	void ClearLoadingThreads()
	{
		while(g_pListRunningThreads->getSize() > 0)
		{
			thread *thLoadStream = g_pListRunningThreads->remove();
			if(thLoadStream == NULL) continue;

			thLoadStream->join();
			delete thLoadStream;
		}
	}

	void ClearPendingChannels(lua_State* state)
	{
		while(g_pListPendingCallbacks->getSize() > 0)
		{
			TChannelCallbackData* pCallbackData = g_pListPendingCallbacks->remove();
			if(pCallbackData == NULL) continue;

			TChannel* pChannel = pCallbackData->pChannel;
			int iError = pCallbackData->iError;
			int iCallbackRef = pCallbackData->iCallbackRef;
			delete pCallbackData;

			LUAINTERFACE::DeleteChannel(pChannel);
			LUA->ReferenceFree(iCallbackRef);
		}
	}

	const char* DecodeBassError(int iCode)
	{
		const char* sError;
		switch(iCode)
		{
			ENUM_TO_VALUE(BASS_OK,					"OK",												sError);
			ENUM_TO_VALUE(BASS_ERROR_UNKNOWN,		"Unknown error!",									sError);

			ENUM_TO_VALUE(BASS_ERROR_MEM,			"Memory error!",									sError);
			ENUM_TO_VALUE(BASS_ERROR_FILEOPEN,		"Can't open the file!",								sError);
			ENUM_TO_VALUE(BASS_ERROR_DRIVER,		"Can't find a driver!",								sError);
			ENUM_TO_VALUE(BASS_ERROR_BUFLOST,		"The sample buffer was lost!",						sError);
			ENUM_TO_VALUE(BASS_ERROR_HANDLE,		"Invalid channel!",									sError);
			ENUM_TO_VALUE(BASS_ERROR_FORMAT,		"Unsupported sample format!",						sError);
			ENUM_TO_VALUE(BASS_ERROR_POSITION,		"Invalid position!",								sError);
			ENUM_TO_VALUE(BASS_ERROR_INIT,			"BASS_Init has not been successfully called!",		sError);
			ENUM_TO_VALUE(BASS_ERROR_START,			"BASS_Start has not been successfully called!",		sError);
			ENUM_TO_VALUE(BASS_ERROR_ALREADY,		"Already initialized, paused or used!",				sError);
			ENUM_TO_VALUE(BASS_ERROR_NOCHAN,		"Can't get a free channel!",						sError);
			ENUM_TO_VALUE(BASS_ERROR_ILLTYPE,		"An illegal type was specified!",					sError);
			ENUM_TO_VALUE(BASS_ERROR_ILLPARAM,		"An illegal parameter was specified! (URL wrong?)",	sError);
			ENUM_TO_VALUE(BASS_ERROR_NO3D,			"No 3D support!",									sError);
			ENUM_TO_VALUE(BASS_ERROR_NOEAX,			"No EAX support!",									sError);
			ENUM_TO_VALUE(BASS_ERROR_DEVICE,		"Illegal device! (No sound card, no access?)",		sError);
			ENUM_TO_VALUE(BASS_ERROR_NOPLAY,		"Not playing!",										sError);
			ENUM_TO_VALUE(BASS_ERROR_FREQ,			"Illegal sample rate!",								sError);
			ENUM_TO_VALUE(BASS_ERROR_NOTFILE,		"The stream is not a file stream!",					sError);
			ENUM_TO_VALUE(BASS_ERROR_NOHW,			"No hardware voices available!",					sError);
			ENUM_TO_VALUE(BASS_ERROR_EMPTY,			"The MOD music has no sequence data!",				sError);
			ENUM_TO_VALUE(BASS_ERROR_NONET,			"No internet connection could be opened!",			sError);
			ENUM_TO_VALUE(BASS_ERROR_CREATE,		"Couldn't create the file!",						sError);
			ENUM_TO_VALUE(BASS_ERROR_NOFX,			"Effects are not available!",						sError);
			ENUM_TO_VALUE(BASS_ERROR_NOTAVAIL,		"Requested data is not available!",					sError);
			ENUM_TO_VALUE(BASS_ERROR_DECODE,		"The channel is a decoding channel!",				sError);
			ENUM_TO_VALUE(BASS_ERROR_DX,			"A sufficient DirectX version is not installed!",	sError);
			ENUM_TO_VALUE(BASS_ERROR_TIMEOUT,		"Connection timedout!",								sError);
			ENUM_TO_VALUE(BASS_ERROR_FILEFORM,		"Unsupported file format!",							sError);
			ENUM_TO_VALUE(BASS_ERROR_SPEAKER,		"Speaker not found!",								sError);
			ENUM_TO_VALUE(BASS_ERROR_VERSION,		"Invalid BASS version!",							sError);
			ENUM_TO_VALUE(BASS_ERROR_CODEC,			"Codec not supported!",								sError);
			ENUM_TO_VALUE(BASS_ERROR_ENDED,			"The channel has ended!",							sError);
			ENUM_TO_VALUE(BASS_ERROR_BUSY,			"The device is busy!",								sError);

			//Custom Errors
			ENUM_TO_VALUE(BASS_ERROR_FILESYSTEM,	"Missing Filesystem!",								sError);

			default: sError = "Unknown error!";
		}

		return sError;
	}
}
