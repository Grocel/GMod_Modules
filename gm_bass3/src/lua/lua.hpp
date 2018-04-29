#include "../globals.hpp"
#include "../util.hpp"
#include "GarrysMod/Lua/Interface.h"
#include "../classes/tchannel.hpp"

using namespace GarrysMod::Lua;

#ifndef LUA_H
#define LUA_H

#define LUA_FUNCTION_GETRAW( OBJTYPE, FUNC, LUATYPE, CTYPE ) \
	this->AddMethod(#FUNC, [](lua_State* state) -> int { \
		ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state); \
		if (ISNULLPTR(pLUA)) return 0; \
		OBJTYPE* pOBJ = LUAINTERFACE::HELPER::GetValid<OBJTYPE>( pLUA, 1 ); \
		pLUA->Push ## LUATYPE ( ( CTYPE )( pOBJ->FUNC () ) ); \
		return 1; \
	})

#define LUA_FUNCTION_SETRAW( OBJTYPE, FUNC, CHECKTYPE, LUATYPE, CTYPE ) \
	this->AddMethod(#FUNC, [](lua_State* state) -> int { \
		ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state); \
		if (ISNULLPTR(pLUA)) return 0; \
		OBJTYPE* pOBJ = LUAINTERFACE::HELPER::GetValid<OBJTYPE>( pLUA, 1 ); \
		pLUA->CheckType( 2, CHECKTYPE ); \
		pOBJ->FUNC ( ( CTYPE )( pLUA->Get ## LUATYPE ( 2 ) ) ); \
		return 0; \
	})

#define LUA_FUNCTION_GETSET( OBJTYPE, FUNC, CHECKTYPE, LUATYPE, CTYPE ) \
	LUA_FUNCTION_GETRAW( OBJTYPE, Get ## FUNC, LUATYPE, CTYPE ); \
	LUA_FUNCTION_SETRAW( OBJTYPE, Set ## FUNC, CHECKTYPE, LUATYPE, CTYPE )

#define LUA_FUNCTION_GETRAW_BOOL( OBJTYPE, FUNC ) LUA_FUNCTION_GETRAW( OBJTYPE, FUNC, Bool, bool)
#define LUA_FUNCTION_GETRAW_NUMBER( OBJTYPE, FUNC, CTYPE ) LUA_FUNCTION_GETRAW( OBJTYPE, FUNC, Number, CTYPE)

#define LUA_FUNCTION_SETRAW_BOOL( OBJTYPE, FUNC ) LUA_FUNCTION_SETRAW( OBJTYPE, FUNC, GarrysMod::Lua::Type::BOOL, Bool, bool)
#define LUA_FUNCTION_SETRAW_NUMBER( OBJTYPE, FUNC, CTYPE ) LUA_FUNCTION_SETRAW( OBJTYPE, FUNC, GarrysMod::Lua::Type::NUMBER, Number, CTYPE)

#define LUA_FUNCTION_GETSET_BOOL( OBJTYPE, FUNC ) LUA_FUNCTION_GETSET( OBJTYPE, FUNC, GarrysMod::Lua::Type::BOOL, Bool, bool)
#define LUA_FUNCTION_GETSET_NUMBER( OBJTYPE, FUNC, CTYPE ) LUA_FUNCTION_GETSET( OBJTYPE, FUNC, GarrysMod::Lua::Type::NUMBER, Number, CTYPE)

#define ADDENUM(name) this->AddEnum( #name, LUAENUM_ ## name )
#define ADDBASSENUM(name) this->AddEnum( #name, BASS_ ## name )

// Channel states
#define LUAENUM_CHANNEL_STOPPED 0
#define LUAENUM_CHANNEL_PLAYING 1
#define LUAENUM_CHANNEL_PAUSED 2
#define LUAENUM_CHANNEL_STALLED 3

// FFT modes
#define LUAENUM_FFT_256 0
#define LUAENUM_FFT_512 1
#define LUAENUM_FFT_1024 2
#define LUAENUM_FFT_2048 3
#define LUAENUM_FFT_4096 4
#define LUAENUM_FFT_8192 5
#define LUAENUM_FFT_16384 6
#define LUAENUM_FFT_32768 7

// Custom FFT modes 
#define LUAENUM_FFT_16 10
#define LUAENUM_FFT_32 11
#define LUAENUM_FFT_64 12
#define LUAENUM_FFT_128 13

// Play modes
#define LUAENUM_MODE_NONE 0x00
#define LUAENUM_MODE_3D 0x01
#define LUAENUM_MODE_LOOP 0x02
#define LUAENUM_MODE_MONO 0x04
#define LUAENUM_MODE_NOPLAY 0x08
#define LUAENUM_MODE_NOBLOCK 0x10

// Tag types
#define LUAENUM_TAG_META 0
#define LUAENUM_TAG_HTTP 1
#define LUAENUM_TAG_ICY 2
#define LUAENUM_TAG_ID3 3
#define LUAENUM_TAG_MF 4
#define LUAENUM_TAG_MP4 5
#define LUAENUM_TAG_APE 6
#define LUAENUM_TAG_OGG 7

// FX types
#define LUAENUM_FX_UNKNOWN -1
#define LUAENUM_FX_CHORUS 0
#define LUAENUM_FX_COMPRESSOR 1
#define LUAENUM_FX_DISTORTION 2
#define LUAENUM_FX_ECHO 3
#define LUAENUM_FX_FLANGER 4
#define LUAENUM_FX_GARGLE 5
#define LUAENUM_FX_I3DL2REVERB 6
#define LUAENUM_FX_PARAMEQ 7
#define LUAENUM_FX_REVERB 8
#define LUAENUM_FX_VOLUME 9

// FX Parameter values
#define LUAENUM_FXPARAMETER_WAVEFORM_TRIANGLE 0
#define LUAENUM_FXPARAMETER_WAVEFORM_SINE 1
#define LUAENUM_FXPARAMETER_WAVEFORM_SQUARE 2

#define LUAENUM_FXPARAMETER_PHASE_N180 BASS_DX8_PHASE_NEG_180
#define LUAENUM_FXPARAMETER_PHASE_N90 BASS_DX8_PHASE_NEG_90
#define LUAENUM_FXPARAMETER_PHASE_ZERO BASS_DX8_PHASE_ZERO
#define LUAENUM_FXPARAMETER_PHASE_P90 BASS_DX8_PHASE_90
#define LUAENUM_FXPARAMETER_PHASE_P180 BASS_DX8_PHASE_180

#define LUAENUM_FXPARAMETER_CURVE_LINEAR 0
#define LUAENUM_FXPARAMETER_CURVE_LOGARITHMIC 1

namespace LUAINTERFACE
{
	namespace HELPER {
		template<class T> int GetTypeID()
		{
			return T::LUAMETAID;
		}

		template<class T> string GetTypeName()
		{
			return T::LUAMETANAME;
		}

		template<class T> void Check(ILuaBase* pLUA, int iStackPos)
		{
			pLUA->CheckType(iStackPos, GetTypeID<T>());
		}

		template<class T> bool IsType(ILuaBase* pLUA, int iStackPos)
		{
			return pLUA->IsType(iStackPos, GetTypeID<T>());
		}

		template<class T> T* Get(ILuaBase* pLUA, int iStackPos)
		{
			return pLUA->GetUserType<T>(iStackPos, GetTypeID<T>());
		}

		template<class T> T* GetValid(ILuaBase* pLUA, int iStackPos)
		{
			Check<T>(pLUA, iStackPos);
			T* pOBJ = Get<T>(pLUA, iStackPos);

			if (ISNULLPTR(pOBJ))
			{
				snprintf(g_pcErrorBuffer, ERROR_PUFFER_SIZE, "The %s object is NULL or not the right type!", GetTypeName<T>().c_str());
				pLUA->ArgError(iStackPos, g_pcErrorBuffer);

				return NULL;
			}

			if (!pOBJ->IsValid())
			{
				snprintf(g_pcErrorBuffer, ERROR_PUFFER_SIZE, "The %s object is invalid!", GetTypeName<T>().c_str());
				pLUA->ArgError(iStackPos, g_pcErrorBuffer);

				return NULL;
			}

			return pOBJ;
		}

		template<class T> void Push(ILuaBase* pLUA, T* pOBJ)
		{
			if (ISNULLPTR(pOBJ))
			{
				pLUA->PushNil();
				return;
			}

			pLUA->PushUserType(pOBJ, GetTypeID<T>());
		}

		template<class T> void Delete(T* pOBJ, bool bOnlyRemoved = false)
		{
			if (ISNULLPTR(pOBJ)) return;

			unsigned int iRefs = pOBJ->RemoveReference();
			if (iRefs) return;

			if (bOnlyRemoved && !pOBJ->IsRemoved()) return;
			delete pOBJ;
		}
	}


	ILuaBase* GetLuaBase(lua_State* state);

	bool IsNil(ILuaBase* pLUA, int iStackPos);
	void PushVector(ILuaBase* pLUA, BASS_3DVECTOR* pVector);
}

#include "classes/tltcbass.hpp"
#include "classes/tlocchannel.hpp"
#include "classes/tloceffectchorus.hpp"
#include "classes/tloceffectcompressor.hpp"
#include "classes/tloceffectdistortion.hpp"
#include "classes/tloceffectecho.hpp"
#include "classes/tloceffectflanger.hpp"
#include "classes/tloceffectgargle.hpp"
#include "classes/tloceffecti3dl2reverb.hpp"
#include "classes/tloceffectparameq.hpp"
#include "classes/tloceffectreverb.hpp"
#include "classes/tloceffectvolume.hpp"

namespace LUAINTERFACE
{
	namespace _G {
		extern TLTCBass* g_BASS3;
	}

	namespace _R {
		extern TLOCChannel* g_IBASS3Channel;
		extern TLOCEffectChorus* g_IBASS3FxChorus;
		extern TLOCEffectCompressor* g_IBASS3FxCompressor;
		extern TLOCEffectDistortion* g_IBASS3FxDistortion;
		extern TLOCEffectEcho* g_IBASS3FxEcho;
		extern TLOCEffectFlanger* g_IBASS3FxFlanger;
		extern TLOCEffectGargle* g_IBASS3FxGargle;
		extern TLOCEffectI3DL2Reverb* g_IBASS3FxI3DL2Reverb;
		extern TLOCEffectParamEQ* g_IBASS3FxParamEQ;
		extern TLOCEffectReverb* g_IBASS3FxReverb;
		extern TLOCEffectVolume* g_IBASS3FxVolume;
	}

	void SetupRealm(ILuaBase* pLUA);
	void Init(ILuaBase* pLUA);
	void Dispose(ILuaBase* pLUA);
}

#endif