/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"


/* -------- Label --------- */
brig_Label::brig_Label():brig_Widget(){}

BRIG_HANDLE brig_Label::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle, unsigned long ulExStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );
   
   handle = brig_CreateLabel( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle, lpCaption, ulExStyle );

   return handle;
}

/* -------- Edit --------- */
brig_Edit::brig_Edit():brig_Widget(){}

BRIG_HANDLE brig_Edit::New( brig_Container * pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle, unsigned long ulExStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateEdit( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle, lpCaption, ulExStyle );

   brig_SetWidgetData( this );

   return handle;
}

bool brig_Edit::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( message );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "edit_onEvent %u\r\n", message );

   return 0;
}

/* -------- Button --------- */
brig_Button::brig_Button():brig_Widget(), pfOnClick(NULL){}

BRIG_HANDLE brig_Button::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle, lpCaption );

   brig_SetWidgetData( this );

   return handle;
}

bool brig_Button::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "btn_onEvent %u\r\n", message );
   switch( message ) {

      case WM_LBUTTONUP:
         //brig_writelog( NULL, "btn_onClick: %lu %lu %d %s\r\n", (unsigned long)this, (unsigned long)handle, iWidgId, GetText() );
         if( pfOnClick )
            pfOnClick( this, wParam, lParam );
         break;
   }
   
   return 0;
}


/* -------- CheckButton --------- */
brig_CheckButton::brig_CheckButton():brig_Widget(), pfOnClick(NULL){}

BRIG_HANDLE brig_CheckButton::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle | BS_AUTOCHECKBOX, lpCaption );

   brig_SetWidgetData( this );

   return handle;
}

bool brig_CheckButton::GetValue()
{
   return brig_CheckBtnGet( handle );
}

void brig_CheckButton::SetValue( bool bValue )
{
   return brig_CheckBtnSet( handle, bValue );
}

bool brig_CheckButton::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "btn_onEvent %u\r\n", message );
   switch( message ) {

      case WM_LBUTTONUP:
         if( pfOnClick )
            pfOnClick( this, wParam, lParam );
         break;
   }
   
   return 0;
}


/* -------- RadioButton --------- */
brig_RadioButton::brig_RadioButton():brig_Widget(), pfOnClick(NULL){}

BRIG_HANDLE brig_RadioButton::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle | BS_AUTORADIOBUTTON, lpCaption );

   brig_SetWidgetData( this );

   return handle;
}

bool brig_RadioButton::GetValue()
{
   return brig_RadioBtnGet( handle );
}

void brig_RadioButton::SetValue( bool bValue )
{
   return brig_RadioBtnSet( handle, bValue );
}

bool brig_RadioButton::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "btn_onEvent %u\r\n", message );
   switch( message ) {

      case WM_LBUTTONUP:
         if( pfOnClick )
            pfOnClick( this, wParam, lParam );
         break;
   }
   
   return 0;
}


/* -------- GroupBox --------- */
brig_GroupBox::brig_GroupBox():brig_Widget(){}

BRIG_HANDLE brig_GroupBox::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, BS_GROUPBOX, lpCaption );

   return handle;
}


/* -------- Panel --------- */
brig_Panel::brig_Panel():brig_Container(), pfOnPaint(NULL) { lBackColor = COLOR_GR_LIGHT; }

brig_Panel::~brig_Panel()
{
   brig_Container::~brig_Container();
}

BRIG_HANDLE brig_Panel::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, brig_Style *ps )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreatePanel( pParent->Handle(), iWidgId, x, y, nWidth, nHeight );

   pStyle = ps;
   brig_SetWidgetData( this );

   return handle;
}

static void brig_paint_Panel( brig_Panel *pPanel )
{
   BRIG_HANDLE hPanel = pPanel->Handle();
   PBRIG_PPS pps = brig_BeginPaint( hPanel );
   RECT rc;

   brig_GetClientRect( hPanel, &rc );

   if( pPanel->pStyle )
      pPanel->pStyle->Draw( pps->hDC, rc.left, rc.top, rc.right, rc.bottom );
   else
   {
      if( !pPanel->hBrush )
         pPanel->hBrush = brigAddBrush( pPanel->lBackColor );
      brig_FillRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom, pPanel->hBrush );
   }

   brig_EndPaint( hPanel, pps );

}

bool brig_Panel::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "panel_onEvent %u\r\n", message );
   switch( message ) {

      case WM_PAINT:

         if( pfOnPaint )
         {
            PBRIG_PPS pps = brig_BeginPaint( handle );

            pfOnPaint( this, pps->hDC );

            brig_EndPaint( handle, pps );
         }
         else
            brig_paint_Panel( this );

         break;
   }
  
   return 0;
}

/* -------- QButton --------- */
brig_QButton::brig_QButton():brig_Widget(), pfOnPaint(NULL), pfOnClick(NULL), iState(0), lBackClr1(COLOR_GR_LIGHT), lBackClr2(COLOR_WHITE), hBrush1(NULL), hBrush2(NULL) { lBackColor = COLOR_GR_LIGHT; }

brig_QButton::~brig_QButton()
{
   brig_Widget::~brig_Widget();
   if( hBrush1 )
      brigDelBrush( hBrush1 );
   if( hBrush2 )
      brigDelBrush( hBrush2 );

}

BRIG_HANDLE brig_QButton::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption,
          brig_Style *psNormal, brig_Style *psOver, brig_Style *psPress )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );
   szCaption = lpCaption;
   handle = brig_CreateQButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight );

   pStyleNormal = psNormal;
   pStyleOver = psOver;
   pStylePress = psPress;

   brig_SetWidgetData( this );

   return handle;
}

static void brig_paint_QButton( brig_QButton *pQButton )
{
   BRIG_HANDLE hQButton = pQButton->Handle();
   PBRIG_PPS pps = brig_BeginPaint( hQButton );
   RECT rc;

   if( pQButton->pStyleNormal )
   {
      if( !pQButton->pStyleOver )
         pQButton->pStyleOver = pQButton->pStyleNormal;
      if( !pQButton->pStylePress )
         pQButton->pStylePress = pQButton->pStyleOver;
      if( pQButton->iState == 0 )
         pQButton->pStyleNormal->Draw( pps->hDC, rc.left, rc.top, rc.right, rc.bottom );
      else if( pQButton->iState == 1 )
         pQButton->pStyleOver->Draw( pps->hDC, rc.left, rc.top, rc.right, rc.bottom );
      else if( pQButton->iState == 2 )
         pQButton->pStylePress->Draw( pps->hDC, rc.left, rc.top, rc.right, rc.bottom );
   }
   else
   {
      if( !pQButton->hBrush )
         pQButton->hBrush = brigAddBrush( pQButton->lBackColor );
      if( !pQButton->hBrush1 )
         pQButton->hBrush1 = brigAddBrush( pQButton->lBackClr1 );
      if( !pQButton->hBrush2 )
         pQButton->hBrush2 = brigAddBrush( pQButton->lBackClr2 );

      brig_GetClientRect( hQButton, &rc );

      brig_FillRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom,
         ( (pQButton->iState == 2)? pQButton->hBrush2 : ( (pQButton->iState == 1)? pQButton->hBrush1 : pQButton->hBrush ) ) );

      if( pQButton->lTextColor >= 0 )
         brig_SetTextColor( pps->hDC, pQButton->lTextColor );

      if( pQButton->iState > 0 )
            brig_DrawRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom );
   }

   if( pQButton->szCaption )
   {
      if( pQButton->hFont )
         brig_SelectObject( pps->hDC, pQButton->hFont );
      brig_SetTransparentMode( pps->hDC, 1 );
      brig_DrawText( pps->hDC, pQButton->szCaption, rc.left, rc.top, rc.right,
         rc.bottom, DT_SINGLELINE | DT_VCENTER | DT_CENTER );
      brig_SetTransparentMode( pps->hDC, 0 );
   }

   brig_EndPaint( hQButton, pps );

}

bool brig_QButton::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   //brig_writelog( NULL, "panel_onEvent %u\r\n", message );
   switch( message ) {

      case WM_PAINT:

         if( pfOnPaint )
         {
            PBRIG_PPS pps = brig_BeginPaint( handle );

            pfOnPaint( this, pps->hDC );

            brig_EndPaint( handle, pps );
         }
         else
            brig_paint_QButton( this );

         break;

      case WM_LBUTTONDOWN:
         iState = 2;
         InvalidateRect( handle, NULL, 1 );
         break;

      case WM_LBUTTONUP:
         if( iState == 2 )
         {
            iState = 1;
            InvalidateRect( handle, NULL, 1 );
            if( pfOnClick )
               pfOnClick( this, wParam, lParam );
         }
         break;
   }
  
   return 0;
}
