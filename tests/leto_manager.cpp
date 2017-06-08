
#include "cletocl.h"
#include "brig.h"
#include <stdio.h>

#define  DATA1_LEN    8
unsigned long ulDataLen;

LETOCONNECTION * pConnection = NULL;
unsigned int uiMode = 0;
char pTable1Data[DATA1_LEN][3][20] = { {"users","","" }, { "Tables","","" }, { "Indexes","","" }, { "Time elapsed","","" },
   { "Operations","","" }, { "KBytes sent","","" }, { "KBytes read","","" }, { "Transactions","","" } };

char (*pTable2Data)[4][20] = NULL;
char (*pTable3Data)[100] = NULL;

#if defined( WINNT ) || defined( _Windows ) || defined( __NT__ ) || defined( _WIN32 ) || defined( _WINDOWS_ ) || defined( __WINDOWS_386__ ) || defined( __WIN32__ )
   #define _OS_WIN
   #define PDYN WINAPI*
   HMODULE hLeto = NULL;
#else
   #include <dlfcn.h>
   #define PDYN *
   void* hLeto = NULL;
#endif

typedef void( PDYN LPLetoInit ) ( void );
typedef void ( PDYN LPLetoExit) ( unsigned int uiFull );
typedef LETOCONNECTION * ( PDYN LPLetoConnectionNew) ( const char * szAddr, int iPort, const char * szUser, const char * szPass, int iTimeOut );
typedef void ( PDYN LPLetoConnectionClose) ( LETOCONNECTION * pConnection );
typedef char * ( PDYN LPLetoGetServerVer) ( LETOCONNECTION * pConnection );
typedef char * ( PDYN LPLetoMgGetInfo ) ( LETOCONNECTION * pConnection );
typedef int ( PDYN LPLetoGetCmdItem )( char ** pptr, char * szDest );
typedef char * ( PDYN LPLetoMgGetUsers) ( LETOCONNECTION * pConnection, const char * szTable );
typedef char * ( PDYN LPLetoMgGetTables) ( LETOCONNECTION * pConnection, const char * szUser );

LPLetoInit fLetoInit = NULL;
LPLetoExit fLetoExit = NULL;
LPLetoConnectionNew fLetoConnectionNew = NULL;
LPLetoConnectionClose fLetoConnectionClose = NULL;
LPLetoGetServerVer fLetoGetServerVer = NULL;
LPLetoMgGetInfo fLetoMgGetInfo = NULL;
LPLetoGetCmdItem fLetoGetCmdItem = NULL;
LPLetoMgGetUsers fLetoMgGetUsers = NULL;
LPLetoMgGetTables fLetoMgGetTables = NULL;

#if defined( _OS_WIN )
static int leto_LoadLib( void )
{
   if( !hLeto )
   {
      hLeto = LoadLibraryA( "letocl.dll" );

      if( hLeto )
      {
         fLetoInit = ( LPLetoInit ) GetProcAddress( hLeto, "_LetoInit" );
         fLetoExit = ( LPLetoExit ) GetProcAddress( hLeto, "_LetoExit" );
         fLetoConnectionNew = ( LPLetoConnectionNew ) GetProcAddress( hLeto, "_LetoConnectionNew" );
         fLetoConnectionClose = ( LPLetoConnectionClose ) GetProcAddress( hLeto, "_LetoConnectionClose" );
         fLetoGetServerVer = ( LPLetoGetServerVer ) GetProcAddress( hLeto, "_LetoGetServerVer" );
         fLetoMgGetInfo = ( LPLetoMgGetInfo ) GetProcAddress( hLeto, "_LetoMgGetInfo" );
         fLetoGetCmdItem = ( LPLetoGetCmdItem ) GetProcAddress( hLeto, "_LetoGetCmdItem" );
         fLetoMgGetUsers = ( LPLetoMgGetUsers ) GetProcAddress( hLeto, "_LetoMgGetUsers" );
         fLetoMgGetTables = ( LPLetoMgGetTables ) GetProcAddress( hLeto, "_LetoMgGetTables" );
         //brig_writelog( NULL, "leto: %d %d %d %d %d %d\r\n", (hLeto)? 1:0, (fLetoInit)? 1:0, (fLetoExit)? 1:0, (fLetoConnectionNew)? 1:0, (fLetoConnectionClose)? 1:0, (fLetoGetServerVer)? 1:0 );
         fLetoInit();
         return 1;
      }
      else
         return 0;
   }
   return 1;
}

static void leto_UnLoadLib( void )
{
   if( hLeto )
   {
      FreeLibrary( hLeto );
      hLeto = NULL;
      fLetoInit = NULL;
      fLetoExit = NULL;
      fLetoConnectionNew = NULL;
      fLetoConnectionClose = NULL;
      fLetoGetServerVer = NULL;
      fLetoMgGetInfo = NULL;
      fLetoGetCmdItem = NULL;
      fLetoMgGetUsers = NULL;
      fLetoMgGetTables = NULL;
   }
}
#else

static int leto_LoadLib( void )
{

   if( !hLeto )
   {
      hLeto = dlopen( "./libletocl.so", RTLD_LAZY );
      if( hLeto )
      {
         fLetoInit = ( LPLetoInit ) dlsym( hLeto, "LetoInit" );
         fLetoExit = ( LPLetoExit ) dlsym( hLeto, "LetoExit" );
         fLetoConnectionNew = ( LPLetoConnectionNew ) dlsym( hLeto, "LetoConnectionNew" );
         fLetoConnectionClose = ( LPLetoConnectionClose ) dlsym( hLeto, "LetoConnectionClose" );
         fLetoGetServerVer = ( LPLetoGetServerVer ) dlsym( hLeto, "LetoGetServerVer" );
         fLetoMgGetInfo = ( LPLetoMgGetInfo ) dlsym( hLeto, "LetoMgGetInfo" );
         fLetoGetCmdItem = ( LPLetoGetCmdItem ) dlsym( hLeto, "LetoGetCmdItem" );
         fLetoMgGetUsers = ( LPLetoMgGetUsers ) dlsym( hLeto, "LetoMgGetUsers" );
         fLetoMgGetTables = ( LPLetoMgGetTables ) dlsym( hLeto, "LetoMgGetTables" );

         fLetoInit();
         return 1;
      }
      else
         return 0;
   }
   return 1;
}

static void leto_UnLoadLib( void )
{
   if( hLeto )
   {
      dlclose( hLeto );
      hLeto = NULL;
      fLetoInit = NULL;
      fLetoExit = NULL;
      fLetoConnectionNew = NULL;
      fLetoConnectionClose = NULL;
      fLetoGetServerVer = NULL;
      fLetoMgGetInfo = NULL;
      fLetoGetCmdItem = NULL;
      fLetoMgGetUsers = NULL;
      fLetoMgGetTables = NULL;
   }
}

#endif

unsigned long fncTable1( brig_Table *pTable, int iOp, unsigned long ulData )
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

PBRIG_CHAR fnc1CellValue( brig_Table *pTable, int iCol )
{
   char (*ptr)[3][20] = (char(*)[3][20]) pTable->pData;

   return (pTable->ulRecCurr <= ulDataLen && pTable->ulRecCurr > 0)?
         ptr[pTable->ulRecCurr-1][iCol-1] : NULL;
}

PBRIG_CHAR fnc2CellValue( brig_Table *pTable, int iCol )
{
   char (*ptr)[4][20] = (char(*)[4][20]) pTable->pData;

   return (pTable->ulRecCurr <= ulDataLen && pTable->ulRecCurr > 0)?
         ptr[pTable->ulRecCurr-1][iCol-1] : NULL;
}

PBRIG_CHAR fnc3CellValue( brig_Table *pTable, int iCol )
{
   char (*ptr)[100] = (char(*)[100]) pTable->pData;

   SYMBOL_UNUSED( iCol );
   return (pTable->ulRecCurr <= ulDataLen && pTable->ulRecCurr > 0)?
         ptr[pTable->ulRecCurr-1] : NULL;
}

void Show1( void )
{
   brig_Table * pTable = (brig_Table *) brigApp.pMainWindow->FindWidget( TYPE_TABLE );
   char * ptr;
   bool bFirst = 0;

   if( pTable->avColumns.empty() )
   {
      ulDataLen = DATA1_LEN;
      pTable->pData = (void*) pTable1Data;
      pTable->pfDataSet = fncTable1;
      pTable->AddColumn( NULL, 160, fnc1CellValue );
      pTable->AddColumn( NULL, 120, fnc1CellValue );
      pTable->AddColumn( NULL, 120, fnc1CellValue );
      bFirst = 1;
   }

   if( ( ptr = fLetoMgGetInfo( pConnection ) ) != NULL && *(ptr-1) == '+' )
   {
      fLetoGetCmdItem( &ptr, pTable1Data[0][1] ); ptr ++;   // Users current
      fLetoGetCmdItem( &ptr, pTable1Data[0][2] ); ptr ++;   // Users max
      fLetoGetCmdItem( &ptr, pTable1Data[1][1] ); ptr ++;   // Tables current
      fLetoGetCmdItem( &ptr, pTable1Data[1][2] ); ptr ++;   // Tables max

      fLetoGetCmdItem( &ptr, pTable1Data[3][2] ); ptr ++;   //
      fLetoGetCmdItem( &ptr, pTable1Data[4][2] ); ptr ++;   // Operations
      fLetoGetCmdItem( &ptr, pTable1Data[5][2] ); ptr ++;   // Kbytes sent
      fLetoGetCmdItem( &ptr, pTable1Data[6][2] ); ptr ++;   // KBytes read
   }
   if( bFirst )
      pTable->Top();
   brig_RedrawWindow( pTable );
}

void Show2( void )
{

   brig_Table * pTable = (brig_Table *) brigApp.pMainWindow->FindWidget( TYPE_TABLE );
   char * ptr;
   bool bFirst = 0;

   ulDataLen = 0;
   if( pTable->avColumns.empty() )
   {
      pTable->pData = (void*) pTable2Data;
      pTable->pfDataSet = fncTable1;
      pTable->AddColumn( NULL, 160, fnc2CellValue );
      pTable->AddColumn( NULL, 120, fnc2CellValue );
      pTable->AddColumn( NULL, 120, fnc2CellValue );
      pTable->AddColumn( NULL, 120, fnc2CellValue );
      bFirst = 1;
   }

   if( ( ptr = fLetoMgGetUsers( pConnection, NULL ) ) != NULL && *(ptr-1) == '+' )
   {
      char szData[64];
      int iUsers, i;
      long lTime;

      fLetoGetCmdItem( &ptr, szData ); ptr ++;
      iUsers = atoi( szData );
      for ( i = 0; i < iUsers; i++ )
      {
         fLetoGetCmdItem( &ptr, szData ); ptr ++;
         fLetoGetCmdItem( &ptr, pTable2Data[i][0] ); ptr ++;
         fLetoGetCmdItem( &ptr, pTable2Data[i][1] ); ptr ++;
         fLetoGetCmdItem( &ptr, pTable2Data[i][2] ); ptr ++;
         fLetoGetCmdItem( &ptr, szData ); ptr ++;
         lTime = atol( szData );
         sprintf( pTable2Data[i][3], "%02d:%02d:%02d", (lTime%86400)/3600, (lTime%3600)/60, lTime%60 );
      }
      ulDataLen = (unsigned long) iUsers;

   }
   if( bFirst )
      pTable->Top();
   brig_RedrawWindow( pTable );
}

void Show3( void )
{
   brig_Table * pTable = (brig_Table *) brigApp.pMainWindow->FindWidget( TYPE_TABLE );
   char * ptr;
   bool bFirst = 0;

   ulDataLen = 0;
   if( pTable->avColumns.empty() )
   {
      pTable->pData = (void*) pTable3Data;
      pTable->pfDataSet = fncTable1;
      pTable->AddColumn( NULL, 300, fnc3CellValue );
      bFirst = 1;
   }
   if( ( ptr = fLetoMgGetTables( pConnection, NULL ) ) != NULL && *(ptr-1) == '+' )
   {
      char szData[64];
      int iTables, i;

      fLetoGetCmdItem( &ptr, szData ); ptr ++;
      iTables = atoi( szData );
      for ( i = 0; i < iTables; i++ )
      {
         fLetoGetCmdItem( &ptr, szData ); ptr ++;
         fLetoGetCmdItem( &ptr, pTable3Data[i] ); ptr ++;
      }
      ulDataLen = (unsigned long) iTables;

   }

   if( bFirst )
      pTable->Top();
   brig_RedrawWindow( pTable );
}

void ShowInfo( void )
{
   switch( uiMode ) {

      case 1:
         Show1();
         break;

      case 2:
         Show2();
         break;

      case 3:
         Show3();
         break;

   }
}

bool btnGoOnClick( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
{

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   if( !leto_LoadLib() )
      brig_MsgStop( (PBRIG_CHAR)"Dynamic library is not loaded", (PBRIG_CHAR)"letocl.dll" );
   else
   {
      int iPort;
      PBRIG_CHAR pAddr = ((brig_Edit *) brigApp.pMainWindow->FindWidget( TYPE_EDIT ))->GetText();
      PBRIG_CHAR pPort = ((brig_Edit *) brigApp.pMainWindow->FindWidget( TYPE_EDIT,2 ))->GetText();

      iPort = atoi( pPort );
      if( !iPort )
         iPort = 2812;
      if( ( pConnection = fLetoConnectionNew( pAddr, iPort, NULL, NULL, 0 ) ) != NULL )
      {
         ((brig_Label *) brigApp.pMainWindow->FindWidget( TYPE_LABEL ))->SetText( (PBRIG_CHAR)fLetoGetServerVer( pConnection ) );
      }
      else
         ((brig_Label *) brigApp.pMainWindow->FindWidget( TYPE_LABEL ))->SetText( (PBRIG_CHAR)"No connection" );
      
      free( pPort );
      free( pAddr );

      if( pConnection )
      {
         uiMode = 1;
         Show1();
         brigSetTimer( 2000, ShowInfo );
      }
   }

   return 0;
}

bool btn1OnClick( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
{

   brig_Table * pTable;

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   if( !pConnection || uiMode == 1 )
      return 0;

   pTable = (brig_Table *) brigApp.pMainWindow->FindWidget( TYPE_TABLE );
   pTable->avColumns.clear();
   pTable->pData = NULL;
   pTable->pfDataSet = NULL;

   uiMode = 1;
   return 1;
}

bool btn2OnClick( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
{

   brig_Table * pTable;

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   if( !pConnection || uiMode == 2 )
      return 0;

   pTable = (brig_Table *) brigApp.pMainWindow->FindWidget( TYPE_TABLE );
   pTable->avColumns.clear();
   pTable->pData = NULL;
   pTable->pfDataSet = NULL;

   if( !pTable2Data )
      pTable2Data = (char(*)[4][20]) malloc( 100 * 4 * 20 );

   uiMode = 2;
   return 1;
}

bool btn3OnClick( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
{

   brig_Table * pTable;

   SYMBOL_UNUSED( pBtn );
   SYMBOL_UNUSED( wParam );
   SYMBOL_UNUSED( lParam );

   if( !pConnection || uiMode == 3 )
      return 0;

   pTable = (brig_Table *) brigApp.pMainWindow->FindWidget( TYPE_TABLE );
   pTable->avColumns.clear();
   pTable->pData = NULL;
   pTable->pfDataSet = NULL;

   if( !pTable3Data )
      pTable3Data = (char(*)[100]) malloc( 100 * 100 );

   uiMode = 3;
   return 1;
}

bool tblOnSize( brig_Widget *pTable, WPARAM wParam, LPARAM lParam )
{
   unsigned long iWidth = ((unsigned long)lParam) & 0xFFFF;
   unsigned long iHeight = ( ((unsigned long)lParam) >> 16 ) & 0xFFFF;

   SYMBOL_UNUSED( wParam );

   pTable->Move( pTable->iLeft, pTable->iTop, iWidth-(pTable->iLeft*2), iHeight-pTable->iTop-20 );
   return 0;

}


int brig_Main( int argc, char *argv[] )
{

   brig_MainWindow * pMain = new brig_MainWindow;
   brig_Edit oEditIp, oEditPort;
   brig_Label oLabelVer;
   brig_Button oBtnGo, oBtn1, oBtn2, oBtn3;
   brig_Table oTable;
   char * pIp = NULL;

   if( argc > 1 )
      pIp = argv[1];

   pMain->Create( 100, 100, 500, 400, (PBRIG_CHAR) "LetoDb Manager" );
   pMain->hFont = brigAddFont( "Georgia", 20 );

   oEditIp.Create( pMain, 12, 8, 120, 26, pIp );

   oEditPort.Create( pMain, 140, 8, 60, 26, (PBRIG_CHAR) "2812" );

   oBtnGo.Create( pMain, 204, 8, 60, 26, (PBRIG_CHAR) "Go" );
   oBtnGo.pfOnClick = btnGoOnClick;

   oLabelVer.Create( pMain, 270, 8, 200, 24, (PBRIG_CHAR)"" );

   oBtn1.Create( pMain, 12, 40, 80, 28, (PBRIG_CHAR) "Main" );
   oBtn1.pfOnClick = btn1OnClick;

   oBtn2.Create( pMain, 112, 40, 80, 28, (PBRIG_CHAR) "Users" );
   oBtn2.pfOnClick = btn2OnClick;

   oBtn3.Create( pMain, 212, 40, 80, 28, (PBRIG_CHAR) "Tables" );
   oBtn3.pfOnClick = btn3OnClick;

   oTable.Create( pMain, 12, 76, 460, 260, WS_BORDER );
   oTable.pfOnSize = tblOnSize;

   brig_SetFocus( &oEditIp );
   pMain->Activate();

   if( hLeto )
   {
      fLetoConnectionClose( pConnection );
      fLetoExit( 1 );
      leto_UnLoadLib();
   }
   if( pTable2Data )
      free( pTable2Data );
   if( pTable3Data )
      free( pTable3Data );

   return 0;
}
