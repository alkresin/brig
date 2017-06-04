
#include "cletocl.h"
#include "brig.h"
#include <stdio.h>

#define  DATA1_LEN    8

LETOCONNECTION * pConnection = NULL;
unsigned int uiMode = 0;
char pTable1Data[DATA1_LEN][3][20] = { {"users","","" }, { "Tables","","" }, { "Indexes","","" }, { "Time elapsed","","" },
   { "Operations","","" }, { "KBytes sent","","" }, { "KBytes read","","" }, { "Transactions","","" } };

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

LPLetoInit fLetoInit = NULL;
LPLetoExit fLetoExit = NULL;
LPLetoConnectionNew fLetoConnectionNew = NULL;
LPLetoConnectionClose fLetoConnectionClose = NULL;
LPLetoGetServerVer fLetoGetServerVer = NULL;
LPLetoMgGetInfo fLetoMgGetInfo = NULL;
LPLetoGetCmdItem fLetoGetCmdItem = NULL;

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
   }
}

#endif

unsigned long fncTable1( brig_Table *pTable, int iOp, unsigned long ulData )
{
   switch( iOp ) {

      case TDS_COUNT:
         return DATA1_LEN;

      case TDS_TOP:
         pTable->ulRecCurr = 1;
         break;

      case TDS_BOTTOM:
         pTable->ulRecCurr = DATA1_LEN;
         break;

      case TDS_FORWARD:
         pTable->ulRecCurr += ulData;
         if( pTable->ulRecCurr > DATA1_LEN )
            pTable->ulRecCurr = DATA1_LEN;
         break;

      case TDS_BACK:
         pTable->ulRecCurr -= ulData;
         if( pTable->ulRecCurr < 1 )
            pTable->ulRecCurr = 1;
         break;

      case TDS_BOF:
         return ( pTable->ulRecCurr == 1 );

      case TDS_EOF:
         return ( pTable->ulRecCurr == DATA1_LEN );

      case TDS_RECNO:
         return pTable->ulRecCurr;

      case TDS_GOTO:
         pTable->ulRecCurr = (ulData>DATA1_LEN)? DATA1_LEN : ulData;
         break;
   }
   return 0;
}

PBRIG_CHAR fncCellValue( brig_Table *pTable, int iCol )
{
   char (*ptr)[3][20] = (char(*)[3][20]) pTable->pData;
   return (pTable->ulRecCurr <= DATA1_LEN && pTable->ulRecCurr > 0)?
         ptr[pTable->ulRecCurr-1][iCol-1] : NULL;
}

void Show1( void )
{
   brig_Table * pTable = (brig_Table *) brigApp.pMainWindow->FindWidget( TYPE_TABLE );
   char * ptr;

   if( pTable->avColumns.empty() )
   {
      pTable->pData = (void*) pTable1Data;
      pTable->pfDataSet = fncTable1;
      pTable->AddColumn( NULL, 160, fncCellValue );
      pTable->AddColumn( NULL, 120, fncCellValue );
      pTable->AddColumn( NULL, 120, fncCellValue );
   }

   if( ( ptr = fLetoMgGetInfo( pConnection ) ) != NULL && *(ptr-1) == '+' )
   {
      fLetoGetCmdItem( &ptr, pTable1Data[0][1] ); ptr ++;   // Users current
      fLetoGetCmdItem( &ptr, pTable1Data[0][2] ); ptr ++;   // Users max
      fLetoGetCmdItem( &ptr, pTable1Data[1][1] ); ptr ++;   // Tables current
      fLetoGetCmdItem( &ptr, pTable1Data[1][2] ); ptr ++;   // Tables max

      fLetoGetCmdItem( &ptr, pTable1Data[3][1] ); ptr ++;   //
      fLetoGetCmdItem( &ptr, pTable1Data[4][1] ); ptr ++;   // Operations
      fLetoGetCmdItem( &ptr, pTable1Data[5][1] ); ptr ++;   // Kbytes sent
      fLetoGetCmdItem( &ptr, pTable1Data[6][1] ); ptr ++;   // KBytes read
   }
   brig_RedrawWindow( pTable );
}

bool fncOnClick( brig_Widget *pBtn, WPARAM wParam, LPARAM lParam )
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

      uiMode = 1;
      Show1();
      brigSetTimer( 2000, Show1 );
   }

   return 0;
}

int brig_Main( int argc, char *argv[] )
{

   brig_MainWindow * pMain = new brig_MainWindow;
   brig_Edit oEditIp, oEditPort;
   brig_Label oLabelVer;
   brig_Button oBtnGo, oBtn1, oBtn2, oBtn3;
   brig_Table oTable;

   pMain->Create( 100, 100, 500, 400, (PBRIG_CHAR) "LetoDb Manager" );

   oEditIp.Create( pMain, 12, 8, 120, 26 );

   oEditPort.Create( pMain, 140, 8, 60, 26 );

   oBtnGo.Create( pMain, 204, 8, 60, 26, (PBRIG_CHAR) "Go" );
   oBtnGo.pfOnClick = fncOnClick;

   oLabelVer.Create( pMain, 270, 8, 200, 24, (PBRIG_CHAR)"" );

   oBtn1.Create( pMain, 12, 40, 80, 28, (PBRIG_CHAR) "Main" );

   oBtn2.Create( pMain, 112, 40, 80, 28, (PBRIG_CHAR) "Users" );

   oBtn3.Create( pMain, 212, 40, 80, 28, (PBRIG_CHAR) "Tables" );

   oTable.Create( pMain, 12, 76, 460, 260, WS_BORDER );

   pMain->Activate();

   if( hLeto )
   {
      fLetoConnectionClose( pConnection );
      fLetoExit( 1 );
      leto_UnLoadLib();
   }

   return 0;
}
