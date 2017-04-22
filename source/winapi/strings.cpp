/*
 * Brig - a ligthweight C++ GUI framework
 * WinAPI version
 * Strings manipulation functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"
#include <stdio.h>

bool brig_utf8NextChar( const unsigned char ucChar, int * n, wchar_t * pwc )
{
   if( *n > 0 )
   {
      if( ( ucChar & 0xc0 ) != 0x80 )
      {
         *n = 0;
         return 0;
      }
      *pwc = ( *pwc << 6 ) | ( ucChar & 0x3f );
      ( *n )--;
      return 1;
   }

   *n = 0;
   *pwc = ucChar;
   if( ucChar >= 0xc0 )
   {
      if( ucChar < 0xe0 )
      {
         *pwc &= 0x1f;
         *n = 1;
      }
      else if( ucChar < 0xf0 )
      {
         *pwc &= 0x0f;
         *n = 2;
      }
      else if( ucChar < 0xf8 )
      {
         *pwc &= 0x07;
         *n = 3;
      }
      else if( ucChar < 0xfc )
      {
         *pwc &= 0x03;
         *n = 4;
      }
      else if( ucChar < 0xfe )
      {
         *pwc &= 0x01;
         *n = 5;
      }
   }
   return 1;
}

unsigned long brig_utf8StringLength( const char * pSrc, unsigned long nLen )
{
   unsigned long ul, nDst;
   wchar_t wc;
   int n = 0;

   for( ul = nDst = 0; ul < nLen; )
   {
      if( brig_utf8NextChar( pSrc[ ul ], &n, &wc ) )
         ++ul;
      if( n == 0 )
         ++nDst;
   }
   if( n > 0 )
      ++nDst;

   return nDst;
}

wchar_t * brig_strToWC( UINT cp, char * szText )
{
   unsigned int uiLen;
   unsigned int uiDest;
   wchar_t * pResult = NULL;

   if( szText )
   {
      uiLen = strlen( szText );
      uiDest = MultiByteToWideChar( cp, 0, szText, uiLen, NULL, 0 );
      pResult = ( wchar_t * ) malloc( ( uiDest + 1 ) * sizeof( wchar_t ) );

      pResult[uiDest] = 0;
      MultiByteToWideChar( cp, 0, szText, uiLen, pResult, uiDest );
   }

   return pResult;
}

PBRIG_CHAR brig_WCTostr( UINT cp, wchar_t * wcText, int uiLen )
{

   unsigned int uiDest;
   char * pResult = NULL;

   if( wcText )
   {
      uiDest = WideCharToMultiByte( cp, 0, wcText, uiLen, NULL, 0, NULL, NULL );
      pResult = ( char * ) malloc( uiDest + 1 );
      WideCharToMultiByte( cp, 0, wcText, uiLen, pResult, uiDest, NULL, NULL );
      pResult[uiDest] = '\0';
   }
   return pResult;
}

#if defined(UNICODE)

void brig_free( PBRIG_WCHAR wcString )
{
   if( wcString )
      free( wcString );
}

#else

PBRIG_CHAR brig_str2WC( PBRIG_CHAR szText )
{
   return szText;
}

PBRIG_CHAR brig_WC2str( PBRIG_CHAR szText, int uiLen )
{
   SYMBOL_UNUSED( uiLen );
   return szText;
}

void brig_free( char * szString )
{
   SYMBOL_UNUSED( szString );
}

#endif

