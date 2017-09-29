/*
 * Brig - a ligthweight C++ GUI framework
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#ifndef BRIG_APP_H_
#define BRIG_APP_H_

#define BRIGAPP_FONT_LIMIT      20
#define BRIGAPP_PEN_LIMIT       50
#define BRIGAPP_BRUSH_LIMIT     50
#define BRIGAPP_STYLE_LIMIT     50

#define TIMER_FIRST_ID       33900

struct BRIGAPP_FONT
{
   PBRIG_FONT  pFont;
   PBRIG_CHAR  pName;
   bool  bNeedToFree;
   int       iHeight;
   int       iWeight;
   int      iCharSet;
   int       iItalic;
   int    iUnderline;
   int    iStrikeOut;
   int        iCount;
};

struct BRIGAPP_PEN
{
   PBRIG_PEN   pPen;
   int       iCount;
};

struct BRIGAPP_BRUSH
{
   PBRIG_BRUSH  pBrush;
   int          iCount;
};

struct BRIGAPP_STYLE
{
   brig_Style  *pStyle;
   int          iCount;
};

struct BRIGAPP_TIMER
{
   unsigned int   uiId;
   unsigned int   uiIdInit;          // for GTK version only
   unsigned int  uiVal;
   bool          bOnce;
   brig_fnc_menu pfAction;
};

class brig_Application
{
public:

   brig_Application();
   ~brig_Application();

   brig_MainWindow *pMainWindow;

   vector<BRIGAPP_FONT> avFonts;
   vector<BRIGAPP_PEN> avPens;
   vector<BRIGAPP_BRUSH> avBrushes;
   vector<BRIGAPP_STYLE> avStyles;
   vector<BRIGAPP_TIMER> avTimers;
};

extern PBRIG_FONT brigAddFont( PBRIG_CHAR fontName, int fnHeight, int fnWeight = 400,
               DWORD fdwCharSet = 0, DWORD fdwItalic = 0, DWORD fdwUnderline = 0, DWORD fdwStrikeOut = 0);
extern PBRIG_FONT brigAddFont( PBRIG_FONT pFontBase, bool bAsBase = 0 );
extern PBRIG_PEN brigAddPen( int iWidth = 1, long int lColor = 0, int iStyle = 0 );
extern void brigDelPen( PBRIG_PEN pPen );
extern PBRIG_BRUSH brigAddBrush( long int lColor );
extern void brigDelBrush( PBRIG_BRUSH pBrush );
extern brig_Style * brigAddStyle( int iColors, long * pColors, int iType = 1,
      long lBorderColor = -1, int iBorder = 1, int iCorners = 0, long * pCorners = NULL );
extern brig_Style * brigAddStyle( long lColor,
      long lBorderColor = -1, int iBorder = 1, int iCorners = 0, long * pCorners = NULL );
extern void brigDelStyle( brig_Style * pStyle );

extern PBRIG_FONT brig_ChooseFont( PBRIG_FONT hFontPrev, BRIGAPP_FONT *pbf = NULL );

extern int brigChoice( std::vector<char*> &pList, PBRIG_CHAR lpTitle, unsigned int iLeft,
      unsigned int iTop, unsigned int iWidth, unsigned int iHeight, PBRIG_FONT hFont = NULL );

extern unsigned int brigSetTimer( unsigned int uiValue, brig_fnc_menu pfAction, bool bOnce = false );
extern void brigRunTimerFunc( unsigned int uiId );
extern void brigKillTimer( unsigned int uiId );
extern unsigned int brig_SetTimer( unsigned int uiId, unsigned int uiValue );
extern void brig_KillTimer( unsigned int uiId );

#endif // BRIG_APP_H_
