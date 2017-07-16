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

static brig_TreeNode * GetNodeByPos( brig_Tree * pTree, int yPos )
{
   int y = 0;
   brig_TreeNode * pNode = pTree->pFirst;

   while( pNode )
   {
      y += pTree->uiRowHeight;
      if( y >= yPos )
         return pNode;
      pNode = NextNode( pNode, 1 );
   }

   return NULL;
}

static void paint_tree_node( brig_TreeNode * pNode, PBRIG_DC hDC, int y )
{

   unsigned int x = pNode->pTree->uiIndent/2 + pNode->pTree->uiIndent * pNode->iLevel;
   vector<brig_TreeNode*> * pItems = (pNode->pParent)? &(pNode->pParent->avItems) : &(pNode->pTree->avItems);
   bool bSelected = ( pNode->pTree->pSelected && pNode->handle == pNode->pTree->pSelected->handle );

   brig_SelectObject( hDC, pNode->pTree->pPenLine );
   brig_DrawLine( hDC, x+5, y+9, x+pNode->pTree->uiIndent-4, y+9 );

   if( pNode->iLevel || (*pItems)[0]->handle != pNode->handle )
      brig_DrawLine( hDC, x+5, y, x+5, y+9 );

   if( pNode->handle != (*pItems).back()->handle )
      brig_DrawLine( hDC, x+5, y+9, x+5, y+pNode->pTree->uiRowHeight+1 );

   if( !pNode->avItems.empty() )
   {
      brig_Rectangle( hDC, x, y+4, x+8, y+12 );
      if( !pNode->bExpanded )
      {
         brig_SelectObject( hDC, pNode->pTree->pPenPlus );
         brig_DrawLine( hDC, x+5, y+5, x+5, y+12 );
         brig_DrawLine( hDC, x+1, y+9, x+8, y+9 );
         brig_SelectObject( hDC, pNode->pTree->pPenLine );
      }
   }

   x += pNode->pTree->uiIndent;
   if( pNode->iImage >= 0 )
   {
      PBRIG_BITMAP hBitmap = ( bSelected && pNode->iSelectedImage >= 0 )?
            pNode->pTree->avImages[pNode->iSelectedImage] : pNode->pTree->avImages[pNode->iImage];
      int iWidth, iHeight;

      brig_GetBitmapSize( hBitmap, &iWidth, &iHeight );
      brig_DrawBitmap( hDC, hBitmap, x, y, iWidth, iHeight );
      x += iWidth + 4;
   }
   if( bSelected )
   {
      unsigned int uiWidth = brig_GetTextWidth( hDC, pNode->szTitle );
      brig_SetTextColor( hDC, pNode->pTree->lTextSelColor );
      brig_FillRect( hDC, x, y, x+uiWidth+2, y+pNode->pTree->uiRowHeight+1, pNode->pTree->hBrushSel );
   }
   brig_DrawText( hDC, pNode->szTitle, x, y, pNode->pTree->iWidth, y+pNode->pTree->uiRowHeight+1, 0 );
   brig_SetTextColor( hDC, pNode->pTree->lTextColor );
}

static void paint_tree( brig_Tree * pTree )
{
   PBRIG_PPS pps = brig_BeginPaint( pTree );
   RECT rc;
   int y;
   brig_TreeNode * pNode;

   brig_GetClientRect( pTree, &rc );

   if( pTree->hFont )
      brig_SelectObject( pps->hDC, pTree->hFont );

   if( !pTree->pPenLine )
   {
      pTree->pPenPlus = brigAddPen( 2, 0 );
      pTree->pPenLine = brigAddPen( 1, COLOR_TREE_LINE, PS_DOT );
      pTree->uiRowHeight = brig_GetCharHeight( pps->hDC );
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
   brig_Tree * pTree = (brig_Tree *) g_object_get_data( (GObject*) widget, "obj" );
   int iMessage;
   brig_TreeNode * pNode;

   if( pTree )
   {
      brig_TreeNode * pSel = pTree->pSelected;

      if( event->type == GDK_BUTTON_PRESS || 
          event->type == GDK_2BUTTON_PRESS ||
          event->type == GDK_BUTTON_RELEASE )
      {
         if( ((GdkEventButton*)event)->button == 3 )
            iMessage = (event->type==GDK_BUTTON_PRESS)? WM_RBUTTONDOWN : 
                 ( (event->type==GDK_BUTTON_RELEASE)? WM_RBUTTONUP : WM_LBUTTONDBLCLK );
         else
            iMessage = (event->type==GDK_BUTTON_PRESS)? WM_LBUTTONDOWN : 
                 ( (event->type==GDK_BUTTON_RELEASE)? WM_LBUTTONUP : WM_LBUTTONDBLCLK );
      }
      else
         sscanf( (char*)data, "%d", &iMessage );

      switch( iMessage ) {
         case WM_PAINT:
            paint_tree( pTree );
            break;

         case WM_LBUTTONDOWN:
            pNode = GetNodeByPos( pTree, ((GdkEventButton*)event)->y );
            if( pNode )
            {
               int x1 = pNode->pTree->uiIndent/2 + pNode->pTree->uiIndent * pNode->iLevel;
               int xPos = ((GdkEventButton*)event)->x;
               if( xPos > x1 )
               {
                  if( xPos < (int)(x1 + pNode->pTree->uiIndent) )
                  {
                     if( pNode->bExpanded )
                     {
                        pNode->bExpanded = 0;
                        brig_RedrawWindow( pTree );
                     }
                     else if( !pNode->avItems.empty() )
                     {
                        pNode->bExpanded = 1;
                        brig_RedrawWindow( pTree );
                     }
                  }
                  else
                  {
                     pNode->pTree->pSelected = pNode;
                     brig_RedrawWindow( pTree );
                  }
               }
            }
            break;

         case WM_RBUTTONDOWN:
            pNode = GetNodeByPos( pTree, ((GdkEventButton*)event)->y );
            if( pNode )
            {
               int x1 = pNode->pTree->uiIndent/2 + pNode->pTree->uiIndent * pNode->iLevel;
               int xPos = ((GdkEventButton*)event)->x;
               if( xPos > (int)(x1 + pNode->pTree->uiIndent) )
               {
                  pNode->pTree->pSelected = pNode;
                  brig_RedrawWindow( pTree );
                  if( pNode->pfRClick )
                     pNode->pfRClick( pNode );
               }
            }
            break;

         case WM_LBUTTONDBLCLK:
            pNode = GetNodeByPos( pTree, ((GdkEventButton*)event)->y );
            if( pNode )
            {
               int x1 = pNode->pTree->uiIndent/2 + pNode->pTree->uiIndent * pNode->iLevel;
               int xPos = ((GdkEventButton*)event)->x;
               if( xPos > (int)(x1 + pNode->pTree->uiIndent) )
                  if( pNode->pfDblClick )
                     pNode->pfDblClick( pNode );
            }
            break;

         case WM_VSCROLL:
            {
               GtkAdjustment *adj = ( GtkAdjustment * ) g_object_get_data( (GObject*) widget, "adjv" );
               int iScrollV = (int) adj->value;
               int iScrV = (int) g_object_get_data( (GObject*) widget, "iscrv" );
               int iStep = (int) adj->step_increment;
               int iPageStep = (int) adj->page_increment;

               g_debug( "scroll %d", iScrollV );
               pNode = pTree->pFirst;
               if( !pNode )
                  return 0;
               if( iScrollV - iScrV == iStep )
               {
                  pNode = NextNode( pNode, 1 );
                  if( !pNode )
                     return 0;
                  pTree->pFirst = pNode;
                  brig_SetScrollPos( pTree, 1, iScrollV+1 );
                  brig_RedrawWindow( pTree );
               }
               else if( iScrollV - iScrV == -iStep )
               {
               }
               else if( iScrollV - iScrV == iPageStep )
               {
               }
               else if( iScrollV - iScrV == -iPageStep )
               {
               }
               else
               {
               }

            }
            break;

      }

      if( pTree->pSelected && ( !pSel || pSel->handle != pTree->pSelected->handle ) )
         if( pTree->pSelected->pfAction )
            pTree->pSelected->pfAction( pTree->pSelected );
   }
   return 0;
}

BRIG_HANDLE brig_CreateTree( brig_Tree *pTree, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle )
{
   BRIG_HANDLE handle = pTree->pParent->Handle();
   GtkWidget *vbox, *hbox;
   GtkWidget *vscroll; //, *hscroll;
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
      //brig_SetSignal( ( gpointer ) adjV, (char*)"value_changed", WM_VSCROLL, 0, 0 );
      g_signal_connect( area, (char*)"value_changed", G_CALLBACK (cb_tree), g_strdup((char*)"277") );
   }
   
   gtk_box_pack_start( GTK_BOX( vbox ), area, TRUE, TRUE, 0 );
   /*
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
   */
   box = ( GtkFixed * ) g_object_get_data( ( GObject * ) handle, "fbox" );
   if( box )
      gtk_fixed_put( box, hbox, x, y );
   g_object_set_data( ( GObject * ) area, "pbox", ( gpointer ) hbox );
   gtk_widget_set_size_request( hbox, nWidth, nHeight );

   g_signal_connect( area, (char*)"expose_event", G_CALLBACK (cb_tree), g_strdup((char*)"015") );

   GTK_WIDGET_SET_FLAGS( area, GTK_CAN_FOCUS );

   gtk_widget_add_events( area, GDK_BUTTON_PRESS_MASK |
         GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |
         GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK );

   g_signal_connect( area, (char*)"button_press_event", G_CALLBACK (cb_tree), NULL );
   g_signal_connect( area, (char*)"button_release_event", G_CALLBACK (cb_tree), NULL );
   brig_SetEvent( ( gpointer ) area, (char*)"motion_notify_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"key_press_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"key_release_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) area, (char*)"scroll_event", 0, 0, 0 );

   pTree->pPenLine = pTree->pPenPlus = NULL;
   pTree->lTextSelColor = COLOR_WHITE;
   pTree->lBackSelColor = COLOR_BLUE;
   pTree->hBrushSel = brigAddBrush( pTree->lBackSelColor );
   pTree->lNodeCount = 0;
   pTree->uiIndent = 20;
   pTree->pFirst = pTree->pSelected = NULL;

   return area;
}

BRIG_TNHANDLE brig_TreeAddNode( brig_TreeNode * pNode, brig_Tree * pTree,
      PBRIG_CHAR szTitle, brig_TreeNode * pParent, brig_TreeNode * pPrev,
      int iPos, int iImage, int iSelectedImage )
{
   int iLen = strlen( szTitle );

   SYMBOL_UNUSED( pPrev );
   SYMBOL_UNUSED( iPos );

   pNode->iImage = iImage;
   pNode->iSelectedImage = iSelectedImage;

   pNode->szTitle = (PBRIG_CHAR) malloc( iLen+1 );
   memcpy( pNode->szTitle, szTitle, iLen );
   pNode->szTitle[iLen] = '\0';

   pNode->pParent = pParent;
   pNode->iLevel = ( pParent )? pParent->iLevel + 1 : 0;
   pNode->handle = ++pTree->lNodeCount;
   pNode->bExpanded = 0;

   return pNode->handle;
}

void brig_TreeAddImage( brig_Tree * pTree, PBRIG_BITMAP pBitmap )
{
   pTree->avImages.push_back( pBitmap );
}

static void TreeNodeRelease( vector<brig_TreeNode*> * pItems )
{
   for( unsigned int ui = 0; ui != (*pItems).size(); ui ++ )
   {
      if( !( (*pItems)[ui]->avItems.empty() ) )
         TreeNodeRelease( & ( (*pItems)[ui]->avItems ) );
      free( (*pItems)[ui]->szTitle );
      delete (*pItems)[ui];
   }
}

void brig_TreeRelease( brig_Tree * pTree )
{

   TreeNodeRelease( & ( pTree->avItems ) );

   brigDelBrush( pTree->hBrushSel );
}
