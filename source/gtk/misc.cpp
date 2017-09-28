/*
 * Brig - a ligthweight C++ GUI framework
 * GTK version
 * Standard Widgets creation and manipulation
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "brig.h"

#define CHUNK_LEN 1024

int brig_RunConsoleApp( PBRIG_CHAR sCommand, PBRIG_CHAR sOutFileName )
{ 
    /* Ensure that output of command does interfere with stdout */
    fflush(stdin);
    FILE *cmd_file = (FILE *) popen( sCommand, "r" );
    FILE *hOut;
    char buf[CHUNK_LEN];
    int bytes_read, iOutExist = 0, iExitCode;

    if( !cmd_file )
        return -1;

    if( sOutFileName )
    {
       hOut = fopen( sOutFileName, "w" );
       iOutExist = 1;
    }

    do
    {
        bytes_read = fread( buf, sizeof(char), CHUNK_LEN, cmd_file );
        if( iOutExist )
           fwrite( buf, 1, bytes_read, hOut );
    } while (bytes_read == CHUNK_LEN);
 
    iExitCode = pclose(cmd_file);
    if( iOutExist )
       fclose( hOut );

    return iExitCode;
}
