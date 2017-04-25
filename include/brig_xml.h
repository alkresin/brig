/*
 * Brig - a ligthweight C++ GUI framework
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#ifndef BRIG_XML_H_
#define BRIG_XML_H_

#include "brig_defs.h"

typedef struct BRIG_XMLITEM_STRU
{
 
   PBRIG_CHAR  szTitle;
   int           iType;
   PBRIG_CHAR   szText;

   std::vector<struct BRIG_XMLITEM_STRU*> avItems;
   std::map<std::string,char*>amAttr;

} BRIG_XMLITEM, *PBRIG_XMLITEM;

#endif // BRIG_XML_H_