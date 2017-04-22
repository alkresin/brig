/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Menu basic functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

BRIG_HMENU brig_CreateMenu( void )
{
   return CreateMenu();
}

BRIG_HMENU brig_CreatePopupMenu( void )
{
   return CreatePopupMenu();
}

int brig_SetMenu( HWND hWnd, HMENU hMenu )
{
   return SetMenu( hWnd, hMenu );
}

HMENU brig_AddMenuItem( HMENU hMenu, PBRIG_CHAR szCaption, int iPos,
         brig_Container *pWnd, unsigned int uiId, bool bSubMenu )
{
   unsigned int uiFlags = MF_BYPOSITION;
   PBRIG_WCHAR wcCaption = NULL;
   MENUITEMINFO mii;
   HMENU hSubMenu = NULL;

   SYMBOL_UNUSED( pWnd );

   if( szCaption )
      wcCaption = brig_str2WC( szCaption );
   else
      uiFlags |= MF_SEPARATOR;

   if( bSubMenu )
   {
      int iCou;
      hSubMenu = CreateMenu();

      uiFlags |= MF_POPUP;
      InsertMenu( hMenu, iPos, uiFlags, ( UINT ) hSubMenu, wcCaption );

      // Code to set the ID of submenus, the API seems to assume that you wouldn't really want to,
      // but if you are used to getting help via IDs for popups in 16bit, then this will help you.
      iCou = GetMenuItemCount( hMenu );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_ID;
      if( GetMenuItemInfo( hMenu, iCou - 1, TRUE, &mii ) )
      {
         mii.wID = uiId;
         SetMenuItemInfo( hMenu, iCou - 1, TRUE, &mii );
      }
   }
   else
      InsertMenu( hMenu, iPos, uiFlags, uiId, wcCaption );

   brig_free( wcCaption );

   return hSubMenu;
}
