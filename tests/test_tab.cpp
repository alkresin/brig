
#include "brig.h"
#include <stdio.h>

int brig_Main( int argc, char *argv[] )
{
   brig_MainWindow oMain;
   brig_Tab oTab;
   brig_Edit oEdit1, oEdit2;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oMain.Create( 200, 200, 400, 300, (PBRIG_CHAR) "A tab test" );

   oTab.Create( &oMain, 20, 20, 360, 240 );

   oTab.StartPage( (PBRIG_CHAR) "First" );
   oEdit1.Create( &oTab, 20, 60, 100, 28, (PBRIG_CHAR) "Start" );
   oTab.EndPage();

   oTab.StartPage( (PBRIG_CHAR) "Second" );
   oEdit2.Create( &oTab, 20, 120, 100, 28, (PBRIG_CHAR) "End" );
   oTab.EndPage();

   oMain.Activate();
   return 0;
}

