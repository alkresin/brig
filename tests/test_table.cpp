
#include "brig.h"
#include <stdio.h>

//std::vector<PBRIG_CHAR*> avTable;
#define  DATA_LEN    10
unsigned long ulDataLen = DATA_LEN;

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
   if( iCol == 1 )
      return NULL;
   else
   {
      char (*ptr)[3][4] = (char(*)[3][4]) pTable->pData;
      return (pTable->ulRecCurr <= ulDataLen && pTable->ulRecCurr > 0)?
            ptr[pTable->ulRecCurr-1][iCol-2] : NULL;
   }
}

int brig_Main( int argc, char *argv[] )
{
   brig_Dialog oDlg;
   brig_Button oBtn;
   brig_Table oTable;

   long pColors1[2] = {0xaaaaaa, 0xdddddd};

   char pTableData[DATA_LEN][3][4] = { {"a11","a22","a33"}, {"b11","b22","b33"}, {"c11","c22","c33"},
      {"d11","d22","d33"}, {"e11","e22","e33"}, {"f11","f22","f33"}, {"g11","g22","g33"},
      {"h11","h22","h33"}, {"i11","i22","i33"}, {"j11","j22","j33"} };

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oDlg.New( NULL, 200, 200, 400, 280, (PBRIG_CHAR) "A table test" );

   oTable.New( &oDlg, 20, 20, 360, 160, WS_VSCROLL );

   oTable.lTextColor = 0xff0000;
   oTable.lSelTColor = 0x000000;
   oTable.pStyleHead = brigAddStyle( 2, pColors1 );
   oTable.pStyle = brigAddStyle( 0xdddddd );
   oTable.pStyleSel = brigAddStyle( 0xeeeeee );
   oTable.pfDataSet = fncTable;
   oTable.pData = (void*) pTableData;
   oTable.AddColumn( "", 28, fncCellValue );
   oTable.avColumns[0]->pStyle = oTable.avColumns[0]->pStyleSel = brigAddStyle( 2, pColors1, 3 );
   oTable.AddColumn( "First", 80, fncCellValue );
   oTable.AddColumn( "Second", 80, fncCellValue );
   oTable.AddColumn( "Third", 100, fncCellValue );

   oBtn.New( &oDlg, 150, 200, 100, 32, (PBRIG_CHAR) "Ok" );
   oBtn.pfOnClick = fncCloseDlg;

   oDlg.Activate( 0 );

   return 0;
}

