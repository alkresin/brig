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
   uiTextHeight = uiHeadRows = uiFootRows = uiRowCount = 0;
   ulRecCurr = ulRecFirst = uiRowSel = 1;
   pHeadPadding[0] = pHeadPadding[2] = pPadding[0] = pPadding[2] = 4;
   pHeadPadding[1] = pHeadPadding[3] = pPadding[1] = pPadding[3] = 2;
   pStyle = pStyleHead = pStyleFoot = NULL;
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
   pColumn->pStyle = pColumn->pStyleHead = pColumn->pStyleFoot = NULL;

   avColumns.push_back( pColumn );
}

static void paint_head( brig_Table *pTable, PBRIG_DC hDC )
{
   PBRIG_TCOL pColumn;
   int x = 0;
   unsigned int uiHeight = pTable->uiTextHeight + pTable->pHeadPadding[1] + pTable->pHeadPadding[3];

   if( !pTable->uiHeadRows )
      return;

   for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
   {
      pColumn = pTable->avColumns[ui];
      if( pColumn->pStyleHead )
         pColumn->pStyleHead->Draw( hDC, x, 0, x+pColumn->iWidth-1, uiHeight );
      else if( pTable->pStyleHead )
         pTable->pStyleHead->Draw( hDC, x, 0, x+pColumn->iWidth-1, uiHeight );

      if( pColumn->szHead )
      {
         if( pTable->hFont )
            brig_SelectObject( hDC, pTable->hFont );

         brig_SetTransparentMode( hDC, 1 );
         brig_DrawText( hDC, pColumn->szHead, x+pTable->pHeadPadding[0], pTable->pHeadPadding[1],
            x+pColumn->iWidth-pTable->pHeadPadding[2]-1, pTable->uiTextHeight + pTable->pHeadPadding[1], DT_SINGLELINE | DT_VCENTER | DT_CENTER );
         brig_SetTransparentMode( hDC, 0 );
      }
      x += pColumn->iWidth;
      brig_SelectObject( hDC, pTable->pPenHdr );
      brig_moveto( hDC, x-2, 0 );
      brig_lineto( hDC, x-2, uiHeight-1 );
   }
}

static void paint_foot( brig_Table *pTable, PBRIG_DC hDC )
{
   SYMBOL_UNUSED( hDC );
   if( !pTable->uiFootRows )
      return;

}

static void paint_row( brig_Table *pTable, PBRIG_DC hDC, unsigned int y )
{
   PBRIG_TCOL pColumn;
   PBRIG_CHAR pCellValue;
   int x = 0;
   unsigned int uiHeight = pTable->uiTextHeight + pTable->pPadding[1] + pTable->pPadding[3];

   for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
   {
      pColumn = pTable->avColumns[ui];
      if( pColumn->pStyle )
         pColumn->pStyle->Draw( hDC, x, y, x+pColumn->iWidth-1, y+uiHeight-1 );
      else if( pTable->pStyle )
         pTable->pStyle->Draw( hDC, x, y, x+pColumn->iWidth-1, y+uiHeight-1 );

      pCellValue = pColumn->pfValue( pTable, ui+1 );
      if( pCellValue )
      {
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
   unsigned int uiRowHeight = 0, y1 = 0, y2 = 30;

   brig_GetClientRect( hTable, &rc );

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
         paint_row( pTable, pps->hDC, y1 + uiRowHeight*iRow );
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
         brig_moveto( pps->hDC, iLeft-2, y1 );
         brig_lineto( pps->hDC, iLeft-2, y1 + pTable->uiRowCount*uiRowHeight );
      }
      for( unsigned int ui = 1; ui<=pTable->uiRowCount; ui++ )
      {
         brig_moveto( pps->hDC, 0, y1 + uiRowHeight*ui );
         brig_lineto( pps->hDC, iLeft-2, y1 + uiRowHeight*ui );
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
   }
  
   return 0;
}
