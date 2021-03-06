/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Main function and windows manipulation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"
#include <stdio.h>
#include <shellapi.h>

static WNDPROC wpOrigDlgProc;

static BRIG_HANDLE hMainWindow = NULL;
static BRIG_HANDLE hFrameWindow = NULL;

extern brig_TreeNode * brig_TreeHitTest( brig_Tree * pTree );

brig_Application brigApp;

BRIG_HANDLE *aDialogs = NULL;
static int s_nDialogs = 0;
int iDialogs = 0;

void brig_writelog( const char * sFile, const char * sTraceMsg, ... )
{
   FILE *hFile;

   if( sFile == NULL )
   {
      hFile = fopen( "ac.log", "a" );
   }
   else
   {
      hFile = fopen( sFile, "a" );
   }

   if( hFile )
   {
      va_list ap;

      va_start( ap, sTraceMsg );
      vfprintf( hFile, sTraceMsg, ap );
      va_end( ap );

      fclose( hFile );
   }

}

void brig_SetWidgetData( brig_Widget *pWidget )
{
   SetWindowLongPtr( pWidget->Handle(), GWLP_USERDATA, ( LPARAM ) pWidget );
   //brig_writelog( NULL, "set: %lu %lu\r\n", (unsigned long)pWidget, (unsigned long)pWidget->Handle() );
}

void brig_SetWindowText( brig_Widget *pWidget, PBRIG_CHAR lpTitle )
{

   PBRIG_WCHAR wcTitle = brig_str2WC( lpTitle);
   SetWindowText( pWidget->Handle(), wcTitle );
   brig_free( wcTitle );

}

PBRIG_CHAR brig_GetWindowText( brig_Widget *pWidget )
{
   BRIG_HANDLE handle = pWidget->Handle();
   unsigned long ulLen = ( unsigned long ) SendMessage( handle, WM_GETTEXTLENGTH, 0, 0 );
   LPTSTR lpText = ( TCHAR * ) malloc( ( ulLen + 1 ) * sizeof( TCHAR ) );

   ulLen = ( ULONG ) SendMessage( handle, WM_GETTEXT, ( WPARAM ) ( ulLen + 1 ),
         ( LPARAM ) lpText );
   lpText[ulLen] = '\0';

#if defined(UNICODE)
   PBRIG_CHAR pResult = brig_WC2str( lpText, ulLen );
   free( lpText );
   return pResult;
#else
   return (PBRIG_CHAR) lpText;
#endif
}

void brig_MoveWindow( brig_Widget *pWidget, int iLeft, int iTop, int iWidth, int iHeight )
{

   MoveWindow( pWidget->Handle(),
         ( iLeft < 0 ) ? pWidget->iLeft : iLeft,       // horizontal position
         ( iTop < 0 )  ? pWidget->iTop : iTop,         // vertical position
         ( iWidth < 0 ) ? pWidget->iWidth : iWidth,    // width
         ( iHeight < 0 ) ? pWidget->iHeight : iHeight, // height
         TRUE                                          // repaint flag
          );
}

void brig_EnableWindow( brig_Widget *pWidget, bool bEnable )
{

   EnableWindow( pWidget->Handle(), bEnable );
}

void brig_ShowWindow( brig_Widget *pWidget, bool bShow )
{

   ShowWindow( pWidget->Handle(), (bShow)? SW_SHOWNORMAL:SW_HIDE );
}

brig_Widget * brig_GetActiveWindow( void )
{
   return ( brig_Widget * ) GetWindowLongPtr( GetActiveWindow(), GWLP_USERDATA );
}

brig_Widget * brig_SetFocus( brig_Widget *pWidget )
{
   return ( brig_Widget * ) GetWindowLongPtr( SetFocus( pWidget->Handle() ), GWLP_USERDATA );
}

brig_Widget * brig_GetFocus( void )
{
   return ( brig_Widget * ) GetWindowLongPtr( GetFocus(), GWLP_USERDATA );
}

void brig_SetTopmost( brig_Widget *pWidget )
{
   SetWindowPos( pWidget->Handle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

void brig_RemoveTopmost( brig_Widget *pWidget )
{
   SetWindowPos( pWidget->Handle(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

static LRESULT onCtlColor( WPARAM wParam, LPARAM lParam )
{
   brig_Widget *pObject = ( brig_Widget * ) GetWindowLongPtr( (BRIG_HANDLE) lParam, GWLP_USERDATA );
   bool b = 0;

   if( pObject )
   {
      if( pObject->SetTextColor() > -1 )
      {
         SetTextColor( ( HDC ) wParam, ( COLORREF ) pObject->SetTextColor() );
         b = 1;
      }
      if( pObject->SetBackColor() > -1 )
      {
         SetBkColor( ( HDC ) wParam, ( COLORREF ) pObject->SetBackColor() );
         return (LRESULT) pObject->hBrush;
      }
      else if( b )
         return NULL_BRUSH;
   }
   return -1;
}

static void onNotify( brig_Widget *pObject, WPARAM wParam, LPARAM lParam )
{
   brig_TreeNode * pNode;
   if( pObject )
      if( pObject->uiType == TYPE_TREE )
         switch( ( ( NMHDR * ) lParam )->code ) {
            case TVN_SELCHANGED:
               pNode = (brig_TreeNode*) ( ( ( NM_TREEVIEW * ) lParam )->itemNew.lParam );
               ((brig_Tree*)pObject)->pSelected = pNode;
               if( pNode->pfAction )
                  pNode->pfAction( pNode );
               break;
            case -3:
               pNode = brig_TreeHitTest( (brig_Tree*) pObject );
               if( pNode->pfDblClick )
                  pNode->pfDblClick( pNode );
               break;
            case -5:
               pNode = brig_TreeHitTest( (brig_Tree*) pObject );
               if( pNode->pfRClick )
                  pNode->pfRClick( pNode );
               break;
         }
      else if( pObject->uiType == TYPE_TAB )
         if( ( ( ( NMHDR * ) lParam )->code ) == TCN_SELCHANGE )
            pObject->onEvent( WM_USER, wParam, TabCtrl_GetCurSel( ( HWND ) pObject->Handle() ) );

}

static LRESULT CALLBACK s_MainWndProc( BRIG_HANDLE handle, UINT message,
      WPARAM wParam, LPARAM lParam )
{

   brig_Widget *pObject;
   LRESULT result;

   brig_MainWindow *pObjWin = ( brig_MainWindow * ) GetWindowLongPtr( handle, GWLP_USERDATA );

   //brig_writelog( NULL, "wndmsg %u\r\n", message );
   if( pObjWin && ( pObjWin->onEvent( message, wParam, lParam ) ) )
      return 0;

   switch( message ) {
      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLOREDIT:
           result = onCtlColor( wParam, lParam );
           if( result == -1 )
              break;
           else
              return result;

      case WM_NOTIFY:
         onNotify( pObjWin->FindWidget( ( ( ( NMHDR * ) lParam )->hwndFrom ) ), wParam, lParam );
         break;

      case WM_DESTROY:
         hMainWindow = NULL;
         return 0;
   }
   return DefWindowProc( handle, message, wParam, lParam );
}

static LRESULT CALLBACK s_DialogProc( BRIG_HANDLE hDlg, UINT message,
      WPARAM wParam, LPARAM lParam )
{
   int i;

   brig_Dialog *pObjWin = ( brig_Dialog * ) GetWindowLongPtr( hDlg, GWLP_USERDATA );
   LRESULT result;
   //brig_writelog( NULL, "dlgmsg %u\r\n", message );

   if( pObjWin && ( pObjWin->onEvent( message, wParam, lParam ) ) )
      return 0;

   switch( message ) {

      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLOREDIT:
           result = onCtlColor( wParam, lParam );
           if( result == -1 )
              break;
           else
              return result;

      case WM_NOTIFY:
         onNotify( pObjWin->FindWidget( ( ( ( NMHDR * ) lParam )->hwndFrom ) ), wParam, lParam );
         break;

      case WM_SYSCOMMAND:
         if( wParam == SC_CLOSE )
         {
            DestroyWindow( hDlg );
            return 1;
         }
         break;

      case WM_DESTROY:
         if( pObjWin->bModal && pObjWin->pParent )
         {
            pObjWin->pParent->Enable();
            brig_SetTopmost( pObjWin->pParent );
            brig_RemoveTopmost( pObjWin->pParent );
         }
         for( i = 0; i < iDialogs; i++ )
            if( aDialogs[i] == hDlg )
            {
               aDialogs[i] = NULL;
               break;
            }
         iDialogs--;
         for( ; i < iDialogs; i++ )
            aDialogs[i] = aDialogs[i + 1];

   }
   return CallWindowProc( wpOrigDlgProc, hDlg, message, wParam, lParam );

}

BRIG_HANDLE brig_InitMainWindow( PBRIG_CHAR lpAppName, PBRIG_CHAR lpTitle,
      int x, int y, int width, int height, long int lStyle,
      PBRIG_ICON hIcon, PBRIG_BRUSH hBrush )
{
   WNDCLASS wndclass;
   HANDLE hInstance = GetModuleHandle( NULL );
   DWORD ExStyle = 0;
   PBRIG_WCHAR wcAppName = brig_str2WC( lpAppName );
   PBRIG_WCHAR wcTitle = brig_str2WC( lpTitle );

   wndclass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
   wndclass.lpfnWndProc = s_MainWndProc;
   wndclass.cbClsExtra = 0;
   wndclass.cbWndExtra = 0;
   wndclass.hInstance = ( HINSTANCE ) hInstance;
   wndclass.hIcon = ( hIcon ) ? hIcon : LoadIcon( ( HINSTANCE ) hInstance, TEXT( "" ) );
   wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
   wndclass.hbrBackground = ( hBrush ) ? hBrush : ( PBRIG_BRUSH ) ( COLOR_WINDOW + 1 );
   wndclass.lpszMenuName = NULL;
   wndclass.lpszClassName = wcAppName;

   if( RegisterClass( &wndclass ) )
   {
      hMainWindow = CreateWindowEx( ExStyle, wcAppName, wcTitle, lStyle,
            x, y,
            ( !width ) ? ( LONG ) CW_USEDEFAULT : width,
            ( !height ) ? ( LONG ) CW_USEDEFAULT : height,
            NULL, NULL, ( HINSTANCE ) hInstance, NULL );
   }

   brig_free( wcAppName );
   brig_free( wcTitle );

   SetWindowLongPtr( hMainWindow, GWLP_USERDATA, 0 );

   return hMainWindow;
}

void brig_ProcessMessage( MSG msg, HACCEL hAcceler, BOOL lMdi )
{

   int i;

   for( i = 0; i < iDialogs; i++ )
   {
      if( IsWindow( aDialogs[i] ) && IsDialogMessage( aDialogs[i], &msg ) )
         break;
   }


   if( i == iDialogs )
   {
      if( lMdi && TranslateMDISysAccel( hFrameWindow, &msg ) )
         return;

      if( !hAcceler || !TranslateAccelerator( hMainWindow, hAcceler, &msg ) )
      {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
   }
}

static void brig_MainLoop( HACCEL hAcceler, BOOL lMdi )
{
   static bool bActive = 0;

   SYMBOL_UNUSED( lMdi );

   //brig_writelog( NULL, "MainLoop - 0\r\n" );
   if( !bActive )
   {
      MSG msg;

      bActive = 1;
      while( GetMessage( &msg, NULL, 0, 0 ) )
      {
         brig_ProcessMessage( msg, hAcceler, 0 );
         if( !iDialogs && !hMainWindow )
         {
            bActive = 0;
            break;
         }
      }
   }

}

static void brig_ModalDlgLoop( HACCEL hAcceler, BRIG_HANDLE hDlg )
{

   MSG msg;
   int i;
   bool bExist = 1;
   //brig_writelog( NULL, "ModalLoop - 0\r\n" );

   while( bExist && GetMessage( &msg, NULL, 0, 0 ) )
   {
      brig_ProcessMessage( msg, hAcceler, 0 );
      bExist = 0;
      for( i = 0; i < iDialogs; i++ )
         if( aDialogs[i] == hDlg )
            bExist = 1;
   }

}

void brig_ActivateMainWindow( int bShow, HACCEL hAcceler, int iMaxMin )
{

   if( bShow )
   {
      ShowWindow( hMainWindow, ( iMaxMin == 1 )? SW_SHOWMAXIMIZED : 
         ( ( iMaxMin == 2 ) ? SW_SHOWMINIMIZED : SW_SHOWNORMAL ) );
   }

   brig_MainLoop( hAcceler, 0 );

}

BRIG_HANDLE brig_InitDialog( PBRIG_CHAR lpTitle,
      int x, int y, int width, int height, long int lStyle )
{
   DWORD ExStyle = WS_EX_CONTROLPARENT;
   BRIG_HANDLE hDialog;
   PBRIG_WCHAR wcTitle = brig_str2WC( lpTitle );

   lStyle |= WS_VISIBLE;
   hDialog = CreateWindowEx( ExStyle, WC_DIALOG, wcTitle, lStyle,
         x, y,
         ( !width ) ? ( LONG ) CW_USEDEFAULT : width,
         ( !height ) ? ( LONG ) CW_USEDEFAULT : height,
         hMainWindow, NULL, ( HINSTANCE ) GetModuleHandle( NULL ), NULL );

   if( hDialog )
   {
      if( iDialogs == s_nDialogs )
      {
         s_nDialogs += 16;
         if( s_nDialogs == 16 )
            aDialogs = ( BRIG_HANDLE * ) malloc( sizeof( BRIG_HANDLE ) * s_nDialogs );
         else
            aDialogs = ( BRIG_HANDLE * ) realloc( aDialogs, sizeof( BRIG_HANDLE ) * s_nDialogs );
      }
      aDialogs[iDialogs++] = hDialog;
      SetWindowLongPtr( hDialog, GWLP_USERDATA, 0 );
   }

   brig_free( wcTitle );

   return hDialog;
}

void brig_ActivateDialog( brig_Dialog *pDialog )
{

   BRIG_HANDLE handle = pDialog->Handle();

   if( pDialog->bModal && pDialog->pParent )
      pDialog->pParent->Enable( 0 );

   wpOrigDlgProc = ( WNDPROC ) SetWindowLongPtr( handle,
         GWLP_WNDPROC, ( LONG_PTR ) s_DialogProc ); //DWLP_DLGPROC

   if( pDialog->bModal )
      brig_ModalDlgLoop( NULL, handle );
   else
      brig_MainLoop( NULL, 0 );
}

void brig_CloseWindow( brig_Widget *pWidget )
{
   SendMessage( pWidget->Handle(), WM_SYSCOMMAND, SC_CLOSE, 0 );
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iShowCmd )
{

   int argc;
   LPWSTR * lpArgv = CommandLineToArgvW( GetCommandLineW(), &argc );
   char ** argv;
   int iLen, i;
   INITCOMMONCONTROLSEX icc;

   SYMBOL_UNUSED( hInst );
   SYMBOL_UNUSED( hPrev );
   SYMBOL_UNUSED( lpCmdLine );
   SYMBOL_UNUSED( iShowCmd );

   argv = (char**) malloc( argc * sizeof(char*) );
   for( i = 0; i < argc; ++i ) {
      iLen = wcslen( lpArgv[i] );
      #if defined(UNICODE)
      argv[i] = brig_WCTostr( CP_UTF8, lpArgv[i], iLen );
      #else
      argv[i] = brig_WCTostr( CP_ACP, lpArgv[i], iLen );
      #endif
   }

   icc.dwSize = sizeof( INITCOMMONCONTROLSEX );
   icc.dwICC =
         ICC_DATE_CLASSES | ICC_INTERNET_CLASSES | ICC_BAR_CLASSES |
         ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES;
   InitCommonControlsEx( &icc );

   brig_Main( argc, argv );

   LocalFree( lpArgv );
   for( i = 0; i < argc; ++i )
      free( argv[i] );
   free( argv );

   if( aDialogs )
      free( aDialogs );

   return 0;

}
