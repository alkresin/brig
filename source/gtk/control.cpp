/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Standard Widgets creation and manipulation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

/* -------- Label ---------
 */
BRIG_HANDLE brig_CreateLabel( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle )
{

   GtkWidget *hCtrl, *hLabel;
   GtkFixed *box;

   if( ( ulStyle & SS_OWNERDRAW ) == SS_OWNERDRAW )
      hCtrl = gtk_drawing_area_new();
   else
   {
      hCtrl = gtk_event_box_new(  );
      hLabel = gtk_label_new( lpCaption );
      gtk_container_add( GTK_CONTAINER( hCtrl ), hLabel );
      g_object_set_data( ( GObject * ) hCtrl, "label", ( gpointer ) hLabel );
      if( ulExtStyle && WS_EX_TRANSPARENT )
      {
         gtk_event_box_set_visible_window( GTK_EVENT_BOX( hCtrl ), 0 );
      }

      if( !( ulStyle & SS_CENTER ) )
         gtk_misc_set_alignment( GTK_MISC( hLabel ),
               ( ulStyle & SS_RIGHT ) ? 1 : 0, 0 );
   }
   box = getFixedBox( ( GObject * ) hParentWindow );
   if( box )
      gtk_fixed_put( box, hCtrl, x, y );
   gtk_widget_set_size_request( hCtrl, nWidth, nHeight );

   if( ( ulStyle & SS_OWNERDRAW ) == SS_OWNERDRAW )
   {
      set_event( ( gpointer ) hCtrl, "expose_event", WM_PAINT, 0, 0 );
   }
   return hCtrl;

}

/* -------- Button ---------
 */

HWND brig_CreateButton( HWND hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_CHAR lpCaption )
{
/*
   CreateButton( hParentWindow, nButtonID, nStyle, x, y, nWidth, nHeight,
                 cCaption )
*/
   GtkWidget *hCtrl, *img;
   GtkFixed *box;

   if( ( ulStyle & 0xf ) == BS_AUTORADIOBUTTON )
   {
      //GSList *group = ( GSList * ) HB_PARHANDLE( 2 );
      //hCtrl = gtk_radio_button_new_with_label( group, lpCaption );
      //group = gtk_radio_button_get_group( ( GtkRadioButton * ) hCtrl );
      //HB_STOREHANDLE( group, 2 );
   }
   else if( ( ulStyle & 0xf ) == BS_AUTOCHECKBOX )
      hCtrl = gtk_check_button_new_with_label( lpCaption );
   else if( ( ulStyle & 0xf ) == BS_GROUPBOX )
      hCtrl = gtk_frame_new( lpCaption );
   else
      hCtrl = gtk_button_new_with_mnemonic( lpCaption );

   box = getFixedBox( ( GObject * ) hParentWindow );
   if( box )
      gtk_fixed_put( box, hCtrl, x, y );
   gtk_widget_set_size_request( hCtrl, nWidth, nHeight );

   /*
   if( hBtn )
   {
      LONG_PTR hProc;
      SetWindowLongPtr( hBtn, GWLP_USERDATA, NULL );
      hProc = SetWindowLongPtr( hBtn, GWLP_WNDPROC, ( LONG_PTR ) s_BtnProc );
      if( !wpOrigBtnProc )
         wpOrigBtnProc = ( WNDPROC ) hProc;
   }
   */
   return hCtrl;

}

bool brig_CheckBtnGet( HWND handle )
{
   return SendMessage( handle, BM_GETCHECK, 0, 0 );
}

void brig_CheckBtnSet( HWND handle, bool bValue )
{
   SendMessage( handle, BM_SETCHECK, bValue, 0 );
}

bool brig_RadioBtnGet( HWND handle )
{
   return SendMessage( handle, BM_GETCHECK, 0, 0 );
}

void brig_RadioBtnSet( HWND handle, bool bValue )
{
   SendMessage( handle, BM_SETCHECK, (bValue)? BST_CHECKED : BST_UNCHECKED, 0 );
}


/* -------- Edit --------- 
 */
static LRESULT CALLBACK s_EditProc( HWND hDlg, UINT message,
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

HWND brig_CreateEdit( HWND hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle )
{
   HWND hEdit;

   hEdit = CreateWindowEx( ulExStyle,
         TEXT( "EDIT" ),
         NULL,
         WS_CHILD | WS_VISIBLE | ulStyle,
         x, y, nWidth, nHeight,
         hParentWindow,                   /* parent window    */
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

/* -------- Panel --------- 
 */

static LRESULT CALLBACK s_PanelProc( HWND hDlg, UINT message,
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

HWND brig_CreatePanel( HWND hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{
   HWND hPanel;

   reg_Panel();
   hPanel = CreateWindow( TEXT( "PANEL" ),
         NULL,                         /* no window title   */
         WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_OWNERDRAW | CCS_TOP,
         x, y, nWidth, nHeight,
         hParentWindow,                /* parent window    */
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

static LRESULT CALLBACK s_QButtonProc( HWND handle, UINT message,
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

HWND brig_CreateQButton( HWND hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{
   HWND hQButton;

   reg_QButton();
   hQButton = CreateWindow( TEXT( "QBUTTON" ),
         NULL,                          /* no window title   */
         WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_OWNERDRAW | CCS_TOP,
         x, y, nWidth, nHeight,
         hParentWindow,                   /* parent window    */
         ( HMENU ) iWidgId,            /* button       ID  */
         GetModuleHandle( NULL ), NULL );

   if( hQButton )
   {
      SetWindowLongPtr( hQButton, GWLP_USERDATA, NULL );
   }

   return hQButton;
}
