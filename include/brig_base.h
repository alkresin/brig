/*
 * Brig - a ligthweight C++ GUI framework
 * Header file for brig_Widget, brig_Container, brig_MainWindow, brig_Dialog classes
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#ifndef BRIG_BASE_H_
#define BRIG_BASE_H_

#define  MENU_FIRST_ID   32000
#define  CONTEXTMENU_FIRST_ID   32900

class brig_Widget;
class brig_Container;

extern void brig_SetWindowText( brig_Widget *pWidget, PBRIG_CHAR lpTitle );
extern PBRIG_CHAR brig_GetWindowText( brig_Widget *pWidget );

typedef bool (*brig_fnc_paint)( brig_Widget *pWidget, PBRIG_DC hDC );
typedef bool (*brig_fnc_mess)( brig_Widget *pWidget, WPARAM wParam, LPARAM lParam );
typedef bool (*brig_fnc_mess_1)( brig_Widget *pWidget );
typedef void (*brig_fnc_menu)( void );

typedef struct BRIG_MENUITEM_STRU
{
 
   PBRIG_CHAR    szName;
   BRIG_HMENU  handle;
   int iId;
   brig_fnc_menu pfMenuItem;
   std::vector<struct BRIG_MENUITEM_STRU*> avItems;

} BRIG_MITEM, *PBRIG_MITEM;


class brig_Widget
{
public:

   brig_Widget();
   ~brig_Widget();

   void SetText( PBRIG_CHAR lpTitle );
   PBRIG_CHAR GetText();
   BRIG_HANDLE Handle( void ) const;

   void New( brig_Container *pParent, int x, int y, int width, int height );
   void SetFont( PBRIG_FONT hFontNew );
   long int SetTextColor( long int lColor = -1, bool bRepaint = 1 );
   long int SetBackColor( long int lColor = -1, bool bRepaint = 1 );
   void Move( int iLeft, int iTop, int iWidth = -1, int iHeight = -1 );
   void Enable( bool bEnable = 1 );
   void Show( bool bShow = 1 );
   virtual bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   unsigned int uiType;
   int iLeft, iTop, iWidth, iHeight;
   long int lTextColor, lBackColor;
   int iWidgId;
   brig_Widget *pParent;
   PBRIG_FONT  hFont;
   PBRIG_BRUSH hBrush;

   brig_fnc_mess pfOnSize;
   brig_fnc_mess_1 pfOnClose;

protected:
   BRIG_HANDLE handle;

};

class brig_Container : public brig_Widget
{
public:
   brig_Container();
   ~brig_Container();

   int NewId( void );
   int iIdCount;

   void AddWidget( brig_Widget *pWidget );
   void OnSize( WPARAM wParam, LPARAM lParam );

   PBRIG_MITEM pMenu;

   vector<brig_Widget*> avWidgets;
};

class brig_MainWindow : public brig_Container
{
public:
   brig_MainWindow();
   void New( int x, int y, int width, int height,
         PBRIG_CHAR lpTitle, long int lStyle = WS_OVERLAPPEDWINDOW,
         PBRIG_ICON hIcon = NULL, PBRIG_BRUSH hBrush = NULL );
   void Activate( void );
   void Close();
   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   PBRIG_CHAR szAppName;

};

class brig_Dialog : public brig_Container
{
public:
   brig_Dialog();
   void New( brig_Container * pParent, int x, int y, int width, int height,
         PBRIG_CHAR lpTitle, long int lStyle = WS_OVERLAPPEDWINDOW );
   void Activate( bool bMode = 1 );
   void Close();
   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   void *pResult;
   bool bModal;

};

extern void brigMenu( brig_Container *pWnd );
extern bool brigSubMenu( PBRIG_CHAR szCaption, int iId );
extern void brigMenuEnd( void );
#define brigMenuSeparator()      brigMenuItemAdd( NULL,0,NULL,0,0 )
extern void brigMenuItemAdd( PBRIG_CHAR szName, int iId, brig_fnc_menu pfMenuItem, int iFlag = 0, int iKey = 0 );
extern PBRIG_MITEM brig_MenuItemFind( PBRIG_MITEM pMenu, int iId );
extern BRIG_HMENU brig_AddMenuItem( BRIG_HMENU hMenu, PBRIG_CHAR szCaption, int iPos,
         brig_Container *pWnd, unsigned int uiId, bool bSubMenu );

extern void brig_SetWidgetData( brig_Widget *pWidget);

#endif // BRIG_BASE_H_