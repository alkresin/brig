/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Messagebox functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

static int s_msgbox( UINT uiType, PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   int iResult;
   PBRIG_WCHAR wcText = brig_str2WC( sText );
   PBRIG_WCHAR wcTitle = brig_str2WC( sTitle );

   iResult = MessageBox( GetActiveWindow(), wcText, wcTitle, uiType );

   brig_free( wcText );
   brig_free( wcTitle );

   return iResult;
}

void brig_MsgInfo( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   s_msgbox( MB_OK | MB_ICONINFORMATION, sText, sTitle );
}

void brig_MsgStop( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   s_msgbox( MB_OK | MB_ICONSTOP, sText, sTitle );
}

int brig_MsgOkCancel( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   return s_msgbox( MB_OKCANCEL | MB_ICONQUESTION, sText, sTitle );
}

int brig_MsgYesNo( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   return ( s_msgbox( MB_YESNO | MB_ICONQUESTION, sText, sTitle ) == IDYES );
}
