/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig_funcs.h"
#include <math.h>

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
   PBRIG_HDC hDC = (PBRIG_HDC) hb_xgrab( sizeof(PBRIG_HDC) );   

   memset( hDC, 0, sizeof(PBRIG_HDC) );
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
   hb_xfree( pps->hDC );
   hb_xfree( pps );

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
   prc->right =  widget->allocation.width;
   prc->bottom = widget->allocation.height;

}

void brig_FillRect( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom, PBRIG_BRUSH hBrush )
{

   brig__setcolor( hDC->cr, brush->color );
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
   xfree( pFont );
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
      
   PBRIG_BRUSH hbrush = (PBRIG_BRUSH) hb_xgrab( sizeof(BRIG_BRUSH) );

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

   return CreatePen( iStyle, iWidth, lColor );

}

bool brig_ComparePen( PBRIG_PEN pPen, int iWidth, long int lColor, int iStyle )
{
   LOGPEN lp = { 0 };

   GetObject( pPen, sizeof( lp ), &lp );

   return ( iWidth == (int)(lp.lopnWidth.x) && lColor == (long int)(lp.lopnColor) && iStyle == (int)(lp.lopnStyle) );

}

PBRIG_FONT brig_CreateFont( PBRIG_CHAR fontName, int fnWidth, int fnHeight, int fnWeight,
               DWORD fdwCharSet, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut  )
{
   PBRIG_FONT hFont;
   PBRIG_WCHAR wcName = brig_str2WC( fontName );

   hFont = CreateFont( fnHeight,// logical height of font
         fnWidth,               // logical average character width
         0,                     // angle of escapement
         0,                     // base-line orientation angle
         fnWeight,              // font weight
         fdwItalic,             // italic attribute flag
         fdwUnderline,          // underline attribute flag
         fdwStrikeOut,          // strikeout attribute flag
         fdwCharSet,            // character set identifier
         0,                     // output precision
         0,                     // clipping precision
         0,                     // output quality
         0,                     // pitch and family
         wcName                 // pointer to typeface name string
          );

   brig_free( wcName );

   return hFont;
}

long int brig_SetTextColor( PBRIG_DC hDC, long int lColor )
{

   return (long int) SetTextColor( hDC, lColor );
}

long int brig_SetBkColor( PBRIG_DC hDC, long int lColor )
{

   return (long int) SetBkColor( hDC, lColor );
}

void brig_SetTransparentMode( PBRIG_DC hDC, bool bTransp )
{
   SetBkMode( hDC, ( bTransp ) ? TRANSPARENT : OPAQUE );
}

int brig_DrawText( PBRIG_DC hDC, PBRIG_CHAR lpText, int x1, int y1, int x2, int y2, unsigned int uiFormat )
{
   PBRIG_WCHAR wcText = brig_str2WC( lpText );
   RECT rc;
   int height;

   rc.left = x1;
   rc.top  = y1;
   rc.right  = x2;
   rc.bottom = y2;

   height = DrawText( hDC, wcText, -1, &rc, uiFormat );

   brig_free( wcText );
   return height;
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
      brig__setcolor( hDC->cr, pArrColor[0] );
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
         color = ( i < user_colors_num ) ? pArrColor[i] : 0xFFFFFF * i;
         hwg_prepare_cairo_colors( color, &r, &g, &b );
         stop = (gdouble) ( ( i < user_stops_num ) ? pArrStop[i] : 1. / (colors_num-1) * (gdouble)i );
         cairo_pattern_add_color_stop_rgb( pat, stop, r, g, b );
      }
   }

   if( pArrRadius ) {
      user_radiuses_num =  pArrRadius->size();
      max_r = ( x2-x1+1 > y2-y1+1 ) ? y2-y1+1 : x2-x1+1;
      max_r /= 2;

      for ( i = 0; i < 4; i++ )
      {
         radius[i] = ( i < user_radiuses_num ) ? pArrRadius[i] : 0;
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
