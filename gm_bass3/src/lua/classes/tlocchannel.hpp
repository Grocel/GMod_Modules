#include "tluaobjectcontainer.hpp"
#include "../../classes/tchannel.hpp"

#include "../../classes/teffect/teffectchorus.hpp"
#include "../../classes/teffect/teffectcompressor.hpp"
#include "../../classes/teffect/teffectdistortion.hpp"
#include "../../classes/teffect/teffectecho.hpp"
#include "../../classes/teffect/teffectflanger.hpp"
#include "../../classes/teffect/teffectgargle.hpp"
#include "../../classes/teffect/teffecti3dl2reverb.hpp"
#include "../../classes/teffect/teffectparameq.hpp"
#include "../../classes/teffect/teffectreverb.hpp"
#include "../../classes/teffect/teffectvolume.hpp"

#include "../../util.hpp"
#include "../../tag.hpp"
#include "../../flag.hpp"

#ifndef T_LOCCHANNEL_H
#define T_LOCCHANNEL_H

namespace LUAINTERFACE
{
	class TLOCChannel : public TLuaObjectContainer
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);

			this->AddMethod("PlayFile", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				LUAINTERFACE::HELPER::Check<T>(pLUA, 1);
				pLUA->CheckType(2, Type::STRING);
				if (!pLUA->IsType(3, Type::NUMBER) && !pLUA->IsType(3, Type::STRING)) pLUA->CheckType(3, Type::NUMBER);

				int iCallbackRef = 0;
				if (!LUAINTERFACE::IsNil(pLUA, 4))
				{
					pLUA->CheckType(4, Type::FUNCTION);
					pLUA->Push(4);
					iCallbackRef = pLUA->ReferenceCreate();
				}

				TChannel* pOBJ = LUAINTERFACE::HELPER::Get<T>(pLUA, 1);
				if (ISNULLPTR(pOBJ))
				{
					pLUA->PushBool(false);
					return 1;
				}

				string sURL = string(pLUA->GetString(2));

				TChannelThreadArgs ThreadArgs;
				ThreadArgs.bIsOnline = false;

				if (pLUA->IsType(3, Type::NUMBER))
				{
					ThreadArgs.eFlags = FLAG::GetPlayFlags((unsigned int)pLUA->GetNumber(3), ThreadArgs.bNoPlay);
				}
				else
				{
					ThreadArgs.eFlags = FLAG::GetPlayFlags((const char*)pLUA->GetString(3), ThreadArgs.bNoPlay);
				}

				ThreadArgs.iCallbackRef = iCallbackRef;
				ThreadArgs.pChannel = pOBJ;
				ThreadArgs.sURL = sURL;

				pLUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
				return 1;
			});

			this->AddMethod("PlayURL", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				LUAINTERFACE::HELPER::Check<T>(pLUA, 1);
				pLUA->CheckType(2, Type::STRING);
				if (!pLUA->IsType(3, Type::NUMBER) && !pLUA->IsType(3, Type::STRING)) pLUA->CheckType(3, Type::NUMBER);

				int iCallbackRef = 0;
				if (!LUAINTERFACE::IsNil(pLUA, 4))
				{
					pLUA->CheckType(4, Type::FUNCTION);
					pLUA->Push(4);
					iCallbackRef = pLUA->ReferenceCreate();
				}

				TChannel* pOBJ = LUAINTERFACE::HELPER::Get<T>(pLUA, 1);
				if (ISNULLPTR(pOBJ))
				{
					pLUA->PushBool(false);
					return 1;
				}

				string sURL = string(pLUA->GetString(2));

				TChannelThreadArgs ThreadArgs;
				ThreadArgs.bIsOnline = true;

				if (pLUA->IsType(3, Type::NUMBER))
				{
					ThreadArgs.eFlags = FLAG::GetPlayFlags((unsigned int)pLUA->GetNumber(3), ThreadArgs.bNoPlay);
				}
				else
				{
					ThreadArgs.eFlags = FLAG::GetPlayFlags((const char*)pLUA->GetString(3), ThreadArgs.bNoPlay);
				}

				ThreadArgs.iCallbackRef = iCallbackRef;
				ThreadArgs.pChannel = pOBJ;
				ThreadArgs.sURL = sURL;

				pLUA->PushBool(UTIL::LoadStream(ThreadArgs, false));
				return 1;
			});

			this->AddMethod("Play", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

				if (LUAINTERFACE::IsNil(pLUA, 2))
				{
					pOBJ->Play();
					return 0;
				}

				pLUA->CheckType(2, Type::BOOL);
				pOBJ->Play(pLUA->GetBool(2));
				return 0;
			});

			this->AddMethod("Stop", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pOBJ->Stop();

				return 0;
			});

			this->AddMethod("Pause", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pOBJ->Pause();

				return 0;
			});

			this->AddMethod("GetState", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				bass_flag iBassFlag = BASS_NULL;

				switch (pOBJ->GetState())
				{
					case BASS_ACTIVE_STOPPED: iBassFlag = LUAENUM_CHANNEL_STOPPED; break;
					case BASS_ACTIVE_PLAYING: iBassFlag = LUAENUM_CHANNEL_PLAYING; break;
					case BASS_ACTIVE_PAUSED:  iBassFlag = LUAENUM_CHANNEL_PAUSED; break;
					case BASS_ACTIVE_STALLED: iBassFlag = LUAENUM_CHANNEL_STALLED; break;
				}

				pLUA->PushNumber(iBassFlag);
				return 1;
			});

			this->AddMethod("FFT", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::TABLE);
				pLUA->CheckType(3, Type::NUMBER);

				bool bRemoveDCbias = false;
				bool bRemoveHannWindow = false;
				bool bToDecibel = false;

				if (!LUAINTERFACE::IsNil(pLUA, 4))
				{
					pLUA->CheckType(4, Type::BOOL);
					bRemoveDCbias = pLUA->GetBool(4);
				}

				if (!LUAINTERFACE::IsNil(pLUA, 5))
				{
					pLUA->CheckType(5, Type::BOOL);
					bRemoveHannWindow = pLUA->GetBool(5);
				}

				if (!LUAINTERFACE::IsNil(pLUA, 6))
				{
					pLUA->CheckType(6, Type::BOOL);
					bToDecibel = pLUA->GetBool(6);
				}

				if (ISNULLPTR(g_pfFFTBuffer))
				{
					pLUA->PushNumber(0);
					return 1;
				}

				bass_flag iFlag = BASS_NULL;
				unsigned int iCount = 0;
				unsigned int iSkip = 1;
				unsigned int iLuaFlag = (unsigned int)pLUA->GetNumber(3);

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

					default: pLUA->PushNumber(0); return 1;
				}

				pLUA->Push(2);
				int iTableRef = pLUA->ReferenceCreate();

				float fTmp;

				unsigned int i;
				unsigned int iIndex = 1;

				if (bRemoveDCbias) iFlag |= BASS_DATA_FFT_REMOVEDC;
				if (bRemoveHannWindow) iFlag |= BASS_DATA_FFT_NOWINDOW;

				pLUA->ReferencePush(iTableRef);
				if (!pOBJ->FFT(iFlag, g_pfFFTBuffer))
				{
					if (bToDecibel)
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							pLUA->PushNumber(iIndex);
							pLUA->PushNumber(BASS_MIN_DECIBELS);
							pLUA->SetTable(-3);

							iIndex++;
						}
					}
					else
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							pLUA->PushNumber(iIndex);
							pLUA->PushNumber(0);
							pLUA->SetTable(-3);

							iIndex++;
						}
					}
				}
				else
				{
					if (bToDecibel)
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							fTmp = g_pfFFTBuffer[i];
							fTmp = ((fTmp > 0) ? 20 * log10(fTmp) : BASS_MIN_DECIBELS);
							if (fTmp < BASS_MIN_DECIBELS) fTmp = BASS_MIN_DECIBELS;

							pLUA->PushNumber(iIndex);
							pLUA->PushNumber(fTmp);
							pLUA->SetTable(-3);

							iIndex++;
						}
					}
					else
					{
						for (i = 0; i < iCount; i += iSkip)
						{
							fTmp = g_pfFFTBuffer[i];
							if (fTmp < 0) fTmp = 0;
							if (fTmp > 1) fTmp = 1;

							pLUA->PushNumber(iIndex);
							pLUA->PushNumber(fTmp);
							pLUA->SetTable(-3);

							iIndex++;
						}
					}
				}
				pLUA->Pop();

				pLUA->PushNumber(iIndex - 1);
				pLUA->ReferenceFree(iTableRef);

				return 1;
			});

			this->AddMethod("FFTComplex", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::TABLE);
				pLUA->CheckType(3, Type::NUMBER);

				bool bRemoveDCbias = false;
				bool bRemoveHannWindow = false;
				bool bBothSides = false;

				if (!LUAINTERFACE::IsNil(pLUA, 4))
				{
					pLUA->CheckType(4, Type::BOOL);
					bRemoveDCbias = pLUA->GetBool(4);
				}

				if (!LUAINTERFACE::IsNil(pLUA, 5))
				{
					pLUA->CheckType(5, Type::BOOL);
					bRemoveHannWindow = pLUA->GetBool(5);
				}

				if (!LUAINTERFACE::IsNil(pLUA, 6))
				{
					pLUA->CheckType(6, Type::BOOL);
					bBothSides = pLUA->GetBool(6);
				}

				if (ISNULLPTR(g_pfFFTBuffer))
				{
					pLUA->PushNumber(0);
					return 1;
				}

				bass_flag iFlag = BASS_NULL;
				unsigned int iCount = 0;
				unsigned int iSkip = 2;
				unsigned int iLuaFlag = (unsigned int)pLUA->GetNumber(3);

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

					default: pLUA->PushNumber(0); return 1;
				}

				iSkip *= 2;
				iCount *= (bBothSides ? 4 : 2);

				pLUA->Push(2);
				int iTableRef = pLUA->ReferenceCreate();

				float fTmp;
				unsigned int iIndex = 1;
				unsigned int i;

				if (bRemoveDCbias) iFlag |= BASS_DATA_FFT_REMOVEDC;
				if (bRemoveHannWindow) iFlag |= BASS_DATA_FFT_NOWINDOW;

				pLUA->ReferencePush(iTableRef);
				if (!pOBJ->FFT(iFlag | BASS_DATA_FFT_COMPLEX, g_pfFFTBuffer))
				{
					for (i = 0; i < iCount; i += iSkip)
					{
						pLUA->PushNumber(iIndex);
						pLUA->PushNumber(0);
						pLUA->SetTable(-3);

						iIndex++;

						pLUA->PushNumber(iIndex);
						pLUA->PushNumber(0);
						pLUA->SetTable(-3);

						iIndex++;
					}
				}
				else
				{
					for (i = 0; i < iCount; i += iSkip)
					{
						// Real Part 
						fTmp = g_pfFFTBuffer[i];
						if (fTmp < -1) fTmp = -1;
						if (fTmp > 1) fTmp = 1;

						pLUA->PushNumber(iIndex);
						pLUA->PushNumber(fTmp);
						pLUA->SetTable(-3);

						iIndex++;

						// Imaginary Part 
						fTmp = g_pfFFTBuffer[i + 1];
						if (fTmp < -1) fTmp = -1;
						if (fTmp > 1) fTmp = 1;

						pLUA->PushNumber(iIndex);
						pLUA->PushNumber(fTmp);
						pLUA->SetTable(-3);

						iIndex++;
					}
				}
				pLUA->Pop();

				pLUA->PushNumber(iIndex - 1);
				pLUA->ReferenceFree(iTableRef);
				return 1;
			});

			this->AddMethod("GetLevel", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

				bool bRMS = false;
				bool bToDecibel = false;
				float fTimeFrame = 0.02;

				if (!LUAINTERFACE::IsNil(pLUA, 2))
				{
					pLUA->CheckType(2, Type::NUMBER);
					fTimeFrame = (float)pLUA->GetNumber(2);
				}

				if (!LUAINTERFACE::IsNil(pLUA, 3))
				{
					pLUA->CheckType(3, Type::BOOL);
					bRMS = pLUA->GetBool(4);
				}

				if (!LUAINTERFACE::IsNil(pLUA, 4))
				{
					pLUA->CheckType(4, Type::BOOL);
					bToDecibel = pLUA->GetBool(4);
				}

				float fTmp;
				unsigned int i;

				if (ISNULLPTR(pOBJ) || !pOBJ->GetLevelEx(g_pfFFTBuffer, fTimeFrame, bRMS))
				{
					if (bToDecibel)
					{
						for (i = 0; i < 2; i++)
						{
							pLUA->PushNumber(BASS_MIN_DECIBELS);
						}
					}
					else
					{
						for (i = 0; i < 2; i++)
						{
							pLUA->PushNumber(0);
						}
					}

					return 2;
				}

				if (bToDecibel)
				{
					for (i = 0; i < 2; i++)
					{
						fTmp = g_pfFFTBuffer[i];
						fTmp = ((fTmp > 0) ? 20 * log10(fTmp) : BASS_MIN_DECIBELS);
						if (fTmp < BASS_MIN_DECIBELS) fTmp = BASS_MIN_DECIBELS;

						pLUA->PushNumber(fTmp);
					}
				}
				else
				{
					for (i = 0; i < 2; i++)
					{
						fTmp = g_pfFFTBuffer[i];
						if (fTmp < 0) fTmp = 0;
						if (fTmp > 1) fTmp = 1;

						pLUA->PushNumber(fTmp);
					}
				}

				return 2;
			});

			this->AddMethod("GetTag", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::NUMBER);

				unsigned int iLuaFlag = (unsigned int)pLUA->GetNumber(2);
				int iTableRef = 0;
				const char* pTagData = NULL;

				if (LUAINTERFACE::IsNil(pLUA, 3))
				{
					pLUA->CreateTable();
				}
				else // Allow recycling
				{
					pLUA->CheckType(3, Type::TABLE);
					pLUA->Push(3);
				}

				iTableRef = pLUA->ReferenceCreate();
				pLUA->ReferencePush(iTableRef);

				if (!ISNULLPTR(pOBJ))
				{
					switch (iLuaFlag)
					{
					case LUAENUM_TAG_META:
						pTagData = pOBJ->GetTag(BASS_TAG_META);
						TAG::META(pTagData, pLUA);
						break;

					case LUAENUM_TAG_ID3:
						pTagData = pOBJ->GetTag(BASS_TAG_ID3);
						TAG::ID3(pTagData, pLUA);
						break;

					case LUAENUM_TAG_HTTP:
						pTagData = pOBJ->GetTag(BASS_TAG_HTTP);
						TAG::HTTP(pTagData, pLUA);
						break;

					case LUAENUM_TAG_ICY:
						pTagData = pOBJ->GetTag(BASS_TAG_ICY);
						TAG::ICY(pTagData, pLUA);
						break;

					case LUAENUM_TAG_MF:
						pTagData = pOBJ->GetTag(BASS_TAG_MF);
						TAG::MF(pTagData, pLUA);
						break;

					case LUAENUM_TAG_MP4:
						pTagData = pOBJ->GetTag(BASS_TAG_MP4);
						TAG::MP4(pTagData, pLUA);
						break;

					case LUAENUM_TAG_APE:
						pTagData = pOBJ->GetTag(BASS_TAG_APE);
						TAG::APE(pTagData, pLUA);
						break;

					case LUAENUM_TAG_OGG:
						pTagData = pOBJ->GetTag(BASS_TAG_OGG);
						TAG::OGG(pTagData, pLUA);
						break;
					}
				}
				pLUA->Pop();

				pLUA->ReferencePush(iTableRef);
				pLUA->ReferenceFree(iTableRef);

				return 1;
			});

			this->AddMethod("GetFileName", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->PushString(pOBJ->GetFileName().c_str());
				return 1;
			});

			this->AddMethod("GetFileFormat", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->PushString(pOBJ->GetFileFormat().c_str());
				return 1;
			});

			if (g_CLIENT)
			{
				this->AddMethod("SetVolumeBoost", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
					pLUA->CheckType(2, Type::NUMBER);
					Warning("IBASS3Channel:SetVolumeBoost() is deprecated and will be removed by May 2018. Use IBASS3Channel:SetVolume(volume > 1) instead!\n");

					pOBJ->SetVolumeBoost((float)pLUA->GetNumber(2));
					return 0;
				});

				this->AddMethod("GetVolumeBoost", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
					Warning("IBASS3Channel:GetVolumeBoost() is deprecated and will be removed along with IBASS3Channel:SetVolumeBoost() by May 2018. Use IBASS3Channel:GetVolume() instead!\n");

					pLUA->PushNumber(pOBJ->GetVolumeBoost());
					return 1;
				});

				this->AddMethod("VolumeFadeTo", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
					pLUA->CheckType(2, Type::NUMBER);
					pLUA->CheckType(3, Type::NUMBER);

					DWORD iIime = (DWORD)(pLUA->GetNumber(3) * 1000);
					pOBJ->VolumeFadeTo((float)pLUA->GetNumber(2), iIime);

					return 0;
				});

				this->AddMethod("BalanceFadeTo", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
					pLUA->CheckType(2, Type::NUMBER);
					pLUA->CheckType(3, Type::NUMBER);

					DWORD iIime = (DWORD)(pLUA->GetNumber(3) * 1000);
					pOBJ->BalanceFadeTo((float)pLUA->GetNumber(2), iIime);

					return 0;
				});

				this->AddMethod("GetPos", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

					BASS_3DVECTOR vBassPos(0, 0, 0);
					BASS_3DVECTOR vBassDir(0, 0, 0);
					BASS_3DVECTOR vBassVel(0, 0, 0);

					pOBJ->GetPos(&vBassPos, &vBassDir, &vBassVel);

					if (!LUAINTERFACE::IsNil(pLUA, 2)) // Allow recycling
					{
						pLUA->CheckType(2, Type::VECTOR);

						Vector pvPos = pLUA->GetVector(2);
						pvPos.x = vBassPos.x;
						pvPos.y = -vBassPos.y;
						pvPos.z = vBassPos.z;

						pLUA->Push(2);
					}
					else
					{
						LUAINTERFACE::PushVector(pLUA, &vBassPos);
					}

					if (!LUAINTERFACE::IsNil(pLUA, 3)) // Allow recycling
					{
						pLUA->CheckType(3, Type::VECTOR);

						Vector pvDir = pLUA->GetVector(3);
						pvDir.x = vBassDir.x;
						pvDir.y = -vBassDir.y;
						pvDir.z = vBassDir.z;

						pLUA->Push(3);
					}
					else
					{
						LUAINTERFACE::PushVector(pLUA, &vBassDir);
					}

					if (!LUAINTERFACE::IsNil(pLUA, 4)) // Allow recycling
					{
						pLUA->CheckType(4, Type::VECTOR);

						Vector pvVel = pLUA->GetVector(4);
						pvVel.x = vBassVel.x;
						pvVel.y = -vBassVel.y;
						pvVel.z = vBassVel.z;

						pLUA->Push(4);
					}
					else
					{
						LUAINTERFACE::PushVector(pLUA, &vBassVel);
					}

					return 3;
				});

				this->AddMethod("SetPos", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
					pLUA->CheckType(2, Type::VECTOR);

					BASS_3DVECTOR vBassPos(0, 0, 0);
					BASS_3DVECTOR vBassDir(0, 0, 0);
					BASS_3DVECTOR vBassVel(0, 0, 0);

					if (!LUAINTERFACE::IsNil(pLUA, 2))
					{
						pLUA->CheckType(2, Type::VECTOR);

						Vector pvPos = pLUA->GetVector(2);
						vBassPos.x = pvPos.x;
						vBassPos.y = -pvPos.y;
						vBassPos.z = pvPos.z;
					}

					if (!LUAINTERFACE::IsNil(pLUA, 3))
					{
						pLUA->CheckType(3, Type::VECTOR);

						Vector pvDir = pLUA->GetVector(3);
						vBassDir.x = pvDir.x;
						vBassDir.y = -pvDir.y;
						vBassDir.z = pvDir.z;
					}

					if (!LUAINTERFACE::IsNil(pLUA, 4))
					{
						pLUA->CheckType(4, Type::VECTOR);

						Vector pvVel = pLUA->GetVector(4);
						vBassVel.x = pvVel.x;
						vBassVel.y = -pvVel.y;
						vBassVel.z = pvVel.z;
					}
					pOBJ->SetPos(&vBassPos, &vBassDir, &vBassVel);

					return 0;
				});

				this->AddMethod("Get3DFadeDistance", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

					float fMin;
					float fMax;

					if (!pOBJ->Get3DFadeDistance(&fMin, &fMax))
					{
						fMin = 0;
						fMax = 0;
					}

					pLUA->PushNumber(fMin);
					pLUA->PushNumber(fMax);
					return 2;
				});

				this->AddMethod("Set3DFadeDistance", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

					float fMin = BASS_NO_CHANGE;
					float fMax = BASS_NO_CHANGE;

					if (!LUAINTERFACE::IsNil(pLUA, 2))
					{
						pLUA->CheckType(2, Type::NUMBER);
						fMin = (float)pLUA->GetNumber(2);
					}

					if (!LUAINTERFACE::IsNil(pLUA, 3))
					{
						pLUA->CheckType(3, Type::NUMBER);
						fMax = (float)pLUA->GetNumber(3);
					}

					pOBJ->Set3DFadeDistance(fMin, fMax);
					return 0;
				});

				this->AddMethod("Get3DCone", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

					DWORD iInnerAngle;
					DWORD iOuterAngle;
					float fOuterVolume;

					if (!pOBJ->Get3DCone(&iInnerAngle, &iOuterAngle, &fOuterVolume))
					{
						iInnerAngle = 0;
						iOuterAngle = 0;
						fOuterVolume = 0;
					}

					pLUA->PushNumber(iInnerAngle);
					pLUA->PushNumber(iOuterAngle);
					pLUA->PushNumber(fOuterVolume);

					return 3;
				});

				this->AddMethod("Set3DCone", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

					DWORD iInnerAngle = BASS_NO_CHANGE;
					DWORD iOuterAngle = BASS_NO_CHANGE;
					float fOuterVolume = BASS_NO_CHANGE;

					if (!LUAINTERFACE::IsNil(pLUA, 2))
					{
						pLUA->CheckType(2, Type::NUMBER);
						iInnerAngle = (DWORD)pLUA->GetNumber(2);
					}

					if (!LUAINTERFACE::IsNil(pLUA, 3))
					{
						pLUA->CheckType(3, Type::NUMBER);
						iOuterAngle = (DWORD)pLUA->GetNumber(3);
					}

					if (!LUAINTERFACE::IsNil(pLUA, 4))
					{
						pLUA->CheckType(4, Type::NUMBER);
						fOuterVolume = (float)pLUA->GetNumber(4);
					}

					pOBJ->Set3DCone(iInnerAngle, iOuterAngle, fOuterVolume);
					return 0;
				});

				this->AddMethod("GetEAXmix", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

					float iMix = pOBJ->GetEAXMix();
					pLUA->PushNumber(iMix);
					return 1;
				});

				this->AddMethod("SetEAXmix", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					TChannel* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

					float iMix = BASS_AUTO;

					if (!LUAINTERFACE::IsNil(pLUA, 2))
					{
						pLUA->CheckType(2, Type::NUMBER);
						iMix = (float)pLUA->GetNumber(2);
					}

					pOBJ->SetEAXMix(iMix);
					return 0;
				});

				this->AddMethod("AddFx", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					LUAINTERFACE::HELPER::Check<T>(pLUA, 1);
					pLUA->CheckType(2, Type::NUMBER);

					TChannel* pOBJ = LUAINTERFACE::HELPER::Get<T>(pLUA, 1);
					bass_flag iLuaFlag = (bass_flag)pLUA->GetNumber(2);

					switch (iLuaFlag)
					{
						case LUAENUM_FX_CHORUS:
						{
							TEffectChorus* pFX = new TEffectChorus();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectChorus>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectChorus>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_COMPRESSOR:
						{
							TEffectCompressor* pFX = new TEffectCompressor();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectCompressor>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectCompressor>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_DISTORTION:
						{
							TEffectDistortion* pFX = new TEffectDistortion();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectDistortion>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectDistortion>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_ECHO:
						{
							TEffectEcho* pFX = new TEffectEcho();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectEcho>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectEcho>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_FLANGER:
						{
							TEffectFlanger* pFX = new TEffectFlanger();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectFlanger>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectFlanger>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_GARGLE:
						{
							TEffectGargle* pFX = new TEffectGargle();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectGargle>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectGargle>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_I3DL2REVERB:
						{
							TEffectI3DL2Reverb* pFX = new TEffectI3DL2Reverb();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectI3DL2Reverb>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectI3DL2Reverb>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_PARAMEQ:
						{
							TEffectParamEQ* pFX = new TEffectParamEQ();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectParamEQ>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectParamEQ>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_REVERB:
						{
							TEffectReverb* pFX = new TEffectReverb();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectReverb>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectReverb>(pLUA, pFX);
							return 1;
						}
						case LUAENUM_FX_VOLUME:
						{
							TEffectVolume* pFX = new TEffectVolume();

							if (!pOBJ->AddFx(pFX))
							{
								LUAINTERFACE::HELPER::Delete<TEffectVolume>(pFX);

								pLUA->PushNil();
								return 1;
							}

							LUAINTERFACE::HELPER::Push<TEffectVolume>(pLUA, pFX);
							return 1;
						}
					}

					pLUA->PushNil();
					return 1;
				});

				this->AddMethod("ClearFx", [](lua_State* state) -> int {
					ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
					if (ISNULLPTR(pLUA)) return 0;

					LUAINTERFACE::HELPER::Check<T>(pLUA, 1);
					TChannel* pOBJ = LUAINTERFACE::HELPER::Get<T>(pLUA, 1);
					if (ISNULLPTR(pOBJ)) return 0;

					pOBJ->ClearFx();
					return 0;
				});

				LUA_FUNCTION_GETRAW_BOOL(T, VolumeIsFading);
				LUA_FUNCTION_GETRAW_BOOL(T, BalanceIsFading);

				LUA_FUNCTION_GETRAW_BOOL(T, Is3D);

				LUA_FUNCTION_GETSET_NUMBER(T, Volume, float);
				LUA_FUNCTION_GETSET_NUMBER(T, Balance, float);

				LUA_FUNCTION_GETSET_BOOL(T, 3DEnabled);
			}

			LUA_FUNCTION_GETSET_NUMBER(T, Time, double);
			LUA_FUNCTION_GETRAW_NUMBER(T, GetLength, double);

			LUA_FUNCTION_SETRAW_BOOL(T, EnableLooping);
			LUA_FUNCTION_GETRAW_BOOL(T, IsBlockStreamed);
			LUA_FUNCTION_GETRAW_BOOL(T, IsLooping);
			LUA_FUNCTION_GETRAW_BOOL(T, IsOnline);
			LUA_FUNCTION_GETRAW_BOOL(T, IsEndless);
			LUA_FUNCTION_GETRAW_BOOL(T, IsSeeking);

			LUA_FUNCTION_GETRAW_NUMBER(T, GetSamplingRate, double);
			LUA_FUNCTION_GETRAW_NUMBER(T, GetBitsPerSample, double);
			LUA_FUNCTION_GETRAW_NUMBER(T, GetAverageBitRate, double);
		}
	public:
		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA); 
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TChannel>(pLUA);
		}

		void Think(ILuaBase* pLUA)
		{
			if (ISNULLPTR(g_pListPendingCallbacks)) return;

			while (g_pListPendingCallbacks->getSize() > 0)
			{
				TChannelCallbackData* pCallbackData = g_pListPendingCallbacks->remove();
				if (ISNULLPTR(pCallbackData)) continue;

				bool bIsRecycledChannel = pCallbackData->bIsRecycledChannel;
				int iCallbackRef = pCallbackData->iCallbackRef;
				int iError = pCallbackData->iError;
				TChannel* pChannel = pCallbackData->pChannel;
				bool bNoPlay = pCallbackData->bNoPlay;

				delete pCallbackData;

				if (iError == BASS_OK && ISNULLPTR(pChannel)) iError = BASS_ERROR_UNKNOWN;

				if (!bIsRecycledChannel && iError != BASS_OK)
				{
					LUAINTERFACE::HELPER::Delete<TChannel>(pChannel);
					pChannel = NULL;
				}

				if (!ISNULLPTR(pChannel))
				{
					if (g_CLIENT)
					{
						pChannel->SetVolume(1);
					}
					else
					{
						// Mute on servers
						pChannel->SetVolume(0);
					}

					if (!bNoPlay)
					{
						pChannel->Play(true);
					}
				}

				if (!iCallbackRef)
				{
					LUAINTERFACE::HELPER::Delete<TChannel>(pChannel);
					pChannel = NULL;

					continue;
				}

				pLUA->ReferencePush(iCallbackRef);

				LUAINTERFACE::HELPER::Push<TChannel>(pLUA, pChannel);

				pLUA->PushNumber(iError);
				pLUA->PushString(UTIL::DecodeBassError(iError).c_str());
				pLUA->Call(3, 0);
				pLUA->ReferenceFree(iCallbackRef);
			}

			for (const auto &pair : TChannel::g_mapObjectInstances) {
				auto name = pair.first;
				auto instance = pair.second;
				if (ISNULLPTR(instance)) continue;

				instance->Think();
			}
		}

		void Dispose(ILuaBase* pLUA)
		{
			if(ISNULLPTR(g_pListPendingCallbacks)) return;

			while(g_pListPendingCallbacks->getSize() > 0)
			{
				TChannelCallbackData* pCallbackData = g_pListPendingCallbacks->remove();
				if(ISNULLPTR(pCallbackData)) continue;

				TChannel* pChannel = pCallbackData->pChannel;
				int iCallbackRef = pCallbackData->iCallbackRef;
				delete pCallbackData;

				LUAINTERFACE::HELPER::Delete<TChannel>(pChannel);
				pLUA->ReferenceFree(iCallbackRef);
			}
		}
	};
}
#endif