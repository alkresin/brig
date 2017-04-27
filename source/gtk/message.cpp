/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Messagebox functions
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

static int MessageBox( const char * cMsg, const char * cTitle, int message_type, int button_type )
{
   GtkWidget * dialog;
   int result;

   dialog = gtk_message_dialog_new( GTK_WINDOW( brig_GetActiveWindow() ),
         (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
         (GtkMessageType) message_type,
         (GtkButtonsType) button_type,
         cMsg );
   if( *cTitle )
      gtk_window_set_title( GTK_WINDOW(dialog), cTitle );
   gtk_window_set_position( GTK_WINDOW(dialog), GTK_WIN_POS_CENTER );
   gtk_window_set_policy( GTK_WINDOW(dialog), TRUE, TRUE, TRUE );

   result = gtk_dialog_run( GTK_DIALOG(dialog) );
   gtk_widget_destroy( dialog );
   return result;
}

void brig_MsgInfo( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   MessageBox( sText, sTitle, GTK_MESSAGE_INFO, GTK_BUTTONS_OK );
}

void brig_MsgStop( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   MessageBox( sText, sTitle, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE );
}

int brig_MsgOkCancel( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   return ( MessageBox( sText, sTitle, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL ) == GTK_RESPONSE_OK );
}

int brig_MsgYesNo( PBRIG_CHAR sText, PBRIG_CHAR sTitle )
{
   return ( MessageBox( sText, sTitle, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO ) == GTK_RESPONSE_YES );
}
