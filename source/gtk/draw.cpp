/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Drawing functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"
#include <math.h>

#define  PS_SOLID   0

void brig_parse_color( long lColor, GdkColor * pColor )
{
   char color[10]={0};

   sprintf( color,"#%0*lX",6,lColor );
   color[8] = color[1]; color[9] = color[2];
   color[1] = color[5]; color[2] = color[6];
   color[5] = color[8]; color[6] = color[9];
   color[7] = '\0';
   gdk_color_parse( color,pColor );
}

static void brig__setcolor( cairo_t * cr, long int nColor )
{
   short int r, g, b;

   nColor %= ( 65536 * 256 );
   r = nColor % 256;
   g = ( ( nColor - r ) % 65536 ) / 256;
   b = ( nColor - g - r ) / 65536;

   cairo_set_source_rgb( cr, ( ( double ) r ) / 255.,
          ( ( double ) g ) / 255., ( ( double ) b ) / 255. );

}

PBRIG_PPS brig_BeginPaint( BRIG_HANDLE handle )
{
   PBRIG_PPS pps = ( PBRIG_PPS ) malloc( sizeof( BRIG_PPS ) );
   PBRIG_DC hDC = (PBRIG_DC) malloc( sizeof(PBRIG_DC) );   

   memset( hDC, 0, sizeof(PBRIG_DC) );
   hDC->widget = handle;

   hDC->window = handle->window;
   hDC->cr = gdk_cairo_create( handle->window );

   hDC->layout = pango_cairo_create_layout( hDC->cr );
   hDC->fcolor = hDC->bcolor = -1;
   
   pps->hDC = hDC;

   return pps;
}

void brig_EndPaint( BRIG_HANDLE handle, PBRIG_PPS pps )
{
   SYMBOL_UNUSED( handle );

   if( pps->hDC->layout )
      g_object_unref( (GObject*) pps->hDC->layout );
   free( pps->hDC );
   free( pps );

}

void brig_moveto( PBRIG_DC hDC, int iLeft, int iTop )
{
   cairo_move_to( hDC->cr, (gdouble)iLeft, (gdouble)iTop );
}

void brig_lineto( PBRIG_DC hDC, int iLeft, int iTop )
{
   cairo_line_to( hDC->cr, (gdouble)iLeft, (gdouble)iTop );
   cairo_stroke( hDC->cr );

}

void brig_GetClientRect( BRIG_HANDLE handle, RECT *prc )
{
   prc->left = 0;
   prc->top = 0;
   prc->right =  handle->allocation.width;
   prc->bottom = handle->allocation.height;

}

void brig_FillRect( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom, PBRIG_BRUSH hBrush )
{

   brig__setcolor( hDC->cr, hBrush->color );
   cairo_rectangle( hDC->cr, (gdouble)iLeft, (gdouble)iTop, 
         (gdouble)(iRight-iLeft+1), (gdouble)(iBottom-iTop+1) );
   cairo_fill( hDC->cr );

}

void brig_DrawRect( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom )
{

   cairo_rectangle( hDC->cr, (gdouble)iLeft, (gdouble)iTop,
        (gdouble)(iRight-iLeft+1), (gdouble)(iBottom-iTop+1) );
   cairo_stroke( hDC->cr );
}

void brig_Rectangle( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom )
{

   cairo_rectangle( hDC->cr, (gdouble)iLeft, (gdouble)iTop,
        (gdouble)(iRight-iLeft+1), (gdouble)(iBottom-iTop+1) );
   cairo_stroke( hDC->cr );
}

void brig_Ellipse( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom )
{

   cairo_arc( hDC->cr, (double)iLeft+(iRight-iLeft)/2, (double)iTop+(iBottom-iTop)/2, (double) (iRight-iLeft)/2, 0, 6.28 );
   cairo_stroke( hDC->cr );

}

void brig_RedrawWindow( BRIG_HANDLE handle )
{
   gtk_widget_queue_draw_area( handle, 0, 0,
        handle->allocation.width, handle->allocation.height );
}

void brig_DeleteObject( PBRIG_BRUSH pBrush )
{
   free( pBrush );
}

void brig_DeleteObject( PBRIG_PEN pPen )
{
   free( pPen );
}


void brig_DeleteObject( PBRIG_FONT pFont )
{
   pango_font_description_free( pFont->hFont );
   pango_attr_list_unref( pFont->attrs );
   free( pFont );
}

PBRIG_BRUSH brig_SelectObject( PBRIG_DC hDC, PBRIG_BRUSH pBrush )
{
   brig__setcolor( hDC->cr, pBrush->color );
   return NULL;
}

PBRIG_PEN brig_SelectObject( PBRIG_DC hDC, PBRIG_PEN pPen )
{
   brig__setcolor( hDC->cr, pPen->color );
   cairo_set_line_width( hDC->cr, pPen->width );
   if( pPen->style != PS_SOLID )
   {
      static const double dashed[] = {2.0, 2.0};
      cairo_set_dash( hDC->cr, dashed, 2, 0 );
   }
   return NULL;
}

PBRIG_FONT brig_SelectObject( PBRIG_DC hDC, PBRIG_FONT pFont )
{
   hDC->hFont = pFont->hFont;
   pango_layout_set_font_description( hDC->layout, hDC->hFont );
   if( pFont->attrs )
      pango_layout_set_attributes( hDC->layout, pFont->attrs );

   return NULL;
}

PBRIG_BRUSH brig_CreateBrush( long int lColor )
{
      
   PBRIG_BRUSH hbrush = (PBRIG_BRUSH) malloc( sizeof(BRIG_BRUSH) );

   hbrush->type = BRIG_OBJECT_BRUSH;
   hbrush->color = lColor;
   
   return hbrush;

}

bool brig_CompareBrush( PBRIG_BRUSH pBrush, long int lColor )
{
   return ( lColor == pBrush->color );

}

PBRIG_PEN brig_CreatePen( int iWidth, long int lColor, int iStyle )
{

   PBRIG_PEN hPen = (PBRIG_PEN) malloc( sizeof(BRIG_PEN) );

   hPen->type = BRIG_OBJECT_PEN;
   hPen->style = (GdkLineStyle) iStyle;
   hPen->width = iWidth;
   hPen->color = lColor;
   return hPen;

}

bool brig_ComparePen( PBRIG_PEN pPen, int iWidth, long int lColor, int iStyle )
{
   return ( iWidth == pPen->width && lColor == pPen->color && iStyle == pPen->style );

}

PBRIG_FONT brig_CreateFont( PBRIG_CHAR fontName, int fnHeight, int fnWeight,
               DWORD fdwCharSet, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut  )
{
   PBRIG_FONT pFont = (PBRIG_FONT) malloc( sizeof(BRIG_FONT) );
   PangoFontDescription *  hFont;

   hFont = pango_font_description_new();
   pango_font_description_set_family( hFont, fontName );
   pango_font_description_set_style( hFont, (fdwItalic)? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL );
   pango_font_description_set_size( hFont, fnHeight );
   pango_font_description_set_weight( hFont, (PangoWeight)fnWeight );

   pFont->type = BRIG_OBJECT_FONT;
   pFont->hFont = hFont;
   if( fdwUnderline || fdwStrikeOut )
   {
      pFont->attrs = pango_attr_list_new();
      if( fdwUnderline )
         pango_attr_list_insert( pFont->attrs, pango_attr_underline_new( PANGO_UNDERLINE_SINGLE) );
      if( fdwStrikeOut )
         pango_attr_list_insert( pFont->attrs, pango_attr_strikethrough_new( 1 ) );
   }
   else
      pFont->attrs = NULL;

   return pFont;
}

void brig_SetFont( BRIG_HANDLE handle, PBRIG_FONT pFont )
{
   GtkWidget * hLabel = (GtkWidget*) g_object_get_data( (GObject*) handle,"label" );   

   if( GTK_IS_BUTTON( handle ) )
      handle = gtk_bin_get_child( GTK_BIN( handle ) );
   else if( GTK_IS_EVENT_BOX( handle ) )
      handle = gtk_bin_get_child( GTK_BIN( handle ) );
   else if( hLabel )
      handle = (GtkWidget*) hLabel;
      
   gtk_widget_modify_font( handle, pFont->hFont );

}

long int brig_SetTextColor( PBRIG_DC hDC, long int lColor )
{

   long int lColorPrev = hDC->fcolor;
   hDC->fcolor = lColor;
   return lColorPrev;
}

long int brig_SetBkColor( PBRIG_DC hDC, long int lColor )
{

   long int lColorPrev = hDC->bcolor;
   hDC->bcolor = lColor;
   return lColorPrev;
}

void brig_SetTransparentMode( PBRIG_DC hDC, bool bTransp )
{
   SYMBOL_UNUSED( hDC );
   SYMBOL_UNUSED( bTransp );
}

int brig_DrawText( PBRIG_DC hDC, PBRIG_CHAR lpText, int x1, int y1, int x2, int y2, unsigned int uiFormat )
{
   PangoRectangle rc;
   int iWidth = x2 - x1;

   SYMBOL_UNUSED( y2 );

   if( lpText && strlen( lpText ) > 0 )
   {
      pango_layout_set_text( hDC->layout, lpText, -1 );

      pango_layout_get_pixel_extents( hDC->layout, &rc, NULL );
      pango_layout_set_width( hDC->layout, -1 );

      if( ( uiFormat & ( DT_CENTER | DT_RIGHT ) ) &&
            ( rc.width < ( iWidth-10 ) ) )
      {
         pango_layout_set_width( hDC->layout, iWidth*PANGO_SCALE );
         pango_layout_set_alignment( hDC->layout, 
             (uiFormat & DT_CENTER)? PANGO_ALIGN_CENTER : PANGO_ALIGN_RIGHT );
      }
      else
         pango_layout_set_alignment( hDC->layout, PANGO_ALIGN_LEFT );

      brig__setcolor( hDC->cr, (hDC->fcolor != -1)? hDC->fcolor : 0 );
      cairo_move_to( hDC->cr, (gdouble)x1, (gdouble)y1 );
      pango_cairo_show_layout( hDC->cr, hDC->layout );
   }

   return 0;
}

static void prepare_cairo_colors( long int nColor, gdouble *r, gdouble *g, gdouble *b )
{
   short int int_r, int_g, int_b;
  
   nColor %= ( 65536 * 256 );
   int_r = nColor % 256;
   int_g = ( ( nColor - int_r ) % 65536 ) / 256;
   int_b = ( nColor - int_r - int_g ) / 65536;

   *r = (gdouble)int_r / 255.;
   *g = (gdouble)int_g / 255.;
   *b = (gdouble)int_b / 255.;
}

void brig_DrawGradient( PBRIG_DC hDC, int x1, int y1, int x2, int y2, int type,
      vector<long> *pArrColor, vector<double> *pArrStop, vector<int> *pArrRadius )
{
   long int color;
   gdouble stop;
   gint user_colors_num, colors_num, user_stops_num, i;
   cairo_pattern_t *pat = NULL;
   gdouble x_center, y_center, gr_radius;
   gdouble r, g, b;
   gint radius[4], max_r;
   gint user_radiuses_num;

   if( !pArrColor || ( user_colors_num = (gint) pArrColor->size() ) == 0 )
      return;

   if( user_colors_num == 1 )
      brig__setcolor( hDC->cr, (*pArrColor)[0] );
   else
   {
      // type of gradient
      type = ( type >= 1 && type <= 9 ) ? type : 1;
      switch( type ) 
      {
         case 1:
            // vertical and down
            pat = cairo_pattern_create_linear( x1, y1, x1, y2 );
            break;
         case 2:
            // vertical and up
            pat = cairo_pattern_create_linear( x1, y2, x1, y1 );
            break;
         case 3:
            // horizontal and to the right
            pat = cairo_pattern_create_linear( x1, y1, x2, y1 );
            break;
         case 4:
            // horizontal and to the left
            pat = cairo_pattern_create_linear( x2, y1, x1, y1 );
            break;
         case 5:
            // diagonal right-up
            pat = cairo_pattern_create_linear( x1, y2, x2, y1 );
            break;
         case 6:
            // diagonal left-down
            pat = cairo_pattern_create_linear( x2, y1, x1, y2 );
            break;
         case 7:
            // diagonal right-down
            pat = cairo_pattern_create_linear( x1, y1, x2, y2 );
            break;
         case 8:
            // diagonal left-up
            pat = cairo_pattern_create_linear( x2, y2, x1, y1 );
            break;
         case 9:
            // radial gradient
            x_center = (x2 - x1) / 2 + x1;
            y_center = (y2 - y1) / 2 + y1;
            gr_radius = sqrt( pow(x2-x1,2) + pow(y2-y1,2) ) / 2;
            pat = cairo_pattern_create_radial(x_center, y_center, 0, x_center, y_center, gr_radius);
            break;
      }

      colors_num = user_colors_num;
      user_stops_num = ( pArrStop ) ? (gint) pArrStop->size() : 0;

      for ( i = 0; i < colors_num; i++ )
      {
         color = (long int) ( ( i < user_colors_num ) ? (*pArrColor)[i] : 0xFFFFFF * i );
         prepare_cairo_colors( color, &r, &g, &b );
         stop = (gdouble) ( ( i < user_stops_num ) ? (*pArrStop)[i] : 1. / (colors_num-1) * (gdouble)i );
         cairo_pattern_add_color_stop_rgb( pat, stop, r, g, b );
      }
   }

   if( pArrRadius ) {
      user_radiuses_num =  pArrRadius->size();
      max_r = ( x2-x1+1 > y2-y1+1 ) ? y2-y1+1 : x2-x1+1;
      max_r /= 2;

      for ( i = 0; i < 4; i++ )
      {
         radius[i] = ( i < user_radiuses_num ) ? (*pArrRadius)[i] : 0;
         radius[i] = ( radius[i] >= 0 ) ? radius[i] : 0;
         radius[i] = ( radius[i] <= max_r ) ? radius[i] : max_r;
      }

      cairo_arc( hDC->cr, x1+radius[0], y1+radius[0], radius[0], M_PI, 3*M_PI/2 );
      cairo_arc( hDC->cr, x2-radius[1], y1+radius[1], radius[1], 3*M_PI/2, 0 );
      cairo_arc( hDC->cr, x2-radius[2], y2-radius[2], radius[2], 0, M_PI/2 );
      cairo_arc( hDC->cr, x1+radius[3], y2-radius[3], radius[3], M_PI/2, M_PI );
      cairo_close_path(hDC->cr);
   }
   else
      cairo_rectangle( hDC->cr, x1, y1, x2-x1+1, y2-y1+1 );

   if( user_colors_num > 1 )
      cairo_set_source( hDC->cr, pat );
   cairo_fill( hDC->cr );

   if( user_colors_num > 1 )
      cairo_pattern_destroy( pat );
}
