#include "bass/bass.h"

// Compiling with Source SDK 2013 for Linux/OSX? Don't forget this:
#include "steam/steamtypes.h"

#include "bassfilesys.h"
#include "util.h"

#include "tier1/interface.h"
#include "filesystem.h"

#define FILESYSTEM_INTERFACE_VERSION_GMOD FILESYSTEM_INTERFACE_VERSION

#ifdef _WIN32
	#define FILESYSTEM_LIB "filesystem_stdio.dll"
#else
	#ifdef _LINUX
		#define FILESYSTEM_LIB "filesystem_stdio.so"
	#else
		#define FILESYSTEM_LIB "filesystem_stdio.dylib"
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

namespace BASSFILESYS
{
	bool Init()
	{
		if(g_pFileSystem != NULL) return true;

		CSysModule* FileSystemFactoryDLL = NULL;
		if (Sys_LoadInterface(FILESYSTEM_LIB, FILESYSTEM_INTERFACE_VERSION_GMOD, &FileSystemFactoryDLL, (void**)&g_pFileSystem))
		{
			if(g_pFileSystem != NULL && g_pFileSystem != nullptr)
			{
				g_pFileSystem->Connect(Sys_GetFactoryThis());
				g_pFileSystem->Init();

				g_pFileSystem->AddSearchPath( "garrysmod", "MOD" );
				//g_pFileSystem->PrintSearchPaths();

				return true;
			}
			g_pFileSystem = NULL;
		}

		Error( "BASS Init error, Error getting " FILESYSTEM_LIB " factory or IFileSystem " FILESYSTEM_INTERFACE_VERSION_GMOD " interface.\n" );
		return false;
	}

	bool PlayFile(const char* sFile, bass_flag eFlags, bass_p* ppHandle, int* piErr)
	{
		//Msg( "BASSFILESYS::PlayFile() 1\n" );
		*ppHandle = BASS_NULL;
		*piErr = BASS_ERROR_FILEOPEN;

		if(sFile == NULL) return false;
		if(ppHandle == NULL) return false;
		if(piErr == NULL) return false;

		//Msg( "BASSFILESYS::PlayFile() 2\n" );
		*piErr = BASS_ERROR_FILESYSTEM;
		if(g_pFileSystem == NULL) return false;
		//Msg( "BASSFILESYS::PlayFile() 2.1\n" );
		bass_p pHandle = BASS_NULL;
		int iErr = -1;
		FileHandle_t fh = NULL;
	
		//Msg( "BASSFILESYS::PlayFile('%s') 2.2\n", sFile );
		fh = g_pFileSystem->Open(sFile, "rb", "GAME");
		//Msg( "BASSFILESYS::PlayFile() 3\n" );
		if(fh != NULL)
		{
			//Msg( "BASSFILESYS::PlayFile() 3a\n" );
			pHandle = BASS_StreamCreateFileUser(STREAMFILE_NOBUFFER, eFlags, &StreamDataTable, fh); // playback
			iErr = BASS_ErrorGetCode();

			if(iErr == BASS_ERROR_FILEFORM)
			{
				//Msg( "BASSFILESYS::PlayFile() 3a1a\n" );

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
					//Msg( "BASSFILESYS::PlayFile() 3b1b\n" );
					iErr = BASS_ERROR_FILEOPEN;
				}
			}
		}
		else
		{
			//Msg( "BASSFILESYS::PlayFile() 3b\n" );
			iErr = BASS_ERROR_FILEOPEN;
		}

		*ppHandle = pHandle;
		*piErr = iErr;
		//Msg( "BASSFILESYS::PlayFile() 4\n" );

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
