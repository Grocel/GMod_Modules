#include "lua.h"
#include "lua_functions.h"
#include "util.h"

#define ADDFUNC(name, func ) LUA->PushCFunction( func ); LUA->SetField( -2, name )
#define ADDMETHOD(obj, name ) ADDFUNC( #name, LUAFUNC::_R::obj::name )
#define ADDTABFUNC(tab, name ) ADDFUNC( #name, LUAFUNC::_G::tab::name )
#define ADDENUM(name) LUA->PushNumber( LUAENUM_ ## name ); LUA->SetField( -2, #name )
#define ADDBASSENUM(name) LUA->PushNumber( BASS_ ## name ); LUA->SetField( -2, #name )

#define CREATEVECTOR(x, y, z) LUA->PushSpecial(SPECIAL_GLOB); LUA->GetField(-1, "Vector"); LUA->PushNumber(x); LUA->PushNumber(y); LUA->PushNumber(z); LUA->Call(3, 1)


using namespace GarrysMod::Lua;
using namespace LUAFUNC;

namespace LUAINTERFACE
{
	void PushChannel(lua_State* state, TChannel* pChannel)
	{
		if(pChannel == NULL)
		{
			LUA->PushNil();
			return;
		}

		GarrysMod::Lua::UserData* ud = (GarrysMod::Lua::UserData*)LUA->NewUserdata(sizeof(GarrysMod::Lua::UserData));
		ud->data = pChannel;
		ud->type = TYPE_CHANNEL;

		LUA->ReferencePush(g_ChannelRef);
		LUA->SetMetaTable(-2);
	}

	void DeleteChannel(TChannel *pChannel)
	{
		if(pChannel == NULL) return;

		unsigned int iRefs = pChannel->RemoveReference();
		if(!iRefs) delete pChannel;
	}

	// Todo: Get Source SDK to compile for Linux, so we can use real Vectors...
	void PushVector(lua_State* state, BASS_3DVECTOR* pBassVector)
	{
		if(pBassVector == NULL)
		{
			LUA->PushNil();
			return;
		}

		int iRef = 0;

		CREATEVECTOR(pBassVector->x, -pBassVector->y, pBassVector->z);
		LUA->Push(-1);
		iRef = LUA->ReferenceCreate();
		LUA->Pop(2);

		LUA->ReferencePush(iRef);
		LUA->ReferenceFree(iRef);
	}

	void SetupGlobalTable(lua_State* state)
	{

		// Is it a client?
		LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			LUA->GetField(-1, "CLIENT");
			g_CLIENT = LUA->GetBool(-1);
		LUA->Pop();
	}

	void SetupBASSTable(lua_State* state)
	{
		int iTabBassRef = 0;
		int iTabBassEnumRef = 0;

		// _G.BASS.ENUM
		LUA->CreateTable();
			// Channel stats
			ADDENUM(CHANNEL_STOPPED);
			ADDENUM(CHANNEL_PLAYING);
			ADDENUM(CHANNEL_PAUSED);
			ADDENUM(CHANNEL_STALLED);

			// Errors
			LUA->PushNumber( BASS_OK );
			LUA->SetField( -2, "ERROR_OK" );
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
		iTabBassEnumRef = LUA->ReferenceCreate();

		// _G.BASS
		LUA->CreateTable();
			// Version of BASS
			LUA->PushNumber(UTIL::GetBASSVersionDecimal()); 
			LUA->SetField(-2, "Version");

			// Version of gm_bass3
			LUA->PushNumber(GM_BASS_VERSION); 
			LUA->SetField(-2, "ModuleVersion");

			// Enums
			LUA->ReferencePush(iTabBassEnumRef);
			LUA->SetField(-2, "ENUM");

			// Functions
			ADDTABFUNC(BASS3, PlayFile);
			ADDTABFUNC(BASS3, PlayURL);
			ADDTABFUNC(BASS3, CreateChannel);
			ADDTABFUNC(BASS3, DecodeError);
		iTabBassRef = LUA->ReferenceCreate();

		// Add BASS3 to _G
		LUA->PushSpecial( SPECIAL_GLOB );
			//LUA->ReferencePush(iTabBassRef);
			//LUA->SetField(-2, "BASS3");

			LUA->PushString( "BASS3" );
			LUA->ReferencePush(iTabBassRef);
		LUA->SetTable( -3 );

		// Add pulling hook
		LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			LUA->GetField(-1, "hook");
				LUA->GetField(-1, "Add");
				LUA->PushString("Tick");
				LUA->PushString("BASS3_PULL");
				LUA->PushCFunction(PullPendingChannels);
			LUA->Call(3, 0);
		LUA->Pop(2);

		LUA->ReferenceFree(iTabBassRef);
		LUA->ReferenceFree(iTabBassEnumRef);
	}

	void SetupChannelObject(lua_State* state)
	{
		int iTabChannelRef = 0;

		// _R.IBASS3Channel
		LUA->CreateTable();
			ADDMETHOD(IBASS3Channel, Remove);
			ADDMETHOD(IBASS3Channel, IsValid);

			ADDMETHOD(IBASS3Channel, StreamFile);
			ADDMETHOD(IBASS3Channel, StreamURL);

			ADDMETHOD(IBASS3Channel, Play);
			ADDMETHOD(IBASS3Channel, Stop);
			ADDMETHOD(IBASS3Channel, Pause);

			ADDMETHOD(IBASS3Channel, GetState);

			ADDMETHOD(IBASS3Channel, SetVolume);
			ADDMETHOD(IBASS3Channel, GetVolume);
			ADDMETHOD(IBASS3Channel, VolumeFadeTo);
			ADDMETHOD(IBASS3Channel, VolumeIsFading);

			ADDMETHOD(IBASS3Channel, SetBalance);
			ADDMETHOD(IBASS3Channel, GetBalance);
			ADDMETHOD(IBASS3Channel, BalanceFadeTo);
			ADDMETHOD(IBASS3Channel, BalanceIsFading);

			ADDMETHOD(IBASS3Channel, Is3D);
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

			ADDMETHOD(IBASS3Channel, GetPos);
			ADDMETHOD(IBASS3Channel, SetPos);

			ADDMETHOD(IBASS3Channel, Get3DFadeDistance);
			ADDMETHOD(IBASS3Channel, Set3DFadeDistance);

			ADDMETHOD(IBASS3Channel, Get3DCone);
			ADDMETHOD(IBASS3Channel, Set3DCone);
		iTabChannelRef = LUA->ReferenceCreate();

		// Add IBASS3Channel to _R
		LUA->CreateMetaTableType(META_CHANNEL, TYPE_CHANNEL);
			LUA->ReferencePush(iTabChannelRef); 
			LUA->SetField(-2, "__index");
			LUA->ReferencePush(iTabChannelRef);
			LUA->SetField(-2, "__newindex");
			ADDFUNC("__gc", LUAFUNC::_R::IBASS3Channel::lua__gc);
			ADDFUNC("__eq", LUAFUNC::_R::IBASS3Channel::lua__eq);
			ADDFUNC("__tostring", LUAFUNC::_R::IBASS3Channel::lua__tostring);
			g_ChannelRef = LUA->ReferenceCreate();
		LUA->Pop();
		LUA->ReferenceFree(iTabChannelRef);
	}

	void UnrefGlobalReferences(lua_State* state)
	{
		LUA->ReferenceFree(g_ChannelRef);
	}
}