/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

static BRIG_CHAR szVersion[24];

static bool fncChoic( brig_Table *pTable )
{
   ((brig_Dialog*)(pTable->pParent))->pResult = (void*) pTable->ulRecCurr;
   ((brig_Dialog*)(pTable->pParent))->Close();

   return 1;
}

static bool fncCloseDlg( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
{
   brig_Table *pTable = (brig_Table *)((brig_Dialog*)(pBtn->pParent))->avWidgets[0];

   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   ((brig_Dialog*)(pBtn->pParent))->pResult = (void*) pTable->ulRecCurr;
   ((brig_Dialog*)(pBtn->pParent))->Close();
   return 1;
}

static unsigned long fncTable( brig_Table *pTable, int iOp, unsigned long ulData )
{

   std::vector<char*> *pList = (std::vector<char*> *) pTable->pData;

   switch( iOp ) {

      case TDS_COUNT:
         return (*pList).size();

      case TDS_TOP:
         pTable->ulRecCurr = 1;
         break;

      case TDS_BOTTOM:
         pTable->ulRecCurr = (*pList).size();
         break;

      case TDS_FORWARD:
         pTable->ulRecCurr += ulData;
         if( pTable->ulRecCurr > (*pList).size() )
            pTable->ulRecCurr = (*pList).size();
         break;

      case TDS_BACK:
         pTable->ulRecCurr -= ulData;
         if( pTable->ulRecCurr < 1 )
            pTable->ulRecCurr = 1;
         break;

      case TDS_BOF:
         return ( pTable->ulRecCurr == 1 );

      case TDS_EOF:
         return ( pTable->ulRecCurr == (*pList).size() );

      case TDS_RECNO:
         return pTable->ulRecCurr;

      case TDS_GOTO:
         pTable->ulRecCurr = (ulData>(*pList).size())? (*pList).size() : ulData;
         break;
   }
   return 0;
}

static PBRIG_CHAR fncCellValue( brig_Table *pTable, int iCol )
{
   std::vector<char*> *pList = (std::vector<char*> *) pTable->pData;
   SYMBOL_UNUSED( iCol );

   return (*pList)[pTable->ulRecCurr-1];
}

int brigChoice( std::vector<char*> &pList, PBRIG_CHAR lpTitle, unsigned int iLeft,
      unsigned int iTop, unsigned int iWidth, unsigned int iHeight )
{
   brig_Dialog oDlg;
   brig_Button oBtn;
   brig_Table oTable;

   brig_Style * pStyle = brigAddStyle( 0xdddddd );
   brig_Style * pStyleSel = brigAddStyle( 0xeeeeee );

   oDlg.New( NULL, iLeft, iTop, iWidth, iHeight, lpTitle, 0 );
   if( brigApp.pMainWindow && brigApp.pMainWindow->hFont )
      oDlg.hFont = brigApp.pMainWindow->hFont;

   oTable.New( &oDlg, 0, 0, iWidth, iHeight - 80 );

   oTable.pStyle = pStyle;
   oTable.pStyleSel = pStyleSel;

   oTable.pData = (void*) &pList;
   oTable.pfDataSet = fncTable;
   oTable.AddColumn( NULL, iWidth, fncCellValue );
   oTable.pfOnDblClick = fncChoic;

   oBtn.New( &oDlg, (iWidth-100)/2, iHeight-72, 100, 32, (PBRIG_CHAR) "Ok" );
   oBtn.pfOnClick = fncCloseDlg;

   brig_SetFocus( &oTable );
   oDlg.Activate();

   brigDelStyle( pStyleSel );
   brigDelStyle( pStyle );

   return (int) oDlg.pResult;
}

PBRIG_CHAR brig_Version( void )
{
   sprintf( szVersion, "Brig %d.%d Build %d", BRIG_VER_MAJOR, BRIG_VER_MINOR, BRIG_VER_BUILD );
   return szVersion;
}

int brig_Version( int iVer )
{
   switch( iVer )
   {
      case BRIG_VERSION_MAJOR:     return BRIG_VER_MAJOR;
      case BRIG_VERSION_MINOR:     return BRIG_VER_MINOR;
      case BRIG_VERSION_BUILD:     return BRIG_VER_BUILD;
   }
   return 0;
}
