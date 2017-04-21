/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

static PBRIG_MITEM pMenuDef = NULL;
static int iIdCurr;
static bool bPopup;
static brig_Container *pWndDef;
static std::vector<struct BRIG_MENUITEM_STRU*> avStack;

void brigMenu( brig_Container *pWnd )
{
   avStack.clear();

   pMenuDef = new BRIG_MITEM;
   pMenuDef->szName = NULL;
   pMenuDef->iId = 0;
   pMenuDef->pfMenuItem = NULL;

   iIdCurr = MENU_FIRST_ID;
   bPopup = 0;
   pWndDef = pWnd;
}

bool brigSubMenu( PBRIG_CHAR szCaption, int iId )
{
   if( pMenuDef )
   {
      PBRIG_MITEM pMenuItem = new BRIG_MITEM;
      PBRIG_MITEM pMenuCurr = ( avStack.empty() )? pMenuDef : avStack.back();

      pMenuItem->szName = szCaption;
      pMenuItem->iId = (iId)? iId : ++iIdCurr;
      pMenuItem->pfMenuItem = NULL;
      pMenuCurr->avItems.push_back( pMenuItem );

      avStack.push_back( pMenuItem );
      return 1;
   }
   else
      return 0;
}

static void brigMenuBuild( brig_Container *pWnd, PBRIG_MITEM pMenuCurr, PBRIG_MITEM pMenuParent )
{
   if( pMenuParent )
      pMenuCurr->handle = brig_AddMenuItem( pMenuParent->handle, pMenuCurr->szName, -1, pWnd, pMenuCurr->iId, 1 );
   else
   {
      if( bPopup )
         pMenuCurr->handle = brig_CreatePopupMenu();
      else
         pMenuCurr->handle = brig_CreateMenu();
   }

   for ( unsigned int i = 0; i < pMenuCurr->avItems.size(); i++ )
   {
      if( pMenuCurr->avItems[i]->avItems.empty() )
         brig_AddMenuItem( pMenuCurr->handle, pMenuCurr->avItems[i]->szName, -1, pWnd, pMenuCurr->avItems[i]->iId, 0 );
      else
         brigMenuBuild( NULL, pMenuCurr->avItems[i], pMenuCurr );
   }

   if( !pMenuParent )
   {
      if( !bPopup )
      {
         brig_SetMenu( pWnd->Handle(), pMenuCurr->handle );
         pWnd->pMenu = pMenuDef;
      }
      pMenuDef = NULL;
   }
}

void brigMenuEnd( void )
{
   if( avStack.empty() )
   {
      brigMenuBuild( pWndDef, pMenuDef, NULL );
   }
   else
      avStack.pop_back();
}

void brigMenuItemAdd( PBRIG_CHAR szName, int iId, brig_fnc_menu pfMenuItem, int iFlag, int iKey )
{
   PBRIG_MITEM pMenuItem = new BRIG_MITEM;
   PBRIG_MITEM pMenuCurr = ( avStack.empty() )? pMenuDef : avStack.back();

   SYMBOL_UNUSED( iFlag );
   SYMBOL_UNUSED( iKey );

   pMenuItem->szName = szName;
   pMenuItem->iId = (iId)? iId : ++iIdCurr;
   pMenuItem->pfMenuItem = pfMenuItem;

   pMenuCurr->avItems.push_back( pMenuItem );

}

PBRIG_MITEM brig_MenuItemFind( PBRIG_MITEM pMenu, int iId )
{
   PBRIG_MITEM pMenuRes;

   //brig_writelog( NULL, "find-0 %d %u\r\n", iId, pMenu->avItems.size() );
   for( unsigned int i = 0; i < pMenu->avItems.size(); i++ )
   {
      //brig_writelog( NULL, "find-1 %d %d\r\n", i, pMenu->avItems[i]->iId );
      if( pMenu->avItems[i]->iId == iId )
         return pMenu->avItems[i];
      else if( !pMenu->avItems[i]->avItems.empty() &&
         ( pMenuRes = brig_MenuItemFind( pMenu->avItems[i], iId ) ) != NULL )
         return pMenuRes;
   }
   return NULL;
}
