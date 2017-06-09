
#include "brig.h"
#include <stdio.h>

//std::vector<PBRIG_CHAR*> avTable;
#define  DATA_LEN    10
unsigned long ulDataLen = DATA_LEN;

brig_Dialog oDlg;

void fncPosChanged( brig_Table *pTable )
{
   brig_Edit * pEdit;
   BRIG_CHAR szInfo[48];

   pEdit = (brig_Edit *) oDlg.FindWidget( TYPE_EDIT );
   sprintf( szInfo, "Current record number: %lu", pTable->ulRecCurr );
   pEdit->SetText( szInfo );
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

   brig_Edit oEdit;
   brig_Table oTable;
   brig_Splitter oSplit;
   vector<brig_Widget*> avLeft, avRight;

   long pColors1[2] = {0xaaaaaa, 0xdddddd};

   char pTableData[DATA_LEN][3][4] = { {"a11","a22","a33"}, {"b11","b22","b33"}, {"c11","c22","c33"},
      {"d11","d22","d33"}, {"e11","e22","e33"}, {"f11","f22","f33"}, {"g11","g22","g33"},
      {"h11","h22","h33"}, {"i11","i22","i33"}, {"j11","j22","j33"} };

   SYMBOL_UNUSED( argc );
   SYMBOL_UNUSED( argv );

   oDlg.Create( NULL, 200, 200, 400, 280, (PBRIG_CHAR) "A table test" );

   oTable.Create( &oDlg, 20, 20, 360, 160, WS_VSCROLL );

   oTable.lTextColor = 0xff0000;
   oTable.lSelTColor = 0x000000;
   oTable.pStyleHead = brigAddStyle( 2, pColors1 );
   oTable.pStyle = brigAddStyle( 0xdddddd );
   oTable.pStyleSel = brigAddStyle( 0xeeeeee );
   oTable.pfDataSet = fncTable;
   oTable.pData = (void*) pTableData;
   oTable.pfOnPosChanged = fncPosChanged;
   oTable.AddColumn( (PBRIG_CHAR)"", 28, fncCellValue );
   oTable.avColumns[0]->pStyle = oTable.avColumns[0]->pStyleSel = brigAddStyle( 2, pColors1, 3 );
   oTable.AddColumn( (PBRIG_CHAR)"First", 80, fncCellValue );
   oTable.AddColumn( (PBRIG_CHAR)"Second", 80, fncCellValue );
   oTable.AddColumn( (PBRIG_CHAR)"Third", 100, fncCellValue );

   avLeft.push_back( &oTable );
   avRight.push_back( &oEdit );

   oSplit.Create( &oDlg, 20, 180, 360, 4, &avLeft, &avRight );
   //oSplit.lColorInside = 0xaaaaaa;
   oEdit.Create( &oDlg, 20, 184, 360, 40, (PBRIG_CHAR) "" );

   oDlg.Activate( 0 );

   return 0;
}

