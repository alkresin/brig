
#if defined( WINNT ) || defined( _Windows ) || defined( __NT__ ) || defined( _WIN32 ) || defined( _WINDOWS_ ) || defined( __WINDOWS_386__ ) || defined( __WIN32__ )
   #define HB_SOCKET_T SOCKET
   #ifndef HB_SOCKET_H_
      #include <winsock2.h>
   #endif
#else
   #define HB_SOCKET_T int
#endif

#define ULONG      unsigned long

#define HB_MAX_FILE_EXT       10
#define LETO_MAX_TAGNAME      10     /* Max len of tag name */
#define LETO_MAX_EXP         256     /* Max len of KEY/FOR expression */
#define LETO_MAX_KEY         256     /* Max len of key */

/* Field types from hbapirdd.h */

#define HB_FT_NONE            0
#define HB_FT_STRING          1     /* "C" */
#define HB_FT_LOGICAL         2     /* "L" */
#define HB_FT_DATE            3     /* "D" */
#define HB_FT_LONG            4     /* "N" */
#define HB_FT_FLOAT           5     /* "F" */
#define HB_FT_INTEGER         6     /* "I" */
#define HB_FT_DOUBLE          7     /* "B" */
#define HB_FT_TIME            8     /* "T" */
#if !( defined( HB_FT_DATETIME ) )
#define HB_FT_DATETIME        8
#endif
#define HB_FT_TIMESTAMP       9     /* "@" */
#if !( defined( HB_FT_DAYTIME ) )
#define HB_FT_DAYTIME         9
#endif
#if !( defined( HB_FT_MODTIME ) )
#define HB_FT_MODTIME         10    /* "=" */
#endif
#define HB_FT_ROWVER          11    /* "^" */
#define HB_FT_AUTOINC         12    /* "+" */
#define HB_FT_CURRENCY        13    /* "Y" */
#define HB_FT_CURDOUBLE       14    /* "Z" */
#define HB_FT_VARLENGTH       15    /* "Q" */
#define HB_FT_MEMO            16    /* "M" */
#define HB_FT_ANY             17    /* "V" */
#define HB_FT_IMAGE           18    /* "P" */
#if !( defined( HB_FT_PICTURE ) )
   #define HB_FT_PICTURE         18
#endif
#define HB_FT_BLOB            19    /* "W" */
#define HB_FT_OLE             20    /* "G" */

#define LETO_INDEX_UNIQ       1
#define LETO_INDEX_ALL        2
#define LETO_INDEX_REST       4
#define LETO_INDEX_DESC       8
#define LETO_INDEX_CUST      16
#define LETO_INDEX_ADD       32
#define LETO_INDEX_TEMP      64
#define LETO_INDEX_FILT     128
#define LETO_INDEX_USEI     256

#if !defined (DBOI_SCOPESET)
#define DBOI_SCOPETOP            39  /* Get or Set the scope top          */
#define DBOI_SCOPEBOTTOM         40  /* Get or Set the scope botto        */
#define DBOI_SCOPETOPCLEAR       41  /* Clear the scope top               */
#define DBOI_SCOPEBOTTOMCLEAR    42  /* Clear the scope bottom            */
#define DBOI_SCOPESET           109  /* set both scopes */
#define DBOI_SCOPECLEAR         110  /* clear both scopes */
#endif

#if !defined (HB_RDD_MAX_ALIAS_LEN)
#define HB_RDD_MAX_ALIAS_LEN 63
#endif

typedef struct _CDPSTRU
{
   char *      szClientCdp;
   char *      szServerCdp;
   struct _CDPSTRU * pNext;
} CDPSTRU, *PCDPSTRU;

typedef struct _LETOBUFFER_
{
   unsigned char * pBuffer;        /* Buffer for records */
   unsigned long ulBufLen;         /* allocated buffer length */
   unsigned long ulBufDataLen;     /* data length in buffer */
   unsigned int  bSetDeleted;      /* current _SET_DELETED flag */
   unsigned long ulDeciSec;        /* buffer time in 1/100 seconds */
   unsigned int  uiShoots;         /* using statistic */
} LETOBUFFER;

typedef struct _LETOFIELD
{
   char       szName[12];
   unsigned int  uiType;
   unsigned int  uiLen;
   unsigned int  uiDec;
} LETOFIELD;

typedef struct _LETOTAGINFO
{
   char *      BagName;
   char *      TagName;
   char *      KeyExpr;
   char *      ForExpr;
   unsigned int UniqueKey;
   unsigned char KeyType;
   unsigned int KeySize;
   unsigned int uiTag;
   unsigned int nField;           /* Field number for simple (one field) key expersion */
   char *      pTopScopeAsString;
   char *      pBottomScopeAsString;
   int         iTopScopeAsString;
   int         iBottomScopeAsString;
   unsigned int UsrAscend;        /* user settable ascending/descending order flag */
   unsigned int Custom;           /* user settable custom order flag */

   unsigned long ulKeyNo;         /* value for ordKeyNo(), if buffering */
   unsigned long ulKeyCount;      /* value for ordKeyCount(), if buffering */

   LETOBUFFER  Buffer;            /* seek buffer */
   unsigned int uiBufSize;        /* records count in buffer */
   unsigned int uiRecInBuf;       /* current records in buffer*/

   void *      pExtra;

   struct _LETOTAGINFO * pNext;
} LETOTAGINFO;

typedef struct _LETOTABLE
{
   unsigned long hTable;
   unsigned int  uiDriver;
   unsigned int  uiConnection;

   unsigned int  uiFieldExtent;
   LETOFIELD *   pFields;
   unsigned int  uiUpdated;
   unsigned char * pFieldUpd;          /* Pointer to updated fields array */
   unsigned int * pFieldOffset;        /* Pointer to field offset array */

   unsigned int  uiOrders;
   char *        szTags;

   char     szMemoExt[HB_MAX_FILE_EXT + 1];    /* MEMO file extension */
   unsigned char bMemoType;           /* MEMO type used in DBF memo fields */
   unsigned int  uiMemoVersion;       /* MEMO file version */

   unsigned int  fShared;             /* Shared file */
   unsigned int  fReadonly;           /* Read only file */
   unsigned int  fBof;                /* HB_TRUE if "bof" */
   unsigned int  fEof;                /* HB_TRUE if "eof" */
   unsigned int  fFound;              /* HB_TRUE if "found" */
   unsigned int  fDeleted;            /* Deleted record */
   unsigned int  fRecLocked;          /* TRUE if record is locked */

   unsigned int  uiRecordLen;         /* Size of record */
   unsigned long ulRecNo;
   unsigned long ulRecCount;          /* Count of records */
   unsigned char * pRecord;           /* Buffer of record data */

   LETOBUFFER  Buffer;                /* skip buffer */
   unsigned char *  ptrBuf;
   unsigned int  uiRecInBuf;
   signed char BufDirection;

   unsigned int  uiSkipBuf;           /* skip buffer size */
   long          lLastUpdate;         /* from dbf header: last update */
   int           iBufRefreshTime;

   LETOTAGINFO * pTagInfo;
   LETOTAGINFO * pTagCurrent;         /* current order */

   unsigned long * pLocksPos;         /* List of records locked */
   unsigned long   ulLocksMax;        /* Number of records locked */
   unsigned long   ulLocksAlloc;      /* Number of records locked (allocated) */
   unsigned int    fFLocked;          /* TRUE if file is locked */

   char *   szDataFileName;           /* Name of data file */

} LETOTABLE;

typedef struct _LETOCONNECTION_
{
   HB_SOCKET_T  hSocket;
   char *       pAddr;
   int          iPort;
   int          iTimeOut;
   char *       szPath;
   char         szVersion[32];
   unsigned int uiMajorVer;
   unsigned int uiMinorVer;
   char         szVerHarbour[80];
   char         szAccess[8];
   char         cDopcode[2];
   short int    bCrypt;
   short int    bCloseAll;
   PCDPSTRU     pCdpTable;

   short int        bTransActive;
   unsigned char *  szTransBuffer;
   unsigned long    ulTransBuffLen;
   unsigned long    ulTransDataLen;
   unsigned long    ulRecsInTrans;
   unsigned long    ulTransBlockLen;
   short int        bParseTransRec;

   short int        bRefreshCount;
   short int        bBufKeyNo;
   short int        bBufKeyCount;

   char *           pBufCrypt;
   unsigned long    ulBufCryptLen;

/* uiBufRefreshTime defines the time interval in 0.01 sec. After this 
   time is up, the records buffer must be refreshed, 100 by default */
   unsigned short      uiBufRefreshTime;

   unsigned short      uiDriver;
   unsigned short      uiMemoType;

   unsigned short      uiProto;
   unsigned short      uiTBufOffset;
   int         iUserStru;

} LETOCONNECTION;
