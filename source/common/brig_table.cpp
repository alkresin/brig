/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

brig_Table::brig_Table():brig_Widget(), pfOnPaint(NULL), pfRows(NULL)
{
   uiColHeight = uiHeadRows = uiFootRows = 0;
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

void brig_Table::AddColumn( PBRIG_CHAR szHead, int iWidth, brig_Style *pStyle )
{
   PBRIG_TCOL pColumn = new BRIG_TCOL;

   pColumn->szHead = szHead;
   pColumn->szFoot = NULL;
   pColumn->iWidth = iWidth;
   pColumn->pStyle = pStyle;

   avColumns.push_back( pColumn );
}

static void paint_head( brig_Table *pTable, PBRIG_DC hDC )
{
   PBRIG_TCOL pColumn;
   int x = 0;

   if( !pTable->uiHeadRows )
      return;

   for( unsigned int ui = 0; ui<pTable->avColumns.size(); ui++ )
   {
      pColumn = pTable->avColumns[ui];
      if( pColumn->pStyle )
      {
         pColumn->pStyle->Draw( hDC, x, 1, x+pColumn->iWidth-1, pTable->uiColHeight );
      }
      if( pColumn->szHead )
      {
         if( pTable->hFont )
            brig_SelectObject( hDC, pTable->hFont );

         brig_SetTransparentMode( hDC, 1 );
         brig_DrawText( hDC, pColumn->szHead, x+2, 3, x+pColumn->iWidth-2,
            pTable->uiColHeight-2, DT_SINGLELINE | DT_VCENTER | DT_CENTER );
         brig_SetTransparentMode( hDC, 0 );
      }
      x += pColumn->iWidth;
      brig_SelectObject( hDC, pTable->pPenHdr );
      brig_moveto( hDC, x-1, 1 );
      brig_lineto( hDC, x-1, pTable->uiColHeight-1 );
   }
}

static void paint_foot( brig_Table *pTable, PBRIG_DC hDC )
{
   SYMBOL_UNUSED( hDC );
   if( !pTable->uiFootRows )
      return;

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

   if( !pTable->pfRows )
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

   if( pTable->pfRows() )
   {
   }

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
