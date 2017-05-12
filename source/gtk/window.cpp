/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Main function and windows manipulation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

#define WM_DESTROY                        2
#define WM_MOVE                           3
#define WM_SIZE                           5
#define WM_SETFOCUS                       7
#define WM_KILLFOCUS                      8
#define WM_KEYDOWN                      256    // 0x0100
#define WM_KEYUP                        257    // 0x0101
#define WM_MOUSEMOVE                    512    // 0x0200
#define WM_LBUTTONDOWN                  513    // 0x0201
#define WM_LBUTTONUP                    514    // 0x0202
#define WM_LBUTTONDBLCLK                515    // 0x0203
#define WM_RBUTTONDOWN                  516    // 0x0204
#define WM_RBUTTONUP                    517    // 0x0205

static BRIG_HANDLE hMainWindow = NULL;
static GtkWidget *pWidgAlloc;

brig_Application brigApp;

static long prevp2 = -1;

void brig_writelog( const char * sFile, const char * sTraceMsg, ... )
{
   FILE *hFile;

   if( sFile == NULL )
   {
      hFile = fopen( "ac.log", "a" );
   }
   else
   {
      hFile = fopen( sFile, "a" );
   }

   if( hFile )
   {
      va_list ap;

      va_start( ap, sTraceMsg );
      vfprintf( hFile, sTraceMsg, ap );
      va_end( ap );

      fclose( hFile );
   }

}

void brig_SetWidgetData( brig_Widget *pWidget )
{
   g_object_set_data( (GObject*) pWidget->Handle(), "obj", (gpointer) pWidget );
   //brig_writelog( NULL, "set: %lu %lu\r\n", (unsigned long)pWidget, (unsigned long)pWidget->Handle() );
}

void brig_SetWindowText( brig_Widget *pWidget, PBRIG_CHAR lpTitle )
{
   BRIG_HANDLE handle = pWidget->Handle();

   switch (pWidget->uiType) {
      case TYPE_WINDOW:
      case TYPE_DIALOG:
         gtk_window_set_title( GTK_WINDOW( handle ), lpTitle );
         break;

      case TYPE_EDIT:
         if( g_object_get_data( ( GObject * ) handle, "multi" ) )
         {
            GtkTextBuffer *buffer =
                  gtk_text_view_get_buffer( GTK_TEXT_VIEW( handle ) );
            gtk_text_buffer_set_text( buffer, lpTitle, -1 );
         }
         else
            gtk_entry_set_text( ( GtkEntry * ) handle, lpTitle );
         break;
   }
}

PBRIG_CHAR brig_GetWindowText( brig_Widget *pWidget )
{
   BRIG_HANDLE handle = pWidget->Handle();

   switch (pWidget->uiType) {
      case TYPE_WINDOW:
      case TYPE_DIALOG:
         return (PBRIG_CHAR) gtk_window_get_title( GTK_WINDOW( handle ) );

      case TYPE_EDIT:
      {
         char *cptr;

         if( g_object_get_data( ( GObject * ) handle, "multi" ) )
         {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW( handle ) );
            GtkTextIter iterStart, iterEnd;

            gtk_text_buffer_get_start_iter( buffer, &iterStart );
            gtk_text_buffer_get_end_iter( buffer, &iterEnd );
            cptr = gtk_text_buffer_get_text( buffer, &iterStart, &iterEnd, 1 );
         }
         else
            cptr = ( char * ) gtk_entry_get_text( ( GtkEntry * ) handle );

         return cptr;
      }
   }
   return NULL;
}

void brig_MoveWindow( BRIG_HANDLE handle, int iLeft, int iTop, int iWidth, int iHeight )
{

   if( g_object_get_data( ( GObject * ) handle, "window" ) )
   {
      gtk_window_move( GTK_WINDOW( handle ), iLeft, iTop );
      gtk_window_resize( GTK_WINDOW( handle ), iWidth, iHeight );
   }
   else
   {
      gpointer gObject = g_object_get_data( (GObject*) handle, "obj" );

      if( gObject )
      {
         brig_Widget * pParent = ( ( brig_Widget* ) gObject )->pParent;
         GtkWidget * hParent = pParent->Handle();
         if( hParent != pWidgAlloc )
         {
            GtkFixed * fbox = (GtkFixed *) g_object_get_data( ( GObject * )hParent, "fbox" );
            BRIG_HANDLE pbox = (BRIG_HANDLE) g_object_get_data( ( GObject * ) handle, "pbox" );
            //gtk_window_move( GTK_WINDOW( (pbox)? pbox : handle ), iLeft, iTop );
            if( iLeft >= 0 )
               gtk_fixed_move( fbox, (pbox)? pbox : handle, iLeft, iTop );
         }
         if( iWidth >= 0 )
            gtk_widget_set_size_request( handle, iWidth, iHeight );
      }

   }
}

void brig_EnableWindow( BRIG_HANDLE handle, bool bEnable )
{

   gtk_widget_set_sensitive( handle, bEnable );
}

void brig_ShowWindow( BRIG_HANDLE handle, bool bShow )
{
   if( bShow )
      gtk_widget_show( handle );
   else
      gtk_widget_hide( handle );
}

BRIG_HANDLE brig_GetActiveWindow( void )
{
   GList * pL = gtk_window_list_toplevels(), * pList;

   pList = pL;
   while( pList )
   {
      if( gtk_window_is_active( (GtkWindow*) (pList->data) ) )
        break;
      pList = g_list_next( pList );
   }
   if( !pList )
      pList = pL;

   return (GtkWidget*) ( ( pList )? pList->data : NULL );
}

BRIG_HANDLE brig_SetFocus( BRIG_HANDLE handle )
{
   GtkWidget * oldhandle = gtk_window_get_focus( (GtkWindow*) gtk_window_list_toplevels()->data );

   if( g_object_get_data( ( GObject * ) handle, "window" ) )
      gtk_window_present( (GtkWindow*) handle );
   else
      gtk_widget_grab_focus( (GtkWidget*) handle );

   return oldhandle;
}

BRIG_HANDLE brig_GetFocus( void )
{
   return gtk_window_get_focus( (GtkWindow*) gtk_window_list_toplevels()->data );
}

void brig_SetTopmost( BRIG_HANDLE handle )
{
   gtk_window_set_keep_above( (GtkWindow*) handle, TRUE );
}

void brig_RemoveTopmost( BRIG_HANDLE handle )
{
   gtk_window_set_keep_above( (GtkWindow*) handle, FALSE );
}

static long ToKey( long a, long b )
{

   if ( a == GDK_asciitilde || a == GDK_dead_tilde)
   {
      if ( b== GDK_A) 
         return (long)GDK_Atilde;
      else if ( b == GDK_a )      
         return (long)GDK_atilde;
      else if ( b== GDK_N) 
         return (long)GDK_Ntilde;
      else if ( b == GDK_n )      
         return (long)GDK_ntilde;
      else if ( b== GDK_O) 
         return (long)GDK_Otilde;
      else if ( b == GDK_o )      
         return (long)GDK_otilde;                   
   }      
   if  ( a == GDK_asciicircum || a ==GDK_dead_circumflex) 
   {
      if ( b== GDK_A) 
         return (long)GDK_Acircumflex;
      else if ( b == GDK_a )      
         return (long)GDK_acircumflex;
      else if ( b== GDK_E) 
         return (long)GDK_Ecircumflex;
      else if ( b == GDK_e )      
         return (long)GDK_ecircumflex;      
      else if ( b== GDK_I) 
         return (long)GDK_Icircumflex;
      else if ( b == GDK_i )      
         return (long)GDK_icircumflex;      
      else if ( b== GDK_O) 
         return (long)GDK_Ocircumflex;
      else if ( b == GDK_o )      
         return (long)GDK_ocircumflex;      
      else if ( b== GDK_U) 
         return (long)GDK_Ucircumflex;
      else if ( b == GDK_u )      
         return (long)GDK_ucircumflex;      
      else if ( b== GDK_C) 
         return (long)GDK_Ccircumflex;
      else if ( b == GDK_C )      
         return (long)GDK_Ccircumflex;
      else if ( b== GDK_H) 
         return (long)GDK_Hcircumflex;
      else if ( b == GDK_h )      
         return (long)GDK_hcircumflex;      
      else if ( b== GDK_J) 
         return (long)GDK_Jcircumflex;
      else if ( b == GDK_j )      
         return (long)GDK_jcircumflex;      
      else if ( b== GDK_G) 
         return (long)GDK_Gcircumflex;
      else if ( b == GDK_g )      
         return (long)GDK_gcircumflex;      
      else if ( b== GDK_S) 
         return (long)GDK_Scircumflex;
      else if ( b == GDK_s )      
         return (long)GDK_scircumflex;            
   }
   	
   if ( a == GDK_grave  || a==GDK_dead_grave ) 
   {
      if ( b== GDK_A) 
         return (long)GDK_Agrave;
      else if ( b == GDK_a )      
         return (long)GDK_agrave;
      else if ( b== GDK_E) 
         return (long)GDK_Egrave;
      else if ( b == GDK_e )      
         return (long)GDK_egrave;      
      else if ( b== GDK_I) 
         return (long)GDK_Igrave;
      else if ( b == GDK_i )      
         return (long)GDK_igrave;      
      else if ( b== GDK_O) 
         return (long)GDK_Ograve;
      else if ( b == GDK_o )      
         return (long)GDK_ograve;      
      else if ( b== GDK_U) 
         return (long)GDK_Ugrave;
      else if ( b == GDK_u )      
         return (long)GDK_ugrave;      
      else if ( b== GDK_C) 
         return (long)GDK_Ccedilla;
      else if ( b == GDK_c )      
         return (long)GDK_ccedilla ;                 
   }

   if ( a == GDK_acute  ||  a == GDK_dead_acute)
   {
     if ( b== GDK_A) 
         return (long)GDK_Aacute;
      else if ( b == GDK_a )      
         return (long)GDK_aacute;
      else if ( b== GDK_E) 
         return (long)GDK_Eacute;
      else if ( b == GDK_e )      
         return (long)GDK_eacute;      
      else if ( b== GDK_I) 
         return (long)GDK_Iacute;
      else if ( b == GDK_i )      
         return (long)GDK_iacute;      
      else if ( b== GDK_O) 
         return (long)GDK_Oacute;
      else if ( b == GDK_o )      
         return (long)GDK_oacute;      
      else if ( b== GDK_U) 
         return (long)GDK_Uacute;
      else if ( b == GDK_u )      
         return (long)GDK_uacute;      
      else if ( b== GDK_Y) 
         return (long)GDK_Yacute;
      else if ( b == GDK_y )      
         return (long)GDK_yacute;            
      else if ( b== GDK_C) 
         return (long)GDK_Cacute;
      else if ( b == GDK_c )      
         return (long)GDK_cacute;
      else if ( b== GDK_L) 
         return (long)GDK_Lacute;
      else if ( b == GDK_l )      
         return (long)GDK_lacute;      
      else if ( b== GDK_N) 
         return (long)GDK_Nacute;
      else if ( b == GDK_n )      
         return (long)GDK_nacute;      
      else if ( b== GDK_R) 
         return (long)GDK_Racute;
      else if ( b == GDK_r )      
         return (long)GDK_racute;      
      else if ( b== GDK_S) 
         return (long)GDK_Sacute;
      else if ( b == GDK_s )      
         return (long)GDK_sacute;      
      else if ( b== GDK_Z) 
         return (long)GDK_Zacute;
      else if ( b == GDK_z )      
         return (long)GDK_zacute;                  
   }
   if ( a == GDK_diaeresis|| a==GDK_dead_diaeresis)	
   {
     if ( b== GDK_A) 
         return (long)GDK_Adiaeresis;
      else if ( b == GDK_a )      
         return (long)GDK_adiaeresis;
      else if ( b== GDK_E) 
         return (long)GDK_Ediaeresis;
      else if ( b == GDK_e )      
         return (long)GDK_ediaeresis;      
      else if ( b== GDK_I) 
         return (long)GDK_Idiaeresis;
      else if ( b == GDK_i )      
         return (long)GDK_idiaeresis;      
      else if ( b== GDK_O) 
         return (long)GDK_Odiaeresis;
      else if ( b == GDK_o )      
         return (long)GDK_odiaeresis;      
      else if ( b== GDK_U) 
         return (long)GDK_Udiaeresis;
      else if ( b == GDK_u )      
         return (long)GDK_udiaeresis;      
      else if ( b== GDK_Y) 
         return (long)GDK_Ydiaeresis;
      else if ( b == GDK_y )      
         return (long)GDK_ydiaeresis;       	
   }

   return b;      
}

gboolean cb_delete_event( GtkWidget *widget, gchar* data )
{
   gpointer gObject = g_object_get_data( (GObject*) widget, "obj" );
   SYMBOL_UNUSED( data );

   if( gObject )
      return ( ( brig_Widget* ) gObject )->onEvent( WM_DESTROY, 0, 0 );

   return FALSE;
}

gint cb_signal_size( GtkWidget *widget, GtkAllocation *allocation, gpointer data )
{
   gpointer gObject = g_object_get_data( (GObject*) widget->parent->parent, "obj" );
   long int p3 = ( (long)(allocation->width) & 0xFFFF ) |
                          ( ( (unsigned long)(allocation->height) << 16 ) & 0xFFFF0000 );
   pWidgAlloc = widget->parent->parent;
   SYMBOL_UNUSED( data );

   if( gObject )
      ( ( brig_Widget* ) gObject )->onEvent( WM_SIZE, 0, p3 );
   pWidgAlloc = NULL;

   return 0;
}

void cb_signal( GtkWidget *widget, gchar* data )
{
   gpointer gObject;
   long p1, p2, p3;

   sscanf( (char*)data,"%ld %ld %ld",&p1,&p2,&p3 );
   if( !p1 )
   {
      p1 = 273;
      widget = (GtkWidget*) p3;
      p3 = 0;
   }

   gObject = g_object_get_data( (GObject*) widget, "obj" );

   if( gObject )
      ( ( brig_Widget* ) gObject )->onEvent( p1, p2, p3 );

}

void brig_SetSignal( gpointer handle, char * cSignal, long int p1, long int p2, long int p3 )
{
   char buf[25]={0};

   sprintf( buf, "%ld %ld %ld", p1, p2, p3 );
   g_signal_connect( handle, cSignal,
                      G_CALLBACK (cb_signal), g_strdup(buf) );
}

static gint cb_event( GtkWidget *widget, GdkEvent * event, gchar* data )
{
   gpointer gObject = g_object_get_data( (GObject*) widget, "obj" );

   if( gObject )
   {
      long p1, p2, p3;

      if( event->type == GDK_KEY_PRESS || event->type == GDK_KEY_RELEASE )
      {
         p1 = (event->type==GDK_KEY_PRESS)? WM_KEYDOWN : WM_KEYUP;
         p2 = ((GdkEventKey*)event)->keyval;

         if ( p2 == GDK_asciitilde  ||  p2 == GDK_asciicircum  ||  p2 == GDK_grave ||  p2 == GDK_acute ||  p2 == GDK_diaeresis || p2 == GDK_dead_acute ||	 p2 ==GDK_dead_tilde || p2==GDK_dead_circumflex || p2==GDK_dead_grave || p2 == GDK_dead_diaeresis)	
         {
            prevp2 = p2 ;
            p2=-1;
         }
         else
         {
            if ( prevp2 != -1 )
            {
               p2 = ToKey(prevp2,(long)p2);
               prevp2=-1;
            }
         }
         p3 = ( ( ((GdkEventKey*)event)->state & GDK_SHIFT_MASK )? 1 : 0 ) |
              ( ( ((GdkEventKey*)event)->state & GDK_CONTROL_MASK )? 2 : 0 ) |
              ( ( ((GdkEventKey*)event)->state & GDK_MOD1_MASK )? 4 : 0 );
      }
      else if( event->type == GDK_SCROLL )
      {
         p1 = WM_KEYDOWN;
         p2 = ( ( (GdkEventScroll*)event )->direction == GDK_SCROLL_DOWN )? 0xFF54 : 0xFF52;
         p3 = 0;
      }
      else if( event->type == GDK_BUTTON_PRESS || 
               event->type == GDK_2BUTTON_PRESS ||
               event->type == GDK_BUTTON_RELEASE )
      {
         if( ((GdkEventButton*)event)->button == 3 )
            p1 = (event->type==GDK_BUTTON_PRESS)? WM_RBUTTONDOWN : 
                 ( (event->type==GDK_BUTTON_RELEASE)? WM_RBUTTONUP : WM_LBUTTONDBLCLK );
         else
            p1 = (event->type==GDK_BUTTON_PRESS)? WM_LBUTTONDOWN : 
                 ( (event->type==GDK_BUTTON_RELEASE)? WM_LBUTTONUP : WM_LBUTTONDBLCLK );
         p2 = 0;
         p3 = ( ((unsigned long)(((GdkEventButton*)event)->x)) & 0xFFFF ) | ( ( ((unsigned long)(((GdkEventButton*)event)->y)) << 16 ) & 0xFFFF0000 );
      }
      else if( event->type == GDK_MOTION_NOTIFY )
      {
         p1 = WM_MOUSEMOVE;
         p2 = ( ((GdkEventMotion*)event)->state & GDK_BUTTON1_MASK )? 1:0;
         p3 = ( ((unsigned long)(((GdkEventMotion*)event)->x)) & 0xFFFF ) | ( ( ((unsigned long)(((GdkEventMotion*)event)->y)) << 16 ) & 0xFFFF0000 );
      }
      else if( event->type == GDK_CONFIGURE )
      {
         p2 = 0;
         if( widget->allocation.width != ((GdkEventConfigure*)event)->width ||
             widget->allocation.height!= ((GdkEventConfigure*)event)->height )
         {
            return 0;
         }
         else
         {
            p1 = WM_MOVE;
            p3 = ( ((GdkEventConfigure*)event)->x & 0xFFFF ) |
                 ( ( ((GdkEventConfigure*)event)->y << 16 ) & 0xFFFF0000 );
         }
      }
      else if( event->type == GDK_ENTER_NOTIFY || event->type == GDK_LEAVE_NOTIFY )
      {
         p1 = WM_MOUSEMOVE;
         p2 = ( ((GdkEventCrossing*)event)->state & GDK_BUTTON1_MASK )? 1:0 | 
              ( event->type == GDK_ENTER_NOTIFY )? 0x10:0;
         p3 = ( ((unsigned long)(((GdkEventCrossing*)event)->x)) & 0xFFFF ) | ( ( ((unsigned long)(((GdkEventMotion*)event)->y)) << 16 ) & 0xFFFF0000 );
      }
      else if( event->type == GDK_FOCUS_CHANGE )
      {
         p1 = ( ((GdkEventFocus*)event)->in )? WM_SETFOCUS : WM_KILLFOCUS;
         p2 = p3 = 0;
      }
      else
         sscanf( (char*)data,"%ld %ld %ld",&p1,&p2,&p3 );

      return ( ( brig_Widget* ) gObject )->onEvent( p1, p2, p3 );
   }
   return 0;
}

void brig_SetEvent( gpointer handle, char * cSignal, long int p1, long int p2, long int p3 )
{
   char buf[25]={0};

   sprintf( buf, "%ld %ld %ld", p1, p2, p3 );
   g_signal_connect( handle, cSignal,
                      G_CALLBACK (cb_event), g_strdup(buf) );
}

BRIG_HANDLE brig_InitMainWindow( PBRIG_CHAR lpAppName, PBRIG_CHAR lpTitle,
      int x, int y, int width, int height, long int lStyle, PBRIG_ICON hIcon, PBRIG_BRUSH hBrush )
{

   GtkWidget *vbox;   
   GtkFixed * box;

   hMainWindow = ( GtkWidget * ) gtk_window_new( GTK_WINDOW_TOPLEVEL );

   if( hIcon )
      gtk_window_set_icon( GTK_WINDOW( hMainWindow ), hIcon->handle  );
   gtk_window_set_title( GTK_WINDOW(hMainWindow), lpTitle );
   gtk_window_set_policy( GTK_WINDOW(hMainWindow), TRUE, TRUE, FALSE );
   gtk_window_set_default_size( GTK_WINDOW(hMainWindow), width, height );
   gtk_window_move( GTK_WINDOW(hMainWindow), x, y );

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER(hMainWindow), vbox);

   box = (GtkFixed*)gtk_fixed_new();
   gtk_box_pack_start( GTK_BOX(vbox), (GtkWidget*)box, TRUE, TRUE, 0 );

   // Set data to identify hWnd as a window, not a control
   g_object_set_data( ( GObject * ) hMainWindow, "window", ( gpointer ) 1 );

   g_object_set_data( (GObject*) hMainWindow, "vbox", (gpointer) vbox );
   g_object_set_data( (GObject*) hMainWindow, "fbox", (gpointer) box );

   gtk_widget_add_events( hMainWindow, GDK_BUTTON_PRESS_MASK |
         GDK_BUTTON_RELEASE_MASK |
         GDK_POINTER_MOTION_MASK | GDK_FOCUS_CHANGE );
   brig_SetEvent( ( gpointer ) hMainWindow, (char*)"button_press_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hMainWindow, (char*)"button_release_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hMainWindow, (char*)"motion_notify_event", 0, 0, 0 );

   g_signal_connect (G_OBJECT (hMainWindow), (char*)"delete-event",
	 	      G_CALLBACK (cb_delete_event), NULL );
   g_signal_connect (G_OBJECT (hMainWindow), (char*)"destroy",
	 	      G_CALLBACK (gtk_main_quit), NULL);

   brig_SetEvent( (gpointer)hMainWindow, (char*)"configure_event", 0, 0, 0 );
   brig_SetEvent( (gpointer)hMainWindow, (char*)"focus_in_event", 0, 0, 0 );

   g_signal_connect_after( box, (char*)"size-allocate", G_CALLBACK (cb_signal_size), NULL );

   return hMainWindow;
}

void brig_ActivateMainWindow( int bShow, HACCEL hAcceler, int iMaxMin )
{
   SYMBOL_UNUSED( bShow );
   SYMBOL_UNUSED( hAcceler );
   SYMBOL_UNUSED( iMaxMin );

   gtk_widget_show_all( hMainWindow );
   gtk_main();
}

gboolean cb_dialog_quit( GtkWidget *widget, gchar* data )
{
   gpointer gObject = g_object_get_data( (GObject*) widget, "obj" );
   SYMBOL_UNUSED( data );

   if( gObject && ( ( brig_Dialog* ) gObject )->bModal )
      gtk_main_quit();

   return FALSE;
}

BRIG_HANDLE brig_InitDialog( PBRIG_CHAR lpTitle,
      int x, int y, int width, int height, long int lStyle )
{
   BRIG_HANDLE hWnd;
   GtkWidget * vbox;
   GtkFixed  * box;

   hWnd = ( GtkWidget * ) gtk_window_new( GTK_WINDOW_TOPLEVEL );

   //if( pIcon )
   //   gtk_window_set_icon(GTK_WINDOW(hWnd), pIcon->handle  );

   gtk_window_set_title( GTK_WINDOW(hWnd), lpTitle );
   gtk_window_set_policy( GTK_WINDOW(hWnd), TRUE, TRUE, FALSE );
   gtk_window_set_default_size( GTK_WINDOW(hWnd), width, height );
   gtk_window_move( GTK_WINDOW(hWnd), x, y );

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER(hWnd), vbox);

   box = (GtkFixed*)gtk_fixed_new();
   gtk_box_pack_end( GTK_BOX(vbox), (GtkWidget*)box, TRUE, TRUE, 0 );

   // Set data to identify hWnd as a window, not a control
   g_object_set_data( ( GObject * ) hWnd, "window", ( gpointer ) 1 ); 

   g_object_set_data( (GObject*) hWnd, "fbox", (gpointer) box );

   gtk_widget_add_events( hWnd, GDK_BUTTON_PRESS_MASK |
         GDK_BUTTON_RELEASE_MASK |
         GDK_POINTER_MOTION_MASK | GDK_FOCUS_CHANGE );
   brig_SetEvent( ( gpointer ) hWnd, (char*)"button_press_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hWnd, (char*)"button_release_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hWnd, (char*)"motion_notify_event", 0, 0, 0 );

   g_signal_connect (G_OBJECT (hWnd), (char*)"delete-event",
      G_CALLBACK (cb_delete_event), NULL );
   g_signal_connect (G_OBJECT (hWnd), (char*)"destroy",
	 	      G_CALLBACK (cb_dialog_quit), NULL);

   brig_SetEvent( (gpointer)hWnd, (char*)"configure_event", 0, 0, 0 );
   brig_SetEvent( (gpointer)hWnd, (char*)"focus_in_event", 0, 0, 0 );

   g_signal_connect( box, (char*)"size-allocate", G_CALLBACK (cb_signal_size), NULL );

   return hWnd;
}

void brig_ActivateDialog( BRIG_HANDLE handle, bool bModal )
{
   gtk_widget_show_all( handle );
   if( bModal )
   {
      gtk_window_set_modal( (GtkWindow*) handle, 1 );
      if( handle->parent )
         gtk_window_set_transient_for( (GtkWindow*) handle, (GtkWindow*) (handle->parent) );

      gtk_main();
   }
}

void brig_CloseWindow( BRIG_HANDLE handle )
{
   gtk_widget_destroy( handle );
}

void brig_gtk_init( void )
{
   gtk_init( 0,0 );
   setlocale( LC_NUMERIC, "C" );
   setlocale( LC_CTYPE, "" );
}


int main( int argc, char *argv[] )
{

   brig_gtk_init();
   brig_Main( argc, argv );

   return 0;

}
