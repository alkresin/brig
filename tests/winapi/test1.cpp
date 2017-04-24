
#include "brig.h"
#include <stdio.h>

brig_MainWindow oMain;
brig_Edit oEdit;

bool fncOnClose( brig_Widget *pDlg )
{
   SYMBOL_UNUSED( pDlg );
   return brig_MsgYesNo( (PBRIG_CHAR) "Really close window?", (PBRIG_CHAR) "Warning" );
}

bool fncOnClick( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
{
   PBRIG_CHAR pText;

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   oEdit.SetFont( brigAddFont( "Georgia", 20, 700 ) );
   oEdit.SetTextColor( 255, 0 );
   oEdit.SetBackColor( 11184810, 1 );
   pText = oEdit.GetText();
   brig_MsgInfo( pText, (PBRIG_CHAR) "StartBox" );
   free( pText );

   return 0;
}

bool fncOnSize( brig_Widget *pPanel, WPARAM wParam, LPARAM lParam )
{
   unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;
   //unsigned long iHeight = ( ((unsigned long)lParam) >> 16 ) & 0xFFFF;

   SYMBOL_UNUSED( wParam );

   //brig_writelog( NULL, "fncOnSize: %d\r\n", iWidth );
   pPanel->Move( 0, 0, iWidth );
   return 0;

}

void fncMenu1( void )
{
   brig_MsgInfo( (PBRIG_CHAR) "1", (PBRIG_CHAR) "Menu" );
}

void fncMenu1e( void )
{
   oMain.Close();
}

bool fncCloseDlg( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
{
   brig_Dialog *pDlg = ((brig_Dialog*)(pBtn->pParent));

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   pDlg->pResult = (void*) pDlg->avWidgets[0]->GetText();
   pDlg->Close();
   return 1;
}

void fncMenu2( void )
{

   brig_Dialog oDlg;
   brig_Button oBtnD;
   brig_Edit oEdit1, oEdit2, oEdit3;

   oDlg.New( &oMain, 200, 200, 200, 250, (PBRIG_CHAR) "A test dialog" );

   oEdit1.New( &oDlg, 20, 20, 100, 28, NULL, WS_TABSTOP );
   oEdit2.New( &oDlg, 20, 50, 100, 28, NULL, WS_TABSTOP );
   oEdit3.New( &oDlg, 20, 80, 100, 28, NULL, WS_TABSTOP );

   oBtnD.New( &oDlg, 50, 170, 100, 32, (PBRIG_CHAR) "Ok" );
   oBtnD.pfOnClick = fncCloseDlg;

   oDlg.Activate();
   if( oDlg.pResult )
   {
      brig_MsgInfo( (PBRIG_CHAR)oDlg.pResult, (PBRIG_CHAR) "Result" );
      free( oDlg.pResult );
   }
      
}

int brig_Main( int argc, char *argv[] )
{

   brig_Panel oPanel;
   brig_Button oBtn;
   brig_QButton oQBtn;
   brig_CheckButton oCheck;
   long pColors1[2] = {0x333333, 0xcccccc};
   PBRIG_PEN pPen2;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );
   /*
   for( int i = 0; i < argc; ++i ) {
      brig_writelog( NULL, "arg%d: %s\r\n", i, argv[i] );
   }
   */
   /*
   brigAddPen( 2, 0x333333 );
   brigAddPen( 2, 0x333333 );
   pPen2 = brigAddPen( 2, 0x333333, 1 );
   brigAddPen( 3, 0x333333 );
   brigAddPen( 3, 0x333333 );

   brig_writelog( NULL, "size: %d\r\n", brigApp.avPens.size() );
   brigDelPen( pPen2 );
   brig_writelog( NULL, "size: %d\r\n", brigApp.avPens.size() );
   */

   oMain.New( 100, 100, 500, 300, (PBRIG_CHAR) "First Brig Window" );
   oMain.pfOnClose = fncOnClose;

   brigMenu( &oMain );

   if( brigSubMenu( "File", 0 ) )
   {
      brigMenuItemAdd( "Open", 0, fncMenu1 );
      brigMenuSeparator();
      brigMenuItemAdd( "Close", 0, fncMenu1e );
      brigMenuEnd();
   }

   brigMenuItemAdd( "Help", 0, fncMenu2 );

   brigMenuEnd();

   oPanel.New( &oMain, 0, 0, 500, 40, brigAddStyle( 2, pColors1 ) );
   oPanel.pfOnSize = fncOnSize;

   oEdit.New( &oMain, 20, 60, 100, 28 );
   oCheck.New( &oMain, 160, 60, 100, 28, "Mark me" );

   oQBtn.New( &oMain, 20, 100, 48, 32, "Ok" );
   oQBtn.lBackColor = oQBtn.lBackClr1 = 0xcccccc;
   oQBtn.SetFont( brigAddFont( "Georgia", 18, 400, 0, 1 ) );

   oBtn.New( &oMain, 100, 100, 100, 32, (PBRIG_CHAR) "Помощь" );
   oBtn.pfOnClick = fncOnClick;

   //brig_writelog( NULL, "edit id: %d\r\n", oEdit.iControlId );
   //brig_writelog( NULL, "button id: %d\r\n", oBtn.iControlId );

   oMain.Activate();

   return 0;
}

