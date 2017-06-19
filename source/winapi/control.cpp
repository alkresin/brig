/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Standard Widgets creation and manipulation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

static WNDPROC wpOrigEditProc = NULL;
static WNDPROC wpOrigBtnProc = NULL;
static WNDPROC wpOrigComboProc = NULL;
static WNDPROC wpOrigTabProc = NULL;

/* -------- Label ---------
 */
BRIG_HANDLE brig_CreateLabel( brig_Label *pLabel, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle )
{

   PBRIG_WCHAR wcCaption = brig_str2WC( lpCaption );
   BRIG_HANDLE hLabel = 
         CreateWindowEx( ulExStyle,   /* extended style */
         TEXT( "STATIC" ),
         wcCaption,                   /* title   */
         WS_CHILD | WS_VISIBLE | ulStyle,
         x, y, nWidth, nHeight,
         pLabel->pParent->Handle(),
         ( HMENU ) iWidgId,           /* widget ID  */
         GetModuleHandle( NULL ), NULL );

   brig_free( wcCaption );
   return hLabel;

}

/* -------- Button ---------
 */
static LRESULT CALLBACK s_BtnProc( BRIG_HANDLE hBtn, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Button *pObject = ( brig_Button * ) GetWindowLongPtr( hBtn, GWLP_USERDATA );
   //brig_writelog( NULL, "btnmsg %lu %lu %u\r\n", (unsigned long)pObject, (unsigned long)hBtn, message );

   if( !pObject || !( pObject->onEvent( message, wParam, lParam ) ) )
      return CallWindowProc( wpOrigBtnProc, hBtn, message, wParam, lParam );
   else
      return 0;
}

BRIG_HANDLE brig_CreateButton( brig_Widget *pWidget, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_CHAR lpCaption )
{

   PBRIG_WCHAR wcCaption = brig_str2WC( lpCaption );
   BRIG_HANDLE hBtn = 
         CreateWindow( TEXT( "BUTTON" ),  /* predefined class  */
         wcCaption,                       /* button text   */
         WS_CHILD | WS_VISIBLE | ulStyle, /* style  */
         x, y, nWidth, nHeight,
         pWidget->pParent->Handle(),      /* parent window    */
         ( HMENU ) iWidgId,               /* button       ID  */
         GetModuleHandle( NULL ), NULL );

   if( hBtn )
   {
      LONG_PTR hProc;
      SetWindowLongPtr( hBtn, GWLP_USERDATA, NULL );
      hProc = SetWindowLongPtr( hBtn, GWLP_WNDPROC, ( LONG_PTR ) s_BtnProc );
      if( !wpOrigBtnProc )
         wpOrigBtnProc = ( WNDPROC ) hProc;
   }

   brig_free( wcCaption );

   return hBtn;

}

bool brig_CheckBtnGet( brig_Widget *pWidget )
{
   return (SendMessage( pWidget->Handle(), BM_GETCHECK, 0, 0 ))? 1 : 0;
}

void brig_CheckBtnSet( brig_Widget *pWidget, bool bValue )
{
   SendMessage( pWidget->Handle(), BM_SETCHECK, bValue, 0 );
}

bool brig_RadioBtnGet( brig_Widget *pWidget )
{
   return (SendMessage( pWidget->Handle(), BM_GETCHECK, 0, 0 ))? 1 : 0;
}

void brig_RadioBtnSet( brig_Widget *pWidget, bool bValue )
{
   SendMessage( pWidget->Handle(), BM_SETCHECK, (bValue)? BST_CHECKED : BST_UNCHECKED, 0 );
}

void brig_RadioGroupSet( brig_RadioGroup *pGroup, int iSelected )
{
   CheckRadioButton( ( BRIG_HANDLE ) pGroup->pParent->Handle(), // handle of dialog box
         pGroup->avButtons[0]->iWidgId,                  // identifier of first radio button in group
         pGroup->avButtons.back()->iWidgId,              // identifier of last radio button in group
         pGroup->avButtons[iSelected-1]->iWidgId         // identifier of radio button to select
          );
}



/* -------- Edit --------- 
 */
static LRESULT CALLBACK s_EditProc( BRIG_HANDLE hDlg, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Edit *pObject;

   //brig_writelog( NULL, "edimsg %u\r\n", message );
   switch( message ) {

      case WM_GETDLGCODE:
         return DLGC_WANTARROWS | DLGC_WANTCHARS; // DLGC_WANTTAB
   }

   pObject = ( brig_Edit * ) GetWindowLongPtr( hDlg, GWLP_USERDATA );
   if( !pObject || !( pObject->onEvent( message, wParam, lParam ) ) )
      return CallWindowProc( wpOrigEditProc, hDlg, message, wParam, lParam );
   else
      return 0;
}

BRIG_HANDLE brig_CreateEdit( brig_Edit *pEdit, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle )
{
   BRIG_HANDLE hEdit;

   hEdit = CreateWindowEx( ulExStyle,
         TEXT( "EDIT" ),
         NULL,
         WS_CHILD | WS_VISIBLE | ulStyle,
         x, y, nWidth, nHeight,
         pEdit->pParent->Handle(),     /* parent window    */
         ( HMENU ) iWidgId,            /* button       ID  */
         GetModuleHandle( NULL ), NULL );

   if( hEdit )
   {
      if( lpCaption )
      {
         PBRIG_WCHAR wcCaption = brig_str2WC( lpCaption );
         SendMessage( hEdit, WM_SETTEXT, 0, ( LPARAM ) wcCaption );
         brig_free( wcCaption );
      }
      SetWindowLongPtr( hEdit, GWLP_USERDATA, NULL );
      wpOrigEditProc = ( WNDPROC ) SetWindowLongPtr( hEdit,
            GWLP_WNDPROC, ( LONG_PTR ) s_EditProc );
   }

   return hEdit;

}

/* -------- Combobox ---------  */

static LRESULT CALLBACK s_ComboProc( BRIG_HANDLE hCombo, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Combo *pObject = ( brig_Combo * ) GetWindowLongPtr( hCombo, GWLP_USERDATA );
   //brig_writelog( NULL, "btnmsg %lu %lu %u\r\n", (unsigned long)pObject, (unsigned long)hBtn, message );

   if( !pObject || !( pObject->onEvent( message, wParam, lParam ) ) )
      return CallWindowProc( wpOrigComboProc, hCombo, message, wParam, lParam );
   else
      return 0;
}

BRIG_HANDLE brig_CreateCombo( brig_Combo *pCombo, int iWidgId,
          int x, int y, int nWidth, int nHeight, int iDisplay, bool bEdit )
{

   BRIG_HANDLE hCombo = CreateWindow( TEXT( "COMBOBOX" ), TEXT( "" ),
         WS_CHILD | WS_VISIBLE | ( (bEdit)? CBS_DROPDOWN:CBS_DROPDOWNLIST ),
         x, y, nWidth, nHeight,
         pCombo->pParent->Handle(),
         ( HMENU ) iWidgId,
         GetModuleHandle( NULL ),
         NULL );

   if( hCombo )
   {
      int iHeightBox = SendMessage( hCombo, CB_GETITEMHEIGHT, - 1, 0 );
      int iHeightItem = SendMessage( hCombo, CB_GETITEMHEIGHT, - 1, 0 );
      LONG_PTR hProc;
      SetWindowLongPtr( hCombo, GWLP_USERDATA, NULL );

      MoveWindow( hCombo, x, y, nWidth, iHeightBox + iHeightItem * iDisplay, TRUE );

      hProc = SetWindowLongPtr( hCombo, GWLP_WNDPROC, ( LONG_PTR ) s_ComboProc );
      if( !wpOrigComboProc )
         wpOrigComboProc = ( WNDPROC ) hProc;
   }

   return hCombo;

}

void brig_ComboSetArray( brig_Widget *pWidget, char **pArray, int iLen )
{

   BRIG_HANDLE hCombo = pWidget->Handle();
   PBRIG_WCHAR wcItem;

   SendMessage( hCombo, CB_RESETCONTENT, 0, 0 );

   for( int i = 0; i < iLen; i++ )
   {
      wcItem = brig_str2WC( pArray[i] );
      SendMessage( hCombo, CB_ADDSTRING, 0, (LPARAM) wcItem );
      brig_free( wcItem );
   }

}

int brig_ComboGetValue( brig_Widget *pWidget )
{
   return SendMessage( pWidget->Handle(), CB_GETCURSEL, 0, 0 );
}

void brig_ComboSetValue( brig_Widget *pWidget, int iSelected )
{
   SendMessage( pWidget->Handle(), CB_SETCURSEL, iSelected, 0 );
}


/* -------- Tab ---------  */

static LRESULT CALLBACK s_TabProc( BRIG_HANDLE hTab, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Tab *pObject = ( brig_Tab * ) GetWindowLongPtr( hTab, GWLP_USERDATA );
   //brig_writelog( NULL, "btnmsg %lu %lu %u\r\n", (unsigned long)pObject, (unsigned long)hBtn, message );

   if( !pObject || !( pObject->onEvent( message, wParam, lParam ) ) )
      return CallWindowProc( wpOrigTabProc, hTab, message, wParam, lParam );
   else
      return 0;
}

BRIG_HANDLE brig_CreateTab( brig_Tab *pTab, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{

   BRIG_HANDLE hTab = CreateWindow( WC_TABCONTROL, NULL, WS_CHILD | WS_VISIBLE,
         x, y, nWidth, nHeight,
         pTab->pParent->Handle(),
         ( HMENU ) iWidgId,
         GetModuleHandle( NULL ),
         NULL );

   if( hTab )
   {
      LONG_PTR hProc;
      SetWindowLongPtr( hTab, GWLP_USERDATA, NULL );
      hProc = SetWindowLongPtr( hTab, GWLP_WNDPROC, ( LONG_PTR ) s_TabProc );
      if( !wpOrigTabProc )
         wpOrigTabProc = ( WNDPROC ) hProc;
   }
   return hTab;

}

void brig_TabAddPage( brig_Tab *pTab, int iPage, PBRIG_CHAR lpName )
{
   TC_ITEM tie;
   PBRIG_WCHAR wcName = brig_str2WC( lpName );

   tie.mask = TCIF_TEXT | TCIF_IMAGE;
   tie.iImage = -1;
   tie.pszText = wcName;
   TabCtrl_InsertItem( pTab->Handle(), iPage, &tie );
   brig_free( wcName );

}

void brig_TabShowPage( brig_Tab *pTab, int iPage, bool bShow )
{
   PBRIG_TABPAGE ptp = &( pTab->avPages[iPage] );

   if( ptp->iFirst >= 0 )
      for( int i = ptp->iFirst; i <= ptp->iLast; i++ )
         pTab->avWidgets[i]->Show( bShow );
}

/* -------- Panel ---------  */

static LRESULT CALLBACK s_PanelProc( BRIG_HANDLE hDlg, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Panel *pObject = ( brig_Panel * ) GetWindowLongPtr( hDlg, GWLP_USERDATA );
   //brig_writelog( NULL, "panelmsg %u\r\n", message );

   if( !pObject || !( pObject->onEvent( message, wParam, lParam ) ) )
      return DefWindowProc( hDlg, message, wParam, lParam );
   else
      return 0;
}

static void reg_Panel( void )
{
   static BOOL bRegistered = FALSE;

   if( !bRegistered )
   {
      WNDCLASS wndclass;

      wndclass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
      wndclass.lpfnWndProc = s_PanelProc;
      wndclass.cbClsExtra = 0;
      wndclass.cbWndExtra = 0;
      wndclass.hInstance = GetModuleHandle( NULL );
      wndclass.hIcon = NULL;
      wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
      wndclass.hbrBackground = ( HBRUSH ) ( COLOR_3DFACE + 1 );
      wndclass.lpszMenuName = NULL;
      wndclass.lpszClassName = TEXT( "PANEL" );

      RegisterClass( &wndclass );
      bRegistered = TRUE;
   }
}

BRIG_HANDLE brig_CreatePanel( brig_Panel *pPanel, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{
   BRIG_HANDLE hPanel;

   reg_Panel();
   hPanel = CreateWindow( TEXT( "PANEL" ),
         NULL,                         /* no window title   */
         WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_OWNERDRAW | CCS_TOP,
         x, y, nWidth, nHeight,
         pPanel->pParent->Handle(),    /* parent window    */
         ( HMENU ) iWidgId,            /* button       ID  */
         GetModuleHandle( NULL ), NULL );

   if( hPanel )
   {
      SetWindowLongPtr( hPanel, GWLP_USERDATA, NULL );
   }

   return hPanel;
}

/* -------- QButton --------- 
 */

static LRESULT CALLBACK s_QButtonProc( BRIG_HANDLE handle, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   int ixPos, iyPos;
   brig_QButton *pObject = ( brig_QButton * ) GetWindowLongPtr( handle, GWLP_USERDATA );
   //brig_writelog( NULL, "panelmsg %u\r\n", message );

   if( !pObject )
      return DefWindowProc( handle, message, wParam, lParam );

   switch( message ) {

      case WM_MOUSEMOVE:
         ixPos = ( (unsigned long) lParam ) & 0xFFFF;
         iyPos = ( ( (unsigned long) lParam ) >> 16 ) & 0xFFFF;
         if( !pObject->iState )
         {
            pObject->iState = 1;
            SetCapture( handle );
            InvalidateRect( handle, NULL, 1 );
         }
         else if( ixPos > pObject->iWidth || iyPos > pObject->iHeight )
         {
            pObject->iState = 0;
            ReleaseCapture();
            InvalidateRect( handle, NULL, 1 );
         }
         return 0;
   }

   if( !( pObject->onEvent( message, wParam, lParam ) ) )
      return DefWindowProc( handle, message, wParam, lParam );
   else
      return 0;
}

static void reg_QButton( void )
{
   static BOOL bRegistered = FALSE;

   if( !bRegistered )
   {
      WNDCLASS wndclass;

      wndclass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
      wndclass.lpfnWndProc = s_QButtonProc;
      wndclass.cbClsExtra = 0;
      wndclass.cbWndExtra = 0;
      wndclass.hInstance = GetModuleHandle( NULL );
      wndclass.hIcon = NULL;
      wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
      wndclass.hbrBackground = ( HBRUSH ) ( COLOR_3DFACE + 1 );
      wndclass.lpszMenuName = NULL;
      wndclass.lpszClassName = TEXT( "QBUTTON" );

      RegisterClass( &wndclass );
      bRegistered = TRUE;
   }
}

BRIG_HANDLE brig_CreateQButton( brig_QButton *pQBtn, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{
   BRIG_HANDLE hQButton;

   reg_QButton();
   hQButton = CreateWindow( TEXT( "QBUTTON" ),
         NULL,                          /* no window title   */
         WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_OWNERDRAW | CCS_TOP,
         x, y, nWidth, nHeight,
         pQBtn->pParent->Handle(),     /* parent window    */
         ( HMENU ) iWidgId,            /* button       ID  */
         GetModuleHandle( NULL ), NULL );

   if( hQButton )
   {
      SetWindowLongPtr( hQButton, GWLP_USERDATA, NULL );
   }

   return hQButton;
}

/* -------- Splitter --------- 
 */

static LRESULT CALLBACK s_SplitterProc( BRIG_HANDLE handle, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Splitter *pObject = ( brig_Splitter * ) GetWindowLongPtr( handle, GWLP_USERDATA );

   //brig_writelog( NULL, "panelmsg %u\r\n", message );

   if( !pObject )
      return DefWindowProc( handle, message, wParam, lParam );

   switch( message ) {

      case WM_LBUTTONDOWN:
         SetCapture( handle );
         break;

      case WM_LBUTTONUP:
         ReleaseCapture();
         break;
   }

   if( !( pObject->onEvent( message, wParam, lParam ) ) )
      return DefWindowProc( handle, message, wParam, lParam );
   else
      return 0;
}

BRIG_HANDLE brig_CreateSplitter( brig_Splitter *pSplitter, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{
   BRIG_HANDLE hSplitter = 
         CreateWindowEx( 0,   /* extended style */
         TEXT( "STATIC" ),
         NULL,                   /* title   */
         WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
         x, y, nWidth, nHeight,
         pSplitter->pParent->Handle(),
         ( HMENU ) iWidgId,           /* widget ID  */
         GetModuleHandle( NULL ), NULL );

   if( hSplitter )
   {
      LONG_PTR hProc;
      SetWindowLongPtr( hSplitter, GWLP_USERDATA, NULL );
      SetWindowLongPtr( hSplitter, GWLP_WNDPROC, ( LONG_PTR ) s_SplitterProc );
   }

   return hSplitter;

}

/* -------- common widget's functions --------- */

PBRIG_CURSOR brig_LoadCursor( int iCursorType )
{
   return LoadCursor( NULL, MAKEINTRESOURCE( iCursorType ) );
}

void brig_SetCursor( PBRIG_CURSOR hCursor, brig_Widget *pWidget )
{
   SYMBOL_UNUSED( pWidget );
   SetCursor( hCursor );
}

void brig_SetFgColor( brig_Widget *pWidget, long lColor )
{
   SYMBOL_UNUSED( pWidget );
   SYMBOL_UNUSED( lColor );
}

void brig_SetBgColor( brig_Widget *pWidget, long lColor )
{
   SYMBOL_UNUSED( pWidget );
   SYMBOL_UNUSED( lColor );
}


static void CALLBACK s_timerProc( HWND hWnd, UINT message, UINT idTimer, DWORD dwTime )
{

   SYMBOL_UNUSED( hWnd );
   SYMBOL_UNUSED( message );
   SYMBOL_UNUSED( dwTime );

   brigRunTimerFunc( idTimer );

}

unsigned int brig_SetTimer( unsigned int uiId, unsigned int uiValue )
{

   return SetTimer( NULL, uiId, uiValue, s_timerProc );

}

void brig_KillTimer( unsigned int uiId )
{

   KillTimer( NULL, uiId );

}

int brig_GetScrollPos( brig_Widget *pWidget, bool bVertical )
{
   return GetScrollPos( pWidget->Handle(), (bVertical)? SB_VERT : SB_HORZ );
}

void brig_SetScrollPos( brig_Widget *pWidget, bool bVertical, int iPos )
{
   SetScrollPos( pWidget->Handle(), (bVertical)? SB_VERT : SB_HORZ, iPos, 1 );
}

int brig_GetScrollRange( brig_Widget *pWidget, bool bVertical, int *pMinPos, int *pMaxPos )
{
   int iRes, iMin, iMax;

   iRes = GetScrollRange( pWidget->Handle(), (bVertical)? SB_VERT : SB_HORZ, &iMin, &iMax );
   if( pMinPos )
      *pMinPos = iMin;
   if( pMaxPos )
      *pMaxPos = iMax;

   return iRes;
}

void brig_SetScrollRange( brig_Widget *pWidget, bool bVertical, int iMinPos, int iMaxPos )
{
   SetScrollRange( pWidget->Handle(), (bVertical)? SB_VERT : SB_HORZ, iMinPos, iMaxPos, 1 );
}

int brig_GetScrollPage( brig_Widget *pWidget, bool bVertical )
{
   SCROLLINFO si;

   memset( (void*) &si, 0, sizeof(SCROLLINFO) );
   si.cbSize = sizeof( si );
   si.fMask = SIF_PAGE;

   GetScrollInfo( pWidget->Handle(), (bVertical)? SB_VERT : SB_HORZ, &si );
   return si.nPage;
}

void brig_SetScrollPage( brig_Widget *pWidget, bool bVertical, int iPage )
{
   SCROLLINFO si;

   memset( (void*) &si, 0, sizeof(SCROLLINFO) );
   si.cbSize = sizeof( si );
   si.fMask = SIF_PAGE;
   si.nPage = iPage;

   SetScrollInfo( pWidget->Handle(), (bVertical)? SB_VERT : SB_HORZ, &si, 1 );
}
