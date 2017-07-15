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

BRIG_HANDLE brig_Label::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle, unsigned long ulExStyle )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );
   
   handle = brig_CreateLabel( (brig_Widget*) this, iWidgId,
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

BRIG_HANDLE brig_Edit::Create( brig_Container * pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle, unsigned long ulExStyle )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateEdit( this, iWidgId,
             x, y, nWidth, nHeight, ulStyle, lpCaption, ulExStyle );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   brig_SetWidgetData( this );

   return handle;
}

bool brig_Edit::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   return brig_Widget::onEvent( message, wParam, lParam );

}

/* -------- Button --------- */
brig_Button::brig_Button():brig_Widget(), pfOnClick(NULL)
{
   uiType = TYPE_BUTTON;
}

BRIG_HANDLE brig_Button::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( this, iWidgId,
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
            pfOnClick( this );
         break;
   }
   
   return 0;
}


/* -------- CheckButton --------- */
brig_CheckButton::brig_CheckButton():brig_Widget(), pfOnClick(NULL)
{
   uiType = TYPE_CHECK;
}

BRIG_HANDLE brig_CheckButton::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( this, iWidgId,
             x, y, nWidth, nHeight, ulStyle | BS_AUTOCHECKBOX, lpCaption );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   brig_SetWidgetData( this );

   return handle;
}

bool brig_CheckButton::GetValue()
{
   return brig_CheckBtnGet( this );
}

void brig_CheckButton::SetValue( bool bValue )
{
   return brig_CheckBtnSet( this, bValue );
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
      pBox->Create( pParent, x, y, nWidth, nHeight, lpCaption );
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

BRIG_HANDLE brig_RadioButton::Create( brig_RadioGroup *pGroup,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle )
{

   brig_Widget::Create( (brig_Container *) (pGroup->pParent), x, y, nWidth, nHeight );
   this->pGroup = pGroup;

   if( pGroup->avButtons.empty() )
      ulStyle |= WS_GROUP;
   handle = brig_CreateButton( this, iWidgId,
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
   return brig_RadioBtnGet( this );
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

BRIG_HANDLE brig_GroupBox::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateButton( this, iWidgId,
             x, y, nWidth, nHeight, BS_GROUPBOX, lpCaption );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   return handle;
}

/* -------- Combobox --------- */
brig_Combo::brig_Combo():brig_Widget(), pfOnChange(NULL)
{
   uiType = TYPE_COMBO;
}

BRIG_HANDLE brig_Combo::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, int iDisplay, bool bEdit, char **pArray, int iLen )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateCombo( this, iWidgId, x, y, nWidth, nHeight, iDisplay, bEdit );

   if( pArray && iLen > 0 )
      brig_ComboSetArray( this, pArray, iLen );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   brig_SetWidgetData( this );

   return handle;
}

bool brig_Combo::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "btn_onEvent %u\r\n", message );
   switch( message ) {

      case CBN_SELCHANGE:
         //brig_writelog( NULL, "combo_onChange: %lu %lu\r\n", (unsigned long)wParam, (unsigned long)lParam );
         if( pfOnChange )
            pfOnChange( this );
         break;
   }
   
   return 0;
}

void brig_Combo::Set( char **pArray, int iLen )
{
   brig_ComboSetArray( this, pArray, iLen );
}

int  brig_Combo::GetValue( void )
{
   return brig_ComboGetValue( this );
}

void brig_Combo::SetValue( int iSelected )
{
   brig_ComboSetValue( this, iSelected );
}


/* -------- Tab --------- */
brig_Tab::brig_Tab():brig_Container(), pfOnChange(NULL), iCurrPage(0)
{
   uiType = TYPE_TAB;
}

BRIG_HANDLE brig_Tab::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateTab( this, iWidgId, x, y, nWidth, nHeight );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   brig_SetWidgetData( this );

   return handle;
}

void brig_Tab::StartPage( PBRIG_CHAR lpName )
{
   BRIG_TABPAGE tp;

   tp.iFirst = (int) (avWidgets.size());
   tp.iLast = -1;
   brig_TabAddPage( this, avPages.size(), lpName );
   avPages.push_back( tp );
}

void brig_Tab::EndPage( void )
{
   PBRIG_TABPAGE ptp = &*avPages.rbegin();

   if( ptp->iFirst == (int) (avWidgets.size()) )
      ptp->iFirst = -1;
   else
   {
      ptp->iLast = avWidgets.size() - 1;
      if( avPages.size() > 1 )
         brig_TabShowPage( this, avPages.size()-1, 0 );
   }
}

bool brig_Tab::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );

   //brig_writelog( NULL, "tab_onEvent %u\r\n", message );
   switch( message ) {

      case WM_USER:
         brig_TabShowPage( this, iCurrPage, 0 );
         iCurrPage = (int) lParam;
         brig_TabShowPage( this, iCurrPage, 1 );
         if( pfOnChange )
            pfOnChange( this );
         break;

   }
   
   return 0;
}

/* -------- Picture --------- */

brig_Picture::brig_Picture():brig_Widget(), pfOnPaint(NULL), pfOnClick(NULL), pfOnDblClick(NULL), pfOnRClick(NULL)
{
   uiType = TYPE_PICTURE;
}

BRIG_HANDLE brig_Picture::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_BITMAP pBitmap )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );
   
   handle = brig_CreateLabel( (brig_Widget*) this, iWidgId, x, y, nWidth, nHeight, SS_OWNERDRAW, NULL, 0 );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );

   this->pBitmap = pBitmap;
   brig_SetWidgetData( this );

   return handle;
}

static void brig_paint_Picture( brig_Picture *pPicture )
{
   PBRIG_PPS pps = brig_BeginPaint( pPicture );
   RECT rc;

   brig_GetClientRect( pPicture, &rc );

   if( pPicture->pBitmap )
      brig_DrawBitmap( pps->hDC, pPicture->pBitmap, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top );

   brig_EndPaint( pPicture, pps );

}

bool brig_Picture::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "Picture_onEvent %u\r\n", message );
   switch( message ) {

      case WM_PAINT:

         if( pfOnPaint )
         {
            PBRIG_PPS pps = brig_BeginPaint( this );

            pfOnPaint( this, pps->hDC );

            brig_EndPaint( this, pps );
         }
         else
            brig_paint_Picture( this );

         break;

      case WM_LBUTTONDOWN:
         if( pfOnClick )
            pfOnClick( this );
         break;

      case WM_LBUTTONUP:
         break;

      case WM_LBUTTONDBLCLK:
         if( pfOnDblClick )
            pfOnDblClick( this );
         break;

      case WM_RBUTTONDOWN:
         if( pfOnRClick )
            pfOnRClick( this );
         break;

   }
  
   return 0;
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

BRIG_HANDLE brig_Panel::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, brig_Style *ps )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreatePanel( this, iWidgId, x, y, nWidth, nHeight );

   pStyle = ps;
   if( !hFont )
      hFont = pParent->hFont;
   brig_SetWidgetData( this );

   return handle;
}

static void brig_paint_Panel( brig_Panel *pPanel )
{
   PBRIG_PPS pps = brig_BeginPaint( pPanel );
   RECT rc;

   brig_GetClientRect( pPanel, &rc );

   if( pPanel->pStyle )
      pPanel->pStyle->Draw( pps->hDC, rc.left, rc.top, rc.right, rc.bottom );
   else
   {
      if( !pPanel->hBrush )
         pPanel->hBrush = brigAddBrush( pPanel->lBackColor );
      brig_FillRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom, pPanel->hBrush );
   }

   brig_EndPaint( pPanel, pps );

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
            PBRIG_PPS pps = brig_BeginPaint( this );

            pfOnPaint( this, pps->hDC );

            brig_EndPaint( this, pps );
         }
         else
            brig_paint_Panel( this );

         break;
   }
  
   return 0;
}

/* -------- QButton --------- */
brig_QButton::brig_QButton():brig_Widget(), pfOnPaint(NULL), pfOnClick(NULL), iState(0), lBackClr1(COLOR_GR_LIGHT), lBackClr2(COLOR_WHITE), hBrush1(NULL), hBrush2(NULL), hPen(NULL)
{
   uiType = TYPE_QBTN;
   lBackColor = COLOR_GR_LIGHT;
}

brig_QButton::~brig_QButton()
{
   if( hBrush1 )
      brigDelBrush( hBrush1 );
   if( hBrush2 )
      brigDelBrush( hBrush2 );
   if( hPen )
      brigDelPen( hPen );

}

BRIG_HANDLE brig_QButton::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption,
          brig_Style *psNormal, brig_Style *psOver, brig_Style *psPress )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );
   szCaption = lpCaption;
   handle = brig_CreateQButton( this, iWidgId,
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
   PBRIG_PPS pps = brig_BeginPaint( pQButton );
   RECT rc;

   brig_GetClientRect( pQButton, &rc );

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

      brig_FillRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom,
         ( (pQButton->iState == 2)? pQButton->hBrush2 : ( (pQButton->iState == 1)? pQButton->hBrush1 : pQButton->hBrush ) ) );

      if( pQButton->lTextColor >= 0 )
         brig_SetTextColor( pps->hDC, pQButton->lTextColor );

      if( pQButton->iState > 0 )
      {
         if( !pQButton->hPen )
            pQButton->hPen = brigAddPen();
         brig_SelectObject( pps->hDC, pQButton->hPen );
         brig_DrawRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom );
      }
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

   brig_EndPaint( pQButton, pps );

}

bool brig_QButton::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   //brig_writelog( NULL, "panel_onEvent %u\r\n", message );
   switch( message ) {

      case WM_PAINT:

         if( pfOnPaint )
         {
            PBRIG_PPS pps = brig_BeginPaint( this );

            pfOnPaint( this, pps->hDC );

            brig_EndPaint( this, pps );
         }
         else
            brig_paint_QButton( this );

         break;

      case WM_LBUTTONDOWN:
         iState = 2;
         brig_RedrawWindow( this );
         break;

      case WM_LBUTTONUP:
         if( iState == 2 )
         {
            iState = 1;
            brig_RedrawWindow( this );
            if( pfOnClick )
               pfOnClick( this, wParam, lParam );
         }
         break;
   }
  
   return 0;
}


/* -------- Splitter --------- */
brig_Splitter::brig_Splitter():brig_Widget(), pfOnPaint(NULL), lColorInside(0x999999), hPenEdge(NULL), hPenInside(NULL), hCursor(NULL)
{
   uiType = TYPE_SPLITTER;
}

brig_Splitter::~brig_Splitter()
{
   if( hPenEdge )
      brigDelPen( hPenEdge );
   if( hPenInside )
      brigDelPen( hPenInside );
   hPenEdge = hPenInside = NULL;

}

BRIG_HANDLE brig_Splitter::Create( brig_Container *pParent, int x, int y, int nWidth,
         int nHeight, vector<brig_Widget*> *pArrLeft, vector<brig_Widget*> *pArrRight )
{

   bVertical = ( nHeight > nWidth );

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateSplitter( this, iWidgId, x, y, nWidth, nHeight );

   avLeft = *pArrLeft;
   avRight = *pArrRight;
   hCursor = brig_LoadCursor( (bVertical)? CURSOR_SIZEWE : CURSOR_SIZENS );
   bCaptured = 0;
   brig_SetWidgetData( this );

   return handle;
}

static void brig_Drag_Splitter( brig_Splitter *pSplitter, int iXPos, int iYPos )
{
   if( pSplitter->bVertical )
   {
      if( iXPos > 32000 )
         iXPos -= 65535;
      if( iXPos == 0 )
         return;
      iXPos += pSplitter->iLeft;
      iYPos = -1;
   }
   else
   {
      if( iYPos > 32000 )
         iYPos -= 65535;
      if( iYPos == 0 )
         return;
      iYPos += pSplitter->iTop;
      iXPos = -1;
   }
   pSplitter->Move( iXPos, iYPos, -1, -1 );

}

static void brig_DragAll_Splitter( brig_Splitter *pSplitter, int iXPos, int iYPos )
{

   unsigned int ui;
   int iDiff;

   brig_Drag_Splitter( pSplitter, iXPos, iYPos );

   iDiff = ( (pSplitter->bVertical)? pSplitter->iLeft : pSplitter->iTop ) - pSplitter->iOldPos;
   if( iDiff )
   {
      for( ui = 0; ui<pSplitter->avLeft.size(); ui++ )
         if( pSplitter->bVertical )
         {
            pSplitter->avLeft[ui]->iWidth += iDiff;
            pSplitter->avLeft[ui]->Move( -1, -1, pSplitter->avLeft[ui]->iWidth, -1 );
         }
         else
         {
            pSplitter->avLeft[ui]->iHeight += iDiff;
            pSplitter->avLeft[ui]->Move( -1, -1, -1, pSplitter->avLeft[ui]->iHeight );
         }

      for( ui = 0; ui<pSplitter->avRight.size(); ui++ )
         if( pSplitter->bVertical )
         {
            pSplitter->avRight[ui]->iLeft += iDiff;
            pSplitter->avRight[ui]->iWidth -= iDiff;
            pSplitter->avRight[ui]->Move( pSplitter->avRight[ui]->iLeft, -1, pSplitter->avRight[ui]->iWidth, -1 );
         }
         else
         {
            pSplitter->avRight[ui]->iTop += iDiff;
            pSplitter->avRight[ui]->iHeight -= iDiff;
            pSplitter->avRight[ui]->Move( -1, pSplitter->avRight[ui]->iTop, -1, pSplitter->avRight[ui]->iHeight );
         }
   }
}

static void brig_paint_Splitter( brig_Splitter *pSplitter )
{
   PBRIG_PPS pps = brig_BeginPaint( pSplitter );
   RECT rc;

   if( !pSplitter->hPenEdge )
      pSplitter->hPenEdge = brigAddPen( 1, pSplitter->lTextColor );
   if( !pSplitter->hPenInside )
      pSplitter->hPenInside = brigAddPen( 1, pSplitter->lColorInside );

   brig_GetClientRect( pSplitter, &rc );

   brig_SelectObject( pps->hDC, pSplitter->hPenEdge );
   brig_moveto( pps->hDC, rc.left, rc.top );
   if( pSplitter->bVertical )
   {
      brig_lineto( pps->hDC, rc.left, rc.bottom );
      brig_moveto( pps->hDC, rc.right, rc.top );
      brig_lineto( pps->hDC, rc.right, rc.bottom );
   }
   else
   {
      brig_lineto( pps->hDC, rc.right, rc.top );
      brig_moveto( pps->hDC, rc.left, rc.bottom );
      brig_lineto( pps->hDC, rc.right, rc.bottom );
   }
   brig_SelectObject( pps->hDC, pSplitter->hPenInside );
   if( pSplitter->bVertical )
      for( int i = rc.left+1; i<rc.right; i++ )
      {
         brig_moveto( pps->hDC, i, rc.top );
         brig_lineto( pps->hDC, i, rc.bottom );
      }
   else
      for( int i = rc.top+1; i<rc.bottom; i++ )
      {
         brig_moveto( pps->hDC, rc.left, i );
         brig_lineto( pps->hDC, rc.right, i );
      }

   brig_EndPaint( pSplitter, pps );
}

bool brig_Splitter::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   int iXPos, iYPos;

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "spli_oneve %d\r\n", message );
   switch( message ) {

      case WM_ERASEBKGND:
         return 1;

      case WM_PAINT:

         if( pfOnPaint )
         {
            PBRIG_PPS pps = brig_BeginPaint( this );

            pfOnPaint( this, pps->hDC );

            brig_EndPaint( this, pps );
         }
         else
            brig_paint_Splitter( this );

         break;

      case WM_MOUSEMOVE:
         brig_SetCursor( hCursor, this );
         if( bCaptured )
         {
            iXPos = ( (unsigned long) lParam ) & 0xFFFF;
            iYPos = ( ( (unsigned long) lParam ) >> 16 ) & 0xFFFF;
            brig_Drag_Splitter( this, iXPos, iYPos );
         }
         break;

      case WM_LBUTTONDOWN:
         brig_SetCursor( hCursor, this );
         bCaptured = 1;
         iOldPos = (bVertical)? iLeft : iTop;
         break;

      case WM_LBUTTONUP:
         bCaptured = 0;
         iXPos = ( (unsigned long) lParam ) & 0xFFFF;
         iYPos = ( ( (unsigned long) lParam ) >> 16 ) & 0xFFFF;
         brig_DragAll_Splitter( this, iXPos, iYPos );
         break;
   }
  
   return 0;
}

/* -------- Tree --------- */

brig_TreeNode::brig_TreeNode()
{
   pfAction = pfDblClick = pfRClick = NULL;
   pData = NULL;
   pParent = NULL;
   pTree = NULL;
}

brig_TreeNode * brig_TreeNode::AddNode( PBRIG_CHAR szTitle, brig_fnc_tree_action pfAct,
      brig_TreeNode *pPrev, brig_TreeNode * pNext, int iImage, int iSelectedImage )
{
   brig_TreeNode * pNode = new brig_TreeNode;
   int iPos = (pPrev)? 0 : 2;
   unsigned int ui;

   if( !pPrev && pNext )
   {
      for( ui = 0; ui <= pTree->avItems.size(); ui++ )
         if( pTree->avItems[ui]->handle == pNext->handle )
            break;
      if( ui )
      {
         pPrev = pTree->avItems[ui-1];
         iPos = 0;
      }
      else
         iPos = 1;
   }

   pNode->handle = brig_TreeAddNode( pNode, pTree, szTitle, this, pPrev, iPos,
         iImage, iSelectedImage );

   pNode->pParent = this;
   pNode->pTree = pTree;
   pNode->pfAction = pfAct;
   pNode->pfDblClick = NULL;
   if( iPos == 0 )
   {
      for( ui = 0; ui <= avItems.size(); ui++ )
         if( avItems[ui]->handle == pPrev->handle )
         {
            avItems.insert( avItems.begin()+ui, pNode );
            break;
         }
   }
   else if( iPos == 1 )
      avItems.insert( avItems.begin(), pNode );
   else if( iPos == 2 )
      avItems.push_back( pNode );

   return pNode;

}

brig_Tree::brig_Tree():brig_Widget()
{
   uiType = TYPE_TREE;
}

brig_Tree::~brig_Tree()
{
   brig_TreeRelease( this );
}

BRIG_HANDLE brig_Tree::Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );
   
   handle = brig_CreateTree( this, iWidgId, x, y, nWidth, nHeight, ulStyle );

   if( !hFont && pParent->hFont )
      SetFont( pParent->hFont );
   brig_SetWidgetData( this );

   return handle;
}

brig_TreeNode * brig_Tree::AddNode( PBRIG_CHAR szTitle, brig_fnc_tree_action pfAct,
      brig_TreeNode *pPrev, brig_TreeNode * pNext, int iImage, int iSelectedImage )
{
   brig_TreeNode * pNode = new brig_TreeNode;
   int iPos = (pPrev)? 0 : 2;
   unsigned int ui;

   if( !pPrev && pNext )
   {
      for( ui = 0; ui <= avItems.size(); ui++ )
         if( avItems[ui]->handle == pNext->handle )
            break;
      if( ui )
      {
         pPrev = avItems[ui-1];
         iPos = 0;
      }
      else
         iPos = 1;
   }

   pNode->handle = brig_TreeAddNode( pNode, this, szTitle, NULL, pPrev, iPos,
      iImage, iSelectedImage );

   pNode->pTree = this;
   pNode->pfAction = pfAct;
   pNode->pfDblClick = NULL;
   if( iPos == 0 )
   {
      for( ui = 0; ui <= avItems.size(); ui++ )
         if( avItems[ui]->handle == pPrev->handle )
         {
            avItems.insert( avItems.begin()+ui, pNode );
            break;
         }
   }
   else if( iPos == 1 )
      avItems.insert( avItems.begin(), pNode );
   else if( iPos == 2 )
      avItems.push_back( pNode );

   return pNode;
}

void brig_Tree::AddImages( PBRIG_BITMAP pBitmap1, PBRIG_BITMAP pBitmap2, PBRIG_BITMAP pBitmap3 )
{
   brig_TreeAddImage( this, pBitmap1 );
   if( pBitmap2 )
      brig_TreeAddImage( this, pBitmap2 );
   if( pBitmap3 )
      brig_TreeAddImage( this, pBitmap3 );
}

bool brig_Tree::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( message );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   //brig_writelog( NULL, "tree_onEvent %u\r\n", message );
   
   return 0;
}
