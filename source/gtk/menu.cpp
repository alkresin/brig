/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Menu basic functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

extern void cb_signal( GtkWidget *widget, gchar* data );

BRIG_HMENU brig_CreateMenu( void )
{
   return gtk_menu_bar_new();
}

BRIG_HMENU brig_CreatePopupMenu( void )
{
   return gtk_menu_new();
}

int brig_SetMenu( BRIG_HANDLE hWnd, BRIG_HMENU hMenu )
{
   GtkFixed * box = ( GtkFixed * ) g_object_get_data( (GObject*) hWnd, "fbox" );
   GtkWidget * vbox = ( (GtkWidget*)box )->parent;
   gtk_box_pack_start( GTK_BOX (vbox), (GtkWidget*)hMenu, FALSE, FALSE, 0);
   gtk_box_reorder_child(GTK_BOX(vbox), (GtkWidget*)hMenu, 0);

   return 1;

}

/*
 *  AddMenuItem( hMenu,cCaption,nPos,hWnd,nId,fState,lSubMenu ) --> hMenuItem
 */
BRIG_HMENU brig_AddMenuItem( BRIG_HMENU hMenu, PBRIG_CHAR szCaption, int iPos,
         brig_Container *pWnd, unsigned int uiId, bool bSubMenu )
{
   GtkWidget * hMenuItem, * hSubMenu;
   bool lString = 0, lStock = 0;

   if( szCaption )
   {
      if( *szCaption == '%' )
         lStock = 1;
      else
         lString = 1;
   }

   /*
   if( lCheck )
   {
      gchar * gcptr = hwg_convert_to_utf8( lpNewItem );
      hMenu = gtk_check_menu_item_new_with_mnemonic( gcptr );
      g_free( gcptr );
   }
   else */
   if( lStock )
      hMenuItem = (GtkWidget *) gtk_image_menu_item_new_from_stock( szCaption+1,NULL );
   else if( lString )
      hMenuItem = (GtkWidget *) gtk_menu_item_new_with_mnemonic( szCaption );
   else
      hMenuItem = (GtkWidget *) gtk_separator_menu_item_new();

   if( bSubMenu )
   {
      hSubMenu = gtk_menu_new();
      gtk_menu_item_set_submenu( GTK_MENU_ITEM (hMenuItem), hSubMenu );
   }
   else
   {
      char buf[20]={0};
      sprintf( buf,"0 %ld %ld", (long) uiId, ( long ) pWnd->Handle() );
      g_signal_connect(G_OBJECT (hMenuItem), "activate",
          G_CALLBACK (cb_signal), (gpointer) g_strdup (buf));
   }
   gtk_menu_shell_append( GTK_MENU_SHELL( hMenu ), hMenuItem );

   gtk_widget_show( hMenuItem );

   return (bSubMenu)? hSubMenu : hMenuItem;

}
