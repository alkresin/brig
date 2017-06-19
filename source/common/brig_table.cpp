/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

brig_Table::brig_Table():brig_Widget(), pfOnPaint(NULL), pfDataSet(NULL),
   pfOnDblClick(NULL), pfOnPosChanged(NULL), pData(NULL), pPenSep(NULL), pPenHdr(NULL)
{
   uiType = TYPE_TABLE;
   lSepColor = 0xc0c0c0;
   lSelTColor = lHeadColor = lTextColor;
   uiTextHeight = uiHeadRows = uiFootRows = uiRowCount = uiColumnSel = 0;
   ulRecCurrOld = 0;
   ulRecCurr = ulRecFirst = uiRowSel = uiColFirst = 1;
   pHeadPadding[0] = pHeadPadding[2] = pPadding[0] = pPadding[2] = 4;
   pHeadPadding[1] = pHeadPadding[3] = pPadding[1] = pPadding[3] = 2;
   pStyle = pStyleSel = pStyleHead = pStyleFoot = pStyleCell = NULL;
   bBodyOnly = 0;
}

BRIG_HANDLE brig_Table::Create( brig_Container *pParent, int x, int y,
      int nWidth, int nHeight, unsigned long ulStyle, PBRIG_FONT hFont, unsigned int iRows )
{

   brig_Widget::Create( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateTable( this, iWidgId, x, y, nWidth, nHeight, ulStyle, hFont, iRows );

   if( hFont )
      this->hFont = hFont;
   else
      this->hFont = pParent->hFont;
   brig_SetWidgetData( this );

   return handle;
}

void brig_Table::AddColumn( PBRIG_CHAR szHead, unsigned int iWidth, brig_fnc_column pfValue, unsigned int iAlign )
{
   PBRIG_TCOL pColumn = new BRIG_TCOL;

   pColumn->szHead = szHead;
   pColumn->szFoot = NULL;
   pColumn->iWidth = iWidth;
   pColumn->iAlign = iAlign;
   pColumn->pfValue = pfValue;
   pColumn->pStyle = pColumn->pStyleSel = pColumn->pStyleHead = pColumn->pStyleFoot = NULL;

   avColumns.push_back( pColumn );
}

static void paint_head( brig_Table *pTable, PBRIG_DC hDC )
{
   PBRIG_TCOL pColumn;
   unsigned int x = 0;
   unsigned int uiHeight = pTable->uiTextHeight + pTable->pHeadPadding[1] + pTable->pHeadPadding[3];
   unsigned int uiWidth;

   if( !pTable->uiHeadRows )
      return;

   for( unsigned int ui = pTable->uiColFirst-1; ui<pTable->avColumns.size(); ui++ )
   {
      pColumn = pTable->avColumns[ui];
      if( x >= 2 )
      {
         brig_SelectObject( hDC, pTable->pPenHdr );
         brig_moveto( hDC, x-2, 0 );
         brig_lineto( hDC, x-2, uiHeight-1 );
      }
      uiWidth = ( ui+1 == pTable->avColumns.size() || pColumn->iWidth > (pTable->uiClientWidth-x) )?
           pTable->uiClientWidth-x : pColumn->iWidth;

      if( pColumn->pStyleHead )
         pColumn->pStyleHead->Draw( hDC, x, 0, x+uiWidth-1, uiHeight );
      else if( pTable->pStyleHead )
         pTable->pStyleHead->Draw( hDC, x, 0, x+uiWidth-1, uiHeight );

      if( pColumn->szHead && uiWidth >= pColumn->iWidth )
      {
         if( pTable->hFont )
            brig_SelectObject( hDC, pTable->hFont );

         brig_SetTextColor( hDC, pTable->lHeadColor );
         brig_SetTransparentMode( hDC, 1 );
         brig_DrawText( hDC, pColumn->szHead, x+pTable->pHeadPadding[0], pTable->pHeadPadding[1],
            x+pColumn->iWidth-pTable->pHeadPadding[2]-1, pTable->uiTextHeight + pTable->pHeadPadding[1], DT_SINGLELINE | DT_VCENTER | DT_CENTER );
         brig_SetTransparentMode( hDC, 0 );
      }
      x += pColumn->iWidth;
      if( x > pTable->uiClientWidth )
         break;
   }
}

static void paint_foot( brig_Table *pTable, PBRIG_DC hDC )
{
   SYMBOL_UNUSED( hDC );
   if( !pTable->uiFootRows )
      return;

}

static void paint_row( brig_Table *pTable, PBRIG_DC hDC, unsigned int y, bool bSel )
{
   PBRIG_TCOL pColumn;
   PBRIG_CHAR pCellValue;
   brig_Style *pStyle;
   int x = 0;
   unsigned int uiHeight = pTable->uiTextHeight + pTable->pPadding[1] + pTable->pPadding[3];
   unsigned int uiWidth;

   for( unsigned int ui = pTable->uiColFirst-1; ui<pTable->avColumns.size(); ui++ )
   {
      pColumn = pTable->avColumns[ui];
      uiWidth = ( ui+1 == pTable->avColumns.size() || pColumn->iWidth > (pTable->uiClientWidth-x) )?
           pTable->uiClientWidth-x : pColumn->iWidth;

      pStyle = NULL;
      if( bSel )
      {
         if( ui+1 == pTable->uiColumnSel && pTable->pStyleCell )
            pStyle = pTable->pStyleCell;
         if( !pStyle )
            pStyle = (pColumn->pStyleSel)? pColumn->pStyleSel : pTable->pStyleSel;
      }
      if( !pStyle )
         pStyle = (pColumn->pStyle)? pColumn->pStyle : pTable->pStyle;
      if( pStyle )
         pStyle->Draw( hDC, x, y, x+uiWidth-1, y+uiHeight-1 );
      else
         brig_FillRect( hDC, x, y, x+uiWidth-1, y+uiHeight-1, pTable->hBrush );

      if( uiWidth >= pColumn->iWidth && ( pCellValue = pColumn->pfValue( pTable, ui+1 ) ) != NULL )
      {
         brig_SetTextColor( hDC, (bSel)? pTable->lSelTColor : pTable->lTextColor );
         brig_SetTransparentMode( hDC, 1 );
         brig_DrawText( hDC, pCellValue, x+pTable->pPadding[0], y+pTable->pPadding[1],
            x+pColumn->iWidth-pTable->pPadding[2]-1, y+pTable->uiTextHeight + pTable->pPadding[1], DT_SINGLELINE | DT_VCENTER | pColumn->iAlign );
         brig_SetTransparentMode( hDC, 0 );
      }

      x += pColumn->iWidth;
   }
}

static void paint_table( brig_Table *pTable )
{
   PBRIG_PPS pps = brig_BeginPaint( pTable );
   RECT rc;
   unsigned long ulRecCount;
   unsigned int uiRowHeight, y1 = 0, y2;

   brig_GetClientRect( pTable, &rc );

   pTable->uiClientWidth = rc.right - rc.left;
   pTable->uiClientHeight = rc.bottom - rc.top;

   if( !pTable->hBrush )
      pTable->SetBackColor( COLOR_WHITE, 0 );

   if( !pTable->pfDataSet )
   {
      brig_EndPaint( pTable, pps );
      return;
   }

   if( !pTable->uiTextHeight )
   {
      if( pTable->hFont )
         brig_SelectObject( pps->hDC, pTable->hFont );
      pTable->uiTextHeight = brig_GetCharHeight( pps->hDC );

      for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
      {
         if( pTable->avColumns[ui]->szHead )
            pTable->uiHeadRows = 1;          
      }
      if( !pTable->pPenSep )
         pTable->pPenSep = brigAddPen( 1, pTable->lSepColor );
      if( !pTable->pPenHdr )
         pTable->pPenHdr = brigAddPen( 1, 0 );
   }

   uiRowHeight = pTable->uiTextHeight + pTable->pPadding[1] + pTable->pPadding[3];
   ulRecCount = pTable->pfDataSet( pTable, TDS_COUNT, 0 );
   if( ulRecCount )
   {
      unsigned long ulTemp;

      y1 = rc.top + (pTable->uiHeadRows)? pTable->uiTextHeight + pTable->pHeadPadding[1] + pTable->pHeadPadding[3] : 0;
      y2 = rc.bottom;

      pTable->uiRowCount = (y2 - y1) / uiRowHeight;
      if( pTable->uiRowCount > ulRecCount )
         pTable->uiRowCount = ulRecCount;
      if( pTable->hFont )
         brig_SelectObject( pps->hDC, pTable->hFont );

      ulTemp = pTable->pfDataSet( pTable, TDS_RECNO, 0 );
      if( pTable->uiRowSel > 1 )
         pTable->pfDataSet( pTable, TDS_BACK, pTable->uiRowSel-1 );

      for( unsigned int iRow = 0; iRow < pTable->uiRowCount; iRow++ )
      {
         paint_row( pTable, pps->hDC, y1 + uiRowHeight*iRow, (iRow+1 == pTable->uiRowSel) );
         pTable->pfDataSet( pTable, TDS_FORWARD, 1 );
      }

      pTable->pfDataSet( pTable, TDS_GOTO, ulTemp );
      if( pTable->ulRecCurrOld != ulTemp )
      {
         pTable->ulRecCurrOld = ulTemp;
         if( pTable->pfOnPosChanged )
            pTable->pfOnPosChanged( pTable );
      }
   }
   else
      pTable->uiRowCount = 0;

   if( !pTable->bBodyOnly )
   {
      paint_head( pTable, pps->hDC );
      paint_foot( pTable, pps->hDC );
   }

   if( ulRecCount )
   {
      unsigned int ui, iLeft = 0;

      brig_SelectObject( pps->hDC, pTable->pPenSep );
      for( ui = 0; ui<pTable->avColumns.size(); ui++ )
      {
         iLeft += pTable->avColumns[ui]->iWidth;
         if( iLeft > pTable->uiClientWidth || ui+1 == pTable->avColumns.size() )
            break;
         brig_moveto( pps->hDC, iLeft-2, y1 );
         brig_lineto( pps->hDC, iLeft-2, y1 + pTable->uiRowCount*uiRowHeight );
      }
      for( ui = 1; ui<=pTable->uiRowCount; ui++ )
      {
         brig_moveto( pps->hDC, 0, y1 + uiRowHeight*ui );
         brig_lineto( pps->hDC, pTable->uiClientWidth, y1 + uiRowHeight*ui );
      }
      ui --;
      if( y1 + uiRowHeight*ui < (unsigned int) rc.bottom )
         brig_FillRect( pps->hDC, rc.left, y1 + uiRowHeight*ui + 1, rc.right, rc.bottom, pTable->hBrush );
   }
   else
      brig_FillRect( pps->hDC, rc.left, y1, rc.right, rc.bottom, pTable->hBrush );

   pTable->bBodyOnly = 0;
   brig_EndPaint( pTable, pps );

}

static void table_SetVScroll( brig_Table *pTable, int iPos = 0 )
{

   int iMinPos, iMaxPos;
   unsigned long ulRecNo, ulRecCount;

   brig_GetScrollRange( pTable, 1, &iMinPos, &iMaxPos );
   ulRecCount = pTable->pfDataSet( pTable, TDS_COUNT, 0 );
   ulRecNo = pTable->pfDataSet( pTable, TDS_RECNO, 0 );
   if( !iPos )
      iPos = (ulRecNo<=1)? iMinPos : (int) ( ( ((double)(iMaxPos-iMinPos)) / (ulRecCount-1) ) * (ulRecNo-1) );
   brig_SetScrollPos( pTable, 1, iPos );

}

static int table_OnBtnDown( brig_Table *pTable, LPARAM lParam )
{
   unsigned int uixPos = ( (unsigned long) lParam ) & 0xFFFF;
   int iyPos = ( ( (unsigned long) lParam ) >> 16 ) & 0xFFFF;
   unsigned int uiHeight = pTable->uiTextHeight + pTable->pPadding[1] + pTable->pPadding[3];
   bool bRepaint = 0;

   iyPos -= (pTable->uiHeadRows)?
         ( pTable->uiTextHeight + pTable->pHeadPadding[1] + pTable->pHeadPadding[3] ) : 0;
   if( iyPos > 0 )
   {
      unsigned int uiWidth = 0;
      unsigned int uiRow = iyPos / uiHeight + 1;
      int iDiff = uiRow-pTable->uiRowSel;

      if( uiRow > pTable->uiRowCount )
         return 0;

      if( iDiff != 0 )
      {
         if( iDiff > 0 )
            pTable->pfDataSet( pTable, TDS_FORWARD, iDiff );
         else
            pTable->pfDataSet( pTable, TDS_BACK, -iDiff );
         pTable->uiRowSel = uiRow;
         bRepaint = 1;
      }
      if( pTable->uiColumnSel )
         for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
         {
            uiWidth += pTable->avColumns[ui]->iWidth;
            if( uiWidth > uixPos )
            {
               pTable->uiColumnSel = ui + 1;
               bRepaint = 1;
               break;
            }
         }

      table_SetVScroll( pTable );
      if( bRepaint )
         brig_RedrawWindow( pTable );
   }
   brig_SetFocus( pTable );
   return 1;
}

void brig_Table::Down( void )
{

   if( pfDataSet( this, TDS_EOF, 0 ) )
      return;

   pfDataSet( this, TDS_FORWARD, 1 );
   if( uiRowSel < uiRowCount )
      uiRowSel++;

   table_SetVScroll( this );

   bBodyOnly = 1;
   brig_RedrawWindow( this );
}

void brig_Table::Up( void )
{
   if( pfDataSet( this, TDS_BOF, 0 ) )
      return;

   pfDataSet( this, TDS_BACK, 1 );
   if( uiRowSel > 1 )
      uiRowSel--;

   table_SetVScroll( this );

   bBodyOnly = 1;
   brig_RedrawWindow( this );
}

void brig_Table::PageDown( void )
{

   if( pfDataSet( this, TDS_EOF, 0 ) )
      return;

   pfDataSet( this, TDS_FORWARD, uiRowCount );
   if( uiRowSel > uiRowCount )
      uiRowSel = uiRowCount;

   table_SetVScroll( this );

   bBodyOnly = 1;
   brig_RedrawWindow( this );
}

void brig_Table::PageUp( void )
{
   unsigned long ulNo;
   if( pfDataSet( this, TDS_BOF, 0 ) )
      return;

   pfDataSet( this, TDS_BACK, uiRowCount );
   if( ( ulNo = pfDataSet( this, TDS_RECNO, 0 ) ) < uiRowSel )
      uiRowSel = ulNo;

   table_SetVScroll( this );

   bBodyOnly = 1;
   brig_RedrawWindow( this );
}

void brig_Table::Top( void )
{
   pfDataSet( this, TDS_TOP, 0 );
   uiRowSel = 1;

   table_SetVScroll( this );

   bBodyOnly = 1;
   brig_RedrawWindow( this );
}

void brig_Table::Bottom( void )
{
   unsigned long ulRecs = pfDataSet( this, TDS_COUNT, 0 );
   pfDataSet( this, TDS_BOTTOM, 0 );
   uiRowSel = ( ulRecs > uiRowCount )? uiRowCount : ulRecs;

   table_SetVScroll( this );

   bBodyOnly = 1;
   brig_RedrawWindow( this );
}

bool brig_Table::onEvent( UINT message, WPARAM wParam, LPARAM lParam )
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
            paint_table( this );

         break;

      case WM_ERASEBKGND:
         return 1;

      case WM_KEYUP:
         return 1;

      case WM_KEYDOWN:
         switch( wParam ) {
            case VK_DOWN:
               Down();
               break;
            case VK_UP:
               Up();
               break;
            case VK_HOME:
               Top();
               break;
            case VK_END:
               Bottom();
               break;
            case VK_NEXT:
               PageDown();
               break;
            case VK_PRIOR:
               PageUp();
               break;
            case VK_RETURN:
               if( pfOnDblClick )
                  pfOnDblClick( this );
               break;
         }
         return 1;

      case WM_LBUTTONDOWN:
         table_OnBtnDown( this, lParam );
         return 1;

      case WM_LBUTTONUP:
         break;

      case WM_LBUTTONDBLCLK:
         if( table_OnBtnDown( this, lParam ) && pfOnDblClick )
            pfOnDblClick( this );
         break;

      case WM_RBUTTONDOWN:
         break;

      case WM_VSCROLL:
         brig_table_OnVScroll( this, wParam );
         break;

      case WM_HSCROLL:
         brig_table_OnHScroll( this, wParam );
         break;

      case WM_MOUSEWHEEL:
         brig_table_OnWheel( this, wParam );
         break;

   }
  
   return 0;
}
