
#include "brig.h"
#include <stdio.h>

brig_Edit oEdit;

bool fncTreeOnSize( brig_Widget *pTree, WPARAM wParam, LPARAM lParam )
{
   unsigned long iHeight = ( ((unsigned long)lParam) >> 16 ) & 0xFFFF;

   SYMBOL_UNUSED( wParam );

   pTree->Move( -1, -1, -1, iHeight-24 );
   return 0;

}

bool fncEditOnSize( brig_Widget *pEdit, WPARAM wParam, LPARAM lParam )
{
   unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;
   unsigned long iHeight = ( ((unsigned long)lParam) >> 16 ) & 0xFFFF;

   SYMBOL_UNUSED( wParam );

   pEdit->Move( -1, -1, iWidth-pEdit->iLeft-12, iHeight-254 );
   return 0;

}

bool fncPictOnSize( brig_Widget *pPict, WPARAM wParam, LPARAM lParam )
{
   //unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;
   //unsigned long iHeight = ( ((unsigned long)lParam) >> 16 ) & 0xFFFF;

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   pPict->Move( -1, oEdit.iTop+oEdit.iHeight+4, -1, -1 );
   return 0;

}

void fncTree( brig_TreeNode *pNode )
{
   char szRes[1024], *ptr;
   PBRIG_XMLITEM pItem;

   if( pNode->pData )
   {
      pItem = (PBRIG_XMLITEM) pNode->pData;
      sprintf( szRes, "-- %s --\r\n", pItem->szTitle );
      ptr = szRes + strlen( szRes );
      if( !( pItem->amAttr.empty() ) )
      {
         for( std::map<std::string,char*>::iterator it = pItem->amAttr.begin(); it != pItem->amAttr.end(); it++ )
         {
            sprintf( ptr, "%s = %s\r\n", (*it).first.c_str(), (*it).second );
            ptr += strlen( ptr );
         }
      }
      if( pItem->szText )
         sprintf( ptr, "------------\r\n%s\r\n", pItem->szText );
   }
   else
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
   PBRIG_BITMAP hBitmap = brig_OpenImage( (PBRIG_CHAR)"images/brig2.jpg" );

   PBRIG_XMLITEM pXmlDoc;

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oMain.Create( 200, 200, 540, 500, (PBRIG_CHAR) "A tree test" );
   oMain.hFont = brigAddFont( (PBRIG_CHAR)"Georgia", 18 );

   oTree.Create( &oMain, 12, 12, 200, 380 );
   oTree.pfOnSize = fncTreeOnSize;

   oTree.AddImages( brig_OpenImage( (PBRIG_CHAR)"images/folder_closed.bmp" ), 
                    brig_OpenImage( (PBRIG_CHAR)"images/folder_open.bmp" ), 
                    brig_OpenImage( (PBRIG_CHAR)"images/book.bmp" ) );

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

   oEdit.Create( &oMain, 220, 12, 300, 220, NULL, ES_MULTILINE );
   oEdit.pfOnSize = fncEditOnSize;

   oPict.Create( &oMain, 220, 240, 300, 225, hBitmap );
   oPict.pfOnSize = fncPictOnSize;

   oMain.Activate();

   if( pXmlDoc )
      brigxml_Release( pXmlDoc );

   return 0;
}

