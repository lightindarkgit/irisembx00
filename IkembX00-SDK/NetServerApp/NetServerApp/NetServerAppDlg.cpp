
// NetServerAppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NetServerApp.h"
#include "NetServerAppDlg.h"
#include "afxdialogex.h"
#include "EncodedNetData.h"
#include "ServerSelect.h"
#include "../../Common/Exectime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
	public:
		CAboutDlg();

		// �Ի�������
		enum { IDD = IDD_ABOUTBOX };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

		// ʵ��
	protected:
		DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	Exectime etm(__FUNCTION__);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	Exectime etm(__FUNCTION__);
	CDialogEx::DoDataExchange(pDX);
}

	BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


	// CNetServerAppDlg �Ի���



	CNetServerAppDlg::CNetServerAppDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CNetServerAppDlg::IDD, pParent)
{
	Exectime etm(__FUNCTION__);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetServerAppDlg::DoDataExchange(CDataExchange* pDX)
{
	Exectime etm(__FUNCTION__);
	CDialogEx::DoDataExchange(pDX);
}

	BEGIN_MESSAGE_MAP(CNetServerAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_RESET, &CNetServerAppDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_ADD_DEPART, &CNetServerAppDlg::OnBnClickedBtnAddDepart)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_COUNT_MESSAGE, OnCountMessage)
END_MESSAGE_MAP()


	// CNetServerAppDlg ��Ϣ�������

BOOL CNetServerAppDlg::OnInitDialog()
{
	Exectime etm(__FUNCTION__);
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	_pNetServer = std::make_shared<ServerSelect>();
	_pNetServer->Init("127.0.0.1",9999); //ע�⣺IP��ַδ����
	_pNetServer->StartThread();
	SetTimer(1, g_timeBeat, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CNetServerAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	Exectime etm(__FUNCTION__);
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

LRESULT CNetServerAppDlg::OnCountMessage(WPARAM wParam,LPARAM lParam)
{
	Exectime etm(__FUNCTION__);
	ParseNetData(NULL, 0);
	return 0;
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNetServerAppDlg::OnPaint()
{
	Exectime etm(__FUNCTION__);
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CNetServerAppDlg::OnQueryDragIcon()
{
	Exectime etm(__FUNCTION__);
	return static_cast<HCURSOR>(m_hIcon);
}



void CNetServerAppDlg::OnBnClickedBtnReset()
{
	Exectime etm(__FUNCTION__);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char buf[12];
	EncodedNetData::EncodedRemoteReset(buf);
	_pNetServer->Send(buf,12);
}


void CNetServerAppDlg::OnBnClickedBtnAddDepart()
{
	Exectime etm(__FUNCTION__);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//sizeof(HeadData1)+vod.size()+vod.size()*612+2
	//12=ͷ+У��+����������4��+612*1
	GUID pd;
	GUID fd;
	char buf[12+4+612];
	OfflineData od;
	std::vector<OfflineData> vecOfd;
	memset(od.DevSn,0x31,64);
	od.DevSn[62] = 0;
	od.EyeFlag = 0;
	CoCreateGuid(&od.Pid);
	CoCreateGuid(&od.Fid);

	for (int num = 0; num < 512; num++)
	{
		od.FeatureData[num] = num;
	}
	vecOfd.push_back(od);
	EncodedNetData::EncodedAddFeature(buf,vecOfd,628);
	_pNetServer->Send(buf, 628);
}


void CNetServerAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	Exectime etm(__FUNCTION__);
	//����������������
	char buf[12];
	EncodedNetData::EncodedKeepLive(buf);
	_pNetServer->Send(buf,12);

	CDialogEx::OnTimer(nIDEvent);
}


void CNetServerAppDlg::OnDestroy()
{
	Exectime etm(__FUNCTION__);
	CDialogEx::OnDestroy();

	//�ͷ���Դ 
	KillTimer(1);
}


void CNetServerAppDlg::ParseNetData(char* buf, int len)
{
	Exectime etm(__FUNCTION__);
	//����
	_beatCount++;

	//��ʾ������
	CString sBeat;
	sBeat.Format(_T("%d"), _beatCount);
	GetDlgItem(IDC_EDIT_BEAT)->SetWindowText(sBeat);
}

