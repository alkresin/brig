/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

/* -------- Widget --------- */

brig_Widget::brig_Widget():lTextColor(0), lBackColor(-1), hFont(NULL), hBrush(NULL), pfOnSize(NULL), pfOnFocusIn(NULL), pfOnFocusOut(NULL) {}

brig_Widget::~brig_Widget()
{
   if( hBrush )
      brigDelBrush( hBrush );
   //brig_writelog( NULL, "destruc %d\r\n", iWidgId );
}

bool brig_Widget::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   switch( message ) {

      case WM_SETFOCUS:
         if( pfOnFocusIn && !pfOnFocusIn( this ) )
            return 1;
         break;

      case WM_KILLFOCUS:
         if( pfOnFocusOut && !pfOnFocusOut( this ) )
            return 1;
         break;
   }

   return 0;
}

BRIG_HANDLE brig_Widget::Handle( void ) const
{

   return handle;
}

void brig_Widget::SetText( PBRIG_CHAR lpTitle )
{

   brig_SetWindowText( this, lpTitle );
}

PBRIG_CHAR brig_Widget::GetText( void )
{

   return brig_GetWindowText( this );
}

void brig_Widget::Create( brig_Container *pParent, int x, int y, int width, int height )
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
   brig_SetFont( this, hFontNew );
}

long int brig_Widget::SetTextColor( long int lColor, bool bRepaint )
{
   long int lRet = lTextColor;

   if( lColor > -1 && lTextColor != lColor )
   {
      lTextColor = lColor;
      brig_SetFgColor( this, lColor );
      if( bRepaint )
         brig_RedrawWindow( this );
   }

   return lRet;
}

long int  brig_Widget::SetBackColor( long int lColor, bool bRepaint )
{
   long int lRet = lBackColor;

   if( lColor > -1 && lBackColor != lColor )
   {
      lBackColor = lColor;
      if( hBrush )
        brigDelBrush( hBrush );
      hBrush = brigAddBrush( lColor );
      brig_SetBgColor( this, lColor );
      if( bRepaint )
         brig_RedrawWindow( this );
   }

   return lRet;
}

void brig_Widget::Move( int iLeft, int iTop, int iWidth, int iHeight )
{
   if( iLeft >= 0 )
      this->iLeft = iLeft;
   if( iTop >= 0 )
      this->iTop = iTop;
   if( iWidth >= 0 )
      this->iWidth = iWidth;
   if( iHeight >= 0 )
      this->iHeight = iHeight;

   brig_MoveWindow( this, iLeft, iTop, iWidth, iHeight );
}

void brig_Widget::Enable( bool bEnable )
{
   brig_EnableWindow( this, bEnable );
}

void brig_Widget::Show( bool bShow )
{
   brig_ShowWindow( this, bShow );
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

brig_MainWindow::brig_MainWindow():brig_Container(), szAppName((PBRIG_CHAR)"Brig_App")
{
   uiType = TYPE_WINDOW;
}

void brig_MainWindow::Create( int x, int y, int width, int height, PBRIG_CHAR lpTitle,
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
      brigApp.pMainWindow = this;
   }
}

void brig_MainWindow::Activate( void )
{

   brig_ActivateMainWindow( 1, NULL, 0 );
}

bool brig_MainWindow::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   int iParLow;

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

      default:
         return brig_Widget::onEvent( message, wParam, lParam );
   }
   
   return 0;
}

void brig_MainWindow::Close( void )
{

   brig_CloseWindow( this );
}

/* -------- Dialog --------- */

brig_Dialog::brig_Dialog():brig_Container(), pResult(NULL)
{
   uiType = TYPE_DIALOG;
}

void brig_Dialog::Create( brig_Container * pParent,
      int x, int y, int width, int height, PBRIG_CHAR lpTitle, long int lStyle )
{
   iLeft = x;
   iTop = y;
   iWidth = width;
   iHeight = height;

   this->pParent = (pParent)? pParent : ( (brigApp.pMainWindow)? brigApp.pMainWindow : NULL );
   handle = brig_InitDialog( lpTitle, x, y, width, height, lStyle );
   brig_SetWidgetData( this );
}

void brig_Dialog::Activate( bool bMode )
{

   bModal = bMode;
   brig_ActivateDialog( this );
}

bool brig_Dialog::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   //brig_writelog( NULL, "DialogOnEvent-0 %d\r\n", message );
   switch( message ) {

      case WM_SIZE:
         if( pfOnSize )
            pfOnSize( this, wParam, lParam );
         brig_Container::OnSize( wParam, lParam );
         break;

      default:
         return brig_Widget::onEvent( message, wParam, lParam );
   }
   
   return 0;
}

void brig_Dialog::Close( void )
{

   brig_CloseWindow( this );
}
