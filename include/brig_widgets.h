/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

extern BRIG_HANDLE brig_CreateLabel( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_CHAR lpCaption, unsigned long ulExStyle );
extern BRIG_HANDLE brig_CreateButton( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_CHAR lpCaption );
extern BRIG_HANDLE brig_CreateEdit( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle );
extern BRIG_HANDLE brig_CreatePanel( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight );
extern BRIG_HANDLE brig_CreateQButton( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight );
extern BRIG_HANDLE brig_CreateTable( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle );
extern BRIG_HANDLE brig_CreateCombo( BRIG_HANDLE hParentWindow, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, char **pArray, int iLen );
extern void brig_ComboSetArray( BRIG_HANDLE hCombo, char **pArray, int iLen );
extern int brig_GetValue( BRIG_HANDLE hCombo );
extern void brig_SetValue( BRIG_HANDLE hCombo, int iSelected );


class brig_Label : public brig_Widget
{
public:
   brig_Label();
   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle = 0, unsigned long ulExStyle = 0 );
};

class brig_Edit : public brig_Widget
{
public:

   brig_Edit();
   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption = NULL, unsigned long ulStyle = WS_BORDER, unsigned long ulExStyle = 0 );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );
};

class brig_GroupBox : public brig_Widget
{
public:

   brig_GroupBox();

   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption );
};

class brig_Button : public brig_Widget
{
public:

   brig_Button();

   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle = 0 );
   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   brig_fnc_mess pfOnClick;
};

class brig_CheckButton : public brig_Widget
{
public:

   brig_CheckButton();

   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle = 0 );
   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   bool GetValue( void );
   void SetValue( bool bValue );

   brig_fnc_mess pfOnClick;

};

class brig_RadioButton;

class brig_RadioGroup
{
public:

   brig_RadioGroup();
   ~brig_RadioGroup();

   void Begin( brig_Container *pParent,
          int x = 0, int y = 0, int nWidth = 0, int nHeight = 0, PBRIG_CHAR lpCaption = NULL );
   void End( int iSelected );

   int  GetValue( void );
   void SetValue( int iSelected );

   brig_Widget *pParent;
   brig_GroupBox * pBox;
   vector<brig_RadioButton*> avButtons;
   int iValue;

};

class brig_RadioButton : public brig_Widget
{
public:

   brig_RadioButton();

   BRIG_HANDLE New( brig_RadioGroup *pGroup,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle = 0 );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );
   bool GetValue( void );

   brig_fnc_mess pfOnClick;
   brig_RadioGroup *pGroup;
   int iOrder;

};

class brig_Combo : public brig_Widget
{
public:

   brig_Combo();

   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle = 0, char **pArray = NULL, int iLen = 0 );
   void Set( char **pArray, int iLen );

   int  GetValue( void );
   void SetValue( int iSelected );

};


class brig_Panel : public brig_Container
{
public:

   brig_Panel();
   ~brig_Panel();

   BRIG_HANDLE New( brig_Container *pParent, int x, int y, int nWidth, int nHeight, brig_Style *ps = NULL );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   brig_fnc_paint pfOnPaint;

   brig_Style *pStyle;

};

class brig_QButton : public brig_Widget
{
public:

   brig_QButton();
   ~brig_QButton();

   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption = NULL,
          brig_Style *psNormal = NULL, brig_Style *psOver = NULL, brig_Style *psPress = NULL );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   brig_fnc_paint pfOnPaint;
   brig_fnc_mess pfOnClick;

   PBRIG_CHAR szCaption;
   short int iState;
   long int lBackClr1, lBackClr2;
   PBRIG_BRUSH hBrush1, hBrush2;
   brig_Style *pStyleNormal, *pStyleOver, *pStylePress;
};

class brig_Table;

#define  TDS_COUNT   1
#define  TDS_TOP     2
#define  TDS_BOTTOM  3
#define  TDS_FORWARD 4
#define  TDS_BACK    5
#define  TDS_BOF     6
#define  TDS_EOF     7
#define  TDS_RECNO   8
#define  TDS_GOTO    9

typedef PBRIG_CHAR (*brig_fnc_column)( brig_Table *pTable, int iCol );

typedef struct BRIG_TCOLUMN_STRU
{
 
   PBRIG_CHAR    szHead, szFoot;
   unsigned int  iWidth;
   brig_fnc_column pfValue;
   brig_Style  *pStyle, *pStyleSel, *pStyleHead, *pStyleFoot;

} BRIG_TCOL, *PBRIG_TCOL;


class brig_Table : public brig_Widget
{
public:

   brig_Table();

   BRIG_HANDLE New( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle = 0 );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );
   void AddColumn( PBRIG_CHAR szHead, int iWidth, brig_fnc_column pfValue );

   brig_fnc_paint pfOnPaint;
   unsigned long (*pfDataSet)( brig_Table *pTable, int iOp, unsigned long ulData );

   void *pData;                // A Data source - an array, data table handle, ...
   unsigned long ulRecCurr;    // A current record number ( in a data source )
   unsigned long ulRecFirst;   // A data source record number, displayed in a first row of a table
   unsigned int  uiColFirst;

   vector<PBRIG_TCOL> avColumns;
   unsigned int uiHeadRows, uiFootRows;
   unsigned int uiTextHeight;  // A height of a text in a row
   unsigned int uiRowCount;    // A number of visible rows
   unsigned int uiRowSel;      // A selected row in a table
   unsigned int uiClientWidth, uiClientHeight;

   unsigned int pPadding[4], pHeadPadding[4];
   PBRIG_PEN pPenSep, pPenHdr;
   unsigned long lSepColor, lSelTColor, lHeadColor;
   brig_Style  *pStyle, *pStyleSel, *pStyleHead, *pStyleFoot;
};

extern void brig_RadioGroupSet( brig_RadioGroup *pGroup, int iSelected );
