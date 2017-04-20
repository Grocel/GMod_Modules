#include "globals.h"

bool g_CLIENT = false;
bool g_IsDedicatedServer = false;
bool g_SELFLOADED = false;
bool g_CLOSING = false;
bool g_IntialTickHappend = false;

int g_ChannelTypeID = 0;

thread* g_thCleanUp = NULL;
SyncList<TChannelCallbackData *>* g_pListPendingCallbacks = NULL;
SyncList<thread *>* g_pListRunningThreads = NULL;
float* g_pfFFTBuffer = NULL;

string g_oldAgentSettings;
string g_oldProxySettings;
