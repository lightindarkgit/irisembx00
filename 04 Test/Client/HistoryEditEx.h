#if !defined(AFX_HISTORYEDITEX_H__316D998E_D748_4351_B387_D7EE3A39E4E0__INCLUDED_)
#define AFX_HISTORYEDITEX_H__316D998E_D748_4351_B387_D7EE3A39E4E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistoryEditEx.h : header file
//

struct CLogFont : public LOGFONT
{
    CLogFont( LONG fontHeight      = -8, 
              LONG fontWeight      = FW_NORMAL,
              UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
              LPCSTR faceName      = _T("MS Sans Serif" )
            )
    {
        const int size = sizeof(*this);

        memset( this, 0, size );

        lfHeight         = fontHeight;
        lfWeight         = fontWeight;
        lfPitchAndFamily = pitchAndFamily;

        _ASSERT( LF_FACESIZE > lstrlen( faceName ) );

        lstrcpy( lfFaceName, faceName );
    }

    // Note: No need for CLogFont& operator =(const CLogFont& lf) {...}
    //       We let the compiler handle it...

    void lfFaceNameSet( LPCSTR faceName )
    {
        _ASSERT( faceName );

        if ( !faceName ) return;

        _ASSERT( LF_FACESIZE > lstrlen( faceName ) );

        lstrcpy( lfFaceName, faceName );        
    }

};

/////////////////////////////////////////////////////////////////////////////
// CHistoryEditEx window

class CHistoryEditEx : public CEdit
{
// Construction
public:
	CHistoryEditEx();

// Attributes
public:

// Operations
public:
    void  AppendString (CString str);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryEditEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHistoryEditEx();

    COLORREF bkColor  ( COLORREF );    
    COLORREF textColor( COLORREF );

    COLORREF bkColor()   const { return m_crBkColor;   }   
    COLORREF textColor() const { return m_crTextColor; }

    void setFont( const LOGFONT* lpLogFont );

    void setFont( LONG fontHeight      = -8, 
                  LONG fontWeight      = FW_NORMAL,
                  UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
                  LPCSTR faceName      = _T("MS Sans Serif" ) );

    void setMaxLine (int nMaxLine);
    int  getMaxLine () const { return m_nStoredLine; }

    // Generated message map functions
protected:
	//{{AFX_MSG(CHistoryEditEx)
		// NOTE - the ClassWizard will add and remove member functions here.
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

    // Customize your brush
    //
    virtual BOOL CreateBrushType();

private:
    int      m_nLineCount;
    int      m_nStoredLine;
    CBrush   m_brBkGround;
    COLORREF m_crBkColor;
    COLORREF m_crTextColor;

    CFont* m_pCFont;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYEDITEX_H__316D998E_D748_4351_B387_D7EE3A39E4E0__INCLUDED_)
