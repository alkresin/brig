
#include "brig.h"
#include <stdio.h>

bool fncCloseDlg( brig_Widget *pDlg, WPARAM wParam, LPARAM lParam )
{
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   ((brig_Dialog*)(pDlg->pParent))->Close();
   return 1;
}

int fncRows( void )
{
   return 0;
}

int brig_Main( int argc, char *argv[] )
{
   brig_Dialog oDlg;
   brig_Button oBtn;
   brig_Table oTable;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oDlg.New( NULL, 200, 200, 400, 300, (PBRIG_CHAR) "A table test" );

   oTable.New( &oDlg, 20, 20, 360, 160 );

   oTable.pfRows = fncRows;
   oTable.AddColumn( "First", 80 );
   oTable.AddColumn( "Second", 80 );

   oBtn.New( &oDlg, 150, 220, 100, 32, (PBRIG_CHAR) "Ok" );
   oBtn.pfOnClick = fncCloseDlg;

   oDlg.Activate( 0 );
   return 0;
}

