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

struct BRIGAPP_FONT
{
   PBRIG_FONT  pFont;
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

class brig_Application
{
public:

   brig_Application();
   ~brig_Application();

   vector<BRIGAPP_FONT> avFonts;
   vector<BRIGAPP_PEN> avPens;
   vector<BRIGAPP_BRUSH> avBrushes;
   vector<BRIGAPP_STYLE> avStyles;
};

extern PBRIG_PEN brigAddPen( int iWidth = 1, long int lColor = 0, int iStyle  = 0 );
extern void brigDelPen( PBRIG_PEN pPen );
extern PBRIG_BRUSH brigAddBrush( long int lColor );
extern void brigDelBrush( PBRIG_BRUSH pBrush );
extern brig_Style * brigAddStyle( int iColors, long * pColors, int iType = 1,
      long lBorderColor = -1, int iBorder = 1, int iCorners = 0, long * pCorners = NULL );
extern void brigDelStyle( brig_Style * pStyle );

#endif // BRIG_APP_H_
