#include "globals.hpp"
#include "lua/lua.hpp"

#ifndef FLAG_H
#define FLAG_H



namespace FLAG
{
	bass_flag GetPlayFlags(unsigned int iLuaFlag, bool& bNoPlay);
	bass_flag GetPlayFlags(const char* sLuaFlag, bool& bNoPlay);
}

#endif