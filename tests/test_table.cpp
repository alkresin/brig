
#include "brig.h"
#include <stdio.h>

//std::vector<PBRIG_CHAR*> avTable;
unsigned long ulDataLen = 1;

bool fncCloseDlg( brig_Widget *pDlg, WPARAM wParam, LPARAM lParam )
{
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   ((brig_Dialog*)(pDlg->pParent))->Close();
   return 1;
}

unsigned long fncTable( brig_Table *pTable, int iOp, unsigned long ulData )
{
   switch( iOp ) {

      case TDS_COUNT:
         return ulDataLen;

      case TDS_TOP:
         pTable->ulRecCurr = 1;
         break;

      case TDS_BOTTOM:
         pTable->ulRecCurr = ulDataLen;
         break;

      case TDS_FORWARD:
         pTable->ulRecCurr += ulData;
         if( pTable->ulRecCurr > ulDataLen )
            pTable->ulRecCurr = ulDataLen;
         break;

      case TDS_BACK:
         pTable->ulRecCurr -= ulData;
         if( pTable->ulRecCurr < 1 )
            pTable->ulRecCurr = 1;
         break;

      case TDS_BOF:
         return ( pTable->ulRecCurr == 1 );

      case TDS_EOF:
         return ( pTable->ulRecCurr == ulDataLen );

      case TDS_RECNO:
         return pTable->ulRecCurr;

      case TDS_GOTO:
         pTable->ulRecCurr = (ulData>ulDataLen)? ulDataLen : ulData;
         break;
   }
   return 0;
}

PBRIG_CHAR fncCellValue( brig_Table *pTable, int iCol )
{

   return (pTable->ulRecCurr <= ulDataLen && pTable->ulRecCurr > 0)?
         ((char***)(pTable->pData))[pTable->ulRecCurr-1][iCol-1] : NULL;
}

int brig_Main( int argc, char *argv[] )
{
   brig_Dialog oDlg;
   brig_Button oBtn;
   brig_Table oTable;

   long pColors1[2] = {0xaaaaaa, 0xdddddd};
   brig_Style *pStyle = brigAddStyle( 2, pColors1 );
   char* pTableData[1][3] = { {"111","222","333"} };

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oDlg.New( NULL, 200, 200, 400, 300, (PBRIG_CHAR) "A table test" );

   oTable.New( &oDlg, 20, 20, 360, 160 );

   oTable.pStyleHead = pStyle;
   oTable.pfDataSet = fncTable;
   oTable.pData = (void*) pTableData;
   oTable.AddColumn( "First", 80, fncCellValue );
   oTable.AddColumn( "Second", 100, fncCellValue );

   oBtn.New( &oDlg, 150, 220, 100, 32, (PBRIG_CHAR) "Ok" );
   oBtn.pfOnClick = fncCloseDlg;

   oDlg.Activate( 0 );
   return 0;
}

