
#include "brig.h"
#include <stdio.h>

bool fncOnSize( brig_Widget *pPanel, WPARAM wParam, LPARAM lParam )
{
   unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;

   SYMBOL_UNUSED( wParam );

   pPanel->Move( 0, 0, iWidth, 40 );
   return 0;

}

int brig_Main( int argc, char *argv[] )
{

   brig_MainWindow * pMain = new brig_MainWindow;   
   brig_Panel oPanel;
   brig_QButton oQBtn1;
   brig_Edit oEdit;

   long pColors1[2] = {0x333333, 0xcccccc};

   if( argc > 1 ) 
      brig_writelog( NULL, "%s\r\n", argv[1] );
   
   pMain->Create( 100, 100, 500, 340, (PBRIG_CHAR) "First Brig Window" );
   pMain->hFont = brigAddFont( (PBRIG_CHAR)"Georgia", 20 );
   
  
   oPanel.Create( pMain, 0, 0, 500, 40, brigAddStyle( 2, pColors1 ) );
   oPanel.pfOnSize = fncOnSize;

   oQBtn1.Create( &oPanel, 0, 0, 48, 40, (PBRIG_CHAR)"Ok" );
   oQBtn1.lBackColor = oQBtn1.lBackClr1 = 0xcccccc;
   oQBtn1.SetFont( brigAddFont( (PBRIG_CHAR)"Georgia", 18, 400, 0, 1 ) );

   oEdit.Create( pMain, 4, 40, 480, 280, (PBRIG_CHAR)"", ES_MULTILINE | WS_BORDER );

   pMain->Activate();

   return 0;
}

