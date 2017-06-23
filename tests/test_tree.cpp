
#include "brig.h"
#include <stdio.h>

int brig_Main( int argc, char *argv[] )
{
   brig_MainWindow oMain;
   brig_Tree oTree;
   brig_Edit oEdit;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oMain.Create( 200, 200, 400, 500, (PBRIG_CHAR) "A tree test" );

   oTree.Create( &oMain, 20, 20, 360, 400 );


   oEdit.Create( &oMain, 20, 430, 360, 28 );

   oMain.Activate();
   return 0;
}

