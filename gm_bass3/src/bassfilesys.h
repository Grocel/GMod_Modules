#include "globals.h"

#ifndef BASSFILESYS_H
#define BASSFILESYS_H

namespace BASSFILESYS
{
	bool Init();
	bool PlayFile(const char* sFile, bass_flag eFlags, bass_p* ppHandle, int* piErr);
}

#endif