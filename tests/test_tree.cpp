
#include "brig.h"
#include <stdio.h>

brig_Edit oEdit;

void fncTree( brig_TreeNode *pNode )
{
   char szRes[16];
   PBRIG_XMLITEM pItem;

   if( pNode->pData )
   {
      pItem = (PBRIG_XMLITEM) pNode->pData;
   }
   sprintf( szRes, "Selected: %lu", (unsigned long) pNode->handle );
   oEdit.SetText( szRes );
}

void AddBranch( brig_TreeNode * pNode, PBRIG_XMLITEM pItem )
{

   brig_TreeNode * pChildNode;
   PBRIG_XMLITEM pChildItem;

   if( pItem->avItems.empty() )
      pChildNode = pNode->AddNode( pItem->szTitle, fncTree, NULL, NULL, 2 );
   else
   {
      int iNum = 0;
      pChildNode = pNode->AddNode( pItem->szTitle, fncTree, NULL, NULL, 0, 1 );
      while( ( pChildItem = brigxml_Next( pItem, &iNum ) ) != NULL )
         AddBranch( pChildNode, pChildItem );
   }
   pChildNode->pData = (void*) pItem;

}

int brig_Main( int argc, char *argv[] )
{
   brig_MainWindow oMain;
   brig_Tree oTree;
   brig_TreeNode * pNode;
   brig_Picture oPict;
   PBRIG_BITMAP hBitmap = brig_OpenImage( "images/brig2.jpg" );

   PBRIG_XMLITEM pXmlDoc;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oMain.Create( 200, 200, 400, 500, (PBRIG_CHAR) "A tree test" );

   oTree.Create( &oMain, 20, 12, 360, 160 );

   brig_TreeAddImage( &oTree, brig_OpenImage( "images/folder_closed.bmp" ) );
   brig_TreeAddImage( &oTree, brig_OpenImage( "images/folder_open.bmp" ) );
   brig_TreeAddImage( &oTree, brig_OpenImage( "images/book.bmp" ) );

   pXmlDoc = brigxml_GetDoc( (PBRIG_CHAR)"test_tree.xml" );
   if( !brigxml_Error() )
   {
      int iNum = 0;
      PBRIG_XMLITEM pParent, pItem;
      if( (pParent = brigxml_First( pXmlDoc )) != NULL )
      {
         pNode = oTree.AddNode( pParent->szTitle, fncTree, NULL, NULL, 0, 1 );
         pNode->pData = (void*) pParent;
         while( ( pItem = brigxml_Next( pParent, &iNum ) ) != NULL )
            AddBranch( pNode, pItem );
      }
   }
   else
   {
      pNode = oTree.AddNode( (PBRIG_CHAR)"First", fncTree, NULL, NULL, 0, 1 );
      pNode->AddNode( (PBRIG_CHAR)"f1", fncTree, NULL, NULL, 2 );
      oTree.AddNode( (PBRIG_CHAR)"Second", fncTree, NULL, NULL, 2 );
   }

   oEdit.Create( &oMain, 20, 180, 360, 28 );

   oPict.Create( &oMain, 50, 220, 300, 225, hBitmap );

   oMain.Activate();

   if( pXmlDoc )
      brigxml_Release( pXmlDoc );

   return 0;
}

