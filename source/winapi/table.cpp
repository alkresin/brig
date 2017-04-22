/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * brig_Table implementation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

static LRESULT CALLBACK s_TableProc( HWND hDlg, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Table *pObject = ( brig_Table * ) GetWindowLongPtr( hDlg, GWLP_USERDATA );
   //brig_writelog( NULL, "tablemsg %u\r\n", message );

   if( !pObject || !( pObject->onEvent( message, wParam, lParam ) ) )
      return DefWindowProc( hDlg, message, wParam, lParam );
   else
      return 0;
}

static void reg_Table( void )
{
   static BOOL bRegistered = FALSE;

   if( !bRegistered )
   {
      WNDCLASS wndclass;

      wndclass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
      wndclass.lpfnWndProc = s_TableProc;
      wndclass.cbClsExtra = 0;
      wndclass.cbWndExtra = 0;
      wndclass.hInstance = GetModuleHandle( NULL );
      wndclass.hIcon = NULL;
      wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
      wndclass.hbrBackground = ( HBRUSH ) ( COLOR_3DFACE + 1 );
      wndclass.lpszMenuName = NULL;
      wndclass.lpszClassName = TEXT( "Table" );

      RegisterClass( &wndclass );
      bRegistered = TRUE;
   }
}

HWND brig_CreateTable( HWND hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle )
{
   HWND hTable;

   reg_Table();
   hTable = CreateWindow( TEXT( "Table" ),
         NULL,                          /* no window title   */
         WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | CCS_TOP | ulStyle,
         x, y, nWidth, nHeight,
         hParentWindow,                /* parent window    */
         ( HMENU ) iWidgId,            /* button       ID  */
         GetModuleHandle( NULL ), NULL );

   if( hTable )
   {
      SetWindowLongPtr( hTable, GWLP_USERDATA, NULL );
   }

   return hTable;
}
