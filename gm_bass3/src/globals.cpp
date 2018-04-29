#include "globals.hpp"

bool g_CLIENT = false;
bool g_IsDedicatedServer = false;
bool g_SELFLOADED = false;
bool g_CLOSING = false;
bool g_IntialTickHappend = false;

thread* g_thCleanUp = NULL;
SyncList<TChannelCallbackData *>* g_pListPendingCallbacks = NULL;
SyncList<thread *>* g_pListRunningThreads = NULL;
float* g_pfFFTBuffer = NULL;
char* g_pcErrorBuffer = NULL;

string g_oldAgentSettings;
string g_oldProxySettings;
