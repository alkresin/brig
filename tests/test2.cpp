
#include "brig.h"
#include <stdio.h>

bool fncCloseDlg( brig_Widget *pDlg, WPARAM wParam, LPARAM lParam )
{
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   ((brig_Dialog*)(pDlg->pParent))->Close();
   return 1;
}

int brig_Main( int argc, char *argv[] )
{
   brig_Dialog oDlg;
   brig_Button oBtn;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oDlg.New( NULL, 200, 200, 400, 300, (PBRIG_CHAR) "A test dialog" );

   oBtn.New( &oDlg, 150, 220, 100, 32, (PBRIG_CHAR) "Ok" );
   oBtn.pfOnClick = fncCloseDlg;

   oDlg.Activate( 0 );
   return 0;
}

