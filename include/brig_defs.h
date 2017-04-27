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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <map>
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
   #include "gdk/gdkkeysyms.h"

   #define BRIG_STRING  std::wstring
   #define BRIG_HANDLE   GtkWidget*
   #define BRIG_HMENU    GtkWidget*
   #define BRIG_WCHAR    gchar
   #define BRIG_CHAR     gchar

   #define HACCEL        void*
   #define DWORD         int
   #define WPARAM        long
   #define LPARAM        long
   #define UINT          unsigned int

   #define WM_DESTROY           2
   #define WM_MOVE              3
   #define WM_SIZE              5
   #define WM_PAINT            15
   #define WM_COMMAND         273
   #define WM_SYSCOMMAND      274
   #define WM_HSCROLL         276
   #define WM_VSCROLL         277
   #define WM_LBUTTONDOWN     513
   #define WM_LBUTTONUP       514

   #define BS_AUTOCHECKBOX      3
   #define BS_RADIOBUTTON       4
   #define BS_GROUPBOX          7
   #define BS_AUTORADIOBUTTON   9
   #define DT_CENTER            1
   #define DT_RIGHT             2
   #define DT_VCENTER           4
   #define DT_SINGLELINE       32
   #define SS_LEFT              0
   #define SS_CENTER            1
   #define SS_RIGHT             2
   #define SS_OWNERDRAW        13
   #define ES_MULTILINE         4
   #define ES_PASSWORD         32
   #define ES_READONLY       2048

   #define WS_TABSTOP       65536
   #define WS_GROUP        131072
   #define WS_VSCROLL     2097152
   #define WS_HSCROLL     1048576
   #define WS_BORDER      8388608
   #define WS_OVERLAPPEDWINDOW  0
   #define WS_EX_TRANSPARENT   32
   #define SC_CLOSE         61536

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
