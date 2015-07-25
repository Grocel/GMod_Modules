#include "bass/bass.h"

// Compiling with Source SDK 2013 for Linux/OSX? Don't forget this:
#include "steam/steamtypes.h"

#include "bassfilesys.h"
#include "util.h"

#include "tier2/tier2.h"
#include "tier1/interface.h"
#include "filesystem.h"

#define FILESYSTEM_INTERFACE_VERSION_GMOD FILESYSTEM_INTERFACE_VERSION

#ifdef _WIN32
	#define FILESYSTEM_LIB "filesystem_stdio.dll"
	#define FILESYSTEM_LIB_DEDISV "filesys_export.dll"

#define MODULE_HEADER HMODULE
	#define MODULE_LOAD LoadLibrary
	#define MODULE_LOAD_FLAGS
	#define MODULE_IMPORT GetProcAddress
	#define MODULE_FREE FreeLibrary
	#define MODULE_ERROR NULL

#else
	#include <dlfcn.h>

	#define MODULE_HEADER void*
	#define MODULE_LOAD dlopen
	#define MODULE_LOAD_FLAGS ,RTLD_LAZY
	#define MODULE_IMPORT dlsym
	#define MODULE_FREE dlclose
	#define MODULE_ERROR dlerror()

	#ifdef _LINUX
		#define FILESYSTEM_LIB "filesystem_stdio.so"
		#define FILESYSTEM_LIB_DEDISV "./garrysmod/addons/filesys_export.so"
	#else
		#define FILESYSTEM_LIB "filesystem_stdio.dylib"
		#define FILESYSTEM_LIB_DEDISV "./garrysmod/addons/filesystem_stdio.dylib"		
	#endif
#endif

IFileSystem* g_pFileSystem = NULL;

// CALLBACKs start
void CALLBACK StreamCloseProc(void *param)
{
	if(g_pFileSystem == NULL) return;

	FileHandle_t pFile = (FileHandle_t) param;
	if(pFile == NULL) return;

	g_pFileSystem->Close(pFile); // close the file
}

QWORD CALLBACK StreamLenProc(void *param)
{
	if(g_pFileSystem == NULL) return 0;

	FileHandle_t pFile = (FileHandle_t) param;
	if(pFile == NULL) return 0;

    return (QWORD)g_pFileSystem->Size(pFile); // return the file length
}

DWORD CALLBACK StreamReadProc(void *buffer, DWORD length, void *param)
{
	if(g_pFileSystem == NULL) return -1;

	FileHandle_t pFile = (FileHandle_t) param;
	if(pFile == NULL) return -1;

	if(g_pFileSystem->EndOfFile(pFile)) 
	{
		return -1;
	}

	return (DWORD)g_pFileSystem->Read(buffer, length, pFile); // read from file;
}

BOOL CALLBACK StreamSeekProc(QWORD offset, void *param)
{
	if(g_pFileSystem == NULL) return false;

	FileHandle_t pFile = (FileHandle_t) param;
	if(pFile == NULL) return false;

	g_pFileSystem->Seek(pFile, (int)offset, FILESYSTEM_SEEK_HEAD);

	return ((QWORD)g_pFileSystem->Tell(pFile) == offset); // seek to offset
}

BASS_FILEPROCS StreamDataTable = {StreamCloseProc, StreamLenProc, StreamReadProc, StreamSeekProc}; // callback table
// CALLBACKs end

typedef IFileSystem* (*GetFilesystem_t)();

namespace BASSFILESYS
{
	bool Init()
	{
		if(g_pFileSystem != NULL) return true;

		char err[256];
		err[0] = 0;
		err[255] = 0;

		try
		{
			if(g_IsDedicatedServer)
			{
				if(FILESYSTEM_LIB_DEDISV == NULL) return false;

				MODULE_HEADER FileSystemExporter = NULL;
				FileSystemExporter = MODULE_LOAD(FILESYSTEM_LIB_DEDISV MODULE_LOAD_FLAGS);
				if(FileSystemExporter == NULL || FileSystemExporter == nullptr)
				{
					char* error = MODULE_ERROR;
					if(error != NULL)
						Warning("BASS Filesystem error, Error getting the " FILESYSTEM_LIB_DEDISV " plugin:\n%s\n", error);
					else
						Warning("BASS Filesystem error, Error getting the " FILESYSTEM_LIB_DEDISV " plugin.\n");
					g_pFileSystem = NULL;
					return false;		
				}
			
				GetFilesystem_t GetFilesystem = (GetFilesystem_t)MODULE_IMPORT(FileSystemExporter, "GetFilesystem");
				if(GetFilesystem == NULL || GetFilesystem == nullptr)
				{
					char* error = MODULE_ERROR;
					if(error != NULL)
						Warning("BASS Filesystem error, Error getting the GetFilesystem function of the " FILESYSTEM_LIB_DEDISV " plugin:\n%s\n", error);
					else
						Warning("BASS Filesystem error, Error getting the GetFilesystem function of the " FILESYSTEM_LIB_DEDISV " plugin.\n");
					g_pFileSystem = NULL;

					MODULE_FREE(FileSystemExporter);
					return false;		
				}

				MODULE_FREE(FileSystemExporter);
				g_pFileSystem = GetFilesystem();

				if(g_pFileSystem == NULL || g_pFileSystem == nullptr)
				{
					Warning("BASS Filesystem error, Error getting the IFileSystem " FILESYSTEM_INTERFACE_VERSION_GMOD " interface from the " FILESYSTEM_LIB_DEDISV " plugin.\n");
					g_pFileSystem = NULL;
					return false;
				}

				g_pFileSystem->Connect(Sys_GetFactoryThis());
				g_pFileSystem->Init();

				return true;
			}
			else
			{
				if(FILESYSTEM_LIB == NULL) return false;

				CSysModule* FileSystemFactoryDLL = NULL;
				if (!Sys_LoadInterface(FILESYSTEM_LIB, FILESYSTEM_INTERFACE_VERSION_GMOD, &FileSystemFactoryDLL, (void**)&g_pFileSystem))
				{
					Warning("BASS Filesystem error, Error getting the " FILESYSTEM_LIB " factory or the IFileSystem " FILESYSTEM_INTERFACE_VERSION_GMOD " interface.\n");
					g_pFileSystem = NULL;
					return false;
				}

				if(g_pFileSystem == NULL && g_pFileSystem == nullptr)
				{
					Warning("BASS Filesystem error, Error getting IFileSystem " FILESYSTEM_INTERFACE_VERSION_GMOD " interface from the " FILESYSTEM_LIB " factory.\n");
					g_pFileSystem = NULL;
					return false;
				}

				g_pFileSystem->Connect(Sys_GetFactoryThis());
				g_pFileSystem->Init();

				return true;
			}
		}
		catch(const overflow_error& e)
		{
			Warning("BASS Filesystem error, exception error: %s\n", e.what());
			return false;
		}
		catch(const runtime_error& e)
		{
			Warning("BASS Filesystem error, exception error: %s\n", e.what());
			return false;
		}
		catch(const exception& e)
		{
			Warning("BASS Filesystem error, exception error: %s\n", e.what());
			return false;
		}
		catch(char *s)
		{
			Warning("BASS Filesystem error, exception error: %s\n", s);
			return false;
		}
		catch(...)
		{
			Warning("BASS Filesystem error, exception error: Unknown");
			return false;
		}
	}

	bool PlayFile(const char* sFile, bass_flag eFlags, bass_p* ppHandle, int* piErr)
	{
		*ppHandle = BASS_NULL;
		*piErr = BASS_ERROR_FILEOPEN;

		if(sFile == NULL) return false;
		if(ppHandle == NULL) return false;
		if(piErr == NULL) return false;

		*piErr = BASS_ERROR_FILESYSTEM;
		if(g_pFileSystem == NULL) return false;
		bass_p pHandle = BASS_NULL;
		int iErr = -1;
		FileHandle_t fh = NULL;
	
		fh = g_pFileSystem->Open(sFile, "rb", "GAME");
		if(fh != NULL)
		{
			pHandle = BASS_StreamCreateFileUser(STREAMFILE_NOBUFFER, eFlags, &StreamDataTable, fh);
			iErr = BASS_ErrorGetCode();

			if(iErr == BASS_ERROR_FILEFORM)
			{
				fh = g_pFileSystem->Open(sFile, "rb", "GAME");
				if(fh != NULL)
				{
					int iLength = (int)g_pFileSystem->Size(fh);

					char *pData = new char[iLength];
					int iLengthRead = g_pFileSystem->Read(pData, iLength, fh);

					pHandle = BASS_MusicLoad(true, pData, 0, iLengthRead, eFlags, 0);
					iErr = BASS_ErrorGetCode();

					g_pFileSystem->Close(fh);
					delete [] pData;
				}
				else
				{
					iErr = BASS_ERROR_FILEOPEN;
				}
			}
		}
		else
		{
			iErr = BASS_ERROR_FILEOPEN;
		}

		*ppHandle = pHandle;
		*piErr = iErr;

		if(iErr != BASS_OK)
		{
			return false;
		}

		if(pHandle == BASS_NULL)
		{
			iErr = BASS_ERROR_UNKNOWN;
			*piErr = iErr;

			return false;
		}

		return true;
	}
}
