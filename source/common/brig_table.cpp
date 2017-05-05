/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

brig_Table::brig_Table():brig_Widget(), pfOnPaint(NULL), pfDataSet(NULL), pData(NULL)
{
   uiColHeight = uiHeadRows = uiFootRows = uiRowCount = 0;
   ulRecCurr = ulRecFirst = 1;
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
   unsigned int uiHeight = pTable->uiColHeight + pTable->pHeadPadding[1] + pTable->pHeadPadding[3];

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
            x+pColumn->iWidth-pTable->pHeadPadding[2]-1, pTable->uiColHeight + pTable->pHeadPadding[1], DT_SINGLELINE | DT_VCENTER | DT_CENTER );
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

static void paint_row( brig_Table *pTable, PBRIG_DC hDC )
{
   PBRIG_TCOL pColumn;
   PBRIG_CHAR pCellValue;
   int x = 0;
   unsigned int uiHeight = pTable->uiColHeight + pTable->pPadding[1] + pTable->pPadding[3];

   for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
   {
      pColumn = pTable->avColumns[ui];
      if( pColumn->pStyleHead )
         pColumn->pStyleHead->Draw( hDC, x, 0, x+pColumn->iWidth-1, uiHeight );
      else if( pTable->pStyleHead )
         pTable->pStyleHead->Draw( hDC, x, 0, x+pColumn->iWidth-1, uiHeight );

      pCellValue = pColumn->pfValue( pTable, ui );
      if( pCellValue )
      {
         brig_SetTransparentMode( hDC, 1 );
         brig_DrawText( hDC, pCellValue, x+pTable->pPadding[0], pTable->pPadding[1],
            x+pColumn->iWidth-pTable->pPadding[2]-1, pTable->uiColHeight + pTable->pPadding[1], DT_SINGLELINE | DT_VCENTER | DT_CENTER );
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

   brig_GetClientRect( hTable, &rc );

   if( !pTable->hBrush )
      pTable->SetBackColor( COLOR_WHITE, 0 );
   brig_FillRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom, pTable->hBrush );

   if( !pTable->pfDataSet )
   {
      brig_EndPaint( hTable, pps );
      return;
   }

   if( !pTable->uiColHeight )
   {
      if( pTable->hFont )
         brig_SelectObject( pps->hDC, pTable->hFont );
      pTable->uiColHeight = brig_GetCharHeight( pps->hDC );

      for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
      {
         if( pTable->avColumns[ui]->szHead )
            pTable->uiHeadRows = 1;          
      }
      if( !pTable->pPenSep )
         pTable->pPenSep = brigAddPen( 1, pTable->lTextColor );
      if( !pTable->pPenHdr )
         pTable->pPenHdr = brigAddPen( 1, 0 );
   }

   if( pTable->pfDataSet( pTable, TDS_COUNT, 0 ) )
   {
      unsigned int y1 = rc.top + pTable->uiColHeight + pTable->pHeadPadding[1] + pTable->pHeadPadding[3];
      unsigned int y2 = rc.bottom;
      unsigned int uiColHeight = pTable->uiColHeight + pTable->pPadding[1] + pTable->pPadding[3];

      pTable->uiRowCount = (y2 - y1) / uiColHeight;
      if( pTable->hFont )
         brig_SelectObject( pps->hDC, pTable->hFont );

   }
   else
      pTable->uiRowCount = 0;

   paint_head( pTable, pps->hDC );
   paint_foot( pTable, pps->hDC );

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
