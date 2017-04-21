/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

brig_Style::brig_Style( int iColors, long * pColors, int iType,
   long lBorderColor, int iBorder, int iCorners, long * pCorners )
{
   iOrient = iType;
   for( int i = 0; i < iColors; i ++ )
      avColor.push_back( pColors[i] );

   if( lBorderColor < 0 )
      pPen = NULL;
   else
      pPen = brigAddPen( iBorder, lBorderColor );

   if( iCorners > 0 && pCorners )
      for( int i = 0; i < iCorners; i ++ )
         avCorners.push_back( pCorners[i] );

}

brig_Style::~brig_Style()
{
   if( pPen )
      brigDelPen( pPen );
}

void brig_Style::Draw( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom )
{
   brig_DrawGradient( hDC, iLeft, iTop, iRight, iBottom, iOrient, &avColor, NULL, &avCorners );
   if( pPen )
   {
      brig_SelectObject( hDC, pPen );
      brig_DrawRect( hDC, iLeft, iTop, iRight, iBottom );
   }
}
