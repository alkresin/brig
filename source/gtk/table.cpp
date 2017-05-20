/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * brig_Table implementation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

extern PangoContext *pangoContext;
extern PangoLayout *pangoLayout;

extern void brig_SetEvent( gpointer handle, char * cSignal, long int p1, long int p2, long int p3 );
extern void brig_SetSignal( gpointer handle, char * cSignal, long int p1, long int p2, long int p3 );

void brig_table_OnVScroll( brig_Table *pTable, WPARAM wParam )
{
   GObject *handle = (GObject*) pTable->Handle();
   GtkAdjustment *adj = ( GtkAdjustment * ) g_object_get_data( handle, "adjv" );
   int iScrollV = adj->value;
   int iScrV = (int) g_object_get_data( handle, "iscrv" );

   SYMBOL_UNUSED( wParam );

   if( iScrollV - iScrV == 1 )
      pTable->Down();
   else if( iScrollV - iScrV == -1 )
      pTable->Up();

   g_object_set_data( handle, "iscrv", ( gpointer ) iScrollV );
}

void brig_table_OnHScroll( brig_Table *pTable, WPARAM wParam )
{
   SYMBOL_UNUSED( pTable );
   SYMBOL_UNUSED( wParam );
}

void brig_table_OnWheel( brig_Table *pTable, WPARAM wParam )
{
   unsigned int uiCode = ( (unsigned long) wParam ) & 0xFFFF;
   unsigned int uiHiWord = ( ( (unsigned long) wParam ) >> 16 ) & 0xFFFF;
   int iDelta = ( uiHiWord>32768 )? uiHiWord - 65535 : uiHiWord;

   if( uiCode & MK_MBUTTON )
   {
      if( iDelta > 0 )
      {
      }
   }
   else
   {
      if( iDelta > 0 )
         pTable->Up();
      else
         pTable->Down();
   }

}

BRIG_HANDLE brig_CreateTable( brig_Table *pTable, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_FONT hFont, unsigned int iRows )
{
   BRIG_HANDLE handle = pTable->pParent->Handle();
   GtkWidget *vbox, *hbox;
   GtkWidget *vscroll, *hscroll;
   GtkWidget *area;
   GtkFixed *box;

   area = gtk_drawing_area_new();

   if( hFont && iRows )
   {
      //cairo_t * cr = gdk_cairo_create( area->window );
      //PangoLayout *layout = pango_cairo_create_layout( cr );
      //PangoContext * context = pango_layout_get_context( layout );
      PangoFontMetrics * metrics = pango_context_get_metrics( pangoContext, hFont->hFont, NULL );
      int iHeight =  ( pango_font_metrics_get_ascent( metrics ) +
               pango_font_metrics_get_descent( metrics ) ) / PANGO_SCALE;

      //g_object_unref( (GObject*) layout );
      //cairo_destroy( cr );

      iHeight = (iHeight + pTable->pPadding[1] + pTable->pPadding[3]) * iRows + 2;
      if( iHeight < nHeight )
      {
         nHeight = iHeight;
         pTable->iHeight = iHeight;
      }
      else if( iHeight > nHeight )
         ulStyle |= WS_VSCROLL;
   }

   hbox = gtk_hbox_new( FALSE, 0 );
   vbox = gtk_vbox_new( FALSE, 0 );

   gtk_box_pack_start( GTK_BOX( hbox ), vbox, TRUE, TRUE, 0 );
   if( ulStyle & WS_VSCROLL )
   {
      GtkObject *adjV;
      adjV = gtk_adjustment_new( 0.0, 0.0, 101.0, 1.0, 10.0, 10.0 );
      vscroll = gtk_vscrollbar_new( GTK_ADJUSTMENT( adjV ) );
      gtk_box_pack_end( GTK_BOX( hbox ), vscroll, FALSE, FALSE, 0 );
      g_object_set_data( ( GObject * ) area, "adjv", ( gpointer ) adjV );
      g_object_set_data( ( GObject * ) area, "iscrv", ( gpointer ) 0 );

      g_object_set_data( (GObject*) adjV, "obj", (gpointer) pTable );
      brig_SetSignal( ( gpointer ) adjV, (char*)"value_changed", WM_VSCROLL, 0, 0 );
   }

   gtk_box_pack_start( GTK_BOX( vbox ), area, TRUE, TRUE, 0 );
   if( ulStyle & WS_HSCROLL )
   {
      GtkObject *adjH;
      adjH = gtk_adjustment_new( 0.0, 0.0, 101.0, 1.0, 10.0, 10.0 );
      hscroll = gtk_hscrollbar_new( GTK_ADJUSTMENT( adjH ) );
      gtk_box_pack_end( GTK_BOX( vbox ), hscroll, FALSE, FALSE, 0 );
      g_object_set_data( ( GObject * ) area, "adjh", ( gpointer ) adjH );
      g_object_set_data( ( GObject * ) area, "iscrh", ( gpointer ) 0 );

      g_object_set_data( (GObject*) adjH, "obj", (gpointer) pTable );
      brig_SetSignal( ( gpointer ) adjH, (char*)"value_changed", WM_HSCROLL, 0, 0 );
   }

   box = ( GtkFixed * ) g_object_get_data( ( GObject * ) handle, "fbox" );
   if( box )
      gtk_fixed_put( box, hbox, x, y );
   g_object_set_data( ( GObject * ) area, "pbox", ( gpointer ) hbox );
   gtk_widget_set_size_request( hbox, nWidth, nHeight );

   //temp = HB_PUTHANDLE( NULL, area );
   //SetObjectVar( pObject, "_AREA", temp );
   //hb_itemRelease( temp );

   //SetWindowObject( area, pObject );
   brig_SetEvent( ( gpointer ) area, (char*)"expose_event", WM_PAINT, 0, 0 );

   GTK_WIDGET_SET_FLAGS( area, GTK_CAN_FOCUS );

   gtk_widget_add_events( area, GDK_BUTTON_PRESS_MASK |
         GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |
         GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK );
   brig_SetEvent( ( gpointer ) area, (char*)"button_press_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"button_release_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"motion_notify_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"key_press_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"key_release_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"scroll_event", 0, 0, 0 );

   //all_signal_connect( ( gpointer ) area );

   return area;
}
