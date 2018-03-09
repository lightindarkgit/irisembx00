
// ClientDlg.h : 头文件
//

#pragma once
#include "NetClient.h"
#include "HistoryEditEx.h"
#include <sstream>

const int g_recvBuf = 1024*1024;           // 缓冲区的大小1M


// CClientDlg 对话框
class CClientDlg : public CDialogEx
{
// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

    ~CClientDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
    NetClient           _connAdmin;

    CHistoryEditEx		_HistoryEditEx;			// 历史状态显示

private:
    HANDLE _event;                //事件
    HANDLE _handle;               //线程句柄
    // char _bufFrame[g_recvBuf];
    char* _bufFrame;

    static unsigned __stdcall DataControlAnalyze(void *pParam);

private:
    void DispInfo(std::string msg);             // 显示历史信息

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnConnect();

    CString _strHostIp;
    int _nHostPort;
    CString _strToken;
    int _nOption;
    int _nResult;
    int _nErrCode;

    afx_msg void OnBnClickedBtnDisconnect();
    afx_msg void OnBnClickedBtnSend();
};
