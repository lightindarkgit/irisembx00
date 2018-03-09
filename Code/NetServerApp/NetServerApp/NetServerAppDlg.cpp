
// NetServerAppDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
	public:
		CAboutDlg();

		// 对话框数据
		enum { IDD = IDD_ABOUTBOX };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

		// 实现
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


	// CNetServerAppDlg 对话框



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


	// CNetServerAppDlg 消息处理程序

BOOL CNetServerAppDlg::OnInitDialog()
{
	Exectime etm(__FUNCTION__);
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	_pNetServer = std::make_shared<ServerSelect>();
	_pNetServer->Init("127.0.0.1",9999); //注意：IP地址未处理
	_pNetServer->StartThread();
	SetTimer(1, g_timeBeat, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNetServerAppDlg::OnPaint()
{
	Exectime etm(__FUNCTION__);
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNetServerAppDlg::OnQueryDragIcon()
{
	Exectime etm(__FUNCTION__);
	return static_cast<HCURSOR>(m_hIcon);
}



void CNetServerAppDlg::OnBnClickedBtnReset()
{
	Exectime etm(__FUNCTION__);
	// TODO:  在此添加控件通知处理程序代码
	char buf[12];
	EncodedNetData::EncodedRemoteReset(buf);
	_pNetServer->Send(buf,12);
}


void CNetServerAppDlg::OnBnClickedBtnAddDepart()
{
	Exectime etm(__FUNCTION__);
	// TODO:  在此添加控件通知处理程序代码
	//sizeof(HeadData1)+vod.size()+vod.size()*612+2
	//12=头+校验+特征个数（4）+612*1
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
	//处理心跳——发送
	char buf[12];
	EncodedNetData::EncodedKeepLive(buf);
	_pNetServer->Send(buf,12);

	CDialogEx::OnTimer(nIDEvent);
}


void CNetServerAppDlg::OnDestroy()
{
	Exectime etm(__FUNCTION__);
	CDialogEx::OnDestroy();

	//释放资源 
	KillTimer(1);
}


void CNetServerAppDlg::ParseNetData(char* buf, int len)
{
	Exectime etm(__FUNCTION__);
	//解析
	_beatCount++;

	//显示到界面
	CString sBeat;
	sBeat.Format(_T("%d"), _beatCount);
	GetDlgItem(IDC_EDIT_BEAT)->SetWindowText(sBeat);
}

