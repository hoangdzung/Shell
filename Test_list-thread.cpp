#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

//  Forward declarations:
BOOL ListProcessThreads( DWORD dwOwnerPID );
void printError( TCHAR* msg );

int main( void )
{
  DWORD exitCode = 0;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  CreateProcess("thread.exe",
                NULL,
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                NULL,
                &si,
                &pi);

  ListProcessThreads(GetCurrentProcessId());
  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return 0;
}

BOOL ListProcessThreads( DWORD dwOwnerPID ) 
{ 
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
  THREADENTRY32 te32; 
 
  // Take a snapshot of all running threads  
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
  if( hThreadSnap == INVALID_HANDLE_VALUE ) 
    return( FALSE ); 
 
  // Fill in the size of the structure before using it. 
  te32.dwSize = sizeof(THREADENTRY32); 
 
  // Retrieve information about the first thread,
  // and exit if unsuccessful
  if( !Thread32First( hThreadSnap, &te32 ) ) 
  {
    CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
    return( FALSE );
  }

  // Now walk the thread list of the system,
  // and display information about each thread
  // associated with the specified process
  do 
  { 
    if( te32.th32OwnerProcessID == dwOwnerPID )
    {
      printf( TEXT("\n     THREAD ID      = 0x%08X"), te32.th32ThreadID ); 
      printf( TEXT("\n     PROCESS ID     = 0x%08X"), te32.th32OwnerProcessID ); 
      printf( TEXT("\n     base priority  = %d"), te32.tpBasePri ); 
      printf( TEXT("\n     delta priority = %d"), te32.tpDeltaPri ); 
      HANDLE a = OpenThread(THREAD_SUSPEND_RESUME, TRUE, te32.th32ThreadID);
      printf( TEXT("\n     Handle         = %p"), a ); 
//      SuspendThread(a);
//      CloseHandle(a);

    }
  } while( Thread32Next(hThreadSnap, &te32 ) );

//  Don't forget to clean up the snapshot object.
  CloseHandle( hThreadSnap );
  return( TRUE );
}
