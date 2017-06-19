/*
 * Brig - a ligthweight C++ GUI framework
 *
 *
 * Copyright 2017 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

class brig_Label;
class brig_Edit;
class brig_Combo;
class brig_Tab;
class brig_Panel;
class brig_QButton;
class brig_Table;
class brig_Splitter;

extern BRIG_HANDLE brig_CreateLabel( brig_Label *pLabel, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_CHAR lpCaption, unsigned long ulExStyle );
extern BRIG_HANDLE brig_CreateButton( brig_Widget *pWidget, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_CHAR lpCaption );
extern BRIG_HANDLE brig_CreateEdit( brig_Edit *pEdit, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle,
          PBRIG_CHAR lpCaption, unsigned long ulExStyle );
extern BRIG_HANDLE brig_CreatePanel( brig_Panel *pPanel, int iWidgId,
          int x, int y, int nWidth, int nHeight );
extern BRIG_HANDLE brig_CreateQButton( brig_QButton *pQBtn, int iWidgId,
          int x, int y, int nWidth, int nHeight );

extern BRIG_HANDLE brig_CreateTable( brig_Table *pTable, int iWidgId,
          int x, int y, int nWidth, int nHeight, unsigned long ulStyle, PBRIG_FONT hFont, unsigned int iRows );
extern void brig_table_OnVScroll( brig_Table *pTable, WPARAM wParam );
extern void brig_table_OnHScroll( brig_Table *pTable, WPARAM wParam );
extern void brig_table_OnWheel( brig_Table *pTable, WPARAM wParam );

extern BRIG_HANDLE brig_CreateCombo( brig_Combo *pCombo, int iWidgId,
          int x, int y, int nWidth, int nHeight, int iDisplay, bool bEdit );
extern void brig_ComboSetArray( brig_Widget *pWidget, char **pArray, int iLen );
extern int brig_ComboGetValue( brig_Widget *pWidget );
extern void brig_ComboSetValue( brig_Widget *pWidget, int iSelected );

extern BRIG_HANDLE brig_CreateTab( brig_Tab *pTab, int iWidgId,
          int x, int y, int nWidth, int nHeight );
extern void brig_TabAddPage( brig_Tab *pTab, int iPage, PBRIG_CHAR lpName );
extern void brig_TabShowPage( brig_Tab *pTab, int iPage, bool bShow );

extern BRIG_HANDLE brig_CreateSplitter( brig_Splitter *pSplitter, int iWidgId,
          int x, int y, int nWidth, int nHeight );

extern int brig_GetScrollPos( brig_Widget *pWidget, bool bVertical );
extern void brig_SetScrollPos( brig_Widget *pWidget, bool bVertical, int iPos );
extern int brig_GetScrollRange( brig_Widget *pWidget, bool bVertical, int *pMinPos, int *pMaxPos );
extern void brig_SetScrollRange( brig_Widget *pWidget, bool bVertical, int iMinPos, int iMaxPos );
extern int brig_GetScrollPage( brig_Widget *pWidget, bool bVertical );
extern void brig_SetScrollPage( brig_Widget *pWidget, bool bVertical, int iPage );


class brig_Label : public brig_Widget
{
public:
   brig_Label();
   BRIG_HANDLE Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle = 0, unsigned long ulExStyle = 0 );
};

class brig_Edit : public brig_Widget
{
public:

   brig_Edit();
   BRIG_HANDLE Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption = NULL, unsigned long ulStyle = WS_BORDER, unsigned long ulExStyle = 0 );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );
};

class brig_GroupBox : public brig_Widget
{
public:

   brig_GroupBox();

   BRIG_HANDLE Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption );
};

class brig_Button : public brig_Widget
{
public:

   brig_Button();

   BRIG_HANDLE Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption, unsigned long ulStyle = 0 );
   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   brig_fnc_mess pfOnClick;
};

class brig_CheckButton : public brig_Widget
{
public:

   brig_CheckButton();

   BRIG_HANDLE Create( brig_Container *pParent,
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

   BRIG_HANDLE Create( brig_RadioGroup *pGroup,
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

   BRIG_HANDLE Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, int iDisplay = 2, bool bEdit = 0, char **pArray = NULL, int iLen = 0 );
   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );
   void Set( char **pArray, int iLen );

   int  GetValue( void );
   void SetValue( int iSelected );
   brig_fnc_mess_1 pfOnChange;

};


typedef struct BRIG_TABPAGE_STRU
{
 
   int  iFirst;
   int  iLast;

} BRIG_TABPAGE, *PBRIG_TABPAGE;

class brig_Tab : public brig_Container
{
public:

   brig_Tab();

   BRIG_HANDLE Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight );
   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );
   void StartPage( PBRIG_CHAR lpName );
   void EndPage( void );

   brig_fnc_mess_1 pfOnChange;
   vector<BRIG_TABPAGE> avPages;
   int iCurrPage;

};

class brig_Panel : public brig_Container
{
public:

   brig_Panel();
   ~brig_Panel();

   BRIG_HANDLE Create( brig_Container *pParent, int x, int y, int nWidth, int nHeight, brig_Style *ps = NULL );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   brig_fnc_paint pfOnPaint;

   brig_Style *pStyle;

};

class brig_QButton : public brig_Widget
{
public:

   brig_QButton();
   ~brig_QButton();

   BRIG_HANDLE Create( brig_Container *pParent,
          int x, int y, int nWidth, int nHeight, PBRIG_CHAR lpCaption = NULL,
          brig_Style *psNormal = NULL, brig_Style *psOver = NULL, brig_Style *psPress = NULL );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   brig_fnc_paint pfOnPaint;
   brig_fnc_mess pfOnClick;

   PBRIG_CHAR szCaption;
   short int iState;
   long int lBackClr1, lBackClr2;
   PBRIG_BRUSH hBrush1, hBrush2;
   PBRIG_PEN hPen;
   brig_Style *pStyleNormal, *pStyleOver, *pStylePress;
};

class brig_Splitter : public brig_Widget
{
public:

   brig_Splitter();
   ~brig_Splitter();

   BRIG_HANDLE Create( brig_Container *pParent, int x, int y, int nWidth, int nHeight,
         vector<brig_Widget*> *pArrLeft, vector<brig_Widget*> *pArrRight );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );

   brig_fnc_paint pfOnPaint;

   bool bVertical;
   bool bCaptured;
   int  iOldPos;
   vector<brig_Widget*> avLeft;
   vector<brig_Widget*> avRight;
   long int lColorInside;
   PBRIG_PEN hPenEdge, hPenInside;
   PBRIG_CURSOR hCursor;

};

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
   unsigned int  iAlign;
   brig_fnc_column pfValue;
   brig_Style  *pStyle, *pStyleSel, *pStyleHead, *pStyleFoot;

} BRIG_TCOL, *PBRIG_TCOL;


class brig_Table : public brig_Widget
{
public:

   brig_Table();

   BRIG_HANDLE Create( brig_Container *pParent, int x, int y, int nWidth, int nHeight,
         unsigned long ulStyle = 0, PBRIG_FONT hFont = NULL, unsigned int iRows = 0 );

   bool onEvent( UINT message, WPARAM wParam, LPARAM lParam );
   void AddColumn( PBRIG_CHAR szHead, unsigned int iWidth, brig_fnc_column pfValue, unsigned int iAlign = DT_LEFT );

   void Down( void );
   void Up( void );
   void Top( void );
   void Bottom( void );

   brig_fnc_paint pfOnPaint;
   bool (*pfOnDblClick)( brig_Table *pTable );
   void (*pfOnPosChanged)( brig_Table *pTable );
   unsigned long (*pfDataSet)( brig_Table *pTable, int iOp, unsigned long ulData );

   void *pData;                // A Data source - an array, data table handle, ...
   unsigned long ulRecCurr;    // A current record number ( in a data source )
   unsigned long ulRecCurrOld; // A previous value of a current record number ( in a data source )
   unsigned long ulRecFirst;   // A data source record number, displayed in a first row of a table
   unsigned int  uiColFirst;

   vector<PBRIG_TCOL> avColumns;
   unsigned int uiHeadRows, uiFootRows;
   unsigned int uiTextHeight;  // A height of a text in a row
   unsigned int uiRowCount;    // A number of visible rows
   unsigned int uiRowSel;      // A selected row in a table
   unsigned int uiColumnSel;   // A selected column in a table. If 0, the columns aren't selected.
   unsigned int uiClientWidth, uiClientHeight;
   bool bBodyOnly;

   unsigned int pPadding[4], pHeadPadding[4];
   PBRIG_PEN pPenSep, pPenHdr;
   unsigned long lSepColor, lSelTColor, lHeadColor;
   brig_Style  *pStyle, *pStyleSel, *pStyleCell, *pStyleHead, *pStyleFoot;
};

extern void brig_RadioGroupSet( brig_RadioGroup *pGroup, int iSelected );
