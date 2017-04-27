/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Common dialogs
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

extern void brig_set_modal( GtkWindow * hDlg, GtkWindow * hParent );
extern void brig_parse_color( long int ncolor, GdkColor * pColor );


void store_color( gpointer colorseldlg )
{
   GtkColorSelection *colorsel;
   GdkColor color;

   colorsel = GTK_COLOR_SELECTION( GTK_COLOR_SELECTION_DIALOG (colorseldlg)->colorsel );
   gtk_color_selection_get_current_color (colorsel, &color);

   //hb_retnl( (HB_ULONG) ( (color.red>>8) + (color.green&0xff00) + ((color.blue&0xff00)<<8) ) );
   gtk_widget_destroy( (GtkWidget*) colorseldlg );
}

long int brig_ChooseColor( long int lDefColor )
{
   GtkWidget *colorseldlg;
   GtkColorSelection *colorsel;
   GtkWidget * hParent = brig_GetActiveWindow();
   const char *cTitle = "Select color";

   colorseldlg = gtk_color_selection_dialog_new( cTitle );
   colorsel = GTK_COLOR_SELECTION( GTK_COLOR_SELECTION_DIALOG (colorseldlg)->colorsel );

   if( lDefColor >= 0 )
   {
      GdkColor color;
      brig_parse_color( lDefColor, &color );
      gtk_color_selection_set_previous_color( colorsel, &color );
      gtk_color_selection_set_current_color( colorsel, &color );
   }
   gtk_color_selection_set_has_palette (colorsel, TRUE);

   g_signal_connect( G_OBJECT (colorseldlg), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

   g_signal_connect_swapped( GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->ok_button),
                     "clicked",
                     G_CALLBACK (store_color),
                     (gpointer) colorseldlg );

   g_signal_connect_swapped( GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->cancel_button),
                             "clicked",
                             G_CALLBACK (gtk_widget_destroy),
                             (gpointer) colorseldlg );

   gtk_window_set_modal( (GtkWindow *) colorseldlg, 1 );
   gtk_window_set_transient_for( (GtkWindow *) colorseldlg, (GtkWindow *) hParent );

   gtk_widget_show( colorseldlg );
   gtk_main();

   return 0;
}

void store_font( gpointer fontseldlg )
{
   char * szFontName = (char*) gtk_font_selection_dialog_get_font_name( (GtkFontSelectionDialog*)fontseldlg );
   PangoFontDescription * hFont = pango_font_description_from_string( szFontName );
   PBRIG_FONT h = (PBRIG_FONT) malloc( sizeof(BRIG_FONT) );
   //PHB_ITEM aMetr = hb_itemArrayNew( 9 );
   //PHB_ITEM temp;

   h->type = BRIG_OBJECT_FONT;
   h->hFont = hFont;
   h->attrs = NULL;
   /*
   temp = HB_PUTHANDLE( NULL, h );
   hb_itemArrayPut( aMetr, 1, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutC( NULL, (char*) pango_font_description_get_family( hFont ) );
   hb_itemArrayPut( aMetr, 2, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutNL( NULL, 0 );
   hb_itemArrayPut( aMetr, 3, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutNL( NULL, (HB_LONG) pango_font_description_get_size( hFont ) );
   hb_itemArrayPut( aMetr, 4, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutNL( NULL, (HB_LONG) pango_font_description_get_weight( hFont ) );
   hb_itemArrayPut( aMetr, 5, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutNI( NULL, 0 );
   hb_itemArrayPut( aMetr, 6, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutNI( NULL, (HB_LONG) pango_font_description_get_style( hFont ) );
   hb_itemArrayPut( aMetr, 7, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutNI( NULL, 0 );
   hb_itemArrayPut( aMetr, 8, temp );
   hb_itemRelease( temp );

   temp = hb_itemPutNI( NULL, 0 );
   hb_itemArrayPut( aMetr, 9, temp );
   hb_itemRelease( temp );

   hb_itemReturn( aMetr );
   hb_itemRelease( aMetr );
   */
   gtk_widget_destroy( (GtkWidget*) fontseldlg );
}

void cancel_font( gpointer fontseldlg )
{
   gtk_widget_destroy( (GtkWidget *) fontseldlg );
}

PBRIG_FONT brig_ChooseFont( PBRIG_FONT hFontPrev, BRIGAPP_FONT *pbf )
{
   GtkWidget *fontseldlg;
   GtkFontSelection *fontsel;
   const char *cTitle = "Select Font";

   fontseldlg = gtk_font_selection_dialog_new( cTitle );
   fontsel = GTK_FONT_SELECTION( GTK_FONT_SELECTION_DIALOG (fontseldlg)->fontsel );
/*
   if( hb_pcount() > 0 && !HB_ISNIL(1) )
   {
      PHB_ITEM pObj = hb_param( 1, HB_IT_OBJECT );
      const char * ptr = hb_itemGetCPtr( GetObjectVar( pObj, "NAME" ) );
      int height = hb_itemGetNI( GetObjectVar( pObj, "HEIGHT" ) );
      int weight = hb_itemGetNI( GetObjectVar( pObj, "WEIGHT" ) );
      int italic = hb_itemGetNI( GetObjectVar( pObj, "ITALIC" ) );
      char szFont[256];

      sprintf( szFont, "%s %s %s %d", ptr, ( (weight<700)? "" : "BOLD" ), ( (italic==0)? "" : "ITALIC" ), height );

      gtk_font_selection_dialog_set_font_name( GTK_FONT_SELECTION_DIALOG(fontseldlg),
           szFont );
   }
*/
   g_signal_connect( G_OBJECT (fontseldlg), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

   g_signal_connect_swapped( GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (fontseldlg)->ok_button),
                     "clicked",
                     G_CALLBACK (store_font),
                     (gpointer) fontseldlg );

   g_signal_connect_swapped( GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (fontseldlg)->cancel_button),
                             "clicked",
                             G_CALLBACK (cancel_font),
                             (gpointer) fontseldlg );

   gtk_widget_show( fontseldlg );

   gtk_window_set_modal( (GtkWindow *) fontseldlg, 1 );

   gtk_main();

   return NULL;
}
