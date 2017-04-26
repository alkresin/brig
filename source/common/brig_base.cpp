/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

brig_MainWindow * pMainWindow = NULL;

/* -------- Widget --------- */

brig_Widget::brig_Widget():lTextColor(-1), lBackColor(-1), hFont(NULL), hBrush(NULL), pfOnSize(NULL) {}

brig_Widget::~brig_Widget()
{
   if( hBrush )
      brigDelBrush( hBrush );
   //brig_writelog( NULL, "destruc %d\r\n", iWidgId );
}

bool brig_Widget::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
   SYMBOL_UNUSED( message );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );
   return 0;
}

BRIG_HANDLE brig_Widget::Handle( void ) const
{

   return handle;
}

void brig_Widget::SetText( PBRIG_CHAR lpTitle )
{

   brig_SetWindowText( handle, lpTitle );
}

PBRIG_CHAR brig_Widget::GetText( void ) const
{

   return brig_GetWindowText( handle );
}

void brig_Widget::New( brig_Container *pParent, int x, int y, int width, int height )
{

   iLeft = x;
   iTop = y;
   iWidth = width;
   iHeight = height;

   this->pParent = pParent;
   pParent->AddWidget( this );
   iWidgId = pParent->NewId();
}

void brig_Widget::SetFont( PBRIG_FONT hFontNew )
{
   hFont = hFontNew;
   brig_SetFont( handle, hFontNew );
}

long int brig_Widget::SetTextColor( long int lColor, bool bRepaint )
{
   long int lRet = lTextColor;

   if( lColor > -1 )
   {
      lTextColor = lColor;
      if( bRepaint )
         brig_RedrawWindow( handle );
   }

   return lRet;
}

long int  brig_Widget::SetBackColor( long int lColor, bool bRepaint )
{
   long int lRet = lBackColor;

   if( lColor > -1 )
   {
      lBackColor = lColor;
      if( hBrush )
        brig_DeleteObject( hBrush );
      hBrush = brigAddBrush( lBackColor );
      if( bRepaint )
         brig_RedrawWindow( handle );
   }

   return lRet;
}

void brig_Widget::Move( int iLeft, int iTop, int iWidth, int iHeight )
{
   brig_MoveWindow( handle, iLeft, iTop, iWidth, iHeight );
}

void brig_Widget::Enable( bool bEnable )
{
   brig_EnableWindow( handle, bEnable );
}

void brig_Widget::Show( bool bShow )
{
   brig_ShowWindow( handle, bShow );
}

/* -------- Container --------- */

brig_Container::brig_Container():brig_Widget(), iIdCount(100) {};

brig_Container::~brig_Container()
{
   //brig_Widget::~brig_Widget();
}

int brig_Container::NewId()
{
   return ++iIdCount;
}

void brig_Container::AddWidget( brig_Widget *pWidget )
{
   avWidgets.push_back( pWidget );
}

void brig_Container::OnSize( WPARAM wParam, LPARAM lParam )
{
   //unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;
   //unsigned long iHeight = ( ((unsigned long)lParam) >> 16 ) & 0xFFFF;

   for ( unsigned int i = 0; i < avWidgets.size(); i++ )
      if( avWidgets[i]->pfOnSize )
         avWidgets[i]->pfOnSize( avWidgets[i], wParam, lParam );
}

/* -------- MainWindow --------- */

brig_MainWindow::brig_MainWindow():brig_Container(), szAppName((PBRIG_CHAR)"Brig_App") {}

void brig_MainWindow::New( int x, int y, int width, int height, PBRIG_CHAR lpTitle,
         long int lStyle, PBRIG_ICON hIcon, PBRIG_BRUSH hBrush )
{
   iLeft = x;
   iTop = y;
   iWidth = width;
   iHeight = height;

   handle = brig_InitMainWindow( szAppName, lpTitle, x, y, width, height, lStyle, hIcon, hBrush );
   if( handle )
   {
      brig_SetWidgetData( this );
      pMainWindow = this;
   }
}

void brig_MainWindow::Activate( void )
{

   brig_ActivateMainWindow( 1, NULL, 0 );
}

bool brig_MainWindow::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   int iParLow;

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   switch( message ) {

      case WM_SIZE:
         if( pfOnSize )
            pfOnSize( this, wParam, lParam );
         brig_Container::OnSize( wParam, lParam );
         break;

      case WM_COMMAND:
         {
            PBRIG_MITEM pMenuCurr;
            iParLow =  ( (unsigned long) wParam ) & 0xFFFF;
            if( pMenu && ( pMenuCurr = brig_MenuItemFind( pMenu, iParLow ) ) != NULL && pMenuCurr->pfMenuItem ) 
               pMenuCurr->pfMenuItem();
            break;
         }

      case WM_SYSCOMMAND:
         if( wParam == SC_CLOSE && pfOnClose && !pfOnClose( this ) )
            return 1;
         break;
   }
   
   return 0;
}

void brig_MainWindow::Close( void )
{

   brig_CloseWindow( handle );
}

/* -------- Dialog --------- */

brig_Dialog::brig_Dialog():brig_Container(), pResult(NULL) {}


void brig_Dialog::New( brig_Container * pParent,
      int x, int y, int width, int height, PBRIG_CHAR lpTitle, long int lStyle )
{
   iLeft = x;
   iTop = y;
   iWidth = width;
   iHeight = height;

   this->pParent = (pParent)? pParent : ( (pMainWindow)? pMainWindow : NULL );
   handle = brig_InitDialog( lpTitle, x, y, width, height, lStyle );
   brig_SetWidgetData( this );
}

void brig_Dialog::Activate( bool bMode )
{

   bModal = bMode;
   if( bModal && pParent )
      pParent->Enable( 0 );

   brig_ActivateDialog( bModal );
}

bool brig_Dialog::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "DialogOnEvent-0 %d\r\n", message );
   switch( message ) {

      case WM_SIZE:
         if( pfOnSize )
            pfOnSize( this, wParam, lParam );
         brig_Container::OnSize( wParam, lParam );
         break;

      case WM_DESTROY:
         if( bModal && pParent )
         {
            pParent->Enable();
         }
         break;
   }
   
   return 0;
}

void brig_Dialog::Close( void )
{

   brig_CloseWindow( handle );
}
