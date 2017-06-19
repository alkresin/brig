/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * brig_Table implementation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

void brig_table_OnVScroll( brig_Table *pTable, WPARAM wParam )
{
   unsigned int uiCode = ( (unsigned long) wParam ) & 0xFFFF;

   switch (uiCode) {

      case SB_LINEDOWN:
         pTable->Down();
         break;

      case SB_LINEUP:
         pTable->Up();
         break;

      case SB_TOP:
         pTable->Top();
         break;

      case SB_BOTTOM:
         pTable->Bottom();
         break;

      case SB_PAGEDOWN:
         pTable->PageDown();
         break;

      case SB_PAGEUP:
         pTable->PageUp();
         break;

      case SB_THUMBPOSITION:
         break;

      case SB_THUMBTRACK:
         break;

   }
}

void brig_table_OnHScroll( brig_Table *pTable, WPARAM wParam )
{

   unsigned int uiCode = ( (unsigned long) wParam ) & 0xFFFF;

   SYMBOL_UNUSED( pTable );

   switch (uiCode) {

      case SB_LINELEFT:
         break;
   }
}

void brig_table_OnWheel( brig_Table *pTable, WPARAM wParam )
{
   unsigned int uiCode = ( (unsigned long) wParam ) & 0xFFFF;
   unsigned int uiHiWord = ( ( (unsigned long) wParam ) >> 16 ) & 0xFFFF;
   int iDelta = ( uiHiWord>32768 )? uiHiWord - 65535 : uiHiWord;

   if( uiCode & MK_MBUTTON )
   {
      if( iDelta > 0 )
      {
      }
   }
   else
   {
      if( iDelta > 0 )
         pTable->Up();
      else
         pTable->Down();
   }
}

static LRESULT CALLBACK s_TableProc( HWND hDlg, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Table *pObject = ( brig_Table * ) GetWindowLongPtr( hDlg, GWLP_USERDATA );
   //brig_writelog( NULL, "tablemsg %u\r\n", message );

   if( message == WM_GETDLGCODE )
      return DLGC_WANTALLKEYS;

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

BRIG_HANDLE brig_CreateTable( brig_Table *pTable, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_FONT hFont, unsigned int iRows )
{
   BRIG_HANDLE hTable;
   BRIG_HANDLE handle = pTable->pParent->Handle();

   reg_Table();

   if( hFont && iRows )
   {
      HDC hDC = GetDC( handle );
      PBRIG_FONT hFontOld = (PBRIG_FONT) SelectObject( hDC, ( HGDIOBJ ) hFont );
      TEXTMETRIC tm;
      int iHeight;

      GetTextMetrics( hDC, &tm );
      SelectObject( hDC, ( HGDIOBJ ) hFontOld );
      ReleaseDC( handle, hDC );

      iHeight = (tm.tmHeight + pTable->pPadding[1] + pTable->pPadding[3]) * iRows + 2;
      if( iHeight < nHeight )
      {
         nHeight = iHeight;
         pTable->iHeight = iHeight;
      }
      else if( iHeight > nHeight )
         ulStyle |= WS_VSCROLL;
   }

   hTable = CreateWindow( TEXT( "Table" ),
         NULL,                         /* no window title   */
         WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | CCS_TOP | ulStyle,
         x, y, nWidth, nHeight,
         handle,                       /* parent window    */
         ( HMENU ) iWidgId,            /* button       ID  */
         GetModuleHandle( NULL ), NULL );

   if( hTable )
   {
      SetWindowLongPtr( hTable, GWLP_USERDATA, 0 );
   }

   return hTable;
}
