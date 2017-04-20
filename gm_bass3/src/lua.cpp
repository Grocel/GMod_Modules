#include "lua.h"
#include "lua_functions.h"
#include "util.h"

#define ADDFUNC(name, func ) pLUA->PushCFunction( func ); pLUA->SetField( -2, name )
#define ADDMETHOD(obj, name ) ADDFUNC( #name, LUAFUNC::_R::obj::name )
#define ADDTABFUNC(tab, name ) ADDFUNC( #name, LUAFUNC::_G::tab::name )
#define ADDENUM(name) pLUA->PushNumber( LUAENUM_ ## name ); pLUA->SetField( -2, #name )
#define ADDBASSENUM(name) pLUA->PushNumber( BASS_ ## name ); pLUA->SetField( -2, #name )

using namespace GarrysMod::Lua;
using namespace LUAFUNC;

namespace LUAINTERFACE
{
	void PushChannel(ILuaBase* pLUA, TChannel* pChannel)
	{
		if(ISNULLPTR(pChannel))
		{
			pLUA->PushNil();
			return;
		}

		pLUA->PushUserType(pChannel, g_ChannelTypeID);
	}

	void DeleteChannel(TChannel *pChannel)
	{
		if(ISNULLPTR(pChannel)) return;
		
		unsigned int iRefs = pChannel->RemoveReference();
		if(!iRefs) delete pChannel;
	}

	void PushVector(ILuaBase* pLUA, BASS_3DVECTOR* pBassVector)
	{
		if (ISNULLPTR(pBassVector))
		{
			pLUA->PushNil();
			return;
		}

		Vector vPos;
		vPos.x = pBassVector->x;
		vPos.y = pBassVector->y;
		vPos.z = pBassVector->z;
		pLUA->PushVector(vPos);
	}

	void SetupRealm(ILuaBase* pLUA)
	{
		// Is it a client?
		pLUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			pLUA->GetField(-1, "CLIENT");
			g_CLIENT = pLUA->GetBool(-1);
		pLUA->Pop();

		if(g_CLIENT)
		{
			g_IsDedicatedServer = false;
		}
		else
		{
			pLUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
				pLUA->GetField(-1, "game");
					pLUA->GetField(-1, "IsDedicated");
					pLUA->Call(0, 1);
					g_IsDedicatedServer = pLUA->GetBool( -1 );
			pLUA->Pop(2);
		}
	}

	void SetupBASSTable(ILuaBase* pLUA)
	{
		int iTabBassRef = 0;
		int iTabBassEnumRef = 0;

		// _G.BASS.ENUM
		pLUA->CreateTable();
			// Channel stats
			ADDENUM(CHANNEL_STOPPED);
			ADDENUM(CHANNEL_PLAYING);
			ADDENUM(CHANNEL_PAUSED);
			ADDENUM(CHANNEL_STALLED);

			// Errors
			pLUA->PushNumber( BASS_OK );
			pLUA->SetField( -2, "ERROR_OK" );
			ADDBASSENUM(ERROR_UNKNOWN);

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
		iTabBassEnumRef = pLUA->ReferenceCreate();

		// _G.BASS
		pLUA->CreateTable();
			// Version of BASS
			pLUA->PushNumber(UTIL::GetBASSVersionDecimal()); 
			pLUA->SetField(-2, "Version");

			// Version of gm_bass3
			pLUA->PushNumber(GM_BASS_VERSION); 
			pLUA->SetField(-2, "ModuleVersion");

			// Enums
			pLUA->ReferencePush(iTabBassEnumRef);
			pLUA->SetField(-2, "ENUM");

			// Functions
			ADDTABFUNC(BASS3, PlayFile);
			ADDTABFUNC(BASS3, PlayURL);
			ADDTABFUNC(BASS3, CreateChannel);
			ADDTABFUNC(BASS3, DecodeError);

			ADDTABFUNC(BASS3, SetAgent);
			ADDTABFUNC(BASS3, GetAgent);
			ADDTABFUNC(BASS3, SetProxy);
			ADDTABFUNC(BASS3, GetProxy);

			if(g_CLIENT)
			{
				ADDTABFUNC(BASS3, GetEAX);
				ADDTABFUNC(BASS3, SetEAX);
			}
		iTabBassRef = pLUA->ReferenceCreate();

		// Add BASS3 to _G
		pLUA->PushSpecial(SPECIAL_GLOB);
			//pLUA->ReferencePush(iTabBassRef);
			//pLUA->SetField(-2, "BASS3");

			pLUA->PushString("BASS3");
			pLUA->ReferencePush(iTabBassRef);
		pLUA->SetTable(-3);

		// Add pulling hook
		pLUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			pLUA->GetField(-1, "hook");
				pLUA->GetField(-1, "Add");
				pLUA->PushString("Tick");
				pLUA->PushString("BASS3_PULL");
				pLUA->PushCFunction(PullPendingChannels);
			pLUA->Call(3, 0);
		pLUA->Pop(2);

		pLUA->ReferenceFree(iTabBassRef);
		pLUA->ReferenceFree(iTabBassEnumRef);
	}

	void SetupChannelObject(ILuaBase* pLUA)
	{
		int iTabChannelRef = 0;

		// _R.IBASS3Channel
		pLUA->CreateTable();
			ADDMETHOD(IBASS3Channel, Remove);
			ADDMETHOD(IBASS3Channel, IsValid);

			ADDMETHOD(IBASS3Channel, PlayFile);
			ADDMETHOD(IBASS3Channel, PlayURL);

			ADDMETHOD(IBASS3Channel, Play);
			ADDMETHOD(IBASS3Channel, Stop);
			ADDMETHOD(IBASS3Channel, Pause);

			ADDMETHOD(IBASS3Channel, GetState);

			ADDMETHOD(IBASS3Channel, IsLooping);
			ADDMETHOD(IBASS3Channel, IsOnline);
			ADDMETHOD(IBASS3Channel, IsBlockStreamed);

			ADDMETHOD(IBASS3Channel, EnableLooping);
			ADDMETHOD(IBASS3Channel, FFT);
			ADDMETHOD(IBASS3Channel, FFTComplex);
			ADDMETHOD(IBASS3Channel, GetLevel);

			ADDMETHOD(IBASS3Channel, GetTime);
			ADDMETHOD(IBASS3Channel, SetTime);

			ADDMETHOD(IBASS3Channel, GetLength);
			ADDMETHOD(IBASS3Channel, IsEndless);

			ADDMETHOD(IBASS3Channel, IsSeeking);

			ADDMETHOD(IBASS3Channel, GetTag);
			ADDMETHOD(IBASS3Channel, GetFileName);
			ADDMETHOD(IBASS3Channel, GetFileFormat);

			ADDMETHOD(IBASS3Channel, GetSamplingRate);
			ADDMETHOD(IBASS3Channel, GetBitsPerSample);
			ADDMETHOD(IBASS3Channel, GetAverageBitRate);

			if(g_CLIENT)
			{
				ADDMETHOD(IBASS3Channel, SetVolume);
				ADDMETHOD(IBASS3Channel, GetVolume);
				ADDMETHOD(IBASS3Channel, SetVolumeBoost);
				ADDMETHOD(IBASS3Channel, GetVolumeBoost);
				ADDMETHOD(IBASS3Channel, VolumeFadeTo);
				ADDMETHOD(IBASS3Channel, VolumeIsFading);

				ADDMETHOD(IBASS3Channel, SetBalance);
				ADDMETHOD(IBASS3Channel, GetBalance);
				ADDMETHOD(IBASS3Channel, BalanceFadeTo);
				ADDMETHOD(IBASS3Channel, BalanceIsFading);
			
				ADDMETHOD(IBASS3Channel, Is3D);

				ADDMETHOD(IBASS3Channel, GetPos);
				ADDMETHOD(IBASS3Channel, SetPos);

				ADDMETHOD(IBASS3Channel, Get3DFadeDistance);
				ADDMETHOD(IBASS3Channel, Set3DFadeDistance);

				ADDMETHOD(IBASS3Channel, Get3DCone);
				ADDMETHOD(IBASS3Channel, Set3DCone);

				ADDMETHOD(IBASS3Channel, Get3DEnabled);
				ADDMETHOD(IBASS3Channel, Set3DEnabled);

				ADDMETHOD(IBASS3Channel, GetEAXmix);
				ADDMETHOD(IBASS3Channel, SetEAXmix);
			}
		iTabChannelRef = pLUA->ReferenceCreate();

		// Add IBASS3Channel to _R
		g_ChannelTypeID = pLUA->CreateMetaTable(META_CHANNEL);
			pLUA->ReferencePush(iTabChannelRef);
			pLUA->SetField(-2, "__index");
			pLUA->ReferencePush(iTabChannelRef);
			pLUA->SetField(-2, "__newindex");
			ADDFUNC("__gc", LUAFUNC::_R::IBASS3Channel::lua__gc);
			ADDFUNC("__eq", LUAFUNC::_R::IBASS3Channel::lua__eq);
			ADDFUNC("__tostring", LUAFUNC::_R::IBASS3Channel::lua__tostring);
			//g_ChannelRef = pLUA->ReferenceCreate();
		pLUA->Pop();
		pLUA->ReferenceFree(iTabChannelRef);
	}
}