/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Standard Widgets creation and manipulation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

extern void brig_SetEvent( gpointer handle, char * cSignal, long int p1, long int p2, long int p3 );

/* -------- Label ---------
 */
BRIG_HANDLE brig_CreateLabel( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle )
{

   GtkWidget *hCtrl, *hLabel;
   GtkFixed *box;

   if( ( ulStyle & SS_OWNERDRAW ) == SS_OWNERDRAW )
      hCtrl = gtk_drawing_area_new();
   else
   {
      hCtrl = gtk_event_box_new(  );
      hLabel = gtk_label_new( lpCaption );
      gtk_container_add( GTK_CONTAINER( hCtrl ), hLabel );
      g_object_set_data( ( GObject * ) hCtrl, "label", ( gpointer ) hLabel );
      if( ulExStyle && WS_EX_TRANSPARENT )
      {
         gtk_event_box_set_visible_window( GTK_EVENT_BOX( hCtrl ), 0 );
      }

      if( !( ulStyle & SS_CENTER ) )
         gtk_misc_set_alignment( GTK_MISC( hLabel ),
               ( ulStyle & SS_RIGHT ) ? 1 : 0, 0 );
   }
   box = ( GtkFixed * ) g_object_get_data( ( GObject * ) hParentWindow, "fbox" );
   if( box )
      gtk_fixed_put( box, hCtrl, x, y );
   gtk_widget_set_size_request( hCtrl, nWidth, nHeight );

   if( ( ulStyle & SS_OWNERDRAW ) == SS_OWNERDRAW )
   {
      brig_SetEvent( ( gpointer ) hCtrl, "expose_event", WM_PAINT, 0, 0 );
   }
   return hCtrl;

}

/* -------- Button ---------
 */

BRIG_HANDLE brig_CreateButton( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_CHAR lpCaption )
{
/*
   CreateButton( hParentWindow, nButtonID, nStyle, x, y, nWidth, nHeight,
                 cCaption )
*/
   GtkWidget *hCtrl, *img;
   GtkFixed *box;

   if( ( ulStyle & 0xf ) == BS_AUTORADIOBUTTON )
   {
      //GSList *group = ( GSList * ) HB_PARHANDLE( 2 );
      //hCtrl = gtk_radio_button_new_with_label( group, lpCaption );
      //group = gtk_radio_button_get_group( ( GtkRadioButton * ) hCtrl );
      //HB_STOREHANDLE( group, 2 );
   }
   else if( ( ulStyle & 0xf ) == BS_AUTOCHECKBOX )
      hCtrl = gtk_check_button_new_with_label( lpCaption );
   else if( ( ulStyle & 0xf ) == BS_GROUPBOX )
      hCtrl = gtk_frame_new( lpCaption );
   else
      hCtrl = gtk_button_new_with_mnemonic( lpCaption );

   box = ( GtkFixed * ) g_object_get_data( ( GObject * ) hParentWindow, "fbox" );
   if( box )
      gtk_fixed_put( box, hCtrl, x, y );
   gtk_widget_set_size_request( hCtrl, nWidth, nHeight );

   /*
   if( hBtn )
   {
      LONG_PTR hProc;
      SetWindowLongPtr( hBtn, GWLP_USERDATA, NULL );
      hProc = SetWindowLongPtr( hBtn, GWLP_WNDPROC, ( LONG_PTR ) s_BtnProc );
      if( !wpOrigBtnProc )
         wpOrigBtnProc = ( WNDPROC ) hProc;
   }
   */
   return hCtrl;

}

bool brig_CheckBtnGet( BRIG_HANDLE handle )
{
   return gtk_toggle_button_get_active( ( GtkToggleButton * ) handle );

}

void brig_CheckBtnSet( BRIG_HANDLE handle, bool bValue )
{
   gtk_toggle_button_set_active( ( GtkToggleButton * ) handle, bValue );
}

bool brig_RadioBtnGet( BRIG_HANDLE handle )
{
   return gtk_toggle_button_get_active( ( GtkToggleButton * ) handle );
}

void brig_RadioBtnSet( BRIG_HANDLE handle, bool bValue )
{
   gtk_toggle_button_set_active( ( GtkToggleButton * ) handle, bValue );
}


/* -------- Edit --------- 
 */

BRIG_HANDLE brig_CreateEdit( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle )
{
   GtkWidget *hCtrl;

   if( ulStyle & ES_MULTILINE )
   {
      hCtrl = gtk_text_view_new(  );
      g_object_set_data( ( GObject * ) hCtrl, "multi", ( gpointer ) 1 );
      if( ulStyle & ES_READONLY )
         gtk_text_view_set_editable( ( GtkTextView * ) hCtrl, 0 );
      gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW(hCtrl), GTK_WRAP_WORD_CHAR);
   }
   else
   {
      hCtrl = gtk_entry_new(  );
      if( ulStyle & ES_PASSWORD )
         gtk_entry_set_visibility( ( GtkEntry * ) hCtrl, FALSE );
   }

   GtkFixed *box = ( GtkFixed * ) g_object_get_data( ( GObject * ) hParentWindow, "fbox" );
   if( box )
      gtk_fixed_put( box, hCtrl, x, y );
   gtk_widget_set_size_request( hCtrl, nWidth, nHeight );

   if( lpCaption )
   {
      if( ulStyle & ES_MULTILINE )
      {
         GtkTextBuffer *buffer =
               gtk_text_view_get_buffer( GTK_TEXT_VIEW( hCtrl ) );
         gtk_text_buffer_set_text( buffer, lpCaption, -1 );
      }
      else
         gtk_entry_set_text( ( GtkEntry * ) hCtrl, lpCaption );
   }

   gtk_widget_add_events( hCtrl, GDK_BUTTON_PRESS_MASK );
   brig_SetEvent( ( gpointer ) hCtrl, "button_press_event", 0, 0, 0 );

   //all_signal_connect( ( gpointer ) hCtrl );

   /*
   if( hEdit )
   {
      if( lpCaption )
      {
         PBRIG_WCHAR wcCaption = brig_str2WC( lpCaption );
         SendMessage( hEdit, WM_SETTEXT, 0, ( LPARAM ) wcCaption );
         brig_free( wcCaption );
      }
      SetWindowLongPtr( hEdit, GWLP_USERDATA, NULL );
      wpOrigEditProc = ( WNDPROC ) SetWindowLongPtr( hEdit,
            GWLP_WNDPROC, ( LONG_PTR ) s_EditProc );
   }
   */
   return hCtrl;

}

/* -------- Panel --------- 
 */

BRIG_HANDLE brig_CreatePanel( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{
   BRIG_HANDLE hPanel;

   GtkWidget *vbox, *hbox;
   GtkWidget *vscroll = NULL, *hscroll = NULL;
   GtkFixed *box, *fbox;
   GObject *handle;

   fbox = ( GtkFixed * ) gtk_fixed_new();

   hbox = gtk_hbox_new( FALSE, 0 );
   vbox = gtk_vbox_new( FALSE, 0 );

   //if( ( ulStyle & SS_OWNERDRAW ) == SS_OWNERDRAW )
      hPanel = gtk_drawing_area_new();
   //else
   //   hPanel = gtk_toolbar_new();

   gtk_box_pack_start( GTK_BOX( hbox ), vbox, TRUE, TRUE, 0 );
   /*if( ulStyle & WS_VSCROLL )
   {
      GtkObject *adjV;
      adjV = gtk_adjustment_new( 0.0, 0.0, 101.0, 1.0, 10.0, 10.0 );
      vscroll = gtk_vscrollbar_new( GTK_ADJUSTMENT( adjV ) );
      gtk_box_pack_end( GTK_BOX( hbox ), vscroll, FALSE, FALSE, 0 );

      //temp = HB_PUTHANDLE( NULL, adjV );
      //SetObjectVar( pObject, "_HSCROLLV", temp );
      //hb_itemRelease( temp );

      SetWindowObject( ( GtkWidget * ) adjV, pObject );
      set_signal( ( gpointer ) adjV, "value_changed", WM_VSCROLL, 0, 0 );
   }

   gtk_box_pack_start( GTK_BOX( vbox ), (GtkWidget*)fbox, TRUE, TRUE, 0 );
   gtk_fixed_put( fbox, hPanel, 0, 0 );
   if( ulStyle & WS_HSCROLL )
   {
      GtkObject *adjH;
      adjH = gtk_adjustment_new( 0.0, 0.0, 101.0, 1.0, 10.0, 10.0 );
      hscroll = gtk_hscrollbar_new( GTK_ADJUSTMENT( adjH ) );
      gtk_box_pack_end( GTK_BOX( vbox ), hscroll, FALSE, FALSE, 0 );

      //temp = HB_PUTHANDLE( NULL, adjH );
      //SetObjectVar( pObject, "_HSCROLLH", temp );
      //hb_itemRelease( temp );

      SetWindowObject( ( GtkWidget * ) adjH, pObject );
      set_signal( ( gpointer ) adjH, "value_changed", WM_HSCROLL, 0, 0 );
   }*/

   box = ( GtkFixed * ) g_object_get_data( ( GObject * ) handle, "fbox" );
   if( box )
   {
      gtk_fixed_put( box, ( GtkWidget * ) hbox, x, y );
      gtk_widget_set_size_request( ( GtkWidget * ) hbox, nWidth, nHeight );
      if( vscroll )
         nWidth -= 12;
      if( hscroll )
         nHeight -= 12;
      gtk_widget_set_size_request( hPanel, nWidth, nHeight );
   }
   
   g_object_set_data( ( GObject * ) hPanel, "fbox", ( gpointer ) fbox );

   //temp = HB_PUTHANDLE( NULL, hbox );
   //SetObjectVar( pObject, "_HBOX", temp );
   //hb_itemRelease( temp );

   GTK_WIDGET_SET_FLAGS( hPanel, GTK_CAN_FOCUS );
   //if( ( ulStyle & SS_OWNERDRAW ) == SS_OWNERDRAW )
      brig_SetEvent( ( gpointer ) hPanel, "expose_event", WM_PAINT, 0, 0 );
   gtk_widget_add_events( hPanel, GDK_BUTTON_PRESS_MASK |
         GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK |
         GDK_LEAVE_NOTIFY_MASK );
   brig_SetEvent( ( gpointer ) hPanel, "button_press_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hPanel, "button_release_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hPanel, "enter_notify_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hPanel, "leave_notify_event", 0, 0, 0 );
   //all_signal_connect( ( gpointer ) hPanel );

   /*
   if( hPanel )
   {
      SetWindowLongPtr( hPanel, GWLP_USERDATA, NULL );
   }
   */
   return hPanel;
}

/* -------- QButton --------- 
 */

BRIG_HANDLE brig_CreateQButton( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight )
{
   BRIG_HANDLE hQButton;

   GtkFixed *box;

   hQButton = gtk_drawing_area_new(  );

   box = ( GtkFixed * ) g_object_get_data( ( GObject * ) hParentWindow, "fbox" );
   if( box )
   {
      gtk_fixed_put( box, hQButton, x, y );
      gtk_widget_set_size_request( hQButton, nWidth, nHeight );
   }
   brig_SetEvent( ( gpointer ) hQButton, "expose_event", WM_PAINT, 0, 0 );
   GTK_WIDGET_SET_FLAGS( hQButton, GTK_CAN_FOCUS );
   gtk_widget_add_events( hQButton, GDK_BUTTON_PRESS_MASK |
         GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK |
         GDK_LEAVE_NOTIFY_MASK );
   brig_SetEvent( ( gpointer ) hQButton, "button_press_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hQButton, "button_release_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hQButton, "enter_notify_event", 0, 0, 0 );
   brig_SetEvent( ( gpointer ) hQButton, "leave_notify_event", 0, 0, 0 );
   //all_signal_connect( ( gpointer ) hQButton );

   /*
   if( hQButton )
   {
      SetWindowLongPtr( hQButton, GWLP_USERDATA, NULL );
   }
   */
   return hQButton;
}