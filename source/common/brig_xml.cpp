/*
 * Brig - a ligthweight C++ GUI framework
 * XML parsing
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
*/

#include <stdio.h>
#include <stdlib.h>
#include <brig_xml.h>

#if (defined(_MSC_VER)&&(_MSC_VER>=1400))
   #define sscanf sscanf_s
#endif

#define SKIPTABSPACES( sptr ) while( *sptr == ' ' || *sptr == '\t' || \
         *sptr == '\r' || *sptr == '\n' ) ( sptr )++
#define SKIPCHARS( sptr ) while( *sptr && *sptr != ' ' && *sptr != '\t' && \
         *sptr != '=' && *sptr != '>' && *sptr != '<' && *sptr != '\"' \
         && *sptr != '\'' && *sptr != '\r' && *sptr != '\n' ) ( sptr )++

#define XML_ERROR_NOT_LT        1
#define XML_ERROR_NOT_GT        2
#define XML_ERROR_WRONG_TAG_END 3
#define XML_ERROR_WRONG_END     4
#define XML_ERROR_WRONG_ENTITY  5
#define XML_ERROR_NOT_QUOTE     6
#define XML_ERROR_TERMINATION   7
#define XML_ERROR_FILE         10

#define XML_TYPE_TAG            0
#define XML_TYPE_SINGLE         1
#define XML_TYPE_COMMENT        2
#define XML_TYPE_CDATA          3
#define XML_TYPE_PI             4

#define ATTR_KEY_LEN           50
#define ATTR_VAL_LEN          100

static unsigned char *pStart;
static int nParseError;
static unsigned long ulOffset;

#define XML_PREDEFS_KOL         6
static int nPredefsKol = XML_PREDEFS_KOL;

static unsigned char *predefinedEntity1[] =
      { ( unsigned char * ) "lt;", ( unsigned char * ) "gt;",
         ( unsigned char * ) "amp;", ( unsigned char * ) "quot;",
         ( unsigned char * ) "apos;", ( unsigned char * ) "nbsp;" };
static unsigned char *predefinedEntity2 = ( unsigned char * ) "<>&\"\' ";

static unsigned char **pEntity1 = NULL;
static unsigned char *pEntity2 = NULL;

int brigxml_Error( void )
{
   return nParseError;
}

static void brigxml_error( int nError, unsigned char *ptr )
{
   nParseError = nError;
   ulOffset = ptr - pStart;
}

/*
 * brigxml_pp( unsigned char * ptr, unsigned long ulLen )
 * Translation of the predefined entities ( &lt;, etc. )
 */
static unsigned char* brigxml_pp( unsigned char *ptr, unsigned long ulLen )
{
   unsigned char *ptrStart = ptr, *pResult;
   int i, nlen;
   unsigned long ul = 0, ul1;

   while( ul < ulLen )
   {
      if( *ptr == '&' )
      {
         if( *( ptr + 1 ) == '#' )
         {
            int iChar;
            sscanf( ( char * ) ptr + 2, "%d", &iChar );
            *ptr = iChar;
            i = 1;
            while( *( ptr + i + 1 ) >= '0' && *( ptr + i + 1 ) <= '9' )
               i++;
            if( *( ptr + i + 1 ) == ';' )
               i++;
            ulLen -= i;
            for( ul1 = ul + 1; ul1 < ulLen; ul1++ )
               *( ptrStart + ul1 ) = *( ptrStart + ul1 + i );
         }
         else
         {
            for( i = 0; i < nPredefsKol; i++ )
            {
               nlen = strlen( ( char * ) pEntity1[i] );
               if( !memcmp( ptr + 1, pEntity1[i], nlen ) )
               {
                  *ptr = pEntity2[i];
                  ulLen -= nlen;
                  for( ul1 = ul + 1; ul1 < ulLen; ul1++ )
                     *( ptrStart + ul1 ) = *( ptrStart + ul1 + nlen );
                  break;
               }
            }
            if( i == nPredefsKol )
               brigxml_error( XML_ERROR_WRONG_ENTITY, ptr );
         }
      }
      ptr++;
      ul++;
   }
   ptr = ptrStart;
   SKIPTABSPACES( ptr );
   ulLen -= ( ptr - ptrStart );
   if( !ulLen )
      return NULL;
   ptrStart = ptr;
   ptr = ptrStart + ulLen - 1;
   while( *ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n' )
   {
      ptr--;
      ulLen--;
   }
   pResult = (unsigned char*) malloc( ulLen+1 );
   memcpy( pResult, ptrStart, ulLen );
   pResult[ulLen] = '\0';
   return pResult;
}

static std::map<std::string,char*> brigxml_getattr( unsigned char **pBuffer, bool * lSingle )
{

   std::map<std::string,char*> amAttr;
   unsigned char *ptr, cQuo;
   unsigned char szKey[ATTR_KEY_LEN], szVal[ATTR_VAL_LEN], *pKey = NULL, *pVal = NULL;
   int iLen;
   bool bPI = 0;

   *lSingle = 0;
   if( **pBuffer == '<' )
   {
      ( *pBuffer )++;
      if( **pBuffer == '?' )
         bPI = 1;
      SKIPTABSPACES( *pBuffer );     // go till tag name
      SKIPCHARS( *pBuffer ); // skip tag name
      if( *( *pBuffer - 1 ) == '/' || *( *pBuffer - 1 ) == '?' )
         ( *pBuffer )--;
      else
         SKIPTABSPACES( *pBuffer );

      while( **pBuffer && **pBuffer != '>' )
      {
         if( !( **pBuffer ) )
         {
            brigxml_error( XML_ERROR_TERMINATION, *pBuffer );
            break;
         }
         if( **pBuffer == '/' || **pBuffer == '?' )
         {
            *lSingle = ( **pBuffer == '/' ) ? 1 : 2;
            ( *pBuffer )++;
            if( **pBuffer != '>' || ( *lSingle == 2 && !bPI ) )
            {
               brigxml_error( XML_ERROR_NOT_GT, *pBuffer );
            }
            break;
         }
         ptr = *pBuffer;
         SKIPCHARS( *pBuffer );      // skip attribute name
         iLen = *pBuffer - ptr;
         // add attribute name to result array
         if( iLen < ATTR_KEY_LEN )
         {
            memcpy( szKey, ptr, iLen );
            szKey[iLen] = '\0';
         }
         else
         {
            pKey = (unsigned char*) malloc( iLen+1 );
            memcpy( pKey, ptr, iLen );
            pKey[iLen] = '\0';
         }
         //brig_writelog( NULL, "getattr-1 %s\r\n", (pKey)? pKey:szKey );

         SKIPTABSPACES( *pBuffer );  // go till '='
         if( **pBuffer == '=' )
         {
            ( *pBuffer )++;
            SKIPTABSPACES( *pBuffer );       // go till attribute value
            cQuo = **pBuffer;
            if( cQuo == '\"' || cQuo == '\'' )
               ( *pBuffer )++;
            else
            {
               brigxml_error( XML_ERROR_NOT_QUOTE, *pBuffer );
               break;
            }
            ptr = *pBuffer;
            while( **pBuffer && **pBuffer != cQuo )
               ( *pBuffer )++;
            if( **pBuffer != cQuo )
            {
               brigxml_error( XML_ERROR_NOT_QUOTE, *pBuffer );
               break;
            }
            iLen = *pBuffer - ptr;
            // add attribute value to result array
            if( iLen < ATTR_VAL_LEN )
            {
               memcpy( szVal, ptr, iLen );
               szVal[iLen] = '\0';
            }
            else
            {
               pVal = (unsigned char*) malloc( iLen+1 );
               memcpy( pVal, ptr, iLen );
               pVal[iLen] = '\0';
            }
            //brig_writelog( NULL, "getattr-2 %s\r\n", (pVal)? pVal:szVal );
            amAttr[ (char*) ((pKey)? pKey : szKey) ] = (char*) ((pVal)? pVal : szVal);
            if( pKey )
               free( pKey );
            if( pVal )
               free( pVal );
            pKey = pVal = NULL;
            ( *pBuffer )++;
         }
         SKIPTABSPACES( *pBuffer );
      }
      if( **pBuffer == '>' )
         ( *pBuffer )++;
   }
   return amAttr;
}

static PBRIG_XMLITEM brigxml_addnode( PBRIG_XMLITEM pParent )
{
   PBRIG_XMLITEM pItem = new BRIG_XMLITEM;

   pItem->szTitle = NULL;
   pItem->szText  = NULL;
   pItem->iType   = XML_TYPE_TAG;

   pParent->avItems.push_back( pItem );
   return pItem;
}

static bool brigxml_readComment( PBRIG_XMLITEM pParent, unsigned char **pBuffer )
{
   unsigned char *ptr;

   ( *pBuffer ) += 4;
   ptr = *pBuffer;
   while( **pBuffer &&
         ( **pBuffer != '-' || *( *pBuffer + 1 ) != '-' ||
               *( *pBuffer + 2 ) != '>' ) )
      ( *pBuffer )++;

   if( **pBuffer )
   {
      if( pParent )
      {
         PBRIG_XMLITEM pNode = brigxml_addnode( pParent );
         long lLen = *pBuffer - ptr;
         pNode->szText = (PBRIG_CHAR) malloc( lLen + 1 );
         memcpy( pNode->szText, ptr, lLen );
         pNode->szText[lLen] = '\0';
         pNode->iType = XML_TYPE_COMMENT;
      }
      ( *pBuffer ) += 3;
   }
   else
      brigxml_error( XML_ERROR_TERMINATION, *pBuffer );

   return ( nParseError ) ? 0 : 1;
}

static bool brigxml_readCDATA( PBRIG_XMLITEM pParent, unsigned char **pBuffer )
{
   unsigned char *ptr;

   ( *pBuffer ) += 9;
   ptr = *pBuffer;
   while( **pBuffer &&
         ( **pBuffer != ']' || *( *pBuffer + 1 ) != ']' ||
               *( *pBuffer + 2 ) != '>' ) )
      ( *pBuffer )++;

   if( **pBuffer )
   {
      PBRIG_XMLITEM pNode = brigxml_addnode( pParent );
      long lLen = *pBuffer - ptr;
      pNode->szText = (PBRIG_CHAR) malloc( lLen + 1 );
      memcpy( pNode->szText, ptr, lLen );
      pNode->szText[lLen] = '\0';
      pNode->iType = XML_TYPE_CDATA;

      ( *pBuffer ) += 3;
   }
   else
      brigxml_error( XML_ERROR_TERMINATION, *pBuffer );

   return ( nParseError ) ? 0 : 1;
}

static bool brigxml_readElement( PBRIG_XMLITEM pParent, unsigned char **pBuffer )
{
   PBRIG_XMLITEM pNode = brigxml_addnode( pParent );
   unsigned char *ptr;
   int iLenNodeName;
   bool lEmpty, lSingle;

   ( *pBuffer )++;
   if( **pBuffer == '?' )
      ( *pBuffer )++;
   ptr = *pBuffer;
   SKIPCHARS( ptr );

   iLenNodeName = ptr - *pBuffer - ( ( *( ptr - 1 ) == '/' ) ? 1 : 0 );
   pNode->szTitle = (PBRIG_CHAR) malloc( iLenNodeName + 1 );
   memcpy( pNode->szTitle, *pBuffer, iLenNodeName );
   pNode->szTitle[iLenNodeName] = '\0';

   ( *pBuffer )--;
   if( **pBuffer == '?' )
      ( *pBuffer )--;
   pNode->amAttr = brigxml_getattr( pBuffer, &lSingle );
   if( nParseError )
   {
      brigxml_Release( pNode );
      return 0;
   }

   pNode->iType = ( lSingle ) ? ( ( lSingle == 2 ) ? XML_TYPE_PI : XML_TYPE_SINGLE ) : XML_TYPE_TAG;

   if( !lSingle )
   {
      while( 1 )
      {
         ptr = *pBuffer;
         lEmpty = 1;
         while( **pBuffer != '<' )
         {
            if( lEmpty && ( **pBuffer != ' ' && **pBuffer != '\t' &&
                        **pBuffer != '\r' && **pBuffer != '\n' ) )
               lEmpty = 1;
            ( *pBuffer )++;
         }
         if( !lEmpty )
         {
            unsigned char * szTemp = brigxml_pp( ptr, *pBuffer - ptr );
            if( !szTemp || nParseError )
            {
               brigxml_Release( pNode );
               return 0;
            }
            pNode->szText = szTemp;
         }

         if( *( *pBuffer + 1 ) == '/' )
         {
            if( memcmp( *pBuffer + 2, pNode->szTitle, iLenNodeName ) )
            {
               brigxml_error( XML_ERROR_WRONG_TAG_END, *pBuffer );
               brigxml_Release( pNode );
               return 0;
            }
            else
            {
               while( **pBuffer != '>' )
                  ( *pBuffer )++;
               ( *pBuffer )++;
               break;
            }
         }
         else
         {
            if( !memcmp( *pBuffer + 1, "!--", 3 ) )
            {
               if( !brigxml_readComment( pNode, pBuffer ) )
               {
                  brigxml_Release( pNode );
                  return 0;
               }
            }
            else if( !memcmp( *pBuffer + 1, "![CDATA[", 8 ) )
            {
               if( !brigxml_readCDATA( pNode, pBuffer ) )
               {
                  brigxml_Release( pNode );
                  return 0;
               }
            }
            else
            {
               if( !brigxml_readElement( pNode, pBuffer ) )
               {
                  brigxml_Release( pNode );
                  return 0;
               }
            }
         }
      }
   }
   return 1;

}

void brigxml_Release( PBRIG_XMLITEM pItem )
{
   if( pItem->szTitle )
      free( pItem->szTitle );
   if( pItem->szText )
      free( pItem->szText );
   pItem->szTitle = pItem->szText = NULL;

   for( unsigned int i = 0; i < pItem->avItems.size(); i++ )
      brigxml_Release( pItem->avItems[i] );

   delete pItem;
}

static unsigned char * brig_ReadFile( PBRIG_CHAR szName )
{
   unsigned char * pBuffer = NULL;
   FILE *f = fopen( szName, "rb" );
   long lSize;

   if( f )
   {
      fseek( f, 0, SEEK_END );
      lSize = ftell( f );
      fseek(f, 0, SEEK_SET);

      if( lSize )
      {
         pBuffer = (unsigned char *) malloc( lSize + 1 );
         fread( pBuffer, lSize, 1, f );
         fclose( f );

         pBuffer[lSize] = 0;
      }
   }
   return pBuffer;
}

PBRIG_XMLITEM brigxml_GetDoc( PBRIG_CHAR szSource, bool bFile )
{
   PBRIG_XMLITEM pDoc = new BRIG_XMLITEM;
   unsigned char *ptr, *pBuffer;
   int iMainTags = 0;

   if( !pEntity1 )
   {
      pEntity1 = predefinedEntity1;
      pEntity2 = predefinedEntity2;
   }
   if( bFile )
      ptr = pBuffer = brig_ReadFile( szSource );
   else
      ptr = szSource;
   pStart = ptr;

   if( !ptr )
   {
      nParseError = XML_ERROR_FILE;
      return NULL;
   }
   nParseError = 0;
   SKIPTABSPACES( ptr );
   if( *ptr != '<' )
      brigxml_error( XML_ERROR_NOT_LT, ptr );
   else
   {
      if( !memcmp( ptr + 1, "?xml", 4 ) )
      {
         bool bSingle;
         pDoc->amAttr = brigxml_getattr( &ptr, &bSingle );
         if( pDoc->amAttr.empty() || nParseError )
         {
            if( bFile )
               free( pBuffer );
            brigxml_Release( pDoc );
            return NULL;
         }
         SKIPTABSPACES( ptr );
      }
      while( 1 )
      {
         if( !memcmp( ptr + 1, "!DOCTYPE", 8 ) )
         {
            while( *ptr != '>' )
               ptr ++;
            ptr ++;

            SKIPTABSPACES( ptr );
         }
         else if( !memcmp( ptr + 1, "?xml", 4 ) )
         {
            while( *ptr != '>' )
               ptr ++;
            ptr ++;
            SKIPTABSPACES( ptr );
         }
         else if( !memcmp( ptr + 1, "!--", 3 ) )
         {
            while( (*ptr != '>') || (*(ptr-1) != '-') || (*(ptr-2) != '-') )
               ptr ++;
            ptr ++;
            SKIPTABSPACES( ptr );
         }
         else
            break;
      }
      while( 1 )
      {
         if( !memcmp( ptr + 1, "!--", 3 ) )
         {
            if( !brigxml_readComment( NULL, &ptr ) )
               break;
         }
         else
         {
            if( iMainTags )
               brigxml_error( XML_ERROR_WRONG_END, ptr );
            if( !brigxml_readElement( pDoc, &ptr ) )
               break;
            iMainTags++;
         }
         SKIPTABSPACES( ptr );
         if( !*ptr )
            break;
      }
   }

   if( bFile )
      free( pBuffer );

   return pDoc;
}
