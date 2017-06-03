/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

brig_Application::brig_Application(): pMainWindow(NULL) {}

brig_Application::~brig_Application()
{
   unsigned int i;

   for( i = 0; i < brigApp.avStyles.size(); i++ )
      delete brigApp.avStyles[i].pStyle;

   for( i = 0; i < brigApp.avPens.size(); i++ )
      brig_DeleteObject( brigApp.avPens[i].pPen );

   for( i = 0; i < brigApp.avBrushes.size(); i++ )
      brig_DeleteObject( brigApp.avBrushes[i].pBrush );

   for( i = 0; i < brigApp.avFonts.size(); i++ )
   {
      brig_DeleteObject( brigApp.avFonts[i].pFont );
      if( brigApp.avFonts[i].bNeedToFree )
         free( brigApp.avFonts[i].pName );
   }

}

PBRIG_FONT brigAddFont( PBRIG_CHAR fontName, int fnHeight, int fnWeight,
               DWORD fdwCharSet, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut )
{
   unsigned int i, iFirst = 0;
   BRIGAPP_FONT bfold;

   for( i = 0; i < brigApp.avFonts.size(); i++ )
   {
      bfold = brigApp.avFonts[i];
      if( bfold.pName == fontName && bfold.iHeight == (int) fnHeight &&
            bfold.iWeight == (int)fnWeight && bfold.iCharSet == (int)fdwCharSet &&
            bfold.iItalic == (int)fdwItalic && bfold.iUnderline == (int)fdwUnderline &&
            bfold.iStrikeOut == (int)fdwStrikeOut )
         break;
      if( brigApp.avFonts[i].iCount == 0 && !iFirst )
         iFirst = i;
   }
   if( i >= brigApp.avFonts.size() )
   {
      BRIGAPP_FONT bf = {0};
      bf.pFont = brig_CreateFont( fontName, fnHeight, fnWeight,
               fdwCharSet, fdwItalic, fdwUnderline, fdwStrikeOut );
      bf.iCount = 1;
      bf.pName = fontName;
      bf.iHeight  = (int) fnHeight;
      bf.iWeight  = (int)fnWeight;
      bf.iCharSet = (int)fdwCharSet;
      bf.iItalic  = (int)fdwItalic;
      bf.iUnderline  = (int)fdwUnderline;
      bf.iStrikeOut = (int)fdwStrikeOut;
      if( brigApp.avFonts.size() > BRIGAPP_FONT_LIMIT && iFirst )
      {
         brig_DeleteObject( brigApp.avFonts[iFirst].pFont );
         brigApp.avFonts.erase( brigApp.avFonts.begin()+iFirst );
      }
      brigApp.avFonts.push_back( bf );
      return bf.pFont;
   }
   else
   {
      brigApp.avFonts[i].iCount ++;
      return brigApp.avFonts[i].pFont;
   }
}

PBRIG_FONT brigAddFont( PBRIG_FONT pFontBase )
{
   unsigned int i, iFirst = 0;
   BRIGAPP_FONT bf = { 0 };
   PBRIG_FONT pFont = brig_ChooseFont( pFontBase, &bf );

   if( pFont )
   {
      BRIGAPP_FONT bfold;
      for( i = 0; i < brigApp.avFonts.size(); i++ )
      {
         bfold = brigApp.avFonts[i];
         if( bfold.pName == bf.pName && bfold.iHeight == bf.iHeight &&
               bfold.iWeight == bf.iWeight && bfold.iCharSet == bf.iCharSet &&
               bfold.iItalic == bf.iItalic && bfold.iUnderline == bf.iUnderline &&
               bfold.iStrikeOut == bf.iStrikeOut )
            break;
         if( brigApp.avFonts[i].iCount == 0 && !iFirst )
            iFirst = i;
      }
      if( i >= brigApp.avFonts.size() )
      {
         bf.pFont = pFont;
         bf.iCount = 1;
         #if defined( UNICODE )
         bf.bNeedToFree = 1;
         #endif
         if( brigApp.avFonts.size() > BRIGAPP_FONT_LIMIT && iFirst )
         {
            brig_DeleteObject( brigApp.avFonts[iFirst].pFont );
            brigApp.avFonts.erase( brigApp.avFonts.begin()+iFirst );
         }
         brigApp.avFonts.push_back( bf );
         return bf.pFont;
      }
      else
      {
         brigApp.avFonts[i].iCount ++;
         brig_DeleteObject( pFont );
         free( bf.pName );
         return brigApp.avFonts[i].pFont;
      }
   }
   return NULL;
}

void brigDelFont( PBRIG_FONT pFont )
{
   unsigned int i;

   for( i = 0; i < brigApp.avFonts.size(); i++ )
      if( brigApp.avFonts[i].pFont == pFont && brigApp.avFonts[i].iCount )
      {
         brigApp.avFonts[i].iCount --;
         break;
      }
}

PBRIG_PEN brigAddPen( int iWidth, long int lColor, int iStyle )
{
   unsigned int i, iFirst = 0;

   for( i = 0; i < brigApp.avPens.size(); i++ )
   {
      if( brig_ComparePen( brigApp.avPens[i].pPen, iWidth, lColor, iStyle ) )
         break;
      if( brigApp.avPens[i].iCount == 0 && !iFirst )
         iFirst = i;
   }
   if( i >= brigApp.avPens.size() )
   {
      BRIGAPP_PEN bp;
      bp.pPen = brig_CreatePen( iWidth, lColor, iStyle );
      bp.iCount = 1;
      if( brigApp.avPens.size() > BRIGAPP_PEN_LIMIT && iFirst )
      {
         brig_DeleteObject( brigApp.avPens[iFirst].pPen );
         brigApp.avPens.erase( brigApp.avPens.begin()+iFirst );
      }
      brigApp.avPens.push_back( bp );
      return bp.pPen;
   }
   else
   {
      brigApp.avPens[i].iCount ++;
      return brigApp.avPens[i].pPen;
   }
}

void brigDelPen( PBRIG_PEN pPen )
{
   unsigned int i;

   for( i = 0; i < brigApp.avPens.size(); i++ )
      if( brigApp.avPens[i].pPen == pPen && brigApp.avPens[i].iCount )
      {
         brigApp.avPens[i].iCount --;
         break;
      }
}

PBRIG_BRUSH brigAddBrush( long int lColor )
{
   unsigned int i, iFirst = 0;

   for( i = 0; i < brigApp.avBrushes.size(); i++ )
   {
      if( brig_CompareBrush( brigApp.avBrushes[i].pBrush, lColor ) )
         break;
      if( brigApp.avBrushes[i].iCount == 0 && !iFirst )
         iFirst = i;
   }
   if( i >= brigApp.avBrushes.size() )
   {
      BRIGAPP_BRUSH bb;
      bb.pBrush = brig_CreateBrush( lColor );
      bb.iCount = 1;
      if( brigApp.avBrushes.size() > BRIGAPP_BRUSH_LIMIT && iFirst )
      {
         brig_DeleteObject( brigApp.avBrushes[iFirst].pBrush );
         brigApp.avBrushes.erase( brigApp.avBrushes.begin()+iFirst );
      }
      brigApp.avBrushes.push_back( bb );
      return bb.pBrush;
   }
   else
   {
      brigApp.avBrushes[i].iCount ++;
      return brigApp.avBrushes[i].pBrush;
   }
}

void brigDelBrush( PBRIG_BRUSH pBrush )
{
   unsigned int i;

   for( i = 0; i < brigApp.avBrushes.size(); i++ )
      if( brigApp.avBrushes[i].pBrush == pBrush && brigApp.avBrushes[i].iCount )
      {
         brigApp.avBrushes[i].iCount --;
         break;
      }
}

bool brig_CompareStyle( brig_Style *pStyle, unsigned int iColors, long * pColors, int iType,
      long lBorderColor, int iBorder, unsigned int iCorners, long * pCorners )
{
   if( pStyle->avColor.size() == iColors && pStyle->iOrient == iType
      && pStyle->avCorners.size() == iCorners )
   {
      if( !brig_ComparePen( pStyle->pPen, iBorder, lBorderColor, 0 ) )
         return 0;
      if( iColors )
         for( unsigned int i = 0; i < iColors; i++ )
            if( pStyle->avColor[i] != pColors[i] )
               return 0;
      if( iCorners )
         for( unsigned int i = 0; i < iCorners; i++ )
            if( pStyle->avCorners[i] != pCorners[i] )
               return 0;
      return 1;
   }
   return 0;
}

brig_Style * brigAddStyle( int iColors, long * pColors, int iType,
      long lBorderColor, int iBorder, int iCorners, long * pCorners )
{
   unsigned int i, iFirst = 0;

   for( i = 0; i < brigApp.avStyles.size(); i++ )
   {
      if( brig_CompareStyle( brigApp.avStyles[i].pStyle, (unsigned int)iColors, pColors, iType,
         lBorderColor, iBorder, (unsigned int)iCorners, pCorners ) )
         break;
      if( brigApp.avStyles[i].iCount == 0 && !iFirst )
         iFirst = i;
   }
   if( i >= brigApp.avStyles.size() )
   {
      BRIGAPP_STYLE bs;
      bs.pStyle = new brig_Style( iColors, pColors, iType, lBorderColor,
         iBorder, iCorners, pCorners );
      bs.iCount = 1;
      if( brigApp.avStyles.size() > BRIGAPP_STYLE_LIMIT && iFirst )
      {
         delete brigApp.avStyles[iFirst].pStyle;
         brigApp.avStyles.erase( brigApp.avStyles.begin()+iFirst );
      }
      brigApp.avStyles.push_back( bs );
      return bs.pStyle;
   }
   else
   {
      brigApp.avStyles[i].iCount ++;
      return brigApp.avStyles[i].pStyle;
   }
}

brig_Style * brigAddStyle( long lColor, long lBorderColor, int iBorder, int iCorners, long * pCorners )
{
   return brigAddStyle( 1, &lColor, 1, lBorderColor, iBorder, iCorners, pCorners );
}

void brigDelStyle( brig_Style * pStyle )
{
   unsigned int i;

   for( i = 0; i < brigApp.avStyles.size(); i++ )
      if( brigApp.avStyles[i].pStyle == pStyle && brigApp.avStyles[i].iCount )
      {
         brigApp.avStyles[i].iCount --;
         break;
      }
}

unsigned int brigSetTimer( unsigned int uiValue, brig_fnc_menu pfAction, bool bOnce )
{

   BRIGAPP_TIMER bt;
   unsigned int uiId = TIMER_FIRST_ID;

   if( !brigApp.avTimers.empty() )
      uiId = brigApp.avTimers.back().uiId + 1;

   bt.uiId = brig_SetTimer( uiId, uiValue );
   bt.bOnce = bOnce;
   bt.uiVal = uiValue;
   bt.pfAction = pfAction;

   brigApp.avTimers.push_back( bt );

   return bt.uiId;
}

void brigRunTimerFunc( unsigned int uiId )
{
   for( unsigned int i = 0; i < brigApp.avTimers.size(); i++ )
      if( brigApp.avTimers[i].uiId == uiId )
      {
         brigApp.avTimers[i].pfAction();
         if( brigApp.avTimers[i].bOnce )
            brigKillTimer( uiId );
         break;
      }

}

void brigKillTimer( unsigned int uiId )
{
   brig_KillTimer( uiId );

   unsigned int i;

   for( i = 0; i < brigApp.avTimers.size(); i++ )
      if( brigApp.avTimers[i].uiId == uiId )
      {
         brigApp.avTimers.erase( brigApp.avTimers.begin()+i );
         break;
      }

}
