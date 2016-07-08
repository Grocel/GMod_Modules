#include <cstring>
#include <string>
#include <algorithm>

#include "lua_functions.h"
#include "lua.h"
#include "util.h"
#include "classes/tchannel.h"
#include "tag.h"

bass_flag GetPlayFlags(unsigned int iLuaFlag, bool& bNoPlay)
{
	bool bNoBlock = false;
	bNoPlay = false;

	bass_flag eFlags = BASS_SAMPLE_FLOAT;
	if(iLuaFlag == LUAENUM_MODE_NONE) return eFlags | BASS_STREAM_BLOCK;

	if(g_CLIENT && (iLuaFlag & LUAENUM_MODE_3D))
		eFlags |= (BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX | BASS_SAMPLE_MONO); // 3D needs mono sound

	if(iLuaFlag & LUAENUM_MODE_LOOP)
		eFlags |= BASS_SAMPLE_LOOP;

	if(iLuaFlag & LUAENUM_MODE_MONO)
		eFlags |= BASS_SAMPLE_MONO;

	if(iLuaFlag & LUAENUM_MODE_NOBLOCK)
		bNoBlock = true;

	if(iLuaFlag & LUAENUM_MODE_NOPLAY)
		bNoPlay = true;
	
	if(!bNoBlock)
		eFlags |= BASS_STREAM_BLOCK;

	return eFlags;
}

bass_flag GetPlayFlags(const char* sLuaFlag, bool& bNoPlay)
{ 
	bool bNoBlock = false;
	bNoPlay = false;

	bass_flag eFlags = BASS_SAMPLE_FLOAT;
	if(ISNULLPTR(sLuaFlag)) return eFlags | BASS_STREAM_BLOCK;

	string sBuf = string(sLuaFlag) + string(" ");
	if(sBuf == " ") return eFlags | BASS_STREAM_BLOCK;

	transform(sBuf.begin(), sBuf.end(), sBuf.begin(), ::tolower);

	string sFind = "";
	string::size_type posMatch = sBuf.find(" ");

	while(posMatch != string::npos)
	{
        sFind = sBuf.substr(0, posMatch);
		sBuf = sBuf.substr(posMatch + 1);
		posMatch = sBuf.find(" ");

		if(sFind == "") continue;
		if(sFind == " ") continue;

		if(g_CLIENT && (sFind == "3d"))
		{
			eFlags |= (BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX | BASS_SAMPLE_MONO); // 3D needs mono sound
			continue;
		}

		if(sFind == "loop")
		{
			eFlags |= BASS_SAMPLE_LOOP;
			continue;
		}

		if(sFind == "mono")
		{
			eFlags |= BASS_SAMPLE_MONO;
			continue;
		}

		if(sFind == "noblock")
		{
			bNoBlock = true;
			continue;
		}

		if(sFind == "noplay")
		{
			bNoPlay = true;
			continue;
		}
	}

	if(!bNoBlock)
		eFlags |= BASS_STREAM_BLOCK;

	return eFlags;
}

unsigned char SetChannelStateFlag(bass_flag iBassFlag)
{
	switch (iBassFlag)
	{
		case BASS_ACTIVE_STOPPED: return LUAENUM_CHANNEL_STOPPED;
		case BASS_ACTIVE_PLAYING: return LUAENUM_CHANNEL_PLAYING;
		case BASS_ACTIVE_PAUSED: return LUAENUM_CHANNEL_PAUSED;
		case BASS_ACTIVE_STALLED: return LUAENUM_CHANNEL_STALLED;
	}

	return BASS_NULL; 
}

namespace LUAFUNC
{
	LUA_FUNCTION(PullPendingChannels)
	{
		if(g_CLOSING) return 0;
		if(ISNULLPTR(g_pListPendingCallbacks)) return 0;

		g_IntialTickHappend = true;

		while(g_pListPendingCallbacks->getSize() > 0)
		{
			TChannelCallbackData* pCallbackData = g_pListPendingCallbacks->remove();
			if(ISNULLPTR(pCallbackData)) continue;

			bool bIsRecycledChannel = pCallbackData->bIsRecycledChannel;
			int iCallbackRef = pCallbackData->iCallbackRef;
			int iError = pCallbackData->iError;
			TChannel* pChannel = pCallbackData->pChannel;
			bool bNoPlay = pCallbackData->bNoPlay;

			delete pCallbackData;

			if(iError == BASS_OK && ISNULLPTR(pChannel)) iError = BASS_ERROR_UNKNOWN;

			if(!bIsRecycledChannel && iError != BASS_OK)
			{
				LUAINTERFACE::DeleteChannel(pChannel);
				pChannel = NULL;
			}

			if(!ISNULLPTR(pChannel))
			{
				if(g_CLIENT)
				{
					pChannel->SetVolume(1);
				}
				else
				{
					// Mute on servers
					pChannel->SetVolume(0);
				}

				if(!bNoPlay)
				{
					pChannel->Play(true);
				}
			}

			if(!iCallbackRef)
			{
				LUAINTERFACE::DeleteChannel(pChannel);
				pChannel = NULL;

				continue;
			}

			LUA->ReferencePush( iCallbackRef );
				LUAINTERFACE::PushChannel(state, pChannel);
				LUA->PushNumber(iError);
				LUA->PushString(UTIL::DecodeBassError(iError));
			LUA->Call(3, 0);
			LUA->ReferenceFree(iCallbackRef);
		}

		return 0;
	}

	namespace _G
	{
		namespace BASS3
		{
			LUA_FUNCTION(PlayFile)
			{
				LUA->CheckType(1, Type::STRING);
				if(!LUA->IsType(2, Type::NUMBER) && !LUA->IsType(2, Type::STRING) ) LUA->CheckType(2, Type::NUMBER);
				LUA->CheckType(3, Type::FUNCTION);

				const char* sURL = LUA->GetString( 1 );

				LUA->Push( 3 );
				int iCallbackRef = LUA->ReferenceCreate();

				TChannelThreadArgs ThreadArgs;
				ThreadArgs.bIsOnline = false;

				if(LUA->IsType(2, Type::NUMBER))
				{
					ThreadArgs.eFlags = GetPlayFlags((unsigned int)LUA->GetNumber( 2 ), ThreadArgs.bNoPlay);
				}
				else
				{
					ThreadArgs.eFlags = GetPlayFlags((const char*)LUA->GetString( 2 ), ThreadArgs.bNoPlay);
				}

				ThreadArgs.iCallbackRef = iCallbackRef;
				ThreadArgs.pChannel = NULL;
				ThreadArgs.sURL = sURL;

				LUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
				return 1;
			}

			LUA_FUNCTION(PlayURL)
			{
				LUA->CheckType(1, Type::STRING);
				if(!LUA->IsType(2, Type::NUMBER) && !LUA->IsType(2, Type::STRING) ) LUA->CheckType(2, Type::NUMBER);
				LUA->CheckType(3, Type::FUNCTION);

				const char* sURL = LUA->GetString( 1 );

				LUA->Push( 3 );
				int iCallbackRef = LUA->ReferenceCreate();

				TChannelThreadArgs ThreadArgs;
				ThreadArgs.bIsOnline = true;

				if(LUA->IsType(2, Type::NUMBER))
				{
					ThreadArgs.eFlags = GetPlayFlags((unsigned int)LUA->GetNumber( 2 ), ThreadArgs.bNoPlay);
				}
				else
				{
					ThreadArgs.eFlags = GetPlayFlags((const char*)LUA->GetString( 2 ), ThreadArgs.bNoPlay);
				}

				ThreadArgs.iCallbackRef = iCallbackRef;
				ThreadArgs.pChannel = NULL;
				ThreadArgs.sURL = sURL;

				LUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
				return 1;
			}

			LUA_FUNCTION(CreateChannel)
			{
				TChannel* pChannel = new TChannel();
				LUAINTERFACE::PushChannel(state, pChannel);
				return 1;
			}

			LUA_FUNCTION(DecodeError)
			{
				LUA->CheckType(1, Type::NUMBER);
				LUA->PushString(UTIL::DecodeBassError((int)LUA->GetNumber(1)));
				return 1;
			}

			LUA_FUNCTION(SetAgent)
			{
				string sAgent = "";
				int iServerPort = 0;

				if(!ISNIL(1))
				{
					LUA->CheckType(1, Type::STRING);
					sAgent = LUA->GetString(1);
				}
				else
				{
					sAgent = "";
				}

				if(sAgent == "")
				{
					BASS_SetConfigPtr(BASS_CONFIG_NET_AGENT, g_oldAgentSettings.c_str());
					return 0;
				}

				BASS_SetConfigPtr(BASS_CONFIG_NET_AGENT, sAgent.c_str());
				return 0;
			}

			LUA_FUNCTION(GetAgent)
			{
				char* cAgentSettings = (char*)BASS_GetConfigPtr(BASS_CONFIG_NET_AGENT);
				if(ISNULLPTR(cAgentSettings))
				{
					LUA->PushString("");
					return 1;
				}

				LUA->PushString(cAgentSettings);
				return 1;
			}

			LUA_FUNCTION(SetProxy)
			{
				string sServerIP = "";
				int iServerPort = 0;

				string sAuthName = "";
				string sAuthPass = "";

				string sAuth = "";
				string sProxy = "";

				if(!ISNIL(1))
				{
					LUA->CheckType(1, Type::STRING);
					sServerIP = LUA->GetString(1);
				}
				else
				{
					sServerIP = "";
				}

				if(sServerIP != "")
				{
					LUA->CheckType(2, Type::NUMBER);
					iServerPort = LUA->GetNumber(2);

					if(iServerPort < 0x0000) iServerPort = 0x0000;
					if(iServerPort > 0xFFFF) iServerPort = 0xFFFF;
				} else {
					iServerPort = 0;
				}

				if(!ISNIL(3))
				{
					LUA->CheckType(3, Type::STRING);
					sAuthName = LUA->GetString(3);

					if(!ISNIL(4))
					{
						LUA->CheckType(4, Type::STRING);
						sAuthPass = LUA->GetString(4);
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

				if(sAuthName != "")
				{
					if(sAuthPass != "")
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
				if(sServerIP != "") {
					sProxy += ":";
					sProxy += to_string(iServerPort);			
				}
				
				int iLen = sProxy.length() + 1;
				if(iLen >= 16384 || iLen < 0)
				{
					return 0;
				}

				BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, sProxy.c_str());
				return 0;
			}

			LUA_FUNCTION(GetProxy)
			{
				char* cProxySettings = (char*)BASS_GetConfigPtr(BASS_CONFIG_NET_PROXY);
				if(ISNULLPTR(cProxySettings))
				{
					LUA->PushString("");
					LUA->PushNumber(0);
					LUA->PushString("");
					LUA->PushString("");

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
				if(!UTIL::STRING::GetKeyValueFromSeperator(sProxy, sSeperator, sAuth, sServerAddress))
				{
					sAuth = "";
					sServerAddress = sProxy;
				}

				sSeperator = ":";
				if(!UTIL::STRING::GetKeyValueFromSeperator(sAuth, sSeperator, sAuthName, sAuthPass))
				{
					sAuthName = sAuth;
					sAuthPass = "";
				}

				if(!UTIL::STRING::GetKeyValueFromSeperator(sServerAddress, sSeperator, sServerIP, sServerPort, true))
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
				catch(...)
				{
					iServerPort = 0;
				}

				if(iServerPort < 0x0000) iServerPort = 0x0000;
				if(iServerPort > 0xFFFF) iServerPort = 0xFFFF;
				
				LUA->PushString(sServerIP.c_str());
				LUA->PushNumber(iServerPort);
				LUA->PushString(sAuthName.c_str());
				LUA->PushString(sAuthPass.c_str());
				return 4;
			}

			LUA_FUNCTION(GetEAX)
			{
				DWORD iENV;
				float fVol;
				float fDecay;
				float fDamp;

#ifdef _WIN32
				if(BASS_GetEAXParameters(&iENV, &fVol, &fDecay, &fDamp) == FALSE)
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

				LUA->PushNumber((iENV >= 0xFFFFFFFF || iENV == -1) ? (double)(BASS_NO_CHANGE) : (double)(iENV));
				LUA->PushNumber(fVol);
				LUA->PushNumber(fDecay);
				LUA->PushNumber(fDamp);
				return 4;
			}

			LUA_FUNCTION(SetEAX)
			{
				DWORD iENV = BASS_NO_CHANGE;
				float fVol = BASS_NO_CHANGE;
				float fDecay = BASS_NO_CHANGE;
				float fDamp = BASS_NO_CHANGE;

				if(!ISNIL(1))
				{
					LUA->CheckType(1, Type::NUMBER);
					iENV = (DWORD)LUA->GetNumber(1);
				}

				if(!ISNIL(2))
				{
					LUA->CheckType(2, Type::NUMBER);
					fVol = (float)LUA->GetNumber(2);

					if(fVol > 1) fVol = 1;
					if(fVol < 0) fVol = 0;
				}

				if(!ISNIL(3))
				{
					LUA->CheckType(3, Type::NUMBER);
					fDecay = (float)LUA->GetNumber(3);
					if(fDecay > 20) fDecay = 20;
					if(fDecay < 0.1f) fDecay = 0.1f;
				}

				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::NUMBER);
					fDamp = (float)LUA->GetNumber(4);

					if(fDamp > 2) fDecay = 2;
					if(fDamp < 0) fDecay = 0;
				}

#ifdef _WIN32
				LUA->PushBool(BASS_SetEAXParameters(iENV, fVol, fDecay, fDamp) != FALSE);
#else
				LUA->PushBool(false);
#endif
				return 1;
			}
		}
	}

	namespace _R
	{
		namespace IBASS3Channel
		{
			LUA_FUNCTION(lua__gc)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);

				LUAINTERFACE::DeleteChannel(pChannel);
				return 0;
			}

			LUA_FUNCTION(lua__eq)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, TYPE_CHANNEL);

				TChannel* pChannelA = GETCHANNEL(1); 
				TChannel* pChannelB = GETCHANNEL(2); 
				LUA->PushBool(pChannelA == pChannelB);
				return 1;
			}

			LUA_FUNCTION(lua__tostring)
			{
				LUA->CheckType(1, TYPE_CHANNEL);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushString("[NULL " META_CHANNEL "]");
					return 1;
				}

				char sToString[1024];
				pChannel->ToString(sToString);
				LUA->PushString(sToString);

				return 1;
			}

			LUA_FUNCTION(Remove)
			{
				LUA->CheckType(1, TYPE_CHANNEL);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				pChannel->Remove();
				return 0;
			}

			LUA_FUNCTION(IsValid)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->IsValid());
				return 1;
			}

			LUA_FUNCTION(PlayFile)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::STRING);
				if(!LUA->IsType(3, Type::NUMBER) && !LUA->IsType(3, Type::STRING) ) LUA->CheckType(3, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				const char* sURL = LUA->GetString( 2 );

				int iCallbackRef = 0;
				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::FUNCTION);
					LUA->Push( 4 );
					iCallbackRef = LUA->ReferenceCreate();
				}

				TChannelThreadArgs ThreadArgs;
				ThreadArgs.bIsOnline = false;

				if(LUA->IsType(3, Type::NUMBER))
				{
					ThreadArgs.eFlags = GetPlayFlags((unsigned int)LUA->GetNumber( 3 ), ThreadArgs.bNoPlay);
				}
				else
				{
					ThreadArgs.eFlags = GetPlayFlags((const char*)LUA->GetString( 3 ), ThreadArgs.bNoPlay);
				}

				ThreadArgs.iCallbackRef = iCallbackRef;
				ThreadArgs.pChannel = pChannel;
				ThreadArgs.sURL = sURL;

				LUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
				return 1;
			}

			LUA_FUNCTION(PlayURL)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::STRING);
				if(!LUA->IsType(3, Type::NUMBER) && !LUA->IsType(3, Type::STRING) ) LUA->CheckType(3, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				const char* sURL = LUA->GetString( 2 );

				int iCallbackRef = 0;
				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::FUNCTION);
					LUA->Push( 4 );
					iCallbackRef = LUA->ReferenceCreate();
				}

				TChannelThreadArgs ThreadArgs;
				ThreadArgs.bIsOnline = true;

				if(LUA->IsType(3, Type::NUMBER))
				{
					ThreadArgs.eFlags = GetPlayFlags((unsigned int)LUA->GetNumber( 3 ), ThreadArgs.bNoPlay);
				}
				else
				{
					ThreadArgs.eFlags = GetPlayFlags((const char*)LUA->GetString( 3 ), ThreadArgs.bNoPlay);
				}

				ThreadArgs.iCallbackRef = iCallbackRef;
				ThreadArgs.pChannel = pChannel;
				ThreadArgs.sURL = sURL;

				LUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
				return 1;
			}

			LUA_FUNCTION(Play)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				if(ISNIL(2))
				{
					pChannel->Play();
					return 0;
				}

				LUA->CheckType(2, Type::BOOL);
				pChannel->Play(LUA->GetBool(2));
				return 0;
			}

			LUA_FUNCTION(Stop)
			{
				LUA->CheckType(1, TYPE_CHANNEL);			

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;
				pChannel->Stop();

				return 0;
			}

			LUA_FUNCTION(Pause)
			{
				LUA->CheckType(1, TYPE_CHANNEL);			

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;
				pChannel->Pause();

				return 0;
			}

			LUA_FUNCTION(GetState)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(false);
					return 1;
				}

				LUA->PushNumber(SetChannelStateFlag(pChannel->GetState()));
				return 1;
			}

			LUA_FUNCTION(SetVolume)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				pChannel->SetVolume((float)LUA->GetNumber(2));
				return 0;
			}

			LUA_FUNCTION(GetVolume)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				LUA->PushNumber(pChannel->GetVolume());
				return 1;
			}

			LUA_FUNCTION(VolumeFadeTo)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);
				LUA->CheckType(3, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				DWORD iIime = (DWORD)(LUA->GetNumber(3) * 1000);

				pChannel->VolumeFadeTo((float)LUA->GetNumber(2), iIime);
				return 0;
			}

			LUA_FUNCTION(VolumeIsFading)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->VolumeIsFading());
				return 1;
			}

			LUA_FUNCTION(SetBalance)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				pChannel->SetBalance((float)LUA->GetNumber(2));
				return 0;
			}

			LUA_FUNCTION(GetBalance)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				LUA->PushNumber(pChannel->GetBalance());
				return 1;
			}

			LUA_FUNCTION(BalanceFadeTo)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);
				LUA->CheckType(3, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				DWORD iIime = (DWORD)(LUA->GetNumber(3) * 1000);

				pChannel->BalanceFadeTo((float)LUA->GetNumber(2), iIime);
				return 0;
			}

			LUA_FUNCTION(BalanceIsFading)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->BalanceIsFading());
				return 1;
			}

			LUA_FUNCTION(Is3D)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->Is3D());
				return 1;
			}

			LUA_FUNCTION(IsLooping)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->IsLooping());
				return 1;
			}

			LUA_FUNCTION(IsOnline)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->IsOnline());
				return 1;
			}

			LUA_FUNCTION(IsBlockStreamed)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->IsBlockStreamed());
				return 1;
			}


			LUA_FUNCTION(EnableLooping)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::BOOL);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				pChannel->EnableLooping(LUA->GetBool(2));
				return 0;
			}

			LUA_FUNCTION(FFT)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::TABLE);
				LUA->CheckType(3, Type::NUMBER);

				bool bRemoveDCbias = false;
				bool bRemoveHannWindow = false;
				bool bToDecibel = false;

				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::BOOL);
					bRemoveDCbias = LUA->GetBool(4);
				}

				if(!ISNIL(5))
				{
					LUA->CheckType(5, Type::BOOL);
					bRemoveHannWindow = LUA->GetBool(5);
				}

				if(!ISNIL(6))
				{
					LUA->CheckType(6, Type::BOOL);
					bToDecibel = LUA->GetBool(6);
				}

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				if(ISNULLPTR(g_pfFFTBuffer))
				{
					LUA->PushNumber(0);
					return 1;
				}

				bass_flag iFlag = BASS_NULL;
				unsigned int iCount = 0;
				unsigned int iSkip = 1;
				unsigned int iLuaFlag = (unsigned int)LUA->GetNumber(3);

				switch (iLuaFlag)
				{
					case LUAENUM_FFT_256: iFlag = BASS_DATA_FFT256; iCount = 128; break;
					case LUAENUM_FFT_512: iFlag = BASS_DATA_FFT512; iCount = 256; break;
					case LUAENUM_FFT_1024: iFlag = BASS_DATA_FFT1024; iCount = 512; break;
					case LUAENUM_FFT_2048: iFlag = BASS_DATA_FFT2048; iCount = 1024; break;
					case LUAENUM_FFT_4096: iFlag = BASS_DATA_FFT4096; iCount = 2048; break;
					case LUAENUM_FFT_8192: iFlag = BASS_DATA_FFT8192; iCount = 4096; break;
					case LUAENUM_FFT_16384: iFlag = BASS_DATA_FFT16384; iCount = 8192; break;
					case LUAENUM_FFT_32768: iFlag = BASS_DATA_FFT32768; iCount = 16384; break;

					case LUAENUM_FFT_128: iFlag = BASS_DATA_FFT256; iCount = 128; iSkip = 2; break;
					case LUAENUM_FFT_64: iFlag = BASS_DATA_FFT256; iCount = 128; iSkip = 4; break;
					case LUAENUM_FFT_32: iFlag = BASS_DATA_FFT256; iCount = 128; iSkip = 8; break;
					case LUAENUM_FFT_16: iFlag = BASS_DATA_FFT256; iCount = 128; iSkip = 16; break;

					default: LUA->PushNumber(0); return 1;
				}

				LUA->Push( 2 );
				int iTableRef = LUA->ReferenceCreate();

				float fTmp;
				unsigned int i;
				unsigned int iIndex = 1;

				if(bRemoveDCbias) iFlag |= BASS_DATA_FFT_REMOVEDC;
				if(bRemoveHannWindow) iFlag |= BASS_DATA_FFT_NOWINDOW;

				LUA->ReferencePush( iTableRef );
				if(!pChannel->FFT(iFlag, g_pfFFTBuffer))
				{
					if(bToDecibel)
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							LUA->PushNumber( iIndex ); 
							LUA->PushNumber( BASS_MIN_DECIBELS );
							LUA->SetTable( -3 );					

							iIndex++;
						}
					}
					else
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							LUA->PushNumber( iIndex ); 
							LUA->PushNumber( 0 );
							LUA->SetTable( -3 );					

							iIndex++;
						}
					}					
				}
				else
				{
					if(bToDecibel)
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							fTmp = g_pfFFTBuffer[i];
							fTmp = ((fTmp > 0) ? 20 * log10(fTmp) : BASS_MIN_DECIBELS);
							if(fTmp < BASS_MIN_DECIBELS) fTmp = BASS_MIN_DECIBELS;

							LUA->PushNumber( iIndex ); 
							LUA->PushNumber( fTmp );
							LUA->SetTable( -3 );

							iIndex++;
						}
					}
					else
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							fTmp = g_pfFFTBuffer[i];
							if(fTmp < 0) fTmp = 0;
							if(fTmp > 1) fTmp = 1;

							LUA->PushNumber( iIndex ); 
							LUA->PushNumber( fTmp );
							LUA->SetTable( -3 );

							iIndex++;
						}
					}
				}
				LUA->Pop();

				LUA->PushNumber(iIndex-1);
				LUA->ReferenceFree(iTableRef);

				return 1;
			}

			LUA_FUNCTION(FFTComplex)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::TABLE);
				LUA->CheckType(3, Type::NUMBER);

				bool bRemoveDCbias = false;
				bool bRemoveHannWindow = false;

				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::BOOL);
					bRemoveDCbias = LUA->GetBool(4);
				}

				if(!ISNIL(5))
				{
					LUA->CheckType(5, Type::BOOL);
					bRemoveHannWindow = LUA->GetBool(5);
				}

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				if(ISNULLPTR(g_pfFFTBuffer))
				{
					LUA->PushNumber(0);
					return 1;
				}

				bass_flag iFlag = BASS_NULL;
				unsigned int iCount = 0;
				unsigned int iSkip = 2;
				unsigned int iLuaFlag = (unsigned int)LUA->GetNumber(3);

				switch (iLuaFlag)
				{
					case LUAENUM_FFT_256: iFlag = BASS_DATA_FFT256; iCount = 512; break;
					case LUAENUM_FFT_512: iFlag = BASS_DATA_FFT512; iCount = 1024; break;
					case LUAENUM_FFT_1024: iFlag = BASS_DATA_FFT1024; iCount = 2048; break;
					case LUAENUM_FFT_2048: iFlag = BASS_DATA_FFT2048; iCount = 4096; break;
					case LUAENUM_FFT_4096: iFlag = BASS_DATA_FFT4096; iCount = 8192; break;
					case LUAENUM_FFT_8192: iFlag = BASS_DATA_FFT8192; iCount = 16384; break;
					case LUAENUM_FFT_16384: iFlag = BASS_DATA_FFT16384; iCount = 32768; break;
					case LUAENUM_FFT_32768: iFlag = BASS_DATA_FFT32768; iCount = 65536; break;

					case LUAENUM_FFT_128: iFlag = BASS_DATA_FFT256; iCount = 512; iSkip = 4; break;
					case LUAENUM_FFT_64: iFlag = BASS_DATA_FFT256; iCount = 512; iSkip = 8; break;
					case LUAENUM_FFT_32: iFlag = BASS_DATA_FFT256; iCount = 512; iSkip = 16; break;
					case LUAENUM_FFT_16: iFlag = BASS_DATA_FFT256; iCount = 512; iSkip = 32; break;

					default: LUA->PushNumber(0); return 1;
				}

				LUA->Push( 2 );
				int iTableRef = LUA->ReferenceCreate();

				float fTmp;
				unsigned int iIndex = 1;
				unsigned int i;

				if(bRemoveDCbias) iFlag |= BASS_DATA_FFT_REMOVEDC;
				if(bRemoveHannWindow) iFlag |= BASS_DATA_FFT_NOWINDOW;

				LUA->ReferencePush( iTableRef );
				if(!pChannel->FFT(iFlag | BASS_DATA_FFT_COMPLEX, g_pfFFTBuffer))
				{
					for (i = 0; i < iCount; i += iSkip)
					{
						LUA->PushNumber( iIndex ); 
						LUA->PushNumber( 0 );
						LUA->SetTable( -3 );					

						iIndex++;

						LUA->PushNumber( iIndex ); 
						LUA->PushNumber( 0 );
						LUA->SetTable( -3 );					

						iIndex++;
					}
				}
				else
				{
					for (i = 0; i < iCount; i += iSkip)
					{
						// Real Part 
						fTmp = g_pfFFTBuffer[i];
						if(fTmp < -1) fTmp = -1;
						if(fTmp > 1) fTmp = 1;

						LUA->PushNumber( iIndex ); 
						LUA->PushNumber( fTmp );
						LUA->SetTable( -3 );

						iIndex++;

						// Imaginary Part 
						fTmp = g_pfFFTBuffer[i+1];
						if(fTmp < -1) fTmp = -1;
						if(fTmp > 1) fTmp = 1;

						LUA->PushNumber( iIndex ); 
						LUA->PushNumber( fTmp );
						LUA->SetTable( -3 );

						iIndex++;
					}
				}
				LUA->Pop();

				LUA->PushNumber(iIndex-1);
				LUA->ReferenceFree(iTableRef);
				return 1;
			}

			LUA_FUNCTION(GetLevel)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);

				bool bRMS = false;
				bool bToDecibel = false;
				float fTimeFrame = 0.02;

				if(!ISNIL(2))
				{
					LUA->CheckType(2, Type::NUMBER);
					fTimeFrame = (float)LUA->GetNumber(2);
				}

				if(!ISNIL(3))
				{
					LUA->CheckType(3, Type::BOOL);
					bRMS = LUA->GetBool(4);
				}

				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::BOOL);
					bToDecibel = LUA->GetBool(4);
				}

				float fTmp;
				unsigned int i;

				if(ISNULLPTR(pChannel) || !pChannel->GetLevelEx(g_pfFFTBuffer, fTimeFrame, bRMS))
				{
					if(bToDecibel)
					{
						for (i = 0; i < 2; i++)
						{
							LUA->PushNumber(BASS_MIN_DECIBELS);
						}
					}
					else
					{
						for (i = 0; i < 2; i++)
						{
							LUA->PushNumber(0);
						}
					}

					return 2;
				}

				if(bToDecibel)
				{
					for (i = 0; i < 2; i++)
					{
						fTmp = g_pfFFTBuffer[i];
						fTmp = ((fTmp > 0) ? 20 * log10(fTmp) : BASS_MIN_DECIBELS);
						if(fTmp < BASS_MIN_DECIBELS) fTmp = BASS_MIN_DECIBELS;

						LUA->PushNumber(fTmp);
					}
				}
				else
				{
					for (i = 0; i < 2; i++)
					{
						fTmp = g_pfFFTBuffer[i];
						if(fTmp < 0) fTmp = 0;
						if(fTmp > 1) fTmp = 1;

						LUA->PushNumber(fTmp);
					}
				}

				return 2;
			}

			LUA_FUNCTION(GetTime)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				LUA->PushNumber(pChannel->GetTime());
				return 1;
			}

			LUA_FUNCTION(SetTime)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				pChannel->SetTime(LUA->GetNumber(2));
				return 0;
			}


			LUA_FUNCTION(GetLength)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				LUA->PushNumber(pChannel->GetLength());
				return 1;
			}

			LUA_FUNCTION(IsEndless)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->IsEndless());
				return 1;
			}


			LUA_FUNCTION(IsSeeking)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->IsSeeking());
				return 1;
			}


			LUA_FUNCTION(GetTag)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);

				unsigned int iLuaFlag = (unsigned int)LUA->GetNumber(2);
				int iTableRef = 0;
				const char* pTagData = NULL;

				if(ISNIL(3))
				{
					LUA->CreateTable();
				}
				else // Allow recycling
				{
					LUA->CheckType(3, Type::TABLE);
					LUA->Push( 3 );
				}

				iTableRef = LUA->ReferenceCreate();
				LUA->ReferencePush( iTableRef );

				if(ISNULLPTR(pChannel))
				{
					switch (iLuaFlag)
					{
						case LUAENUM_TAG_META:
							pTagData = pChannel->GetTag(BASS_TAG_META);
							TAG::META(pTagData, state);
						break;

						case LUAENUM_TAG_ID3:
							pTagData = pChannel->GetTag(BASS_TAG_ID3);
							TAG::ID3(pTagData, state);
						break;

						case LUAENUM_TAG_HTTP:
							pTagData = pChannel->GetTag(BASS_TAG_HTTP);
							TAG::HTTP(pTagData, state);
						break;

						case LUAENUM_TAG_ICY:
							pTagData = pChannel->GetTag(BASS_TAG_ICY);
							TAG::ICY(pTagData, state);
						break;

						case LUAENUM_TAG_MF:
							pTagData = pChannel->GetTag(BASS_TAG_MF);
							TAG::MF(pTagData, state);
						break;

						case LUAENUM_TAG_MP4:
							pTagData = pChannel->GetTag(BASS_TAG_MP4);
							TAG::MP4(pTagData, state);
						break;

						case LUAENUM_TAG_APE:
							pTagData = pChannel->GetTag(BASS_TAG_APE);
							TAG::APE(pTagData, state);
						break;

						case LUAENUM_TAG_OGG:
							pTagData = pChannel->GetTag(BASS_TAG_OGG);
							TAG::OGG(pTagData, state);
						break;
					}
				}
				LUA->Pop();

				LUA->ReferencePush( iTableRef );
				LUA->ReferenceFree( iTableRef );

				return 1;
			}

			LUA_FUNCTION(GetFileName)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushString("");
					return 1;
				}

				LUA->PushString(pChannel->GetFileName());
				return 1;
			}

			LUA_FUNCTION(GetFileFormat)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushString("");
					return 1;
				}

				LUA->PushString(pChannel->GetFileFormat());
				return 1;
			}


			LUA_FUNCTION(GetSamplingRate)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				LUA->PushNumber(pChannel->GetSamplingRate());
				return 1;
			}

			LUA_FUNCTION(GetBitsPerSample)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				LUA->PushNumber(pChannel->GetBitsPerSample());
				return 1;
			}

			LUA_FUNCTION(GetAverageBitRate)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(0);
					return 1;
				}

				LUA->PushNumber(pChannel->GetAverageBitRate());
				return 1;
			}

			LUA_FUNCTION(GetPos)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);

				BASS_3DVECTOR vBassPos(0, 0, 0);
				BASS_3DVECTOR vBassDir(0, 0, 0);
				BASS_3DVECTOR vBassVel(0, 0, 0);

				if(ISNULLPTR(pChannel))
				{
					pChannel->GetPos(&vBassPos, &vBassDir, &vBassVel);
				}

				if(!ISNIL(2)) // Allow recycling
				{
					LUA->CheckType(2, Type::VECTOR);

					Vector* pvPos = GETVECTOR(2);
					pvPos->x = vBassPos.x;
					pvPos->y = -vBassPos.y;
					pvPos->z = vBassPos.z;

					LUA->Push(2);
				}
				else
				{
					LUAINTERFACE::PushVector(state, &vBassPos);
				}

				if(!ISNIL(3)) // Allow recycling
				{
					LUA->CheckType(3, Type::VECTOR);

					Vector* pvDir = GETVECTOR(3);
					pvDir->x = vBassDir.x;
					pvDir->y = -vBassDir.y;
					pvDir->z = vBassDir.z;

					LUA->Push(3);
				}
				else
				{
					LUAINTERFACE::PushVector(state, &vBassDir);
				}

				if(!ISNIL(4)) // Allow recycling
				{
					LUA->CheckType(4, Type::VECTOR);

					Vector* pvVel = GETVECTOR(4);
					pvVel->x = vBassVel.x;
					pvVel->y = -vBassVel.y;
					pvVel->z = vBassVel.z;

					LUA->Push(4);
				}
				else
				{
					LUAINTERFACE::PushVector(state, &vBassVel);
				}

				return 3;
			}

			LUA_FUNCTION(SetPos)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::VECTOR);

				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				BASS_3DVECTOR vBassPos(0, 0, 0);
				BASS_3DVECTOR vBassDir(0, 0, 0);
				BASS_3DVECTOR vBassVel(0, 0, 0);

				if(!ISNIL(2))
				{
					LUA->CheckType(2, Type::VECTOR);

					Vector* pvPos = GETVECTOR(2);
					vBassPos.x = pvPos->x;
					vBassPos.y = -pvPos->y;
					vBassPos.z = pvPos->z;
				}

				if(!ISNIL(3))
				{
					LUA->CheckType(3, Type::VECTOR);

					Vector* pvDir = GETVECTOR(3);
					vBassDir.x = pvDir->x;
					vBassDir.y = -pvDir->y;
					vBassDir.z = pvDir->z;
				}

				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::VECTOR);

					Vector* pvVel = GETVECTOR(4);
					vBassVel.x = pvVel->x;
					vBassVel.y = -pvVel->y;
					vBassVel.z = pvVel->z;
				}
				pChannel->SetPos(&vBassPos, &vBassDir, &vBassVel);

				return 0;
			}


			LUA_FUNCTION(Get3DFadeDistance)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);

				float fMin;
				float fMax;

				if(ISNULLPTR(pChannel) || !pChannel->Get3DFadeDistance(&fMin, &fMax))
				{
					fMin = 0;	
					fMax = 0;	
				}

				LUA->PushNumber(fMin);
				LUA->PushNumber(fMax);
				return 2;
			}

			LUA_FUNCTION(Set3DFadeDistance)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				float fMin = BASS_NO_CHANGE;
				float fMax = BASS_NO_CHANGE;

				if(!ISNIL(2))
				{
					LUA->CheckType(2, Type::NUMBER);				
					fMin = (float)LUA->GetNumber(2);
				}

				if(!ISNIL(3))
				{
					LUA->CheckType(3, Type::NUMBER);				
					fMax = (float)LUA->GetNumber(3);
				}

				pChannel->Set3DFadeDistance(fMin, fMax);
				return 0;
			}


			LUA_FUNCTION(Get3DCone)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);

				DWORD iInnerAngle;
				DWORD iOuterAngle;
				float fOuterVolume;

				if(ISNULLPTR(pChannel) || !pChannel->Get3DCone(&iInnerAngle, &iOuterAngle, &fOuterVolume))
				{
					iInnerAngle = 0;	
					iOuterAngle = 0;
					fOuterVolume = 0;
				}

				LUA->PushNumber(iInnerAngle);
				LUA->PushNumber(iOuterAngle);
				LUA->PushNumber(fOuterVolume);

				return 3;
			}

			LUA_FUNCTION(Set3DCone)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				DWORD iInnerAngle = BASS_NO_CHANGE;
				DWORD iOuterAngle = BASS_NO_CHANGE;
				float fOuterVolume = BASS_NO_CHANGE;

				if(!ISNIL(2))
				{
					LUA->CheckType(2, Type::NUMBER);				
					iInnerAngle = (DWORD)LUA->GetNumber(2);
				}

				if(!ISNIL(3))
				{
					LUA->CheckType(3, Type::NUMBER);				
					iOuterAngle = (DWORD)LUA->GetNumber(3);
				}

				if(!ISNIL(4))
				{
					LUA->CheckType(4, Type::NUMBER);				
					fOuterVolume = (float)LUA->GetNumber(4);
				}

				pChannel->Set3DCone(iInnerAngle, iOuterAngle, fOuterVolume);
				return 0;
			}

			LUA_FUNCTION(Get3DEnabled)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if (ISNULLPTR(pChannel))
				{
					LUA->PushBool(false);
					return 1;
				}

				LUA->PushBool(pChannel->Get3DEnabled());
				return 1;
			}

			LUA_FUNCTION(Set3DEnabled)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if (ISNULLPTR(pChannel)) return 0;

				bool bEnabled = false;

				if (!ISNIL(2))
				{
					LUA->CheckType(2, Type::BOOL);
					bEnabled = LUA->GetBool(2);
				}

				pChannel->Set3DEnabled(bEnabled);
				return 0;
			}

			LUA_FUNCTION(GetEAXmix)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel))
				{
					LUA->PushNumber(BASS_AUTO);
					return 1;
				}

				float iMix = pChannel->GetEAXMix();
				LUA->PushNumber(iMix);
				return 1;
			}

			LUA_FUNCTION(SetEAXmix)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(ISNULLPTR(pChannel)) return 0;

				float iMix = BASS_AUTO;

				if(!ISNIL(2))
				{
					LUA->CheckType(2, Type::NUMBER);				
					iMix = (float)LUA->GetNumber(2);
				}

				pChannel->SetEAXMix(iMix);
				return 0;
			}
		}
	}
}