#include <cstring>
#include <string>
#include <algorithm>

#include "flag.hpp"

namespace FLAG
{
	bass_flag GetPlayFlags(unsigned int iLuaFlag, bool& bNoPlay)
	{
		bool bNoBlock = false;
		bNoPlay = false;

		bass_flag eFlags = BASS_SAMPLE_FLOAT | BASS_SAMPLE_FX;
		if (iLuaFlag == LUAENUM_MODE_NONE) return eFlags | BASS_STREAM_BLOCK;

		if (g_CLIENT && (iLuaFlag & LUAENUM_MODE_3D))
			eFlags |= (BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX | BASS_SAMPLE_MONO); // 3D needs mono sound

		if (iLuaFlag & LUAENUM_MODE_LOOP)
			eFlags |= BASS_SAMPLE_LOOP;

		if (iLuaFlag & LUAENUM_MODE_MONO)
			eFlags |= BASS_SAMPLE_MONO;

		if (iLuaFlag & LUAENUM_MODE_NOBLOCK)
			bNoBlock = true;

		if (iLuaFlag & LUAENUM_MODE_NOPLAY)
			bNoPlay = true;

		if (!bNoBlock)
			eFlags |= BASS_STREAM_BLOCK;

		return eFlags;
	}

	bass_flag GetPlayFlags(const char* sLuaFlag, bool& bNoPlay)
	{
		bool bNoBlock = false;
		bNoPlay = false;

		bass_flag eFlags = BASS_SAMPLE_FLOAT | BASS_SAMPLE_FX;
		if (ISNULLPTR(sLuaFlag)) return eFlags | BASS_STREAM_BLOCK;

		string sBuf = string(sLuaFlag) + string(" ");
		if (sBuf == " ") return eFlags | BASS_STREAM_BLOCK;

		transform(sBuf.begin(), sBuf.end(), sBuf.begin(), ::tolower);

		string sFind = "";
		string::size_type posMatch = sBuf.find(" ");

		while (posMatch != string::npos)
		{
			sFind = sBuf.substr(0, posMatch);
			sBuf = sBuf.substr(posMatch + 1);
			posMatch = sBuf.find(" ");

			if (sFind == "") continue;
			if (sFind == " ") continue;

			if (g_CLIENT && (sFind == "3d"))
			{
				eFlags |= (BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX | BASS_SAMPLE_MONO); // 3D needs mono sound
				continue;
			}

			if (sFind == "loop")
			{
				eFlags |= BASS_SAMPLE_LOOP;
				continue;
			}

			if (sFind == "mono")
			{
				eFlags |= BASS_SAMPLE_MONO;
				continue;
			}

			if (sFind == "noblock")
			{
				bNoBlock = true;
				continue;
			}

			if (sFind == "noplay")
			{
				bNoPlay = true;
				continue;
			}
		}

		if (!bNoBlock)
			eFlags |= BASS_STREAM_BLOCK;

		return eFlags;
	}
}