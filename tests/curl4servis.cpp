
#include "brig.h"
#include <stdio.h>

static PBRIG_CHAR pCmdFile = "curl.ccc";
static PBRIG_CHAR pUrl = NULL;
static PBRIG_CHAR pLogin = NULL;
static PBRIG_CHAR pTns = NULL;
static PBRIG_CHAR pFunc = NULL;
static PBRIG_CHAR pParam = NULL;
static long pColors1[2] = {0x333333, 0xcccccc};

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
      brig_Container *pParent = ((brig_Container*)(pBtn->pParent->pParent));
      brig_Edit * pEdit = (brig_Edit *) pParent->FindWidget( TYPE_EDIT );
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

bool fncCloseDlg( brig_Widget *pBtn, unsigned int i, long l )
{
   SYMBOL_UNUSED( i );
   SYMBOL_UNUSED( l );

   brig_Dialog *pDlg = ((brig_Dialog*)(pBtn->pParent->pParent));

   pDlg->pResult = (void*) pDlg->avWidgets[0]->GetText();
   pDlg->Close();
   return 1;
}

static void showResult( void )
{

   PBRIG_CHAR pBuffer = brig_ReadFile( (PBRIG_CHAR) "curl.out" );

   if( !pBuffer )
      return;

   brig_Dialog oDlg;
   brig_Panel oPanel;
   brig_QButton oQBtn1, oQBtn2;
   brig_Edit oEdit;
   PBRIG_CHAR ptr;
   PBRIG_CHAR ptr2 = NULL;

   ptr = strstr( pBuffer, "<m:return" );
   if( ptr )
   {
      ptr = strchr( ptr, '>' );
      if( ptr )
      {
         ptr ++;
         ptr2 = strstr( ptr, "</m:return" );
         *ptr2 = '\0';
      }
   }
   if( !ptr || !ptr2 )
      ptr = pBuffer;

   oDlg.Create( brigApp.pMainWindow, 200, 200, 400, 250, (PBRIG_CHAR) "Result" );
   oDlg.hFont = brigAddFont( brigApp.pMainWindow->hFont );

   oPanel.Create( &oDlg, 0, 0, 400, 40, brigAddStyle( 2, pColors1 ) );
   oPanel.pfOnSize = fnc_paneOnSize;

   oQBtn1.Create( &oPanel, 0, 0, 48, 40, (PBRIG_CHAR)"Close" );
   oQBtn1.lBackColor = oQBtn1.lBackClr1 = 0xcccccc;
   oQBtn1.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );
   oQBtn1.pfOnClick = fncCloseDlg;

   oQBtn2.Create( &oPanel, 48, 0, 48, 40, (PBRIG_CHAR)"Save" );
   oQBtn2.lBackColor = oQBtn2.lBackClr1 = 0xcccccc;
   oQBtn2.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );
   oQBtn2.pfOnClick = fncSave;

   oEdit.Create( &oDlg, 4, 44, 390, 200, ptr, ES_MULTILINE );
   oEdit.pfOnSize = fnc_editOnSize;
   free( pBuffer );

   oDlg.Activate();

}

static void RunCode( PBRIG_CHAR pCode )
{
   PBRIG_CHAR pData = (PBRIG_CHAR) malloc( sizeof(PBRIG_CHAR) * ( strlen(pCode)+512 ) );
   PBRIG_CHAR ptr = pData;
   BRIG_CHAR szCmd[64];

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

   sprintf( szCmd, "curl -K %s", pCmdFile );
   brig_RunConsoleApp( szCmd );

   showResult();

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

   
   pMain->Create( 100, 100, 500, 340, (PBRIG_CHAR) "Curl For WebService" );
   pMain->hFont = brigAddFont( (PBRIG_CHAR)"Georgia", 20 );
   
   oPanel.Create( pMain, 0, 0, 500, 40, brigAddStyle( 2, pColors1 ) );
   oPanel.pfOnSize = fnc_paneOnSize;

   oQBtn1.Create( &oPanel, 0, 0, 48, 40, (PBRIG_CHAR)"Run" );
   oQBtn1.lBackColor = oQBtn1.lBackClr1 = 0xcccccc;
   oQBtn1.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );
   oQBtn1.pfOnClick = fncRun;

   oQBtn2.Create( &oPanel, 48, 0, 48, 40, (PBRIG_CHAR)"Open" );
   oQBtn2.lBackColor = oQBtn2.lBackClr1 = 0xcccccc;
   oQBtn2.SetFont( brigAddFont( oQBtn1.hFont ) );
   oQBtn2.pfOnClick = fncOpen;

   oQBtn3.Create( &oPanel, 96, 0, 48, 40, (PBRIG_CHAR)"Save" );
   oQBtn3.lBackColor = oQBtn3.lBackClr1 = 0xcccccc;
   oQBtn3.SetFont( brigAddFont( oQBtn1.hFont ) );
   oQBtn3.pfOnClick = fncSave;

   oEdit.Create( pMain, 4, 44, 480, 280, (PBRIG_CHAR)"", ES_MULTILINE | WS_BORDER );
   oEdit.pfOnSize = fnc_editOnSize;
   if( argc > 1 ) 
   {
      PBRIG_CHAR pBuffer = brig_ReadFile( argv[1] );
      if( pBuffer )
      {
         oEdit.SetText( pBuffer );
         free( pBuffer );
      }
   }

   pMain->Activate();

   return 0;
}

