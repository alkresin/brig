/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Drawing functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"
#include <math.h>

#if defined( __BORLANDC__ ) && __BORLANDC__ == 0x0550
#ifdef __cplusplus
extern "C"
{
   STDAPI OleLoadPicture( LPSTREAM, LONG, BOOL, REFIID, PVOID * );
}
#endif
#endif /* __BORLANDC__ */
/*
#ifdef __cplusplus
#ifdef CINTERFACE
#undef CINTERFACE
#endif
#endif
*/

#include <olectl.h>
#include <ole2.h>
#include <ocidl.h>

PBRIG_PPS brig_BeginPaint( brig_Widget *pWidget )
{
   PBRIG_PPS pps = ( BRIG_PPS * ) malloc( sizeof( BRIG_PPS ) );

   pps->pps = ( PAINTSTRUCT * ) malloc( sizeof( PAINTSTRUCT ) );
   pps->hDC = BeginPaint( pWidget->Handle(), pps->pps );

   return pps;
}

void brig_EndPaint( brig_Widget *pWidget, PBRIG_PPS pps )
{

   EndPaint( pWidget->Handle(), pps->pps );
   free( pps->pps );
   free( pps );

}

PBRIG_DC brig_GetDC( brig_Widget *pWidget )
{
   return GetDC( pWidget->Handle() );
}

void brig_ReleaseDC( brig_Widget *pWidget, PBRIG_DC hDC )
{
   ReleaseDC( pWidget->Handle(), hDC );
}

void brig_moveto( PBRIG_DC hDC, int iLeft, int iTop )
{
   MoveToEx( hDC, iLeft, iTop, NULL );
}

void brig_lineto( PBRIG_DC hDC, int iLeft, int iTop )
{
   LineTo( hDC, iLeft, iTop );
}

void brig_GetClientRect( brig_Widget *pWidget, RECT *prc )
{
   GetClientRect( pWidget->Handle(), prc );
}

void brig_FillRect( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom, HBRUSH hBrush )
{
   RECT rc;

   rc.left = iLeft;
   rc.top = iTop;
   rc.right = iRight;
   rc.bottom = iBottom;
   FillRect( hDC, &rc, hBrush );
}

void brig_DrawRect( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom )
{

   MoveToEx( hDC, iLeft, iTop, NULL );
   LineTo( hDC, iRight-1, iTop );
   LineTo( hDC, iRight-1, iBottom-1 );
   LineTo( hDC, iLeft, iBottom-1 );
   LineTo( hDC, iLeft, iTop );
}

void brig_Rectangle( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom )
{

   Rectangle( hDC, iLeft, iTop, iRight, iBottom );
}

void brig_Ellipse( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom )
{

   Ellipse( hDC, iLeft, iTop, iRight, iBottom );

}

PBRIG_BITMAP brig_OpenImage( PBRIG_CHAR lpName, bool bString, int iType )
{
   PBRIG_BITMAP hBmp;
   int iFileSize;
   FILE *fp;
   LPPICTURE pPic;
   IStream *pStream;
   HGLOBAL hG;

   if( bString )
   {
      iFileSize = strlen( lpName );
      hG = GlobalAlloc( GPTR, iFileSize );
      if( !hG )
         return NULL;
      memcpy( ( void * ) hG, ( void * ) lpName, iFileSize );
   }
   else
   {
      fp = fopen( lpName, "rb" );
      if( !fp )
         return NULL;

      fseek( fp, 0, SEEK_END );
      iFileSize = ftell( fp );
      hG = GlobalAlloc( GPTR, iFileSize );
      if( !hG )
      {
         fclose( fp );
         return NULL;
      }
      fseek( fp, 0, SEEK_SET );
      fread( ( void * ) hG, 1, iFileSize, fp );
      fclose( fp );
   }

   CreateStreamOnHGlobal( hG, 0, &pStream );

   if( !pStream )
   {
      GlobalFree( hG );
      return NULL;
   }

#if defined(__cplusplus)
   OleLoadPicture( pStream, 0, 0, IID_IPicture, ( void ** ) &pPic );
   pStream->Release(  );
#else
   OleLoadPicture( pStream, 0, 0, &IID_IPicture, ( void ** ) ( void * ) &pPic );
   pStream->lpVtbl->Release( pStream );
#endif

   GlobalFree( hG );

   if( !pPic )
      return NULL;

   if( iType == IMAGE_BITMAP )
   {
      HBITMAP hBitmap = 0;
#if defined(__cplusplus)
      pPic->get_Handle( ( OLE_HANDLE * ) & hBitmap );
#else
      pPic->lpVtbl->get_Handle( pPic, ( OLE_HANDLE * ) ( void * ) &hBitmap );
#endif

      hBmp = (HBITMAP) CopyImage( hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG );
   }
   else if( iType == IMAGE_ICON )
   {
      HICON hIcon = 0;
#if defined(__cplusplus)
      pPic->get_Handle( ( OLE_HANDLE * ) & hIcon );
#else
      pPic->lpVtbl->get_Handle( pPic, ( OLE_HANDLE * ) ( void * ) &hIcon );
#endif

      hBmp =  (HBITMAP) CopyImage( hIcon, IMAGE_ICON, 0, 0, 0 );
   }
   else
   {
      HCURSOR hCur = 0;
#if defined(__cplusplus)
      pPic->get_Handle( ( OLE_HANDLE * ) & hCur );
#else
      pPic->lpVtbl->get_Handle( pPic, ( OLE_HANDLE * ) ( void * ) &hCur );
#endif

      hBmp = (HBITMAP) CopyImage( hCur, IMAGE_CURSOR, 0, 0, 0 );
   }

#if defined(__cplusplus)
   pPic->Release(  );
#else
   pPic->lpVtbl->Release( pPic );
#endif
   return hBmp;
}

void brig_DrawBitmap( PBRIG_DC hDC, PBRIG_BITMAP hBitmap, int iLeft, int iTop, int iWidth, int iHeight )
{
   HDC hDCmem = CreateCompatibleDC( hDC );
   DWORD dwraster = SRCCOPY;
   BITMAP bitmap;

   SelectObject( hDCmem, hBitmap );
   GetObject( hBitmap, sizeof( BITMAP ), ( LPVOID ) & bitmap );
   if( iWidth && ( iWidth != bitmap.bmWidth || iHeight != bitmap.bmHeight ) )
   {
      SetStretchBltMode( hDC, COLORONCOLOR );
      StretchBlt( hDC, iLeft, iTop, iWidth, iHeight,
            hDCmem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, dwraster );
   }
   else
      BitBlt( hDC, iLeft, iTop, bitmap.bmWidth, bitmap.bmHeight, hDCmem, 0, 0, dwraster );

   DeleteDC( hDCmem );
}

void brig_RedrawWindow( brig_Widget *pWidget )
{
   RedrawWindow( pWidget->Handle(), NULL, NULL, RDW_ERASE | RDW_INVALIDATE );
}

void brig_DeleteObject( PBRIG_BRUSH pBrush )
{
   DeleteObject( ( HGDIOBJ ) pBrush );
}

void brig_DeleteObject( PBRIG_PEN pPen )
{
   DeleteObject( ( HGDIOBJ ) pPen );
}


void brig_DeleteObject( PBRIG_FONT pFont )
{
   DeleteObject( ( HGDIOBJ ) pFont );
}

PBRIG_BRUSH brig_SelectObject( PBRIG_DC hDC, PBRIG_BRUSH pBrush )
{
   return (PBRIG_BRUSH) SelectObject( hDC, ( HGDIOBJ ) pBrush );
}

PBRIG_PEN brig_SelectObject( PBRIG_DC hDC, PBRIG_PEN pPen )
{
   return (PBRIG_PEN) SelectObject( hDC, ( HGDIOBJ ) pPen );
}

PBRIG_FONT brig_SelectObject( PBRIG_DC hDC, PBRIG_FONT pFont )
{
   return (PBRIG_FONT) SelectObject( hDC, ( HGDIOBJ ) pFont );
}

PBRIG_BRUSH brig_CreateBrush( long int lColor )
{
      
   return CreateSolidBrush( ( COLORREF ) lColor );

}

bool brig_CompareBrush( PBRIG_BRUSH pBrush, long int lColor )
{
   LOGBRUSH lb = { 0 };

   GetObject( pBrush, sizeof( lb ), &lb );

   return ( lColor == (long int)(lb.lbColor) );

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

PBRIG_FONT brig_CreateFont( PBRIG_CHAR fontName, int fnHeight, int fnWeight,
               DWORD fdwCharSet, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut  )
{
   PBRIG_FONT hFont;
   PBRIG_WCHAR wcName = brig_str2WC( fontName );

   hFont = CreateFont( fnHeight,// logical height of font
         0,                     // logical average character width
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

void brig_SetFont( brig_Widget *pWidget, PBRIG_FONT pFont )
{
   SendMessage( pWidget->Handle(), WM_SETFONT, ( WPARAM ) pFont, 0L );

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

int brig_GetCharHeight( PBRIG_DC hDC )
{
   TEXTMETRIC tm;

   GetTextMetrics( hDC, &tm );

   return tm.tmHeight;

}

int brig_GetTextWidth( PBRIG_DC hDC, PBRIG_CHAR lpText )
{
   PBRIG_WCHAR wcText = brig_str2WC( lpText );
   SIZE sz;

#if defined(UNICODE)
   GetTextExtentPoint32( hDC, wcText, wcslen(wcText), &sz );
#else
   GetTextExtentPoint32( hDC, wcText, strlen(wcText), &sz );
#endif
   brig_free( wcText );

   return sz.cx;

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

#define GRADIENT_MAX_COLORS 16

#ifndef GRADIENT_FILL_RECT_H

#define GRADIENT_FILL_RECT_H 0
#define GRADIENT_FILL_RECT_V 1

#if !defined(__WATCOMC__) && !defined(__MINGW32__)
typedef struct _GRADIENT_RECT
{
   ULONG UpperLeft;
   ULONG LowerRight;
} GRADIENT_RECT;
#endif

#if defined(__DMC__)
typedef struct _TRIVERTEX
{
   LONG x;
   LONG y;
   USHORT Red;
   USHORT Green;
   USHORT Blue;
   USHORT Alpha;
} TRIVERTEX, *PTRIVERTEX;
#endif

#endif
#ifndef M_PI
#define M_PI             3.14159265358979323846
#endif
#ifndef M_TWOPI
#define M_TWOPI         (M_PI * 2.0)
#endif
#ifndef M_PI_2
#define M_PI_2           1.57079632679489661923
#endif
#ifndef M_PI_4
#define M_PI_4           0.78539816339744830962
#endif

typedef int ( _stdcall * GRADIENTFILL ) ( HDC, PTRIVERTEX, int, PVOID, int, int );

static GRADIENTFILL FuncGradientFill = NULL;

#define SECTORS_NUM 100

/*
 * brig_DrawGradient( hDC, x1, y1, x2, y2, int type, array colors, array stops, array radiuses )
 * This function draws rectangle with rounded corners and fill it with gradient pattern.
 * hDC - handle of device context;
 * x1 and y1 - coordinates of upper left corner;
 * x2 and y2 - coordinates of bottom right corner;
 * type - the type of gradient filling:
 *    1 - vertical and down;
 *    2 - vertical and up;
 *    3 - horizontal and to the right;
 *    4 - horizontal and to the left;
 *    5 - diagonal right-up;
 *    6 - diagonal left-down;
 *    7 - diagonal right-down;
 *    8 - diagonal left-up;
 *    9 - radial gradient;
 * colors - our colors (maximum - 16 colors), a color can be represented as 0xBBGGRR;
 * stops - fractions on interval [0;1] that correspond to the colors,
 * a stop determines the position where the corresponding color reaches its maximum;
 * radiuses - for our rounded corners:
 *    first  - for upper left;
 *    second - for upper right;
 *    third  - for bottom right;
 *    fourth - for bottom left;
 */
void brig_DrawGradient( PBRIG_DC hDC, int x1, int y1, int x2, int y2, int type,
      vector<long> *pArrColor, vector<double> *pArrStop, vector<int> *pArrRadius )
{
   long int color;
   int red[GRADIENT_MAX_COLORS], green[GRADIENT_MAX_COLORS], blue[GRADIENT_MAX_COLORS], index;
   int cur_red, cur_green, cur_blue, section_len;
   double red_step, green_step, blue_step;
   double stop;
   int stop_x[GRADIENT_MAX_COLORS], stop_y[GRADIENT_MAX_COLORS], coord_stop;
   int isH = 0, isV = 0, isD = 0, is_5_6 = 0, isR = 0;
   int x_center = 0, y_center = 0, gr_radius = 0;
   int radius[4];
   double angle, angle_step, coord_x, coord_y, min_delta, delta;
   int user_colors_num, colors_num, user_stops_num, user_radiuses_num, i, j, k;
   HDC hDC_mem = NULL;
   HBITMAP bmp = NULL;
   HPEN hPen;
   HBRUSH hBrush;
   TRIVERTEX vertex[(GRADIENT_MAX_COLORS-1)*2];
   GRADIENT_RECT gRect[GRADIENT_MAX_COLORS-1];
   int fill_type;
   POINT polygon[(SECTORS_NUM+1)*4], coords[SECTORS_NUM+1], candidates[4], center[4], edge[4];
   int polygon_len = 0, nearest_coord = 0, cycle_start, cycle_stop, cycle_step;
   int convert[4][2] = { {-1,1}, {1,1}, {1,-1}, {-1,-1} };
   long x, y;
   
   if ( (pArrColor == NULL) || ( user_colors_num = (int) pArrColor->size() ) == 0 )
      return;
   
   if ( user_colors_num >= 2 )
   {
      colors_num = ( user_colors_num <= GRADIENT_MAX_COLORS ) ? user_colors_num : GRADIENT_MAX_COLORS;
      user_stops_num = ( pArrStop ) ? ( (int) pArrStop->size() ) : 0;

      type = ( type >= 1 && type <= 9 ) ? type : 1;
      if ( type == 1 || type == 2 ) isV = 1;
      if ( type == 3 || type == 4 ) isH = 1;
      if ( type >= 5 && type <= 8 ) isD = 1;
      if ( type == 9 )
      {
         isR = 1;
         x_center = (x2 - x1) / 2 + x1;
         y_center = (y2 - y1) / 2 + y1;
         gr_radius = sqrt( pow(x2-x1,2) + pow(y2-y1,2) ) / 2;
      }

      // calculate stops and colors for our gradient
      for ( i = 0; i < colors_num; i++ )
      {
         stop = ( i < user_stops_num ) ? (*pArrStop)[i] : 1. / (colors_num-1) * i;
         if ( isV )
         {
            coord_stop = floor( stop * (y2-y1+1) + 0.5 );
            if ( type == 1 )
               stop_y[i] = y1 + coord_stop;
            else
               stop_y[colors_num-1-i] = y2 + 1 - coord_stop;
         }
         if ( isH )
         {
            coord_stop = floor( stop * (x2-x1+1) + 0.5 );
            if ( type == 3 )
               stop_x[i] = x1 + coord_stop;
            else
               stop_x[colors_num-1-i] = x2 + 1 - coord_stop;
         }
         if ( isD )
         {
            coord_stop = floor( stop * 2*(x2-x1+1) + 0.5 );
            if ( type == 5 || type == 7 )
               stop_x[i] = 2*x1-x2-1 + coord_stop;
            else
               stop_x[colors_num-1-i] = x2 + 1 - coord_stop;
         }
         if ( isR )
            stop_x[i] = floor( stop * gr_radius + 0.5 );

         color = (*pArrColor)[i];
         index = ( type == 2 || type == 4 || type == 6 || type == 8 ) ? colors_num-1-i : i;
         red[ index ]   = color % 256;
         green[ index ] = color / 256 % 256;
         blue[ index ]  = color / 256 / 256 % 256;
      }

      // Initially we draw gradient pattern into memory device -
      // create the memory device context that compatable to our main device.
      hDC_mem = CreateCompatibleDC( hDC );

      if ( type >= 1 && type <= 4 ) // horizontal and vertical gradients
      {
     
         // We create a bitmap that compatable to our main device
         // and attach it to the memory device context.
         bmp = ( HBITMAP ) CreateCompatibleBitmap( hDC, x2+1, y2+1 );
         SelectObject( hDC_mem, bmp );
        
         // 1. Array of TRIVERTEX structures that describe
         // positional and color values for each vertex
         // (for a rectangle two vertices need to be defined: upper-left and lower-right).
         // 2. Array of GRADIENT_RECT structures that
         // reference the TRIVERTEX vertices.
         for ( i = 1; i < colors_num; i++ )
         {
            vertex[(i-1)*2].x     = ( isH ) ? stop_x[i-1] : x1;
            vertex[(i-1)*2].y     = ( isV ) ? stop_y[i-1] : y1;
            vertex[(i-1)*2].Red   = (COLOR16) (red[i-1] * 256);
            vertex[(i-1)*2].Green = (COLOR16) (green[i-1] * 256);
            vertex[(i-1)*2].Blue  = (COLOR16) (blue[i-1] * 256);
            vertex[(i-1)*2].Alpha = 0x0000;
         
            vertex[(i-1)*2+1].x     = ( isH ) ? stop_x[i] : x2 + 1;
            vertex[(i-1)*2+1].y     = ( isV ) ? stop_y[i] : y2 + 1;
            vertex[(i-1)*2+1].Red   = (COLOR16) (red[i] * 256);
            vertex[(i-1)*2+1].Green = (COLOR16) (green[i] * 256);
            vertex[(i-1)*2+1].Blue  = (COLOR16) (blue[i] * 256);
            vertex[(i-1)*2+1].Alpha = 0x0000;
            
            gRect[i-1].UpperLeft  = (i-1)*2;
            gRect[i-1].LowerRight = (i-1)*2+1;
         }
               
         if( FuncGradientFill == NULL )
            FuncGradientFill = ( GRADIENTFILL )
               GetProcAddress( LoadLibrary( TEXT( "MSIMG32.DLL" ) ),
                  "GradientFill" );

         fill_type = ( isV ) ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H;

         // drawing gradient on the bitmap in the memory device context
         FuncGradientFill( hDC_mem, vertex, (colors_num-1)*2, gRect, (colors_num-1), fill_type );

         // shifts of edges
         if( ( isV && stop_y[0] > y1 ) || ( isH && stop_x[0] > x1 ) )
         {
            hPen = CreatePen( PS_SOLID, 1, RGB(red[0], green[0], blue[0]) );
            SelectObject( hDC_mem, hPen );
            hBrush = CreateSolidBrush( RGB(red[0], green[0], blue[0]) );
            SelectObject( hDC_mem, hBrush );
            if ( isV )
               Rectangle( hDC_mem, x1, y1, x2 + 1, stop_y[0] );
            else
               Rectangle( hDC_mem, x1, y1, stop_x[0], y2 + 1 );

            DeleteObject( hPen );
            DeleteObject( hBrush );
         }
         if ( ( isV && stop_y[colors_num-1] < y2 + 1 ) || ( isH && stop_x[colors_num-1] < x2 + 1 ) )
         {
            hPen = CreatePen( PS_SOLID, 1, RGB(red[colors_num-1], green[colors_num-1], blue[colors_num-1]) );
            SelectObject( hDC_mem, hPen );
            hBrush = CreateSolidBrush( RGB(red[colors_num-1], green[colors_num-1], blue[colors_num-1]) );
            SelectObject( hDC_mem, hBrush );
            if ( isV )
               Rectangle( hDC_mem, x1, stop_y[colors_num-1], x2 + 1, y2 + 1 );
            else
               Rectangle( hDC_mem, stop_x[colors_num-1], y1, x2 + 1, y2 + 1 );

            DeleteObject( hPen );
            DeleteObject( hBrush );
         }
         
      } // end horizontal and vertical gradients
      else if ( type >= 5 && type <= 8 ) // diagonal gradients
      {
         // We create a bitmap that compatable to our main device
         // and attach it to the memory device context.
         bmp = ( HBITMAP ) CreateCompatibleBitmap( hDC, 2*x2-x1+2, y2+1 );
         SelectObject( hDC_mem, bmp );

         if ( type == 5 || type == 6 ) is_5_6 = 1;

         for ( i = 1; i < colors_num; i++ )
         {
            section_len = stop_x[i] - stop_x[i-1];
            red_step = (double)( red[i] - red[i-1] ) / section_len;
            green_step = (double)( green[i] - green[i-1] ) / section_len;
            blue_step = (double)( blue[i] - blue[i-1] ) / section_len;
            for ( j = stop_x[i-1], k = 0; j <= stop_x[i]; j++, k++ )
            {
               cur_red = floor( red[i-1] + k * red_step + 0.5 );
               cur_green = floor( green[i-1] + k * green_step + 0.5 );
               cur_blue = floor( blue[i-1] + k * blue_step + 0.5 );
               hPen = CreatePen( PS_SOLID, 1, RGB( cur_red, cur_green, cur_blue ) );
               SelectObject( hDC_mem, hPen );

               MoveToEx( hDC_mem, j, (is_5_6)?y1:y2, NULL );
               LineTo( hDC_mem, j + x2-x1+1, (is_5_6)?y2:y1 );
               // LineTo doesn't draw the last pixel
               SetPixel( hDC_mem, j + x2-x1+1, (is_5_6)?y2:y1, RGB( cur_red, cur_green, cur_blue ) );

               DeleteObject( hPen );
            }
         }
         
         // shifts of edges
         if ( stop_x[0] > 2*x1-x2-1 ) // on the left
         {
            hPen = CreatePen( PS_SOLID, 1, RGB(red[0], green[0], blue[0]) );
            SelectObject( hDC_mem, hPen );
            hBrush = CreateSolidBrush( RGB(red[0], green[0], blue[0]) );
            SelectObject( hDC_mem, hBrush );

            edge[0].x = x1;
            edge[0].y = ( is_5_6 ) ? y2 : y1;
            edge[1].x = stop_x[0] + x2 - x1;
            edge[1].y = ( is_5_6 ) ? y2 : y1;
            edge[2].x = stop_x[0] - 1;
            edge[2].y = ( is_5_6 ) ? y1 : y2;
            edge[3].x = 2*x1 - x2 - 1;
            edge[3].y = ( is_5_6 ) ? y1 : y2;
            
            Polygon( hDC_mem, edge, 4 );
            
            DeleteObject( hPen );
            DeleteObject( hBrush );
         }
         if ( stop_x[colors_num-1] < x2 ) // on the right
         {
            hPen = CreatePen( PS_SOLID, 1, RGB(red[colors_num-1], green[colors_num-1], blue[colors_num-1]) );
            SelectObject( hDC_mem, hPen );
            hBrush = CreateSolidBrush( RGB(red[colors_num-1], green[colors_num-1], blue[colors_num-1]) );
            SelectObject( hDC_mem, hBrush );

            edge[0].x = x2;
            edge[0].y = ( is_5_6 ) ? y1 : y2;
            edge[1].x = stop_x[colors_num-1] + 1;
            edge[1].y = ( is_5_6 ) ? y1 : y2;
            edge[2].x = stop_x[colors_num-1] + x2 - x1 + 2;
            edge[2].y = ( is_5_6 ) ? y2 : y1;
            edge[3].x = 2*x2 - x1 + 1;
            edge[3].y = ( is_5_6 ) ? y2 : y1;
            
            Polygon( hDC_mem, edge, 4 );

            DeleteObject( hPen );
            DeleteObject( hBrush );
         }
         
      } // end diagonal gradients
      else if ( type == 9 ) // radial gradients
      {
         // We create a bitmap that compatable to our main device
         // and attach it to the memory device context.
         bmp = ( HBITMAP ) CreateCompatibleBitmap( hDC, x2+1, y2+1 );
         SelectObject( hDC_mem, bmp );
      
         // shifts of edge
         if ( stop_x[colors_num-1] < gr_radius )
         {
            hPen = CreatePen( PS_SOLID, 1, RGB(red[colors_num-1], green[colors_num-1], blue[colors_num-1]) );
            SelectObject( hDC_mem, hPen );
            hBrush = CreateSolidBrush( RGB(red[colors_num-1], green[colors_num-1], blue[colors_num-1]) );
            SelectObject( hDC_mem, hBrush );
            
            Rectangle( hDC_mem, x1, y1, x2+1, y2+1);

            DeleteObject( hPen );
            DeleteObject( hBrush );
         }

         for ( i = colors_num-1; i > 0; i-- )
         {
            section_len = stop_x[i] - stop_x[i-1];
            red_step = (double)( red[i-1] - red[i] ) / section_len;
            green_step = (double)( green[i-1] - green[i] ) / section_len;
            blue_step = (double)( blue[i-1] - blue[i] ) / section_len;
            for ( j = stop_x[i], k = 0; j >= stop_x[i-1]; j--, k++ )
            {
               cur_red = floor( red[i] + k * red_step + 0.5 );
               cur_green = floor( green[i] + k * green_step + 0.5 );
               cur_blue = floor( blue[i] + k * blue_step + 0.5 );
               hPen = CreatePen( PS_SOLID, 1, RGB( cur_red, cur_green, cur_blue ) );
               SelectObject( hDC_mem, hPen );
               hBrush = CreateSolidBrush( RGB( cur_red, cur_green, cur_blue ) );
               SelectObject( hDC_mem, hBrush );

               Ellipse( hDC_mem, x_center - j, y_center - j, x_center + j+1, y_center + j+1 );
       
               DeleteObject( hPen );
               DeleteObject( hBrush );
            }
         }
      
      } // end radial gradients

   } // user passes two colors or more

   // We draw polygon that looks like rectangle with rounded corners.
   // WinAPI allows to fill this figure with brush.
   user_radiuses_num = ( pArrRadius ) ? (int)pArrRadius->size() : 0;
   for ( i = 0; i < 4; i++ )
   {
      radius[i] = ( i < user_radiuses_num ) ? (*pArrRadius)[i] : 0;
      radius[i] = ( radius[i] >= 0 ) ? radius[i] : 0;
   }
   
   center[0].x = x1 + radius[0];
   center[0].y = y1 + radius[0];
   center[1].x = x2 - radius[1];
   center[1].y = y1 + radius[1];
   center[2].x = x2 - radius[2];
   center[2].y = y2 - radius[2];
   center[3].x = x1 + radius[3];
   center[3].y = y2 - radius[3];

   for ( i = 0; i < 4; i++ )
   {
      if ( radius[i] == 0 )
      {
         // This is not rounded corner.
         polygon[ polygon_len ].x = center[i].x;
         polygon[ polygon_len ].y = center[i].y;
         polygon_len++;
      }
      else
      {
         if ( i == 0 || radius[i] != radius[i-1] )
         {
            // The radius is greater than zero, so we draw a quarter circle.
            // The drawing uses the principle of Bresenham's circle algorithm
            // for finding in the group of pixels the nearest pixel to a circle.
            // At first we calculate the coordinates of the pixels 
            // in the quadrant from -Pi/2 to 0. This is a handy quadrant - 
            // when the angle increases, the values on bouth X-axis and Y-axis
            // are monotonically increase.
            // Then, the coordinates are converted for the corresponding quarter
            // and for the corresponding circle center.
            coords[0].x = 0;
            coords[0].y = -radius[i];
            coords[ SECTORS_NUM ].x = radius[i];
            coords[ SECTORS_NUM ].y = 0;

            angle = -M_PI_2;
            angle_step = M_PI_2 / SECTORS_NUM;
            for( j = 1; j < SECTORS_NUM; j++ )
            {
               angle += angle_step;
               coord_x = cos( angle ) * radius[i];
               coord_y = sin( angle ) * radius[i];

               candidates[0].x = floor( coord_x );
               candidates[0].y = floor( coord_y );
               candidates[1].x = ceil( coord_x );
               candidates[1].y = floor( coord_y );
               candidates[2].x = floor( coord_x );
               candidates[2].y = ceil( coord_y );
               candidates[3].x = ceil( coord_x );
               candidates[3].y = ceil( coord_y );
               min_delta = 1000000;
               for( k = 0; k < 4; k++ )
               {
                  delta = abs( pow( candidates[k].x, 2 ) + pow( candidates[k].y, 2 ) -
                     pow( radius[i], 2 ) );
                  if ( delta < min_delta )
                  {
                     nearest_coord = k;
                     min_delta = delta;
                  }
               }
               
               coords[j].x = candidates[ nearest_coord ].x;
               coords[j].y = candidates[ nearest_coord ].y;
            }    
         }
         
         cycle_start = ( i%2 == 0 ) ? SECTORS_NUM : 0;
         cycle_stop = ( i%2 == 0 ) ? -1 : SECTORS_NUM + 1;
         cycle_step = ( i%2 == 0 ) ? -1 : 1;
         for( j = cycle_start; j != cycle_stop; j += cycle_step )
         {
            x = convert[ i ][ 0 ] * coords[ j ].x + center[ i ].x;
            y = convert[ i ][ 1 ] * coords[ j ].y + center[ i ].y;
            if ( polygon_len == 0 || x != polygon[ polygon_len-1 ].x || y != polygon[ polygon_len-1 ].y )
            {
               polygon[ polygon_len ].x = x;
               polygon[ polygon_len ].y = y;
               polygon_len++;
            }
         }
      }
   }

   // We draw polygon and fill it with brush
   if( user_colors_num >= 2 )
   {
      hPen = CreatePen( PS_NULL, 1, RGB( 0, 0, 0 ) );
      hBrush = CreatePatternBrush( bmp );
   }
   else
   {
      color = (*pArrColor)[0];
      hPen = CreatePen( PS_SOLID, 1, color );
      hBrush = CreateSolidBrush( color );
   }
   
   SelectObject( hDC, hPen );
   SelectObject( hDC, hBrush );
   Polygon( hDC, polygon, polygon_len );
   
   if( user_colors_num >= 2 )
   {
      // In WinAPI rightmost column and bottommost row of pixels are ignored while filling polygon
      // with "PS_NULL" border, so we draw additional figures to complete our glory.
      Rectangle( hDC, x2, y1+radius[1], x2+2, y2-radius[2]+2 );
      Rectangle( hDC, x1+radius[3], y2, x2-radius[2]+2, y2+2 );

      if( bmp ) 
         DeleteObject( bmp );
      if( hDC_mem )
         DeleteDC( hDC_mem );
   }

   DeleteObject( hPen );
   DeleteObject( hBrush );

}
