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

#define BRIG_VER_MAJOR    0
#define BRIG_VER_MINOR    9
#define BRIG_VER_BUILD    1

#define BRIG_VERSION_MAJOR    1
#define BRIG_VERSION_MINOR    2
#define BRIG_VERSION_BUILD    3

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
   #define PBRIG_BITMAP  HBITMAP
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

   typedef struct BRIG_PIXBUF_STRU
   {
      short int type;
      long int  trcolor;
      GdkPixbuf * handle;
   } BRIG_BITMAP, * PBRIG_BITMAP;

   typedef struct BRIG_ICON_STRU
   {
      short int type;
      long int  trcolor;
      GdkPixbuf * handle;
   } BRIG_ICON, *PBRIG_ICON;

   #define HACCEL        void*
   #define DWORD         int
   #define WPARAM        long
   #define LPARAM        long
   #define UINT          unsigned int

   #define WM_DESTROY           2
   #define WM_MOVE              3
   #define WM_SIZE              5
   #define WM_PAINT            15
   #define WM_ERASEBKGND       20
   #define WM_KEYDOWN         256
   #define WM_KEYUP           257
   #define WM_CHAR            258
   #define WM_COMMAND         273
   #define WM_SYSCOMMAND      274
   #define WM_HSCROLL         276
   #define WM_VSCROLL         277
   #define WM_LBUTTONDOWN     513
   #define WM_LBUTTONUP       514
   #define WM_LBUTTONDBLCLK   515
   #define WM_RBUTTONDOWN     516
   #define WM_MOUSEWHEEL   0x020A

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

   #define  VK_RIGHT   GDK_Right
   #define  VK_LEFT    GDK_Left
   #define  VK_HOME    GDK_Home
   #define  VK_END     GDK_End
   #define  VK_DOWN    GDK_Down
   #define  VK_UP      GDK_Up
   #define  VK_NEXT    GDK_Page_Down
   #define  VK_PRIOR   GDK_Page_Up
   #define  VK_INSERT  GDK_Insert
   #define  VK_RETURN  GDK_Return
   #define  VK_TAB     GDK_Tab
   #define  VK_ESCAPE  GDK_Escape
   #define  VK_BACK    GDK_BackSpace
   #define  VK_DELETE  GDK_Delete
   #define  VK_F1      GDK_F1
   #define  VK_F2      GDK_F2
   #define  VK_F3      GDK_F3
   #define  VK_F4      GDK_F4
   #define  VK_F5      GDK_F5
   #define  VK_F6      GDK_F6
   #define  VK_F7      GDK_F7
   #define  VK_F8      GDK_F8
   #define  VK_F9      GDK_F9
   #define  VK_F10     GDK_F10
   #define  VK_F11     GDK_F11
   #define  VK_F12     GDK_F12

   #define  VK_SHIFT   GDK_Shift_L
   #define  VK_CONTROL GDK_Control_L
   #define  VK_MENU    GDK_Menu
   #define  VK_HELP    GDK_Help
   #define  VK_PAUSE   GDK_Pause
   #define  VK_CAPITAL GDK_Caps_Lock

   #define VK_SCROLL   GDK_Scroll_Lock
   #define VK_SELECT   GDK_Select
   #define VK_PRINT    GDK_Print
   #define VK_EXECUTE  GDK_Execute

   #define VK_NUMLOCK  GDK_Num_Lock
   #define VK_NUMPAD0  GDK_KP_0
   #define VK_NUMPAD1  GDK_KP_1
   #define VK_NUMPAD2  GDK_KP_2
   #define VK_NUMPAD3  GDK_KP_3
   #define VK_NUMPAD4  GDK_KP_4
   #define VK_NUMPAD5  GDK_KP_5
   #define VK_NUMPAD6  GDK_KP_6
   #define VK_NUMPAD7  GDK_KP_7
   #define VK_NUMPAD8  GDK_KP_8
   #define VK_NUMPAD9  GDK_KP_9
   #define VK_MULTIPLY  GDK_KP_Multiply
   #define VK_ADD       GDK_KP_Add
   #define VK_SEPARATOR GDK_KP_Separator
   #define VK_SUBTRACT  GDK_KP_Subtract
   #define VK_DECIMAL   GDK_KP_Decimal
   #define VK_DIVIDE    GDK_KP_Divide

   #define MK_MBUTTON          16
#endif

#define PBRIG_WCHAR   BRIG_WCHAR*
#define PBRIG_CHAR    BRIG_CHAR*

#define SYMBOL_UNUSED( symbol )  ( void ) symbol

#define COLOR_GR_LIGHT   0xeeeeee
#define COLOR_WHITE      0xffffff

#define TYPE_WINDOW        1
#define TYPE_DIALOG        2
#define TYPE_LABEL        11
#define TYPE_EDIT         12
#define TYPE_BUTTON       13
#define TYPE_CHECK        14
#define TYPE_RADIO        15
#define TYPE_GROUP        16
#define TYPE_COMBO        17
#define TYPE_PANEL        51
#define TYPE_QBTN         52
#define TYPE_TABLE        53

#endif  // BRIG_DEFS_H_
