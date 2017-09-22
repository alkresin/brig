
#include "brig.h"
#include <stdio.h>

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

int brig_Main( int argc, char *argv[] )
{

   brig_MainWindow * pMain = new brig_MainWindow;   
   brig_Panel oPanel;
   brig_QButton oQBtn1, oQBtn2;
   brig_Edit oEdit;

   long pColors1[2] = {0x333333, 0xcccccc};

   if( argc > 1 ) 
      brig_writelog( NULL, "%s\r\n", argv[1] );
   
   pMain->Create( 100, 100, 500, 340, (PBRIG_CHAR) "First Brig Window" );
   pMain->hFont = brigAddFont( (PBRIG_CHAR)"Georgia", 20 );
   
   oPanel.Create( pMain, 0, 0, 500, 40, brigAddStyle( 2, pColors1 ) );
   oPanel.pfOnSize = fnc_paneOnSize;

   oQBtn1.Create( &oPanel, 0, 0, 48, 40, (PBRIG_CHAR)"Run" );
   oQBtn1.lBackColor = oQBtn1.lBackClr1 = 0xcccccc;
   oQBtn1.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );

   oQBtn2.Create( &oPanel, 48, 0, 48, 40, (PBRIG_CHAR)"Open" );
   oQBtn2.lBackColor = oQBtn2.lBackClr1 = 0xcccccc;
   oQBtn2.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );
   oQBtn2.pfOnClick = fncOpen;

   oEdit.Create( pMain, 4, 44, 480, 280, (PBRIG_CHAR)"", ES_MULTILINE | WS_BORDER );
   oEdit.pfOnSize = fnc_editOnSize;

   pMain->Activate();

   return 0;
}

