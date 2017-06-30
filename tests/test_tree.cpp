
#include "brig.h"
#include <stdio.h>

brig_Edit oEdit;

void fncTree( brig_TreeNode *pNode )
{
   char szRes[16];

   sprintf( szRes, "Selected: %lu", (unsigned long) pNode->handle );
   oEdit.SetText( szRes );
}

int brig_Main( int argc, char *argv[] )
{
   brig_MainWindow oMain;
   brig_Tree oTree;
   brig_TreeNode * pNode;
   brig_Picture oPict;
   PBRIG_BITMAP hBitmap = brig_OpenImage( "images/brig2.jpg" );

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oMain.Create( 200, 200, 400, 500, (PBRIG_CHAR) "A tree test" );

   oTree.Create( &oMain, 20, 12, 360, 160 );

   pNode = oTree.AddNode( (PBRIG_CHAR)"First", fncTree );
   pNode->AddNode( (PBRIG_CHAR)"f1", fncTree );
   pNode->AddNode( (PBRIG_CHAR)"f2", fncTree );
   oTree.AddNode( (PBRIG_CHAR)"Second", fncTree );
   
   oEdit.Create( &oMain, 20, 180, 360, 28 );

   oPict.Create( &oMain, 50, 220, 300, 225, hBitmap );

   oMain.Activate();
   return 0;
}

