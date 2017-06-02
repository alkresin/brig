
#include "cletocl.h"
#include "brig.h"
#include <stdio.h>

LETOCONNECTION * pConnection = NULL;

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

LPLetoInit fLetoInit = NULL;
LPLetoExit fLetoExit = NULL;
LPLetoConnectionNew fLetoConnectionNew = NULL;
LPLetoConnectionClose fLetoConnectionClose = NULL;
LPLetoGetServerVer fLetoGetServerVer = NULL;

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
   }
}

#endif


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
      //if( ( pConnection = fLetoConnectionNew( "199.10.11.5", 2812, NULL, NULL, 0 ) ) != NULL )
      {
         ((brig_Label *) brigApp.pMainWindow->FindWidget( TYPE_LABEL ))->SetText( (PBRIG_CHAR)fLetoGetServerVer( pConnection ) );
      }
      else
         ((brig_Label *) brigApp.pMainWindow->FindWidget( TYPE_LABEL ))->SetText( (PBRIG_CHAR)"No connection" );
      
      free( pPort );
      free( pAddr );
      //fLetoConnectionClose( pConnection );
      //fLetoExit( 1 );
      //leto_UnLoadLib();
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

   pMain->Create( 100, 100, 500, 300, (PBRIG_CHAR) "LetoDb Manager" );

   oEditIp.Create( pMain, 12, 8, 120, 26 );

   oEditPort.Create( pMain, 140, 8, 60, 26 );

   oBtnGo.Create( pMain, 204, 8, 60, 26, (PBRIG_CHAR) "Go" );
   oBtnGo.pfOnClick = fncOnClick;

   oLabelVer.Create( pMain, 270, 8, 200, 24, "" );

   oBtn1.Create( pMain, 12, 40, 80, 28, (PBRIG_CHAR) "Main" );

   oBtn2.Create( pMain, 112, 40, 80, 28, (PBRIG_CHAR) "Users" );

   oBtn3.Create( pMain, 212, 40, 80, 28, (PBRIG_CHAR) "Tables" );

   oTable.Create( pMain, 12, 76, 460, 160, WS_BORDER );

   pMain->Activate();

   return 0;
}
