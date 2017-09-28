/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Miscellaneous functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"
#include <stdio.h>

#define BUFSIZE  1024

int brig_RunConsoleApp( PBRIG_CHAR sCommand, PBRIG_CHAR sOutFileName )
{
   SECURITY_ATTRIBUTES sa; 
   HANDLE g_hChildStd_OUT_Rd = NULL;
   HANDLE g_hChildStd_OUT_Wr = NULL;
   PROCESS_INFORMATION pi;
   STARTUPINFO si;
   BOOL bSuccess;

   DWORD dwRead, dwWritten, dwExitCode;
   CHAR chBuf[BUFSIZE]; 
   HANDLE hOut = NULL;

   sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
   sa.bInheritHandle = TRUE; 
   sa.lpSecurityDescriptor = NULL; 

   // Create a pipe for the child process's STDOUT. 
   if( ! CreatePipe( &g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0 ) )
      return 1;

   // Ensure the read handle to the pipe for STDOUT is not inherited.
   if( ! SetHandleInformation( g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0 ) )
      return 2;

   // Set up members of the PROCESS_INFORMATION structure. 
   ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
 
   // Set up members of the STARTUPINFO structure. 
   // This structure specifies the STDIN and STDOUT handles for redirection.
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   si.wShowWindow = SW_HIDE;
   si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
   si.hStdOutput = g_hChildStd_OUT_Wr;
   si.hStdError = g_hChildStd_OUT_Wr;

   PBRIG_WCHAR wcCommand = brig_str2WC( sCommand );

   bSuccess = CreateProcess( NULL, wcCommand, NULL, NULL,
      TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

   brig_free( wcCommand );
   
   if ( ! bSuccess ) 
      return 3;

   WaitForSingleObject( pi.hProcess, INFINITE );
   GetExitCodeProcess( pi.hProcess, &dwExitCode );
   CloseHandle( pi.hProcess );
   CloseHandle( pi.hThread );
   CloseHandle( g_hChildStd_OUT_Wr );

   if( sOutFileName )
   {
      PBRIG_WCHAR wcOutFileName = brig_str2WC( sOutFileName );
      hOut = CreateFile( wcOutFileName, GENERIC_WRITE, 0, 0,
             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
      brig_free( wcOutFileName );
   }
   while( 1 ) 
   { 
      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL );
      if( ! bSuccess || dwRead == 0 ) break; 

      if( sOutFileName )
      {
         bSuccess = WriteFile( hOut, chBuf, dwRead, &dwWritten, NULL );
         if( ! bSuccess ) break; 
      }
   } 

   if( sOutFileName )
      CloseHandle( hOut );
   CloseHandle( g_hChildStd_OUT_Rd );

   return (int) dwExitCode;
}
