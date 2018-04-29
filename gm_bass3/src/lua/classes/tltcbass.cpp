#include "tltcbass.hpp"

#include "../../classes/tchannel.hpp"
#include "../../tag.hpp"
#include "../../flag.hpp"

namespace LUAINTERFACE
{
	TLTCBass::TLTCBass()
	{
		this->AddEnum("ERROR_OK", BASS_OK);
		this->AddEnum("ERROR_UNKNOWN", BASS_ERROR_UNKNOWN);

		this->AddConstant("Version", UTIL::GetBASSVersionDecimal());
		this->AddConstant("ModuleVersion", GM_BASS_VERSION);

		ADDENUM(CHANNEL_STOPPED);
		ADDENUM(CHANNEL_PLAYING);
		ADDENUM(CHANNEL_PAUSED);
		ADDENUM(CHANNEL_STALLED);

		// Errors
		ADDBASSENUM(ERROR_MEM);
		ADDBASSENUM(ERROR_FILEOPEN);
		ADDBASSENUM(ERROR_DRIVER);
		ADDBASSENUM(ERROR_BUFLOST);
		ADDBASSENUM(ERROR_HANDLE);
		ADDBASSENUM(ERROR_FORMAT);
		ADDBASSENUM(ERROR_POSITION);
		ADDBASSENUM(ERROR_INIT);
		ADDBASSENUM(ERROR_START);
		ADDBASSENUM(ERROR_ALREADY);
		ADDBASSENUM(ERROR_NOCHAN);
		ADDBASSENUM(ERROR_ILLTYPE);
		ADDBASSENUM(ERROR_ILLPARAM);
		ADDBASSENUM(ERROR_NO3D);
		ADDBASSENUM(ERROR_NOEAX);
		ADDBASSENUM(ERROR_DEVICE);
		ADDBASSENUM(ERROR_NOPLAY);
		ADDBASSENUM(ERROR_FREQ);
		ADDBASSENUM(ERROR_NOTFILE);
		ADDBASSENUM(ERROR_NOHW);
		ADDBASSENUM(ERROR_EMPTY);
		ADDBASSENUM(ERROR_NONET);
		ADDBASSENUM(ERROR_CREATE);
		ADDBASSENUM(ERROR_NOFX);
		ADDBASSENUM(ERROR_NOTAVAIL);
		ADDBASSENUM(ERROR_DECODE);
		ADDBASSENUM(ERROR_DX);
		ADDBASSENUM(ERROR_TIMEOUT);
		ADDBASSENUM(ERROR_FILEFORM);
		ADDBASSENUM(ERROR_SPEAKER);
		ADDBASSENUM(ERROR_VERSION);
		ADDBASSENUM(ERROR_CODEC);
		ADDBASSENUM(ERROR_ENDED);
		ADDBASSENUM(ERROR_BUSY);
		ADDBASSENUM(ERROR_FILESYSTEM);

		// FFT modes
		ADDENUM(FFT_256);
		ADDENUM(FFT_512);
		ADDENUM(FFT_1024);
		ADDENUM(FFT_2048);
		ADDENUM(FFT_4096);
		ADDENUM(FFT_8192);
		ADDENUM(FFT_16384);
		ADDENUM(FFT_32768);

		// Custom FFT modes
		ADDENUM(FFT_128);
		ADDENUM(FFT_64);
		ADDENUM(FFT_32);
		ADDENUM(FFT_16);


		// Play modes
		ADDENUM(MODE_NONE);
		ADDENUM(MODE_3D);
		ADDENUM(MODE_LOOP);
		ADDENUM(MODE_MONO);
		ADDENUM(MODE_NOPLAY);
		ADDENUM(MODE_NOBLOCK);

		// Tag types 
		ADDENUM(TAG_META);
		ADDENUM(TAG_HTTP);
		ADDENUM(TAG_ICY);
		ADDENUM(TAG_ID3);
		ADDENUM(TAG_MF);
		ADDENUM(TAG_MP4);
		ADDENUM(TAG_APE);
		ADDENUM(TAG_OGG);

		if (g_CLIENT)
		{
			// FX types 
			ADDENUM(FX_UNKNOWN);
			ADDENUM(FX_ECHO);
			ADDENUM(FX_CHORUS);
			ADDENUM(FX_COMPRESSOR);
			ADDENUM(FX_DISTORTION);
			ADDENUM(FX_ECHO);
			ADDENUM(FX_FLANGER);
			ADDENUM(FX_GARGLE);
			ADDENUM(FX_I3DL2REVERB);
			ADDENUM(FX_PARAMEQ);
			ADDENUM(FX_REVERB);
			ADDENUM(FX_VOLUME);

			// FX Parameter values
			ADDENUM(FXPARAMETER_WAVEFORM_TRIANGLE);
			ADDENUM(FXPARAMETER_WAVEFORM_SINE);
			ADDENUM(FXPARAMETER_WAVEFORM_SQUARE);

			ADDENUM(FXPARAMETER_PHASE_N180);
			ADDENUM(FXPARAMETER_PHASE_N90);
			ADDENUM(FXPARAMETER_PHASE_ZERO);
			ADDENUM(FXPARAMETER_PHASE_P90);
			ADDENUM(FXPARAMETER_PHASE_P180);

			ADDENUM(FXPARAMETER_CURVE_LINEAR);
			ADDENUM(FXPARAMETER_CURVE_LOGARITHMIC);
		}

		this->AddFunction("PlayFile", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			pLUA->CheckType(1, Type::STRING);
			if (!pLUA->IsType(2, Type::NUMBER) && !pLUA->IsType(2, Type::STRING)) pLUA->CheckType(2, Type::NUMBER);
			pLUA->CheckType(3, Type::FUNCTION);

			string sURL = string(pLUA->GetString(1));

			pLUA->Push(3);
			int iCallbackRef = pLUA->ReferenceCreate();

			TChannelThreadArgs ThreadArgs;
			ThreadArgs.bIsOnline = false;

			if (pLUA->IsType(2, Type::NUMBER))
			{
				ThreadArgs.eFlags = FLAG::GetPlayFlags((unsigned int)pLUA->GetNumber(2), ThreadArgs.bNoPlay);
			}
			else
			{
				ThreadArgs.eFlags = FLAG::GetPlayFlags((const char*)pLUA->GetString(2), ThreadArgs.bNoPlay);
			}

			ThreadArgs.iCallbackRef = iCallbackRef;
			ThreadArgs.pChannel = NULL;
			ThreadArgs.sURL = sURL;

			pLUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
			return 1;
		});

		this->AddFunction("PlayURL", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			pLUA->CheckType(1, Type::STRING);
			if (!pLUA->IsType(2, Type::NUMBER) && !pLUA->IsType(2, Type::STRING)) pLUA->CheckType(2, Type::NUMBER);
			pLUA->CheckType(3, Type::FUNCTION);

			string sURL = string(pLUA->GetString(1));

			pLUA->Push(3);
			int iCallbackRef = pLUA->ReferenceCreate();

			TChannelThreadArgs ThreadArgs;
			ThreadArgs.bIsOnline = true;

			if (pLUA->IsType(2, Type::NUMBER))
			{
				ThreadArgs.eFlags = FLAG::GetPlayFlags((unsigned int)pLUA->GetNumber(2), ThreadArgs.bNoPlay);
			}
			else
			{
				ThreadArgs.eFlags = FLAG::GetPlayFlags((const char*)pLUA->GetString(2), ThreadArgs.bNoPlay);
			}

			ThreadArgs.iCallbackRef = iCallbackRef;
			ThreadArgs.pChannel = NULL;
			ThreadArgs.sURL = sURL;

			pLUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
			return 1;
		});

		this->AddFunction("CreateChannel", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			TChannel* pChannel = new TChannel();
			LUAINTERFACE::HELPER::Push<TChannel>(pLUA, pChannel);
			return 1;
		});

		this->AddFunction("DecodeError", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			pLUA->CheckType(1, Type::NUMBER);
			pLUA->PushString(UTIL::DecodeBassError((int)pLUA->GetNumber(1)).c_str());
			return 1;
		});

		this->AddFunction("SetAgent", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			string sAgent = "";
			int iServerPort = 0;

			if (!LUAINTERFACE::IsNil(pLUA, 1))
			{
				pLUA->CheckType(1, Type::STRING);
				sAgent = pLUA->GetString(1);
			}
			else
			{
				sAgent = "";
			}

			if (sAgent == "")
			{
				BASS_SetConfigPtr(BASS_CONFIG_NET_AGENT, g_oldAgentSettings.c_str());
				return 0;
			}

			BASS_SetConfigPtr(BASS_CONFIG_NET_AGENT, sAgent.c_str());
			return 0;
		});

		this->AddFunction("GetAgent", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			char* cAgentSettings = (char*)BASS_GetConfigPtr(BASS_CONFIG_NET_AGENT);
			if (ISNULLPTR(cAgentSettings))
			{
				pLUA->PushString("");
				return 1;
			}

			pLUA->PushString(cAgentSettings);
			return 1;
		});

		this->AddFunction("SetProxy", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			string sServerIP = "";
			int iServerPort = 0;

			string sAuthName = "";
			string sAuthPass = "";

			string sAuth = "";
			string sProxy = "";

			if (!LUAINTERFACE::IsNil(pLUA, 1))
			{
				pLUA->CheckType(1, Type::STRING);
				sServerIP = pLUA->GetString(1);
			}
			else
			{
				sServerIP = "";
			}

			if (sServerIP != "")
			{
				pLUA->CheckType(2, Type::NUMBER);
				iServerPort = pLUA->GetNumber(2);

				if (iServerPort < 0x0000) iServerPort = 0x0000;
				if (iServerPort > 0xFFFF) iServerPort = 0xFFFF;
			}
			else {
				iServerPort = 0;
			}

			if (!LUAINTERFACE::IsNil(pLUA, 3))
			{
				pLUA->CheckType(3, Type::STRING);
				sAuthName = pLUA->GetString(3);

				if (!LUAINTERFACE::IsNil(pLUA, 4))
				{
					pLUA->CheckType(4, Type::STRING);
					sAuthPass = pLUA->GetString(4);
				}
				else
				{
					sAuthPass = "";
				}
			}
			else
			{
				sAuthName = "";
				sAuthPass = "";
			}

			UTIL::STRING::RemoveChars(sAuthName, "@:");
			UTIL::STRING::RemoveChars(sAuthPass, "@:");
			UTIL::STRING::RemoveChars(sServerIP, "@");
			UTIL::STRING::Trim(sAuthName);
			UTIL::STRING::Trim(sServerIP);

			if (sAuthName != "")
			{
				if (sAuthPass != "")
				{
					sAuth += sAuthName;
					sAuth += ':';
					sAuth += sAuthPass;
					sAuth += '@';
				}
				else
				{
					sAuth += sAuthName;
					sAuth += '@';
				}
			}

			sProxy += sAuth;
			sProxy += sServerIP;
			if (sServerIP != "") {
				sProxy += ":";
				sProxy += to_string(iServerPort);
			}

			int iLen = sProxy.length() + 1;
			if (iLen >= 16384 || iLen < 0)
			{
				return 0;
			}

			BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, sProxy.c_str());
			return 0;
		});

		this->AddFunction("GetProxy", [](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;

			char* cProxySettings = (char*)BASS_GetConfigPtr(BASS_CONFIG_NET_PROXY);
			if (ISNULLPTR(cProxySettings))
			{
				pLUA->PushString("");
				pLUA->PushNumber(0);
				pLUA->PushString("");
				pLUA->PushString("");

				return 4;
			}

			string sProxy(cProxySettings);
			cProxySettings = NULL;

			string sSeperator = "";

			string sAuth = "";
			string sServerAddress = "";

			string sServerIP = "";
			string sServerPort = "";
			string sServerPort2 = "";
			int iServerPort = 0;

			string sAuthName = "";
			string sAuthPass = "";

			sSeperator = "@";
			if (!UTIL::STRING::GetKeyValueFromSeperator(sProxy, sSeperator, sAuth, sServerAddress))
			{
				sAuth = "";
				sServerAddress = sProxy;
			}

			sSeperator = ":";
			if (!UTIL::STRING::GetKeyValueFromSeperator(sAuth, sSeperator, sAuthName, sAuthPass))
			{
				sAuthName = sAuth;
				sAuthPass = "";
			}

			if (!UTIL::STRING::GetKeyValueFromSeperator(sServerAddress, sSeperator, sServerIP, sServerPort, true))
			{
				sServerIP = sServerAddress;
				sServerPort = "";
			}

			UTIL::STRING::RemoveChars(sAuthName, "@:");
			UTIL::STRING::RemoveChars(sAuthPass, "@:");
			UTIL::STRING::RemoveChars(sServerIP, "@");
			UTIL::STRING::Trim(sAuthName);
			UTIL::STRING::Trim(sServerIP);
			UTIL::STRING::Trim(sServerPort);

			try
			{
				iServerPort = stoi(sServerPort);
			}
			catch (...)
			{
				iServerPort = 0;
			}

			if (iServerPort < 0x0000) iServerPort = 0x0000;
			if (iServerPort > 0xFFFF) iServerPort = 0xFFFF;

			pLUA->PushString(sServerIP.c_str());
			pLUA->PushNumber(iServerPort);
			pLUA->PushString(sAuthName.c_str());
			pLUA->PushString(sAuthPass.c_str());
			return 4;
		});

		if (g_CLIENT)
		{
			this->AddFunction("GetEAX", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				DWORD iENV;
				float fVol;
				float fDecay;
				float fDamp;

#ifdef _WIN32
				if (BASS_GetEAXParameters(&iENV, &fVol, &fDecay, &fDamp) == FALSE)
				{
					iENV = BASS_NO_CHANGE;
					fVol = BASS_NO_CHANGE;
					fDecay = BASS_NO_CHANGE;
					fDamp = BASS_NO_CHANGE;
				}
#else
				iENV = BASS_NO_CHANGE;
				fVol = BASS_NO_CHANGE;
				fDecay = BASS_NO_CHANGE;
				fDamp = BASS_NO_CHANGE;
#endif

				pLUA->PushNumber((iENV >= 0xFFFFFFFF || iENV == -1) ? (double)(BASS_NO_CHANGE) : (double)(iENV));
				pLUA->PushNumber(fVol);
				pLUA->PushNumber(fDecay);
				pLUA->PushNumber(fDamp);
				return 4;
			});

			this->AddFunction("SetEAX", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				DWORD iENV = BASS_NO_CHANGE;
				float fVol = BASS_NO_CHANGE;
				float fDecay = BASS_NO_CHANGE;
				float fDamp = BASS_NO_CHANGE;

				if (!LUAINTERFACE::IsNil(pLUA, 1))
				{
					pLUA->CheckType(1, Type::NUMBER);
					iENV = (DWORD)pLUA->GetNumber(1);
				}

				if (!LUAINTERFACE::IsNil(pLUA, 2))
				{
					pLUA->CheckType(2, Type::NUMBER);
					fVol = (float)pLUA->GetNumber(2);

					if (fVol > 1) fVol = 1;
					if (fVol < 0) fVol = 0;
				}

				if (!LUAINTERFACE::IsNil(pLUA, 3))
				{
					pLUA->CheckType(3, Type::NUMBER);
					fDecay = (float)pLUA->GetNumber(3);
					if (fDecay > 20) fDecay = 20;
					if (fDecay < 0.1f) fDecay = 0.1f;
				}

				if (!LUAINTERFACE::IsNil(pLUA, 4))
				{
					pLUA->CheckType(4, Type::NUMBER);
					fDamp = (float)pLUA->GetNumber(4);

					if (fDamp > 2) fDecay = 2;
					if (fDamp < 0) fDecay = 0;
				}

#ifdef _WIN32
				pLUA->PushBool(BASS_SetEAXParameters(iENV, fVol, fDecay, fDamp) != FALSE);
#else
				pLUA->PushBool(false);
#endif
				return 1;
			});
		}
	}

	void TLTCBass::Init(ILuaBase* pLUA)
	{
		TLuaTableContainer::Build(pLUA, "BASS3");
	}

	void TLTCBass::Think(ILuaBase* pLUA)
	{
	}

	void TLTCBass::Dispose(ILuaBase* pLUA)
	{
	}
}
