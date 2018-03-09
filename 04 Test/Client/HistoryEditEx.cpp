// HistoryEditEx.cpp : implementation file
//
#include "stdafx.h"
#include "HistoryEditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryEditEx

CHistoryEditEx::CHistoryEditEx() : m_pCFont(0)
{
    // Use system colors for defaults
    //
    m_crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
    //m_crBkColor   = ::GetSysColor( COLOR_WINDOW     );
	m_crBkColor   = ::GetSysColor( COLOR_BACKGROUND );

    // The default brush type: SOLID
    //
    //CreateBrushType();

    m_nStoredLine = 30;      // 默认值为最多保存30行数据

    m_nLineCount = 1;
}


CHistoryEditEx::~CHistoryEditEx()
{
    if ( m_pCFont ) delete m_pCFont; 
    m_brBkGround.DeleteObject();
}


BEGIN_MESSAGE_MAP(CHistoryEditEx, CEdit)
	//{{AFX_MSG_MAP(CHistoryEditEx)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHistoryEditEx message handlers
//
//  Purpose:
//    Appends a text string to the history buffer.
//
//  Returns:
//    None.
//
void CHistoryEditEx::AppendString (CString str)
{
  CString   strBuffer;    // current contents of edit control
  CString   strCount;
  CString   strTrimBuffer;
  int       nFirstCount;

  if (m_nStoredLine>1)
  {
	  // Append string
	  strCount.Format("%d.", m_nLineCount);

	  GetWindowText (strBuffer);
	  if (!strBuffer.IsEmpty())
		  strBuffer += "\r\n";
	  else strBuffer = "";

	  strBuffer += strCount;

	  strBuffer += str;

	  if (m_nLineCount>m_nStoredLine)
	  {
		  nFirstCount = strBuffer.Find('\n');
		  strTrimBuffer = strBuffer.Right(strBuffer.GetLength()-nFirstCount-1);
		  SetWindowText (strTrimBuffer);
	  }
	  else 
	  {
		  SetWindowText (strBuffer);
	  }

	  // Scroll the edit control
	  LineScroll (GetLineCount(), 0);

	  m_nLineCount++;
  }
  else if (m_nStoredLine==1)
  {
	  strBuffer = str;
	  SetWindowText(strBuffer);
  }

}


COLORREF CHistoryEditEx::bkColor( COLORREF crColor )
{
    _ASSERT(::IsWindow(m_hWnd)); 

    COLORREF crPrevBkColor = m_crBkColor;

    m_crBkColor = crColor;
    
    m_brBkGround.DeleteObject();

    CreateBrushType();

    Invalidate();

    return crPrevBkColor;
}

COLORREF CHistoryEditEx::textColor( COLORREF crColor )
{
    _ASSERT(::IsWindow(m_hWnd)); 
    
    COLORREF crPrevTextColor = m_crTextColor;
    
    m_crTextColor = crColor;
    
    Invalidate();
    
    return crPrevTextColor;
}


HBRUSH CHistoryEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here

    pDC->SetTextColor( m_crTextColor );

	// m_crBkColor   = ::GetSysColor( COLOR_MENU );
    pDC->SetBkColor( m_crBkColor );
    pDC->SetBkMode(OPAQUE);
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return (HBRUSH)m_brBkGround;
}

BOOL CHistoryEditEx::CreateBrushType()
{
    return m_brBkGround.CreateSolidBrush( m_crBkColor );
}



void CHistoryEditEx::setFont( const LOGFONT* lpLogFont )
{
    _ASSERT( lpLogFont ); // logfont is not defined!!!

    if ( !lpLogFont ) return;

    if ( m_pCFont ) delete m_pCFont;
    
    m_pCFont = new CFont;
    m_pCFont->CreateFontIndirect( lpLogFont );

    SetFont( m_pCFont );
}


void CHistoryEditEx::setFont( LONG fontHeight,      // = -8                         
                       LONG fontWeight,      // = FW_NORMAL                  
                       UCHAR pitchAndFamily, // = DEFAULT_PITCH | FF_DONTCARE
                       LPCSTR faceName )     // = _T("MS Sans Serif")        
{
    if ( m_pCFont ) 
    {
        delete m_pCFont;
    }
    m_pCFont = new CFont;

    const CLogFont lf( fontHeight, 
                       FW_NORMAL,
                       pitchAndFamily,
                       faceName
                     );

    m_pCFont->CreateFontIndirect( &lf );

    SetFont( m_pCFont );
}

void CHistoryEditEx::setMaxLine (int nMaxLine)
{
	if (nMaxLine>=1)
		m_nStoredLine = nMaxLine;
}
