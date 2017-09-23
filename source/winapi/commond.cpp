/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Common dialogs
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

long int brig_ChooseColor( long int lDefColor )
{
   CHOOSECOLOR cc;
   COLORREF rgb[16];
   DWORD dwStyle = 0;

   memset( ( void * ) &cc, 0, sizeof( CHOOSECOLOR ) );

   cc.lStructSize = sizeof( CHOOSECOLOR );
   cc.hwndOwner = GetActiveWindow();
   cc.lpCustColors = rgb;
   if( lDefColor >= 0 )
   {
      cc.rgbResult = ( COLORREF ) lDefColor;
      dwStyle |= CC_RGBINIT;
   }
   cc.Flags = dwStyle;

   if( ChooseColor( &cc ) )
      return ( ( LONG ) cc.rgbResult );
   else
      return -1;
}

PBRIG_FONT brig_ChooseFont( PBRIG_FONT hFontPrev, BRIGAPP_FONT *pbf )
{
   CHOOSEFONT cf;
   LOGFONT lf = { 0 };

   /* Initialize members of the CHOOSEFONT structure. */
   if( hFontPrev )
      GetObject( hFontPrev, sizeof( lf ), &lf );

   cf.lStructSize = sizeof( CHOOSEFONT );
   cf.hwndOwner = ( HWND ) NULL;
   cf.hDC = ( HDC ) NULL;
   cf.lpLogFont = &lf;
   cf.iPointSize = 0;
   cf.Flags = CF_SCREENFONTS | ( ( hFontPrev ) ? CF_INITTOLOGFONTSTRUCT : 0 );
   cf.rgbColors = RGB( 0, 0, 0 );
   cf.lCustData = 0L;
   cf.lpfnHook = ( LPCFHOOKPROC ) NULL;
   cf.lpTemplateName = NULL;

   cf.hInstance = ( HINSTANCE ) NULL;
   cf.lpszStyle = NULL;
   cf.nFontType = SCREEN_FONTTYPE;
   cf.nSizeMin = 0;
   cf.nSizeMax = 0;

   /* Display the CHOOSEFONT common-dialog box. */
   if( !ChooseFont( &cf ) )
      return NULL;

   if( pbf )
   {
      pbf->pName = brig_WC2str( lf.lfFaceName, -1 );
      pbf->iHeight  = lf.lfHeight;
      pbf->iWeight  = lf.lfWeight;
      pbf->iCharSet = lf.lfCharSet;
      pbf->iItalic  = lf.lfItalic;
      pbf->iUnderline = lf.lfUnderline;
      pbf->iStrikeOut = lf.lfStrikeOut;
   }
   /* Create a logical font based on the user's   */
   /* selection and return a handle identifying   */
   /* that font.                                  */
   return CreateFontIndirect( cf.lpLogFont );

}

PBRIG_CHAR brig_ChooseFile( PBRIG_CHAR sLabel, PBRIG_CHAR sMask )
{
   PBRIG_CHAR lpFilter;
   int nLen1, nLen2;

   nLen1 = strlen( sLabel );
   nLen2 = strlen( sMask );
   lpFilter =
         ( PBRIG_CHAR ) malloc( ( nLen1 + nLen2 + 4 ) * sizeof( BRIG_CHAR ) );
   memset( lpFilter, 0, ( nLen1 + nLen2 + 4 ) * sizeof( BRIG_CHAR ) );
   memcpy( lpFilter, sLabel, nLen1 * sizeof( BRIG_CHAR ) );
   memcpy( lpFilter + nLen1 + 1, sMask, nLen2 * sizeof( BRIG_CHAR ) );

   OPENFILENAME ofn;
   BRIG_WCHAR buffer[1024];
   PBRIG_WCHAR wcText = brig_str2WC( lpFilter, nLen1 + nLen2 + 2 );
   bool bOk;

   memset( ( void * ) &ofn, 0, sizeof( OPENFILENAME ) );
   ofn.lStructSize = sizeof( ofn );
   ofn.hwndOwner = GetActiveWindow(  );
   ofn.lpstrFilter = wcText;
   ofn.lpstrFile = buffer;
   buffer[0] = 0;
   ofn.nMaxFile = 1024;
   ofn.lpstrInitialDir = NULL;
   ofn.lpstrTitle = NULL;
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;

   bOk = GetOpenFileName( &ofn );

   brig_free( wcText );
   free( lpFilter );

   if( bOk )
   {
      int iLen;

   #if defined(UNICODE)
      iLen = wcslen( ofn.lpstrFile );
      return brig_WCTostr( CP_UTF8, ofn.lpstrFile, iLen );
   #else
      PBRIG_CHAR sRet;
      iLen = strlen( ofn.lpstrFile );
      sRet = ( PBRIG_CHAR ) malloc( iLen * sizeof( BRIG_CHAR ) + 1 );
      memcpy( sRet, ofn.lpstrFile, iLen * sizeof( BRIG_CHAR ) );
      sRet[iLen] = '\0';
      return sRet;
   #endif
   }
   else
      return NULL;

}


PBRIG_CHAR brig_SaveFile( PBRIG_CHAR sLabel, PBRIG_CHAR sMask, PBRIG_CHAR sInit )
{
   PBRIG_CHAR lpFilter;
   int nLen1, nLen2;

   SYMBOL_UNUSED( sInit );

   nLen1 = strlen( sLabel );
   nLen2 = strlen( sMask );
   lpFilter =
         ( PBRIG_CHAR ) malloc( ( nLen1 + nLen2 + 4 ) * sizeof( BRIG_CHAR ) );
   memset( lpFilter, 0, ( nLen1 + nLen2 + 4 ) * sizeof( BRIG_CHAR ) );
   memcpy( lpFilter, sLabel, nLen1 * sizeof( BRIG_CHAR ) );
   memcpy( lpFilter + nLen1 + 1, sMask, nLen2 * sizeof( BRIG_CHAR ) );

/*
   lpFileName = HB_PARSTR( 1, &hFileName, &nSize );
   if( nSize < 1024 )
   {
      memcpy( buffer, lpFileName, nSize * sizeof( TCHAR ) );
      memset( &buffer[nSize], 0, ( 1024 - nSize ) * sizeof( TCHAR ) );
      lpFileBuff = buffer;
      nSize = 1024;
   }
   else
      lpFileBuff = HB_STRUNSHARE( &hFileName, lpFileName, nSize );
*/

   OPENFILENAME ofn;
   BRIG_WCHAR buffer[1024];
   PBRIG_WCHAR wcText = brig_str2WC( lpFilter, nLen1 + nLen2 + 2 );
   bool bOk;

   sprintf( (PBRIG_CHAR)buffer, (PBRIG_CHAR) "*.*" );
   memset( ( void * ) &ofn, 0, sizeof( OPENFILENAME ) );
   ofn.lStructSize = sizeof( ofn );
   ofn.hwndOwner = GetActiveWindow(  );
   ofn.lpstrFilter = wcText;
   ofn.lpstrFile = buffer;
   ofn.nMaxFile = 1024;
   ofn.lpstrInitialDir = NULL;
   ofn.lpstrTitle = NULL;
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;

   bOk = GetSaveFileName( &ofn );

   brig_free( wcText );
   free( lpFilter );

   if( bOk )
   {
      int iLen;

   #if defined(UNICODE)
      iLen = wcslen( ofn.lpstrFile );
      return brig_WCTostr( CP_UTF8, ofn.lpstrFile, iLen );
   #else
      PBRIG_CHAR sRet;
      iLen = strlen( ofn.lpstrFile );
      sRet = ( PBRIG_CHAR ) malloc( iLen * sizeof( BRIG_CHAR ) + 1 );
      memcpy( sRet, ofn.lpstrFile, iLen * sizeof( BRIG_CHAR ) );
      sRet[iLen] = '\0';
      return sRet;
   #endif
   }
   else
      return NULL;
}

