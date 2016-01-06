#include "main.h"

using namespace GarrysMod::Lua;

//
// Called when you module is opened
//

int Init(lua_State* state)
{
	g_pListPendingCallbacks = new SyncList<TChannelCallbackData *>;
	g_pListRunningThreads = new SyncList<thread *>;
	g_pfFFTBuffer = new float[32768];
	g_ProxySettings = new char[16384];
	g_ProxySettings[0] = 0;
	g_ProxySettings[16383] = 0;

	try
	{
		g_thCleanUp = new thread(thfnCleanUp);
	}
	catch(...)
	{
		g_thCleanUp = NULL;
		LUA->ThrowError("BASS Init failed, error creating cleanup thread.");
		return 0;
	}

	BASS_Stop();
	BASSFILESYS::Init();

	LUAINTERFACE::SetupGlobalTable(state);
	LUAINTERFACE::SetupBASSTable(state);
	LUAINTERFACE::SetupChannelObject(state);
	BASS_Start();

	try
	{
		BASS_Stop();

#ifdef _WIN32
		BASS_SetEAXParameters(EAX_PRESET_GENERIC);
#endif

		BASS_SetConfig(BASS_CONFIG_MF_VIDEO, TRUE);
		BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 2);
		BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, TRUE);
		BASS_SetConfig(BASS_CONFIG_VISTA_TRUEPOS, TRUE);
		BASS_SetConfig(BASS_CONFIG_OGG_PRESCAN, TRUE);

		BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_FULL);
		BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 2);

		BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 33);
		BASS_SetConfig(BASS_CONFIG_BUFFER, 500);
		BASS_SetConfig(BASS_CONFIG_NET_BUFFER, 5000);
		BASS_SetConfig(BASS_CONFIG_NET_TIMEOUT, 12500);
		BASS_SetConfig(BASS_CONFIG_NET_READTIMEOUT, 15000);
		BASS_SetConfig(BASS_CONFIG_VERIFY, 0x8000); // 32 kB
		BASS_SetConfig(BASS_CONFIG_VERIFY_NET, 0x8000); // 32 kB

		g_oldProxySettings = (char*)BASS_GetConfigPtr(BASS_CONFIG_NET_PROXY);
		BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, g_oldProxySettings);

		if(g_oldProxySettings != NULL && g_oldProxySettings != nullptr)
		{
			int iLen = strlen(g_oldProxySettings) + 1;
			if(iLen >= 16384 || iLen < 0)
			{
				g_ProxySettings[0] = 0;	
			}
			else
			{
				UTIL::safe_cpy(g_ProxySettings, g_oldProxySettings, iLen);
				g_ProxySettings[16383] = 0;
			}
		}
		else
		{
			g_ProxySettings[0] = 0;
			g_oldProxySettings = NULL;
		}

		BASS_Start();
	}
	catch(...)
	{
		BASS_Stop();
		BASS_Start();

		LUA->ThrowError("BASS Init failed, exception error: Unknown");
		return 0;
	}

	return 0;
}

GMOD_MODULE_OPEN()
{
	g_CLOSING = false;
	g_IntialTickHappend = false;
	g_SELFLOADED = false;

	unsigned int iVer = HIWORD(BASS_GetVersion());
	if(iVer < BASSVERSION)
	{
		unsigned int iHeaderVer = UTIL::VersionToDecimal(BASSVERSION << 16);
		char err[128];

		snprintf(err, 128, "BASS Init failed, outdated BASS Version (%x expected, got %i).\n", iHeaderVer, UTIL::GetBASSVersionDecimal());
		LUA->ThrowError(err);

		return 0;
	}

	LUAINTERFACE::SetupRealm(state);

	if(g_CLIENT)
	{
		return Init(state);
	}
	else // Only for the server
	{
		char err[256];
		err[0] = 0;
		err[255] = 0;

		try
		{
			BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, true);
			if(!BASS_Init(-1, 44100, BASS_NULL, NULL, NULL)) // Try to load Bass.
			{
				int error = BASS_ErrorGetCode();
				if (error == BASS_ERROR_ALREADY) return Init(state); // Bass is already loaded, so use that.

				if (error == BASS_ERROR_DX)
				{
					snprintf(err, 256, "BASS Init failed, error code %d.\nDirectX or ALSA is not available.\nMake sure you installed ASIO on Linux or DirectX on Windows.", error);

					err[255] = 0;
					LUA->ThrowError(err);

					return 0;
				}

				if (error == BASS_ERROR_DRIVER)
				{
					snprintf(err, 256, "BASS Init failed, error code %d.\nSound driver is not available.\nIf you are on Linux, make sure you installed ASIO.\nOn Linux the user running this application needs 'xrw' access to the sound interface too.", error);

					err[255] = 0;
					LUA->ThrowError(err);

					return 0;
				}

				if (error == BASS_ERROR_DEVICE)
				{
					snprintf(err, 256, "BASS Init failed, error code %d.\nSound driver is not available or the device was not found.\nIf you are on Linux, make sure you installed ASIO.", error);

					err[255] = 0;
					LUA->ThrowError(err);

					return 0;
				}

				snprintf(err, 256, "BASS Init failed, error code %d.\n", error);

				err[255] = 0;
				LUA->ThrowError(err);

				return 0;
			}
		}
		catch(const overflow_error& e)
		{
			snprintf(err, 256, "BASS Init failed, exception error: %s\n", e.what());

			err[255] = 0;
			LUA->ThrowError(err);
			return 0;
		}
		catch(const runtime_error& e)
		{
			snprintf(err, 256, "BASS Init failed, exception error: %s\n", e.what());

			err[255] = 0;
			LUA->ThrowError(err);
			return 0;
		}
		catch(const exception& e)
		{
			snprintf(err, 256, "BASS Init failed, exception error: %s\n", e.what());

			err[255] = 0;
			LUA->ThrowError(err);
			return 0;
		}
		catch(char *s)
		{
			snprintf(err, 256, "BASS Init failed, exception error: %s\n", s);

			err[255] = 0;
			LUA->ThrowError(err);
			return 0;
		}
		catch(...)
		{
			LUA->ThrowError("BASS Init failed, exception error: Unknown");
			return 0;
		}
	}

	g_SELFLOADED = true;

	try
	{
		BASS_Stop();
		BASS_Start();
	}
	catch(...)
	{
		LUA->ThrowError("BASS Init failed, exception error: Unknown");
		return 0;
	}

	return Init(state);
}

//
// Called when your module is closed
//
GMOD_MODULE_CLOSE()
{
	g_CLOSING = true;

	if(g_thCleanUp != NULL)
	{
		g_thCleanUp->join();
		delete g_thCleanUp;
		g_thCleanUp = NULL;
	}

	UTIL::ClearPendingChannels(state);
	LUAINTERFACE::UnrefGlobalReferences(state);

	if(g_pListRunningThreads != NULL)
	{
		delete g_pListRunningThreads;
		g_pListRunningThreads = NULL;
	}

	if(g_pListPendingCallbacks != NULL)
	{
		delete g_pListPendingCallbacks;
		g_pListPendingCallbacks = NULL;
	}

	if(g_pfFFTBuffer != NULL)
	{
		delete [] g_pfFFTBuffer;
		g_pfFFTBuffer = NULL;
	}

	if(g_ProxySettings != NULL)
	{
		delete [] g_ProxySettings;
		g_ProxySettings = NULL;
	}

#ifdef _WIN32
	BASS_SetEAXParameters(EAX_PRESET_GENERIC);
#endif

	BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, g_oldProxySettings);

	if(g_SELFLOADED)
	{
		try
		{
			BASS_Stop();

			BASS_PluginFree(BASS_NULL);
			BASS_Free();
		}
		catch(...)
		{
		}
	}

	return 0;
}