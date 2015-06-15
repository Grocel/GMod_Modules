//#include "main.h"
#include <thread>
#include <mutex>
#include <cstdio>
#include <cstdlib>

// Compiling with Source SDK 2013 for Linux/OSX? Don't forget this:
#include "steam/steamtypes.h"
#include "tier0/dbg.h"

#include "list.h"
#include "bass/bass.h"

#ifdef _WIN32
#else
	#include <unistd.h>
#endif

#ifndef GLOBALS_H
#define GLOBALS_H

#define GM_BASS_VERSION 1

#define ENUM_TO_VALUE(ENUM, VALUE, VAR) case ENUM: VAR = VALUE; break

#define BASS_NO_CHANGE -1
#define BASS_NULL 0

#define META_CHANNEL "IBASS3Channel"
#define TYPE_CHANNEL 66

#define BASS_ERROR_FILESYSTEM 102

#ifdef _WIN32
	#define snprintf sprintf_s
	#define SLEEP(X) Sleep(X)
#else
	#define SLEEP(X) usleep(X)
#endif

typedef double lua_n;
typedef DWORD bass_p;
typedef DWORD bass_flag;
typedef QWORD bass_time;

class TChannel;

struct TChannelCallbackData
{
	TChannel* pChannel;
	bool bIsRecycledChannel;
	int iError;
	int iCallbackRef;
	bool bNoPlay;
};

struct TChannelThreadArgs
{
	TChannel* pChannel;
	const char* sURL;
	bool bIsOnline;
	bass_flag eFlags;
	int iCallbackRef;
	bool bNoPlay;
};

extern bool g_CLIENT;
extern bool g_SELFLOADED;
extern bool g_CLOSING;
extern bool g_IntialTickHappend;
extern int g_ChannelRef;
extern int g_VectorRef;

extern thread* g_thCleanUp;
extern SyncList<TChannelCallbackData *>* g_pListPendingCallbacks;
extern SyncList<thread *>* g_pListRunningThreads;
extern float* g_pfFFTBuffer;

#endif

