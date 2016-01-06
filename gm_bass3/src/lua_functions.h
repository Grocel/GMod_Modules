#include "globals.h"
#include "GarrysMod/Lua/Interface.h"

#ifndef LUA_FUNCTIONS_H
#define LUA_FUNCTIONS_H

using namespace GarrysMod::Lua;
#define LUA_FUNCTION(fn) int fn(lua_State* state)

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

namespace LUAFUNC
{
	LUA_FUNCTION(PullPendingChannels);

	namespace _G
	{
		namespace BASS3
		{
			LUA_FUNCTION(PlayFile);
			LUA_FUNCTION(PlayURL);
			LUA_FUNCTION(CreateChannel);
			LUA_FUNCTION(DecodeError);
			LUA_FUNCTION(SetProxy);
			LUA_FUNCTION(GetProxy);

			LUA_FUNCTION(GetEAX);
			LUA_FUNCTION(SetEAX);
		}
	}

	namespace _R
	{
		namespace IBASS3Channel
		{
			LUA_FUNCTION(lua__gc);
			LUA_FUNCTION(lua__eq);
			LUA_FUNCTION(lua__tostring);

			LUA_FUNCTION(Remove);
			LUA_FUNCTION(IsValid);

			LUA_FUNCTION(PlayFile);
			LUA_FUNCTION(PlayURL);

			LUA_FUNCTION(Play);
			LUA_FUNCTION(Stop);
			LUA_FUNCTION(Pause);

			LUA_FUNCTION(GetState);

			LUA_FUNCTION(SetVolume);
			LUA_FUNCTION(GetVolume);
			LUA_FUNCTION(VolumeFadeTo);
			LUA_FUNCTION(VolumeIsFading);

			LUA_FUNCTION(SetBalance);
			LUA_FUNCTION(GetBalance);
			LUA_FUNCTION(BalanceFadeTo);
			LUA_FUNCTION(BalanceIsFading);

			LUA_FUNCTION(Is3D);
			LUA_FUNCTION(IsLooping);
			LUA_FUNCTION(IsOnline);
			LUA_FUNCTION(IsBlockStreamed);

			LUA_FUNCTION(EnableLooping);
			LUA_FUNCTION(FFT);
			LUA_FUNCTION(FFTComplex);
			LUA_FUNCTION(GetLevel);

			LUA_FUNCTION(GetTime);
			LUA_FUNCTION(SetTime);

			LUA_FUNCTION(GetLength);
			LUA_FUNCTION(IsEndless);

			LUA_FUNCTION(IsSeeking);

			LUA_FUNCTION(GetTag);
			LUA_FUNCTION(GetFileName);
			LUA_FUNCTION(GetFileFormat);

			LUA_FUNCTION(GetSamplingRate);
			LUA_FUNCTION(GetBitsPerSample);

			LUA_FUNCTION(GetPos);
			LUA_FUNCTION(SetPos);

			LUA_FUNCTION(Get3DFadeDistance);
			LUA_FUNCTION(Set3DFadeDistance);

			LUA_FUNCTION(Get3DCone);
			LUA_FUNCTION(Set3DCone);

			LUA_FUNCTION(GetEAXmix);
			LUA_FUNCTION(SetEAXmix);
		}
	}
}

#endif