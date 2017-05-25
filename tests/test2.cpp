
#include "brig.h"
#include <stdio.h>

void fnc1( brig_Widget *pDlg )
{
   RECT rc;
   brig_GetClientRect( pDlg, &rc );
   brig_writelog( NULL, "%d %d %d %d\r\n", rc.left, rc.top, rc.right, rc.bottom );
}

bool fncCloseDlg( brig_Widget *pDlg, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   fnc1( pDlg->pParent );
   ((brig_Dialog*)(pDlg->pParent))->Close();
   return 1;
}

int brig_Main( int argc, char *argv[] )
{
   brig_Dialog oDlg;
   brig_Button oBtn;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oDlg.Create( NULL, 200, 200, 400, 300, (PBRIG_CHAR) "A test dialog" );

   fnc1( &oDlg );

   oBtn.Create( &oDlg, 10, 4, 380, 32, (PBRIG_CHAR) "Ok" );
   oBtn.pfOnClick = fncCloseDlg;

   oDlg.Activate( 0 );
   return 0;
}

