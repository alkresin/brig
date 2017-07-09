/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Tree widget
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

#define COLOR_TREE_LINE   0x737373

extern void cb_signal( GtkWidget *widget, gchar* data );
extern void brig_SetEvent( gpointer handle, char * cSignal, long int p1, long int p2, long int p3 );
extern void brig_SetSignal( gpointer handle, char * cSignal, long int p1, long int p2, long int p3 );
extern void brig_parse_color( long lColor, GdkColor * pColor );

static brig_TreeNode * NextNode( brig_TreeNode * pNode, bool bFirst )
{
   vector<brig_TreeNode*> * pItems;

   if( bFirst && pNode->bExpanded )
      return pNode->avItems[0];

   pItems = (pNode->pParent)? &(pNode->pParent->avItems) : &(pNode->pTree->avItems);
   for( unsigned int ui = 0; ui != (*pItems).size(); ui ++ )
      if( (*pItems)[ui]->handle == pNode->handle )
      {
         if( ++ui < (*pItems).size() )
            return (*pItems)[ui];
         else if( pNode->pParent )
            return NextNode( pNode->pParent, 0 );
         else
            return NULL;
      }

   return NULL;
}

static void paint_tree_node( brig_TreeNode * pNode, PBRIG_DC hDC, int y )
{

   unsigned int x = pNode->pTree->uiIndent/2 + pNode->pTree->uiIndent * pNode->iLevel;
   vector<brig_TreeNode*> * pItems = (pNode->pParent)? &(pNode->pParent->avItems) : &(pNode->pTree->avItems);

   brig_SelectObject( hDC, pNode->pTree->pPenLine );
   brig_DrawLine( hDC, x+1, y+9, x+pNode->pTree->uiIndent-4, y+9 );

   if( pNode->iLevel && (*pItems)[0]->handle != pNode->handle )
      brig_DrawLine( hDC, x+5, y+9, x+5, y+pNode->pTree->uiRowHeight+1 );

   if( pNode->handle != (*pItems).back()->handle )
   {
   }
   if( !pNode->avItems.empty() )
   {
   }
}

static void paint_tree( brig_Tree * pTree )
{
   PBRIG_PPS pps = brig_BeginPaint( pTree );
   RECT rc;
   unsigned long ulRecCount;
   unsigned int y;
   brig_TreeNode * pNode;

   brig_GetClientRect( pTree, &rc );

   if( !pTree->pPenLine )
   {
      pTree->pPenLine = brigAddPen( 1, COLOR_TREE_LINE, PS_DOT );
      pTree->pPenPlus = brigAddPen( 2, 0 );
      if( pTree->hFont )
         brig_SelectObject( pps->hDC, pTree->hFont );
      pTree->uiRowHeight = brig_GetCharHeight( pps->hDC ) + 8;
   }

   if( !pTree->hBrush )
      pTree->SetBackColor( COLOR_WHITE, 0 );

   brig_FillRect( pps->hDC, rc.left, rc.top, rc.right, rc.bottom, pTree->hBrush );

   if( pTree->avItems.empty() )
   {
      brig_EndPaint( pTree, pps );
      return;
   }
   else if( !pTree->pFirst )
      pTree->pFirst = pTree->avItems[0];

   y = rc.top;
   pNode = pTree->pFirst;

   while( y < rc.bottom && pNode )
   {
      paint_tree_node( pNode, pps->hDC, y );
      pNode = NextNode( pNode, 1 );
      y += pTree->uiRowHeight;
   }

   brig_EndPaint( pTree, pps );
}

static gint cb_tree( GtkWidget *widget, GdkEvent * event, gchar* data )
{
   gpointer gObject = g_object_get_data( (GObject*) widget, "obj" );

   SYMBOL_UNUSED( data );

   if( gObject )
   {
      paint_tree( (brig_Tree *) gObject );
   }
   return 0;
}

BRIG_HANDLE brig_CreateTree( brig_Tree *pTree, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle )
{
   BRIG_HANDLE handle = pTree->pParent->Handle();
   GtkWidget *vbox, *hbox;
   GtkWidget *vscroll, *hscroll;
   GtkWidget *area;
   GtkFixed *box;

   area = gtk_drawing_area_new();

   /*
   if( hFont && iRows )
   {
      PangoFontMetrics * metrics = pango_context_get_metrics( pangoContext, hFont->hFont, NULL );
      int iHeight =  ( pango_font_metrics_get_ascent( metrics ) +
               pango_font_metrics_get_descent( metrics ) ) / PANGO_SCALE;

      iHeight = (iHeight + pTree->pPadding[1] + pTree->pPadding[3]) * iRows + 2;
      if( iHeight < nHeight )
      {
         nHeight = iHeight;
         pTree->iHeight = iHeight;
      }
      else if( iHeight > nHeight )
         ulStyle |= WS_VSCROLL;
   }
   */

   hbox = gtk_hbox_new( FALSE, 0 );
   vbox = gtk_vbox_new( FALSE, 0 );

   gtk_box_pack_start( GTK_BOX( hbox ), vbox, TRUE, TRUE, 0 );
   
   if( ulStyle & WS_VSCROLL )
   {
      GtkObject *adjV = gtk_adjustment_new( 0.0, 0.0, 400.0, 10.0, 20.0, 20.0 );
      vscroll = gtk_vscrollbar_new( GTK_ADJUSTMENT( adjV ) );
      gtk_box_pack_end( GTK_BOX( hbox ), vscroll, FALSE, FALSE, 0 );
      g_object_set_data( ( GObject * ) area, "adjv", ( gpointer ) adjV );
      g_object_set_data( ( GObject * ) area, "iscrv", ( gpointer ) 0 );

      g_object_set_data( (GObject*) adjV, "obj", (gpointer) pTree );
      brig_SetSignal( ( gpointer ) adjV, (char*)"value_changed", WM_VSCROLL, 0, 0 );
   }
   
   gtk_box_pack_start( GTK_BOX( vbox ), area, TRUE, TRUE, 0 );
   
   if( ulStyle & WS_HSCROLL )
   {
      GtkObject *adjH = gtk_adjustment_new( 0.0, 0.0, 101.0, 1.0, 10.0, 10.0 );
      hscroll = gtk_hscrollbar_new( GTK_ADJUSTMENT( adjH ) );
      gtk_box_pack_end( GTK_BOX( vbox ), hscroll, FALSE, FALSE, 0 );
      g_object_set_data( ( GObject * ) area, "adjh", ( gpointer ) adjH );
      g_object_set_data( ( GObject * ) area, "iscrh", ( gpointer ) 0 );

      g_object_set_data( (GObject*) adjH, "obj", (gpointer) pTree );
      brig_SetSignal( ( gpointer ) adjH, (char*)"value_changed", WM_HSCROLL, 0, 0 );
   }
   
   box = ( GtkFixed * ) g_object_get_data( ( GObject * ) handle, "fbox" );
   if( box )
      gtk_fixed_put( box, hbox, x, y );
   g_object_set_data( ( GObject * ) area, "pbox", ( gpointer ) hbox );
   gtk_widget_set_size_request( hbox, nWidth, nHeight );

   brig_SetEvent( ( gpointer ) area, (char*)"expose_event", WM_PAINT, 0, 0 );
   g_signal_connect( area, (char*)"expose_event", G_CALLBACK (cb_tree), NULL );

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

   pTree->pPenLine = pTree->pPenPlus = NULL;
   pTree->lNodeCount = 0;
   pTree->uiIndent = 20;

   return area;
}

BRIG_TNHANDLE brig_TreeAddNode( brig_TreeNode * pNode, brig_Tree * pTree,
      PBRIG_CHAR szTitle, brig_TreeNode * pParent, brig_TreeNode * pPrev,
      int iPos, int iImage, int iSelectedImage )
{
   SYMBOL_UNUSED( pPrev );
   SYMBOL_UNUSED( iPos );
   SYMBOL_UNUSED( iImage );
   SYMBOL_UNUSED( iSelectedImage );

   pNode->szTitle = szTitle;
   pNode->pParent = pParent;
   pNode->iLevel = ( pParent )? pParent->iLevel + 1 : 0;
   pNode->handle = ++pTree->lNodeCount;
   pNode->bExpanded = 0;

   return pNode->handle;
}

void brig_TreeAddImage( brig_Tree * pTree, PBRIG_BITMAP pBitmap )
{

   SYMBOL_UNUSED( pTree );
   SYMBOL_UNUSED( pBitmap );
}
