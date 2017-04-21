/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#ifndef BRIG_STYLE_H_
#define BRIG_STYLE_H_

class brig_Style
{
public:
   brig_Style( int iColors, long * pColors, int iType = 1,
      long lBorderColor = -1, int iBorder = 1, int iCorners = 0, long * pCorners = NULL );
   ~brig_Style();

   void Draw( PBRIG_DC hDC, int iLeft, int iTop, int iRight, int iBottom );


   int iOrient;
   PBRIG_PEN pPen;
   vector<long> avColor;
   vector<int> avCorners;

};

#endif  // BRIG_STYLE_H_
