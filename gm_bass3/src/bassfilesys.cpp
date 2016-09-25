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
	if(ISNULLPTR(g_pFileSystem)) return;

	FileHandle_t pFile = (FileHandle_t) param;
	if(ISNULLPTR(pFile)) return;

	g_pFileSystem->Close(pFile); // close the file
}

QWORD CALLBACK StreamLenProc(void *param)
{
	if(ISNULLPTR(g_pFileSystem)) return 0;

	FileHandle_t pFile = (FileHandle_t) param;
	if(ISNULLPTR(pFile)) return 0;

    return (QWORD)g_pFileSystem->Size(pFile); // return the file length
}

DWORD CALLBACK StreamReadProc(void *buffer, DWORD length, void *param)
{
	if(ISNULLPTR(g_pFileSystem)) return -1;

	FileHandle_t pFile = (FileHandle_t) param;
	if(ISNULLPTR(pFile)) return -1;

	if(g_pFileSystem->EndOfFile(pFile)) 
	{
		return -1;
	}

	return (DWORD)g_pFileSystem->Read(buffer, length, pFile); // read from file;
}

BOOL CALLBACK StreamSeekProc(QWORD offset, void *param)
{
	if(ISNULLPTR(g_pFileSystem)) return false;

	FileHandle_t pFile = (FileHandle_t) param;
	if(ISNULLPTR(pFile)) return false;

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
		if(!ISNULLPTR(g_pFileSystem)) return true;

		char err[256];
		err[0] = 0;
		err[255] = 0;

		try
		{
			if(g_IsDedicatedServer)
			{
				if(ISNULLPTR(FILESYSTEM_LIB_DEDISV)) return false;

				MODULE_HEADER FileSystemExporter = NULL;
				FileSystemExporter = MODULE_LOAD(FILESYSTEM_LIB_DEDISV MODULE_LOAD_FLAGS);
				if(ISNULLPTR(FileSystemExporter))
				{
					char* error = MODULE_ERROR;
					if(!ISNULLPTR(error))
						Warning("BASS Filesystem error, Error getting the " FILESYSTEM_LIB_DEDISV " plugin:\n%s\n", error);
					else
						Warning("BASS Filesystem error, Error getting the " FILESYSTEM_LIB_DEDISV " plugin.\n");
					g_pFileSystem = NULL;
					return false;		
				}
			
				GetFilesystem_t GetFilesystem = (GetFilesystem_t)MODULE_IMPORT(FileSystemExporter, "GetFilesystem");
				if(ISNULLPTR(GetFilesystem))
				{
					char* error = MODULE_ERROR;
					if(!ISNULLPTR(error))
						Warning("BASS Filesystem error, Error getting the GetFilesystem function of the " FILESYSTEM_LIB_DEDISV " plugin:\n%s\n", error);
					else
						Warning("BASS Filesystem error, Error getting the GetFilesystem function of the " FILESYSTEM_LIB_DEDISV " plugin.\n");
					g_pFileSystem = NULL;

					MODULE_FREE(FileSystemExporter);
					return false;		
				}

				MODULE_FREE(FileSystemExporter);
				g_pFileSystem = GetFilesystem();

				if(ISNULLPTR(g_pFileSystem))
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
				if(ISNULLPTR(FILESYSTEM_LIB)) return false;

				CSysModule* FileSystemFactoryDLL = NULL;
				if (!Sys_LoadInterface(FILESYSTEM_LIB, FILESYSTEM_INTERFACE_VERSION_GMOD, &FileSystemFactoryDLL, (void**)&g_pFileSystem))
				{
					Warning("BASS Filesystem error, Error getting the " FILESYSTEM_LIB " factory or the IFileSystem " FILESYSTEM_INTERFACE_VERSION_GMOD " interface.\n");
					g_pFileSystem = NULL;
					return false;
				}

				if(ISNULLPTR(g_pFileSystem))
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
		catch(string s)
		{
			Warning("BASS Filesystem error, exception error: %s\n", s.c_str());
			return false;
		}
		catch(char* s)
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

	bool PlayFile(string& sFile, bass_flag eFlags, bass_p& pHandleOut, int& iErrOut)
	{
		pHandleOut = BASS_NULL;
		iErrOut = BASS_ERROR_FILESYSTEM;

		if(ISNULLPTR(g_pFileSystem)) return false;

		bass_p pHandle = BASS_NULL;
		int iErr = -1;
		FileHandle_t fh = NULL;

		char* sFileTemp = UTIL::STRING::safe_getnewcstr(sFile);

		V_FixSlashes(sFileTemp);
		V_FixDoubleSlashes(sFileTemp);

		sFile = string(sFileTemp);
		delete[] sFileTemp;

		fh = g_pFileSystem->Open(sFile.c_str(), "rb", "GAME");
		if(!ISNULLPTR(fh))
		{
			pHandle = BASS_StreamCreateFileUser(STREAMFILE_NOBUFFER, eFlags, &StreamDataTable, fh);
			iErr = BASS_ErrorGetCode();

			if(iErr == BASS_ERROR_FILEFORM)
			{
				fh = g_pFileSystem->Open(sFile.c_str(), "rb", "GAME");
				if(!ISNULLPTR(fh))
				{
					unsigned int iLength = g_pFileSystem->Size(fh);

					char* pData = new char[iLength];
					int iLengthRead = g_pFileSystem->Read(pData, iLength, fh);
					g_pFileSystem->Close(fh);

					pHandle = BASS_MusicLoad(true, pData, 0, iLengthRead, eFlags, 0);
					iErr = BASS_ErrorGetCode();

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

		if(pHandle == BASS_NULL)
		{
			iErr = BASS_ERROR_UNKNOWN;
		}

		pHandleOut = pHandle;
		iErrOut = iErr;

		return iErr == BASS_OK;
	}
}
