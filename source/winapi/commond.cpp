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

   //if( HB_ISCHAR( 1 ) && HB_ISCHAR( 2 ) )
   {
      int nLen1, nLen2;

      nLen1 = strlen( sLabel );
      nLen2 = strlen( sMask );
      lpFilter =
            ( PBRIG_CHAR ) malloc( ( nLen1 + nLen2 + 4 ) * sizeof( BRIG_CHAR ) );
      memset( lpFilter, 0, ( nLen1 + nLen2 + 4 ) * sizeof( BRIG_CHAR ) );
      memcpy( lpFilter, sLabel, nLen1 * sizeof( BRIG_CHAR ) );
      memcpy( lpFilter + nLen1 + 1, sMask, nLen2 * sizeof( BRIG_CHAR ) );

   }
   /*else if( HB_ISARRAY( 1 ) && HB_ISARRAY( 2 ) )
   {
      struct _hb_arrStr
      {
         void *hStr1;
         LPCTSTR lpStr1;
         HB_SIZE nLen1;
         void *hStr2;
         LPCTSTR lpStr2;
         HB_SIZE nLen2;
      } *pArrStr;
      PHB_ITEM pArr1 = hb_param( 1, HB_IT_ARRAY );
      PHB_ITEM pArr2 = hb_param( 2, HB_IT_ARRAY );
      HB_SIZE n, nArrLen = hb_arrayLen( pArr1 ), nSize;
      LPTSTR ptr;

      pArrStr = ( struct _hb_arrStr * )
            hb_xgrab( nArrLen * sizeof( struct _hb_arrStr ) );
      nSize = 4;
      for( n = 0; n < nArrLen; n++ )
      {
         pArrStr[n].lpStr1 = HB_ARRAYGETSTR( pArr1, n + 1,
               &pArrStr[n].hStr1, &pArrStr[n].nLen1 );
         pArrStr[n].lpStr2 = HB_ARRAYGETSTR( pArr2, n + 1,
               &pArrStr[n].hStr2, &pArrStr[n].nLen2 );
         nSize += pArrStr[n].nLen1 + pArrStr[n].nLen2 + 2;
      }
      lpFilter = ( LPTSTR ) hb_xgrab( nSize * sizeof( TCHAR ) );
      ptr = lpFilter;
      for( n = 0; n < nArrLen; n++ )
      {
         memcpy( ptr, pArrStr[n].lpStr1, pArrStr[n].nLen1 * sizeof( TCHAR ) );
         ptr += pArrStr[n].nLen1;
         *ptr++ = 0;
         memcpy( ptr, pArrStr[n].lpStr2, pArrStr[n].nLen2 * sizeof( TCHAR ) );
         ptr += pArrStr[n].nLen2;
         *ptr++ = 0;
         hb_strfree( pArrStr[n].hStr1 );
         hb_strfree( pArrStr[n].hStr2 );
      }
      *ptr++ = 0;
      *ptr = 0;
      hb_xfree( pArrStr );
   }
   else
   {
      hb_retc( NULL );
      return;
   }
   */

   OPENFILENAME ofn;
   BRIG_WCHAR buffer[1024];
   PBRIG_WCHAR wcText = brig_str2WC( lpFilter );
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
      #if defined(UNICODE)
      int iLen = wcslen( ofn.lpstrFile );
      return brig_WCTostr( CP_UTF8, ofn.lpstrFile, iLen );
      #else
      return ofn.lpstrFile;
      #endif
   }
   else
      return NULL;

}

/*
HB_FUNC( HWG_SAVEFILE )
{
   OPENFILENAME ofn;
   TCHAR buffer[1024];
   void *hFileName, *hStr1, *hStr2, *hTitle, *hInitDir;
   LPCTSTR lpFileName, lpStr1, lpStr2;
   HB_SIZE nSize, nLen1, nLen2;
   LPTSTR lpFilter, lpFileBuff;

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


   lpStr1 = HB_PARSTRDEF( 2, &hStr1, &nLen1 );
   lpStr2 = HB_PARSTRDEF( 3, &hStr2, &nLen2 );

   lpFilter = ( LPTSTR ) hb_xgrab( ( nLen1 + nLen2 + 4 ) * sizeof( TCHAR ) );
   memset( lpFilter, 0, ( nLen1 + nLen2 + 4 ) * sizeof( TCHAR ) );
   memcpy( lpFilter, lpStr1, nLen1 * sizeof( TCHAR ) );
   memcpy( lpFilter + nLen1 + 1, lpStr2, nLen2 * sizeof( TCHAR ) );

   hb_strfree( hStr1 );
   hb_strfree( hStr2 );

   memset( ( void * ) &ofn, 0, sizeof( OPENFILENAME ) );
   ofn.lStructSize = sizeof( ofn );
   ofn.hwndOwner = GetActiveWindow(  );
   ofn.lpstrFilter = lpFilter;
   ofn.lpstrFile = lpFileBuff;
   ofn.nMaxFile = nSize;
   ofn.lpstrInitialDir = HB_PARSTR( 4, &hInitDir, NULL );
   ofn.lpstrTitle = HB_PARSTR( 5, &hTitle, NULL );
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
   if( HB_ISLOG( 6 ) && hb_parl( 6 ) )
      ofn.Flags = ofn.Flags | OFN_OVERWRITEPROMPT;

   if( GetSaveFileName( &ofn ) )
      HB_RETSTR( ofn.lpstrFile );
   else
      hb_retc( NULL );
   hb_xfree( lpFilter );

   hb_strfree( hFileName );
   hb_strfree( hInitDir );
   hb_strfree( hTitle );
}
*/
