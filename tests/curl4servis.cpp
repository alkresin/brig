
#include "brig.h"
#include <stdio.h>

static PBRIG_CHAR pCmdFile = "curl.ccc";
static PBRIG_CHAR pUrl = NULL;
static PBRIG_CHAR pLogin = NULL;
static PBRIG_CHAR pTns = NULL;
static PBRIG_CHAR pFunc = NULL;
static PBRIG_CHAR pParam = NULL;

bool fnc_paneOnSize( brig_Widget *pPanel, WPARAM wParam, LPARAM lParam )
{
   unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;

   SYMBOL_UNUSED( wParam );

   pPanel->Move( 0, 0, iWidth, 40 );
   return 0;

}

bool fnc_editOnSize( brig_Widget *pEdit, WPARAM wParam, LPARAM lParam )
{
   unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;
   unsigned long iHeight = ( ((unsigned long)lParam) >> 16 ) & 0xFFFF;

   SYMBOL_UNUSED( wParam );

   pEdit->Move( -1, -1, iWidth-12, iHeight-60 );
   return 0;

}

bool fncOpen( brig_Widget *pBtn, unsigned int i, long l )
{

   PBRIG_CHAR pFileName;
   PBRIG_CHAR pBuffer;

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( i );
   SYMBOL_UNUSED( l );

   pFileName = brig_ChooseFile( "All files( *.* )", "*.*" );

   if( pFileName )
   {
      pBuffer = brig_ReadFile( pFileName );
      if( pBuffer )
      {
         brig_Edit * pEdit = (brig_Edit *) brigApp.pMainWindow->FindWidget( TYPE_EDIT );
         pEdit->SetText( pBuffer );
         free( pBuffer );
      }
      free( pFileName );
   }

   return 0;
}

bool fncSave( brig_Widget *pBtn, unsigned int i, long l )
{

   PBRIG_CHAR pFileName;

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( i );
   SYMBOL_UNUSED( l );

   pFileName = brig_SaveFile( "All files( *.* )", "*.*" );

   if( pFileName )
   {
      brig_Edit * pEdit = (brig_Edit *) brigApp.pMainWindow->FindWidget( TYPE_EDIT );
      PBRIG_CHAR pBuffer = pEdit->GetText();

      if( pBuffer )
      {
         brig_WriteFile( pFileName, pBuffer );
         free( pBuffer );
      }
      free( pFileName );
   }

   return 0;
}

void RunCode( PBRIG_CHAR pCode )
{
   BRIG_CHAR pData = (PBRIG_CHAR) malloc( sizeof(PBRIG_CHAR) * ( strlen(pCode)+512 ) );
   PBRIG_CHAR ptr = pData;

   if( !pUrl )
      return;

   sprintf( ptr, "url = %s\r\n", pUrl );
   ptr += strlen( ptr );
   sprintf( ptr, "request = POST\r\ndata = @curl.data\r\n-S\r\n-s\r\noutput = \"curl.out\"\r\n-L\r\n-k\r\nstderr = err.out\r\n" );
   ptr += strlen( ptr );
   if( pLogin )
      sprintf( ptr, "-u %s\r\n", pLogin );

   brig_WriteFile( pCmdFile, pData );

   ptr = pData;
   sprintf( ptr, "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:tns=\"%s\">\r\n", pTns );
   ptr += strlen( ptr );
   sprintf( ptr, "<soap:Header/><soap:Body><tns:%s>\r\n", pFunc );
   ptr += strlen( ptr );
   sprintf( ptr, "<tns:%s><![CDATA[%s]]></tns:%s>\r\n", pParam, pCode, pParam );
   ptr += strlen( ptr );
   sprintf( ptr, "</tns:%s>\r\n", pFunc );
   ptr += strlen( ptr );
   sprintf( ptr, "</soap:Body></soap:Envelope>\r\n" );

   brig_WriteFile( "curl.data", pData );

   free( pData );
}

bool fncRun( brig_Widget *pBtn, unsigned int i, long l )
{

   PBRIG_CHAR pBuffer;
   brig_Edit * pEdit;

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( i );
   SYMBOL_UNUSED( l );

   pEdit = (brig_Edit *) brigApp.pMainWindow->FindWidget( TYPE_EDIT );
   pBuffer = pEdit->GetText();
   if( strlen( pBuffer ) > 8 )
   {
      RunCode( pBuffer );
   }

   free( pBuffer );
   return 0;
}


int brig_Main( int argc, char *argv[] )
{

   brig_MainWindow * pMain = new brig_MainWindow;   
   brig_Panel oPanel;
   brig_QButton oQBtn1, oQBtn2, oQBtn3;
   brig_Edit oEdit;
   PBRIG_XMLITEM pXmlDoc;

   long pColors1[2] = {0x333333, 0xcccccc};

   if( argc > 1 ) 
      brig_writelog( NULL, "%s\r\n", argv[1] );

   pXmlDoc = brigxml_GetDoc( (PBRIG_CHAR)"curl4servis.xml" );
   if( !brigxml_Error() )
   {
      int iNum = 0;
      PBRIG_XMLITEM pParent, pItem;
      if( (pParent = brigxml_First( pXmlDoc )) != NULL )
      {
         PBRIG_CHAR pAttrName;
         while( ( pItem = brigxml_Next( pParent, &iNum ) ) != NULL )
         {
             if( !strcmp( pItem->szTitle, "service" ) && !( pItem->amAttr.empty() ) )
             {
                for( std::map<std::string,char*>::iterator it = pItem->amAttr.begin(); it != pItem->amAttr.end(); it++ )
                {
                   pAttrName = (PBRIG_CHAR) ( (*it).first.c_str() );
                   if( !strcmp( pAttrName, "url" ) )
                      pUrl = (*it).second;
                   else if( !strcmp( pAttrName, "login" ) )
                      pLogin = (*it).second;
                   else if( !strcmp( pAttrName, "tns" ) )
                      pTns = (*it).second;
                   else if( !strcmp( pAttrName, "func" ) )
                      pFunc = (*it).second;
                   else if( !strcmp( pAttrName, "param" ) )
                      pParam = (*it).second;
                }
                if( !pTns )
                   pTns = (PBRIG_CHAR) "wstest";
                if( !pFunc )
                   pFunc = (PBRIG_CHAR) "wst_exec";
                if( !pParam )
                   pParam = (PBRIG_CHAR) "cCode";
             }
         }
      }
   }

   
   pMain->Create( 100, 100, 500, 340, (PBRIG_CHAR) "First Brig Window" );
   pMain->hFont = brigAddFont( (PBRIG_CHAR)"Georgia", 20 );
   
   oPanel.Create( pMain, 0, 0, 500, 40, brigAddStyle( 2, pColors1 ) );
   oPanel.pfOnSize = fnc_paneOnSize;

   oQBtn1.Create( &oPanel, 0, 0, 48, 40, (PBRIG_CHAR)"Run" );
   oQBtn1.lBackColor = oQBtn1.lBackClr1 = 0xcccccc;
   oQBtn1.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );
   oQBtn1.pfOnClick = fncRun;

   oQBtn2.Create( &oPanel, 48, 0, 48, 40, (PBRIG_CHAR)"Open" );
   oQBtn2.lBackColor = oQBtn2.lBackClr1 = 0xcccccc;
   oQBtn2.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );
   oQBtn2.pfOnClick = fncOpen;

   oQBtn3.Create( &oPanel, 96, 0, 48, 40, (PBRIG_CHAR)"Save" );
   oQBtn3.lBackColor = oQBtn3.lBackClr1 = 0xcccccc;
   oQBtn3.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );
   oQBtn3.pfOnClick = fncSave;

   oEdit.Create( pMain, 4, 44, 480, 280, (PBRIG_CHAR)"", ES_MULTILINE | WS_BORDER );
   oEdit.pfOnSize = fnc_editOnSize;

   pMain->Activate();

   return 0;
}

