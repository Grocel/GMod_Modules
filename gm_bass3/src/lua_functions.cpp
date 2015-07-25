#include <cstring>
#include <string>
#include <algorithm>

#include "lua_functions.h"
#include "lua.h"
#include "util.h"
#include "classes/tchannel.h"

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
	if(sLuaFlag == NULL) return eFlags | BASS_STREAM_BLOCK;

	string sBuf = string(sLuaFlag) + string(" ");
	if(sBuf == " ") return eFlags | BASS_STREAM_BLOCK;

	transform(sBuf.begin(), sBuf.end(), sBuf.begin(), ::tolower);

	string sFind = "";
	size_t posSpace = sBuf.find(" ");

	while(posSpace != string::npos)
	{
        std::string sFind = sBuf.substr(0, posSpace);
		sBuf = sBuf.substr(posSpace + 1);

		if(sFind == "" || sFind == " ")
		{
			posSpace = sBuf.find(" ");
			continue;
		}

		if(g_CLIENT && (sFind == "3d"))
		{
			eFlags |= (BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX | BASS_SAMPLE_MONO); // 3D needs mono sound

			posSpace = sBuf.find(" ");
			continue;
		}

		if(sFind == "loop")
		{
			eFlags |= BASS_SAMPLE_LOOP;

			posSpace = sBuf.find(" ");
			continue;
		}

		if(sFind == "mono")
		{
			eFlags |= BASS_SAMPLE_MONO;

			posSpace = sBuf.find(" ");
			continue;
		}

		if(sFind == "noblock")
		{
			bNoBlock = true;

			posSpace = sBuf.find(" ");
			continue;
		}

		if(sFind == "noplay")
		{
			bNoPlay = true;

			posSpace = sBuf.find(" ");
			continue;
		}

        posSpace = sBuf.find(" ");
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
		if(g_pListPendingCallbacks == NULL) return 0;

		g_IntialTickHappend = true;

		while(g_pListPendingCallbacks->getSize() > 0)
		{
			TChannelCallbackData* pCallbackData = g_pListPendingCallbacks->remove();
			if(pCallbackData == NULL) continue;

			bool bIsRecycledChannel = pCallbackData->bIsRecycledChannel;
			int iCallbackRef = pCallbackData->iCallbackRef;
			int iError = pCallbackData->iError;
			TChannel* pChannel = pCallbackData->pChannel;
			bool bNoPlay = pCallbackData->bNoPlay;

			delete pCallbackData;

			if(iError == BASS_OK && pChannel == NULL) iError = BASS_ERROR_UNKNOWN;

			if(!bIsRecycledChannel && iError != BASS_OK)
			{
				LUAINTERFACE::DeleteChannel(pChannel);
				pChannel = NULL;
			}

			if(pChannel != NULL)
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
				if(pChannel == NULL)
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
				if(pChannel == NULL) return 0;

				pChannel->Remove();
				return 0;
			}

			LUA_FUNCTION(IsValid)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL)
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
				if(pChannel == NULL) return 0;

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
				if(pChannel == NULL) return 0;

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
				if(pChannel == NULL) return 0;

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
				if(pChannel == NULL) return 0;
				pChannel->Stop();

				return 0;
			}

			LUA_FUNCTION(Pause)
			{
				LUA->CheckType(1, TYPE_CHANNEL);			

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;
				pChannel->Pause();

				return 0;
			}

			LUA_FUNCTION(GetState)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushNumber(SetChannelStateFlag(pChannel->GetState()));
				return 1;
			}

			LUA_FUNCTION(SetVolume)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				pChannel->SetVolume((float)LUA->GetNumber(2));
				return 0;
			}

			LUA_FUNCTION(GetVolume)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushNumber(pChannel->GetVolume());
				return 1;
			}

			LUA_FUNCTION(VolumeFadeTo)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);
				LUA->CheckType(3, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				DWORD iIime = (DWORD)(LUA->GetNumber(3) * 1000);

				pChannel->VolumeFadeTo((float)LUA->GetNumber(2), iIime);
				return 0;
			}

			LUA_FUNCTION(VolumeIsFading)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->VolumeIsFading());
				return 1;
			}

			LUA_FUNCTION(SetBalance)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				pChannel->SetBalance((float)LUA->GetNumber(2));
				return 0;
			}

			LUA_FUNCTION(GetBalance)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushNumber(pChannel->GetBalance());
				return 1;
			}

			LUA_FUNCTION(BalanceFadeTo)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);
				LUA->CheckType(3, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				DWORD iIime = (DWORD)(LUA->GetNumber(3) * 1000);

				pChannel->BalanceFadeTo((float)LUA->GetNumber(2), iIime);
				return 0;
			}

			LUA_FUNCTION(BalanceIsFading)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->BalanceIsFading());
				return 1;
			}

			LUA_FUNCTION(Is3D)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->Is3D());
				return 1;
			}

			LUA_FUNCTION(IsLooping)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->IsLooping());
				return 1;
			}

			LUA_FUNCTION(IsOnline)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->IsOnline());
				return 1;
			}

			LUA_FUNCTION(IsBlockStreamed)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->IsBlockStreamed());
				return 1;
			}


			LUA_FUNCTION(EnableLooping)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::BOOL);

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

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
				if(pChannel == NULL) return 0;
				if(g_pfFFTBuffer == NULL) return 0;

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
					for (i = 0; i < iCount; i += iSkip)
					{
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
						fTmp = g_pfFFTBuffer[i];
						if(fTmp < 0) fTmp = 0;
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
				if(pChannel == NULL) return 0;
				if(g_pfFFTBuffer == NULL) return 0;

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
				if(pChannel == NULL) return 0;
				
				WORD iLevelLeft = 0;
				WORD iLevelRight = 0;
				pChannel->GetLevel(&iLevelLeft, &iLevelRight);

				LUA->PushNumber((double)(iLevelLeft) / 0x7FFF);
				LUA->PushNumber((double)(iLevelRight) / 0x7FFF);
				return 2;
			}


			LUA_FUNCTION(GetTime)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushNumber(pChannel->GetTime());
				return 1;
			}

			LUA_FUNCTION(SetTime)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				pChannel->SetTime(LUA->GetNumber(2));
				return 0;
			}


			LUA_FUNCTION(GetLength)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushNumber(pChannel->GetLength());
				return 1;
			}

			LUA_FUNCTION(IsEndless)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->IsEndless());
				return 1;
			}


			LUA_FUNCTION(IsSeeking)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushBool(pChannel->IsSeeking());
				return 1;
			}


			LUA_FUNCTION(GetTag)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				LUA->CheckType(2, Type::NUMBER);

				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				unsigned int iLuaFlag = (unsigned int)LUA->GetNumber(2);
				int iTableRef = 0;
				const char* pTagData = NULL;
				const char* pTagDataTemp = NULL;
				bool bIsStringlist = false;

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

				switch (iLuaFlag)
				{
					case LUAENUM_TAG_META:
					{
						char cTmp[1024];
						cTmp[0] = 0;
						cTmp[1023] = 0;

						pTagData = pChannel->GetTag(BASS_TAG_META);
						pTagDataTemp = pTagData;

						if (pTagData != NULL)
						{
							pTagDataTemp=strstr(pTagData, "StreamTitle='"); // look for title
							if (pTagDataTemp != NULL)
							{ // found it, copy it...
								UTIL::safe_cpy(cTmp, pTagDataTemp + 13, 1024);
								strchr(cTmp, ';')[-1] = 0;
							}

							cTmp[1023] = 0;
							LUA->PushString( "StreamTitle" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
							cTmp[0] = 0;

							pTagDataTemp=strstr(pTagData, "StreamUrl='"); // look for url
							if (pTagDataTemp != NULL)
							{ // found it, copy it...
								UTIL::safe_cpy(cTmp, pTagDataTemp + 11, 1024);
								strchr(cTmp, ';')[-1] = 0;
							}

							cTmp[1023] = 0;
							LUA->PushString( "StreamUrl" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
							cTmp[0] = 0;
						}
						else
						{
							cTmp[0] = 0;
							LUA->PushString( "StreamTitle" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );

							LUA->PushString( "StreamUrl" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
						}
					}
					break;

					case LUAENUM_TAG_ID3:
					{
						pTagData = pChannel->GetTag(BASS_TAG_ID3);
						pTagDataTemp = pTagData;

						const TAG_ID3* pTagID3 = (const TAG_ID3*)pTagData;

						char cTmp[64];
						cTmp[0] = 0;
						cTmp[63] = 0;


						if (pTagID3 != NULL)
						{
							LUA->PushString( "id" ); 
							LUA->PushNumber( atoi(pTagID3->id) );
							LUA->SetTable( -3 );

							LUA->PushString( "year" ); 
							LUA->PushNumber( atoi(pTagID3->year) );
							LUA->SetTable( -3 );

							LUA->PushString( "genre" ); 
							LUA->PushNumber( pTagID3->genre );
							LUA->SetTable( -3 );

							UTIL::safe_cpy(cTmp, pTagID3->title, 64);
							cTmp[63] = 0;
							LUA->PushString( "title" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
							cTmp[0];

							UTIL::safe_cpy(cTmp, pTagID3->artist, 64);
							cTmp[63] = 0;
							LUA->PushString( "artist" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
							cTmp[0];

							UTIL::safe_cpy(cTmp, pTagID3->album, 64);
							cTmp[63] = 0;
							LUA->PushString( "album" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
							cTmp[0];


							UTIL::safe_cpy(cTmp, pTagID3->comment, 64);
							cTmp[63] = 0;
							LUA->PushString( "comment" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
							cTmp[0];
						}
						else
						{
							cTmp[0] = 0;

							LUA->PushString( "id" ); 
							LUA->PushNumber( 0 );
							LUA->SetTable( -3 );

							LUA->PushString( "year" ); 
							LUA->PushNumber( 0 );
							LUA->SetTable( -3 );

							LUA->PushString( "genre" ); 
							LUA->PushNumber( 0 );
							LUA->SetTable( -3 );

							LUA->PushString( "title" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );

							LUA->PushString( "artist" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );

							LUA->PushString( "album" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );

							LUA->PushString( "comment" ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
						}
					}
					break;

					case LUAENUM_TAG_HTTP:
						pTagData = pChannel->GetTag(BASS_TAG_HTTP);
						bIsStringlist = true;
					break;

					case LUAENUM_TAG_ICY:
						pTagData = pChannel->GetTag(BASS_TAG_ICY);
						bIsStringlist = true;
					break;

					case LUAENUM_TAG_MF:
						pTagData = pChannel->GetTag(BASS_TAG_MF);
						bIsStringlist = true;
					break;

					case LUAENUM_TAG_MP4:
						pTagData = pChannel->GetTag(BASS_TAG_MP4);
						bIsStringlist = true;
					break;

					case LUAENUM_TAG_APE:
						pTagData = pChannel->GetTag(BASS_TAG_APE);
						bIsStringlist = true;
					break;

					case LUAENUM_TAG_OGG:
						pTagData = pChannel->GetTag(BASS_TAG_OGG);
						bIsStringlist = true;
					break;
				}

				pTagDataTemp = pTagData;
				if(bIsStringlist)
				{
					if (pTagDataTemp != NULL)
					{
						unsigned int i = 0;
						char cTmp[1024];
						cTmp[0] = 0;
						cTmp[1023] = 0;

						// Getting a list of null-terminated strings
						while (*pTagDataTemp)
						{
							i++;
							UTIL::safe_cpy(cTmp, pTagDataTemp, 1024);

							cTmp[1023] = 0;
							LUA->PushNumber( i ); 
							LUA->PushString( cTmp );
							LUA->SetTable( -3 );
							cTmp[0];

							pTagDataTemp += strlen(pTagDataTemp) + 1; // move on to next string
						}
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
				if(pChannel == NULL) return 0;

				LUA->PushString(pChannel->GetFileName());
				return 1;
			}

			LUA_FUNCTION(GetFileFormat)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushString(pChannel->GetFileFormat());
				return 1;
			}


			LUA_FUNCTION(GetSamplingRate)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushNumber(pChannel->GetSamplingRate());
				return 1;
			}

			LUA_FUNCTION(GetBitsPerSample)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				LUA->PushNumber(pChannel->GetBitsPerSample());
				return 1;
			}

			LUA_FUNCTION(GetPos)
			{
				LUA->CheckType(1, TYPE_CHANNEL);
				TChannel* pChannel = GETCHANNEL(1);
				if(pChannel == NULL) return 0;

				BASS_3DVECTOR vBassPos(0, 0, 0);
				BASS_3DVECTOR vBassDir(0, 0, 0);
				BASS_3DVECTOR vBassVel(0, 0, 0);

				pChannel->GetPos(&vBassPos, &vBassDir, &vBassVel);

				//int iRef = 0;

				if(!ISNIL(2)) // Allow recycling
				{
					LUA->CheckType(2, Type::VECTOR);

					Vector* pvPos = GETVECTOR(2);
					pvPos->x = vBassPos.x;
					pvPos->y = -vBassPos.y;
					pvPos->z = vBassPos.z;

					LUA->Push(2);
					//iRef = LUA->ReferenceCreate();
					//LUA->Pop();

					//LUA->ReferencePush(iRef);
					//LUA->ReferenceFree(iRef);
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
					//iRef = LUA->ReferenceCreate();
					//LUA->Pop();

					//LUA->ReferencePush(iRef);
					//LUA->ReferenceFree(iRef);
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
					//iRef = LUA->ReferenceCreate();
					//LUA->Pop();

					//LUA->ReferencePush(iRef);
					//LUA->ReferenceFree(iRef);
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
				if(pChannel == NULL) return 0;

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
				if(pChannel == NULL) return 0;

				float fMin;
				float fMax;

				if(!pChannel->Get3DFadeDistance(&fMin, &fMax))
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
				if(pChannel == NULL) return 0;

				float fMin;
				float fMax;

				if(ISNIL(2))
				{
					fMin = -1;
				}
				else
				{
					LUA->CheckType(2, Type::NUMBER);				
					fMin = (float)LUA->GetNumber(2);
				}

				if(ISNIL(3))
				{
					fMax = -1;
				}
				else
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
				if(pChannel == NULL) return 0;

				DWORD iInnerAngle;
				DWORD iOuterAngle;
				float fOuterVolume;

				if(!pChannel->Get3DCone(&iInnerAngle, &iOuterAngle, &fOuterVolume))
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
				if(pChannel == NULL) return 0;

				DWORD iInnerAngle;
				DWORD iOuterAngle;
				float fOuterVolume;

				if(ISNIL(2))
				{
					iInnerAngle = -1;
				}
				else
				{
					LUA->CheckType(2, Type::NUMBER);				
					iInnerAngle = (DWORD)LUA->GetNumber(2);
				}

				if(ISNIL(3))
				{
					iOuterAngle = -1;
				}
				else
				{
					LUA->CheckType(3, Type::NUMBER);				
					iOuterAngle = (DWORD)LUA->GetNumber(3);
				}

				if(ISNIL(4))
				{
					fOuterVolume = -1;
				}
				else
				{
					LUA->CheckType(4, Type::NUMBER);				
					fOuterVolume = (float)LUA->GetNumber(4);
				}

				pChannel->Set3DCone(iInnerAngle, iOuterAngle, fOuterVolume);
				return 0;
			}
		}
	}
}