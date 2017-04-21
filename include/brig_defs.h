/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#ifndef BRIG_DEFS_H_
#define BRIG_DEFS_H_

#if defined( UNICODE )
   #define _UNICODE
#endif

#include <vector>
#include <string>
using std::vector;

#if defined( WINNT ) || defined( _Windows ) || defined( __NT__ ) || defined( _WIN32 ) || defined( _WINDOWS_ ) || defined( __WINDOWS_386__ ) || defined( __WIN32__ )
   #define BRIG_OS_WIN
#endif

#if defined( BRIG_OS_WIN )
   #include <windows.h>
   #include <commctrl.h>

   #if defined( UNICODE )
      #define BRIG_WCHAR    wchar_t
   #else
      #define BRIG_WCHAR    char
   #endif
   #define BRIG_HANDLE   HWND
   #define BRIG_HMENU    HMENU
   #define BRIG_CHAR     char
   #define PBRIG_BRUSH   HBRUSH
   #define PBRIG_DC      HDC
   #define PBRIG_PEN     HPEN
   #define PBRIG_FONT    HFONT
   #define PBRIG_ICON    HICON

   typedef struct BRIG_PPS_STRU
   {
     PBRIG_DC hDC;
     PAINTSTRUCT *pps;
   } BRIG_PPS, * PBRIG_PPS;

#else
   #include <gtk/gtk.h>

   #define BRIG_STRING  std::wstring
   #define BRIG_HANDLE   GtkWidget*
   #define BRIG_HMENU    GtkWidget*
   #define BRIG_WCHAR    gchar
   #define BRIG_CHAR     gchar

   #define BRIG_OBJECT_PEN    1
   #define BRIG_OBJECT_BRUSH  2
   #define BRIG_OBJECT_FONT   3
   #define BRIG_OBJECT_PIXBUF 4

   typedef struct _RECT {
     long    left;
     long     top;
     long   right;
     long  bottom;
   } RECT, *PRECT;

   typedef struct BRIG_DC_STRU
   {
     GtkWidget * widget;
     GdkWindow * window;
     cairo_surface_t *surface;
     cairo_t *cr;
     PangoFontDescription * hFont;
     PangoLayout * layout;
     long fcolor, bcolor;
   } BRIG_DC, * PBRIG_DC;

   typedef struct BRIG_PPS_STRU
   {
     PBRIG_DC hDC;
   } BRIG_PPS, * PBRIG_PPS;

   typedef struct BRIG_BRUSH_STRU
   {
      short int type;
      long int color;
   } BRIG_BRUSH, *PBRIG_BRUSH;

   typedef struct BRIG_PEN_STRU
   {
      short int type;
      gdouble width;
      GdkLineStyle style;
      long int  color;
   } BRIG_PEN, *PBRIG_PEN;

   typedef struct BRIG_FONT_STRU
   {
      short int type;
      PangoFontDescription * hFont;
      PangoAttrList * attrs;
   } BRIG_FONT, *PBRIG_FONT;

   typedef struct HWGUI_ICON_STRU
   {
      short int type;
      long int  trcolor;
      GdkPixbuf * handle;
   } BRIG_ICON, *PBRIG_ICON;

#endif

#define PBRIG_WCHAR   BRIG_WCHAR*
#define PBRIG_CHAR    BRIG_CHAR*

#define SYMBOL_UNUSED( symbol )  ( void ) symbol

#define COLOR_GR_LIGHT   0xeeeeee
#define COLOR_WHITE      0xffffff

#endif  // BRIG_DEFS_H_
