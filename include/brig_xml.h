/*
 * Brig - a ligthweight C++ GUI framework
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#ifndef BRIG_XML_H_
#define BRIG_XML_H_

#include "brig_defs.h"
#include "brig_funcs.h"

typedef struct BRIG_XMLITEM_STRU
{
 
   PBRIG_CHAR  szTitle;
   int           iType;
   PBRIG_CHAR   szText;

   std::vector<struct BRIG_XMLITEM_STRU*> avItems;
   std::map<std::string,char*>amAttr;

} BRIG_XMLITEM, *PBRIG_XMLITEM;

extern PBRIG_XMLITEM brigxml_GetDoc( PBRIG_CHAR szSource, unsigned long ulLen = 0 );
extern int brigxml_Error( void );
extern void brigxml_Release( PBRIG_XMLITEM pItem );
extern PBRIG_CHAR brigxml_GetAttr( PBRIG_XMLITEM pItem, PBRIG_CHAR szKey );
extern void brigxml_SetAttr( PBRIG_XMLITEM pItem, PBRIG_CHAR szKey, PBRIG_CHAR szVal );
extern PBRIG_XMLITEM brigxml_First( PBRIG_XMLITEM pParent, int *iNum = NULL );
extern PBRIG_XMLITEM brigxml_Last( PBRIG_XMLITEM pParent, int *iNum = NULL );
extern PBRIG_XMLITEM brigxml_Next( PBRIG_XMLITEM pParent, int *iNum );
extern PBRIG_XMLITEM brigxml_Prev( PBRIG_XMLITEM pParent, int *iNum );

#endif // BRIG_XML_H_