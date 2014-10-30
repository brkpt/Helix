#include <process.h>
#include "Kernel/Callback.h"
#include "ThreadLoad.h"

// ****************************************************************************
// ****************************************************************************
namespace Helix {

	const int	STACK_SIZE	=	16*1024;
	bool		m_threadLoaderInitialized = false;
	bool		m_loadThreadShutdown = false;
	HANDLE		m_hLoadThread = NULL;
	HANDLE		m_hStartLoading = NULL;
	HANDLE		m_hLoadList = NULL;
	LoadObject	*	m_loadList = NULL;

// ****************************************************************************
// Thread function
// ****************************************************************************
void LoadThreadFunc(void *data);

// ****************************************************************************
// ****************************************************************************
void InitializeThreadLoader()
{
	_ASSERT(m_threadLoaderInitialized == false);
	m_threadLoaderInitialized = true;

	m_hStartLoading = CreateEvent(NULL,false,false,"StartLoading");
	_ASSERT(m_hStartLoading != NULL);

	m_hLoadList = CreateMutex(NULL,false,"ThreadLoadList");
	_ASSERT(m_hLoadList != NULL);

	// Now create the thread
	m_hLoadThread = (HANDLE)_beginthread( Helix::LoadThreadFunc, STACK_SIZE, NULL );
}

// ****************************************************************************
// ****************************************************************************
bool GetLoadThreadShutdown()
{
	return m_loadThreadShutdown;
}

// ****************************************************************************
// ****************************************************************************
void ShutdownLoadThread()
{
	m_loadThreadShutdown = true;
}

// ****************************************************************************
// ****************************************************************************
bool LoadFileAsync(const std::string &file, void (*callbackFn)(void *,long,void *), void *data)
{
	LoadObject *loadObject = new LoadObject;
	loadObject->filename = file;
	
	loadObject->callback = new Helix::StaticCallback3<void *,long,void *>(callbackFn);
	loadObject->userData = data;
	loadObject->next = NULL;

	DWORD result = WaitForSingleObject(m_hLoadList,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);
	LoadObject *scan = m_loadList;

	if(scan == NULL)
	{
		m_loadList = loadObject;
	}
	else
	{
		while(scan->next != NULL)
			scan = scan->next;
		scan->next = loadObject;
	}

	result = ReleaseMutex(m_hLoadList);
	_ASSERT(result != 0);

	// Signal the thread to start loading
	SetEvent(m_hStartLoading);

	return true;
}

// ****************************************************************************
// ****************************************************************************
void LoadThreadFunc(void *data)
{
	while(!GetLoadThreadShutdown())
	{
		DWORD result = WaitForSingleObject(m_hStartLoading,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);

		while(m_loadList!= NULL)
		{
			// Pop the next one
			result = WaitForSingleObject(m_hLoadList,INFINITE);
			_ASSERT(result == WAIT_OBJECT_0);

			LoadObject *loadObject = m_loadList;
			m_loadList = m_loadList->next;

			result = ReleaseMutex(m_hLoadList);
			_ASSERT(result != 0);

			HANDLE hFile = CreateFile(loadObject->filename.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				// Read in the data
				DWORD fileSize = GetFileSize(hFile,NULL);
				_ASSERT(fileSize != INVALID_FILE_SIZE);
				char *buffer = new char[fileSize];
				memset(buffer,0,fileSize);
				SetFilePointer(hFile,0,NULL,FILE_BEGIN);
				DWORD bytesRead;
				BOOL retVal = ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
				if(retVal == 0)
				{
					DWORD errval = GetLastError();
					int a =1;
				}
				_ASSERT(retVal != 0);
				CloseHandle(hFile);
				(*loadObject->callback)(buffer,bytesRead,loadObject->userData);
			}
			else
			{
				(*loadObject->callback)(NULL,0,loadObject->userData);
			}
			delete loadObject;
		}

		// The start loading event isn't cleared until we clear it.  This allows us to set it each 
		// time we add a file
		ResetEvent(m_hStartLoading);
	}
}

// ****************************************************************************
// ****************************************************************************
bool LoadFileAsync(const std::string &file, void (*callbackFn)(void *), void *data)
{
	return true;
}

}