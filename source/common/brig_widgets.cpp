/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"


/* -------- Label --------- */
brig_Label::brig_Label():brig_Widget()
{
   uiType = TYPE_LABEL;
}

BRIG_HANDLE brig_Label::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle, unsigned long ulExStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );
   
   handle = brig_CreateLabel( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle, lpCaption, ulExStyle );
   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );

   return handle;
}

/* -------- Edit --------- */
brig_Edit::brig_Edit():brig_Widget()
{
   uiType = TYPE_EDIT;
}

BRIG_HANDLE brig_Edit::New( brig_Container * pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle, unsigned long ulExStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateEdit( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle, lpCaption, ulExStyle );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
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
brig_Button::brig_Button():brig_Widget(), pfOnClick(NULL)
{
   uiType = TYPE_BUTTON;
}

BRIG_HANDLE brig_Button::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle, lpCaption );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
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
brig_CheckButton::brig_CheckButton():brig_Widget(), pfOnClick(NULL)
{
   uiType = TYPE_CHECK;
}

BRIG_HANDLE brig_CheckButton::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle | BS_AUTOCHECKBOX, lpCaption );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
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


/* -------- Radio --------- */

brig_RadioGroup::brig_RadioGroup(): pBox(NULL){}

brig_RadioGroup::~brig_RadioGroup()
{
   if( pBox )
      delete pBox;
}

void brig_RadioGroup::Begin( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption )
{
   this->pParent = pParent;

   if( x > 0 && y > 0 && nWidth > 0 && nHeight > 0 )
   {
      pBox = new brig_GroupBox;
      pBox->New( pParent, x, y, nWidth, nHeight, lpCaption );
   }
}

void brig_RadioGroup::End( int iSelected )
{
   brig_RadioGroupSet( this, iSelected );
   iValue = iSelected;
}

int brig_RadioGroup::GetValue( void )
{
   return iValue;
}

void brig_RadioGroup::SetValue( int iSelected )
{
   brig_RadioGroupSet( this, iSelected );
   iValue = iSelected;
}

brig_RadioButton::brig_RadioButton():brig_Widget(), pfOnClick(NULL)
{
   uiType = TYPE_RADIO;
}

BRIG_HANDLE brig_RadioButton::New( brig_RadioGroup *pGroup,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::New( (brig_Container *) (pGroup->pParent), x, y, nWidth, nHeight );
   this->pGroup = pGroup;

   if( pGroup->avButtons.empty() )
      ulStyle |= WS_GROUP;
   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle | BS_AUTORADIOBUTTON, lpCaption );

   pGroup->avButtons.push_back( this );
   iOrder = pGroup->avButtons.size();

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   brig_SetWidgetData( this );

   return handle;
}

bool brig_RadioButton::GetValue()
{
   return brig_RadioBtnGet( handle );
}

bool brig_RadioButton::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "radio_onEvent %u\r\n", message );
   switch( message ) {

      case WM_LBUTTONUP:
         pGroup->iValue = iOrder;
         if( pfOnClick )
            pfOnClick( this, wParam, lParam );
         break;
   }
   
   return 0;
}


/* -------- GroupBox --------- */
brig_GroupBox::brig_GroupBox():brig_Widget()
{
   uiType = TYPE_GROUP;
}

BRIG_HANDLE brig_GroupBox::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, BS_GROUPBOX, lpCaption );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   return handle;
}

/* -------- Combobox --------- */
brig_Combo::brig_Combo():brig_Widget()
{
   uiType = TYPE_COMBO;
}

BRIG_HANDLE brig_Combo::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, char **pArray, int iLen )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateCombo( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle, pArray, iLen );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   brig_SetWidgetData( this );

   return handle;
}

void brig_Combo::Set( char **pArray, int iLen )
{
   brig_ComboSetArray( handle, pArray, iLen );
}

int  brig_Combo::GetValue( void )
{
   return brig_GetValue( handle );
}

void brig_Combo::SetValue( int iSelected )
{
   brig_SetValue( handle, iSelected );
}


/* -------- Panel --------- */
brig_Panel::brig_Panel():brig_Container(), pfOnPaint(NULL)
{
   uiType = TYPE_PANEL;
   lBackColor = COLOR_GR_LIGHT;
}

brig_Panel::~brig_Panel()
{
   //brig_Container::~brig_Container();
}

BRIG_HANDLE brig_Panel::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, brig_Style *ps )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreatePanel( pParent->Handle(), iWidgId, x, y, nWidth, nHeight );

   pStyle = ps;
   if( !hFont )
      hFont = pParent->hFont;
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
brig_QButton::brig_QButton():brig_Widget(), pfOnPaint(NULL), pfOnClick(NULL), iState(0), lBackClr1(COLOR_GR_LIGHT), lBackClr2(COLOR_WHITE), hBrush1(NULL), hBrush2(NULL)
{
   uiType = TYPE_QBTN;
   lBackColor = COLOR_GR_LIGHT;
}

brig_QButton::~brig_QButton()
{
   //brig_Widget::~brig_Widget();
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

   if( !hFont )
      hFont = pParent->hFont;
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
         brig_RedrawWindow( handle );
         break;

      case WM_LBUTTONUP:
         if( iState == 2 )
         {
            iState = 1;
            brig_RedrawWindow( handle );
            if( pfOnClick )
               pfOnClick( this, wParam, lParam );
         }
         break;
   }
  
   return 0;
}
