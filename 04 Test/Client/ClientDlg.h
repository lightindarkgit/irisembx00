
// ClientDlg.h : ͷ�ļ�
//

#pragma once
#include "NetClient.h"
#include "HistoryEditEx.h"
#include <sstream>

const int g_recvBuf = 1024*1024;           // �������Ĵ�С1M


// CClientDlg �Ի���
class CClientDlg : public CDialogEx
{
// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

    ~CClientDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
    NetClient           _connAdmin;

    CHistoryEditEx		_HistoryEditEx;			// ��ʷ״̬��ʾ

private:
    HANDLE _event;                //�¼�
    HANDLE _handle;               //�߳̾��
    // char _bufFrame[g_recvBuf];
    char* _bufFrame;

    static unsigned __stdcall DataControlAnalyze(void *pParam);

private:
    void DispInfo(std::string msg);             // ��ʾ��ʷ��Ϣ

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
