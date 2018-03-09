
// NetServerAppDlg.h : ͷ�ļ�
//

#pragma once
#include "NetServer.h"

// CNetServerAppDlg �Ի���
class CNetServerAppDlg : public CDialogEx
{
// ����
public:
	CNetServerAppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NETSERVERAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnCountMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnReset();
private:
	std::shared_ptr<NetServer> _pNetServer;               //������������
	int _beatCount;
public:
	afx_msg void OnBnClickedBtnAddDepart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	void ParseNetData(char* buf, int len);
};
