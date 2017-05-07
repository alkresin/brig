/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

brig_Table::brig_Table():brig_Widget(), pfOnPaint(NULL), pfDataSet(NULL), pData(NULL), pPenSep(NULL), pPenHdr(NULL)
{
   lSepColor = 0xc0c0c0;
   lSelTColor = lHeadColor = lTextColor;
   uiTextHeight = uiHeadRows = uiFootRows = uiRowCount = 0;
   ulRecCurr = ulRecFirst = uiRowSel = uiColFirst = 1;
   pHeadPadding[0] = pHeadPadding[2] = pPadding[0] = pPadding[2] = 4;
   pHeadPadding[1] = pHeadPadding[3] = pPadding[1] = pPadding[3] = 2;
   pStyle = pStyleSel = pStyleHead = pStyleFoot = NULL;
}

BRIG_HANDLE brig_Table::New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle )
{

   brig_Widget::New( pParent, x, y, nWidth, nHeight );

   handle = brig_CreateTable( pParent->Handle(), iWidgId,
             x, y, nWidth, nHeight, ulStyle );

   if( !hFont )
      hFont = pParent->hFont;
   brig_SetWidgetData( this );

   return handle;
}

void brig_Table::AddColumn( PBRIG_CHAR szHead, int iWidth, brig_fnc_column pfValue )
{
   PBRIG_TCOL pColumn = new BRIG_TCOL;

   pColumn->szHead = szHead;
   pColumn->szFoot = NULL;
   pColumn->iWidth = iWidth;
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
   brig_Style *pStyle = NULL;
   int x = 0;
   unsigned int uiHeight = pTable->uiTextHeight + pTable->pPadding[1] + pTable->pPadding[3];
   unsigned int uiWidth;

   for( unsigned int ui = pTable->uiColFirst-1; ui<pTable->avColumns.size(); ui++ )
   {
      pColumn = pTable->avColumns[ui];
      uiWidth = ( ui+1 == pTable->avColumns.size() || pColumn->iWidth > (pTable->uiClientWidth-x) )?
           pTable->uiClientWidth-x : pColumn->iWidth;

      if( bSel )
         pStyle = (pColumn->pStyleSel)? pColumn->pStyleSel : pTable->pStyleSel;
      if( !pStyle )
         pStyle = (pColumn->pStyle)? pColumn->pStyle : pTable->pStyle;
      if( pStyle )
         pStyle->Draw( hDC, x, y, x+uiWidth-1, y+uiHeight-1 );

      if( uiWidth >= pColumn->iWidth && ( pCellValue = pColumn->pfValue( pTable, ui+1 ) ) != NULL )
      {
         brig_SetTextColor( hDC, (bSel)? pTable->lSelTColor : pTable->lTextColor );
         brig_SetTransparentMode( hDC, 1 );
         brig_DrawText( hDC, pCellValue, x+pTable->pPadding[0], y+pTable->pPadding[1],
            x+pColumn->iWidth-pTable->pPadding[2]-1, y+pTable->uiTextHeight + pTable->pPadding[1], DT_SINGLELINE | DT_VCENTER | DT_CENTER );
         brig_SetTransparentMode( hDC, 0 );
      }

      x += pColumn->iWidth;
   }
}

static void brig_paint_Table( brig_Table *pTable )
{
   BRIG_HANDLE hTable = pTable->Handle();
   PBRIG_PPS pps = brig_BeginPaint( hTable );
   RECT rc;
   unsigned long ulRecCount;
   unsigned int uiRowHeight, y1 = 0, y2;

   brig_GetClientRect( hTable, &rc );

   pTable->uiClientWidth = rc.right - rc.left;
   pTable->uiClientHeight = rc.bottom - rc.top;

   if( !pTable->hBrush )
      pTable->SetBackColor( COLOR_WHITE, 0 );
   brig_FillRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom, pTable->hBrush );

   if( !pTable->pfDataSet )
   {
      brig_EndPaint( hTable, pps );
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

      y1 = rc.top + uiRowHeight;
      y2 = rc.bottom;

      pTable->uiRowCount = (y2 - y1) / uiRowHeight;
      if( pTable->uiRowCount > ulRecCount )
         pTable->uiRowCount = ulRecCount;
      if( pTable->hFont )
         brig_SelectObject( pps->hDC, pTable->hFont );

      if( pTable->uiRowSel > 1 )
      {
         ulTemp = pTable->pfDataSet( pTable, TDS_RECNO, 0 );
         pTable->pfDataSet( pTable, TDS_BACK, pTable->uiRowSel-1 );
      }

      for( unsigned int iRow = 0; iRow < pTable->uiRowCount; iRow++ )
      {
         paint_row( pTable, pps->hDC, y1 + uiRowHeight*iRow, (iRow+1 == pTable->uiRowSel) );
         pTable->pfDataSet( pTable, TDS_FORWARD, 1 );
      }

      if( pTable->uiRowSel > 1 )
         pTable->pfDataSet( pTable, TDS_GOTO, ulTemp );
   }
   else
      pTable->uiRowCount = 0;

   paint_head( pTable, pps->hDC );
   paint_foot( pTable, pps->hDC );

   if( ulRecCount )
   {
      unsigned int iLeft = 0;

      brig_SelectObject( pps->hDC, pTable->pPenSep );
      for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
      {
         iLeft += pTable->avColumns[ui]->iWidth;
         if( iLeft > pTable->uiClientWidth || ui+1 == pTable->avColumns.size() )
            break;
         brig_moveto( pps->hDC, iLeft-2, y1 );
         brig_lineto( pps->hDC, iLeft-2, y1 + pTable->uiRowCount*uiRowHeight );
      }
      for( unsigned int ui = 1; ui<=pTable->uiRowCount; ui++ )
      {
         brig_moveto( pps->hDC, 0, y1 + uiRowHeight*ui );
         brig_lineto( pps->hDC, pTable->uiClientWidth, y1 + uiRowHeight*ui );
      }
   }
   brig_EndPaint( hTable, pps );

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
            PBRIG_PPS pps = brig_BeginPaint( handle );

            pfOnPaint( this, pps->hDC );

            brig_EndPaint( handle, pps );
         }
         else
            brig_paint_Table( this );

         break;

      case WM_KEYUP:
      case WM_KEYDOWN:
         break;

      case WM_LBUTTONDOWN:
         {
            //unsigned int uixPos = ( (unsigned long) lParam ) & 0xFFFF;
            unsigned int uiyPos = ( ( (unsigned long) lParam ) >> 16 ) & 0xFFFF;
            unsigned int uiHeight = uiTextHeight + pPadding[1] + pPadding[3];

            uiyPos -= ( uiTextHeight + pHeadPadding[1] + pHeadPadding[3] );
            if( uiyPos > 0 )
            {
               unsigned int uiRow = uiyPos / uiHeight + 1;
               if( uiRowSel != uiRow )
               {
                  uiRowSel = uiRow;
                  brig_RedrawWindow( handle );
               }
            }
         }
         break;

      case WM_LBUTTONUP:
      case WM_LBUTTONDBLCLK:
      case WM_RBUTTONDOWN:
         break;
   }
  
   return 0;
}
