
// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg 对话框



CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
    , _strHostIp(_T(""))
    , _nHostPort(0)
    , _bufFrame(nullptr)
    , _strToken(_T(""))
    , _nOption(0)
    , _nResult(0)
    , _nErrCode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    _strHostIp = "10.2.1.14";
    _nHostPort = 9000;
    _strToken = "1";
    _nOption = 1;

    _bufFrame = new char[g_recvBuf];
}

CClientDlg::~CClientDlg()
{
    //////////////////////
    // 由于需要的缓冲区变大，改为使用new的方法来分配
    if (_bufFrame != nullptr)
    {
        delete[] _bufFrame;
        _bufFrame = nullptr;
    }
    ///////////////////////
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDT_HOST, _strHostIp);
    DDX_Text(pDX, IDC_EDT_PORT, _nHostPort);
    DDV_MinMaxInt(pDX, _nHostPort, 0, 65535);
    DDX_Text(pDX, IDC_EDT_TOKEN, _strToken);
    DDX_Text(pDX, IDC_EDT_OPTION, _nOption);
    DDX_Text(pDX, IDC_EDT_RESULT, _nResult);
    DDX_Text(pDX, IDC_EDT_ERRCODE, _nErrCode);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_CONNECT, &CClientDlg::OnBnClickedBtnConnect)
    ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CClientDlg::OnBnClickedBtnDisconnect)
    ON_BN_CLICKED(IDC_BTN_SEND, &CClientDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
    _HistoryEditEx.SubclassDlgItem(IDC_EDT_INFO, this);

    // modify the status edit box's text color and font
    //_HistoryEditEx.bkColor((COLORREF)RGB(214, 211, 206));	// no used now, use window's default background
    //_HistoryEditEx.textColor((COLORREF)RGB(176, 71, 154)); // need to set the textcolor, or, it will used COLOR_WINDOWTEXT
    _HistoryEditEx.bkColor((COLORREF)RGB(179, 168, 150));	// no used now, use window's default background
    // _HistoryEditEx.textColor((COLORREF)RGB(255, 255, 255)); // need to set the textcolor, or, it will used COLOR_WINDOWTEXT
    _HistoryEditEx.textColor((COLORREF)RGB(20, 68, 106)); // need to set the textcolor, or, it will used COLOR_WINDOWTEXT
    _HistoryEditEx.setFont(-12, FW_NORMAL, DEFAULT_PITCH | FF_DONTCARE, _T("Tahoma"));
    _HistoryEditEx.setMaxLine(30);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
{
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 显示历史信息
void CClientDlg::DispInfo(std::string msg)
{
    CString dispMsg = msg.c_str();
    _HistoryEditEx.AppendString(dispMsg);
}

void CClientDlg::OnBnClickedBtnConnect()
{
    // TODO:  在此添加控件通知处理程序代码
    _connAdmin.DisConn();
    UpdateData();

    std::string adminIp = _strHostIp.GetString();

    int nret = _connAdmin.Connect(adminIp, _nHostPort);

    std::stringstream oss;
    oss << "连接服务 (" << adminIp << ":" << _nHostPort << ") ";

    if (nret == netClientSuccess)
    {
        oss << "成功";
        GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
    }
    else
    {
        oss << "失败";
    }
    
    DispInfo(oss.str().c_str());
}


void CClientDlg::OnBnClickedBtnDisconnect()
{
    // TODO:  在此添加控件通知处理程序代码
    _connAdmin.DisConn();
    GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);

    std::string adminIp = _strHostIp.GetString();
    std::stringstream oss;
    oss << "断开服务 (" << adminIp << ":" << _nHostPort << ") ";
    DispInfo(oss.str().c_str());
}


void CClientDlg::OnBnClickedBtnSend()
{
    // TODO:  在此添加控件通知处理程序代码
    if (!_connAdmin.IsConnect())
    {
        DispInfo("服务尚未连接，请连接后重试。");
        return;
    }

    UpdateData();

    PPACKHEAD phd = (PPACKHEAD)_bufFrame;
    phd->cmdCode = 0x0004;
    phd->length = packHeadSize;

    std::stringstream oss;
    /*
    oss << "{"
        << "\"token\" : \"1\","
        << "\"option\" : 3,"
        << "\"result\" : 0,"
        << "\"errorcode\" : 0,"
        << "\"desc\" : \"新增一个人员，此人是XXX类型的犯人\""
        << "}"
        ;
        */

    oss << "{"
        << "\"token\" : \"" << _strToken <<  "\","
        << "\"option\" : " << _nOption << ","
        << "\"result\" : " << _nResult << ","
        << "\"errorcode\" : " << _nErrCode << ","
        << "\"desc\" : \"新增一个人员，此人是XXX类型的犯人\""
        << "}"
        ;

    std::string sendmsg = oss.str();
    char* msg = (char*)_bufFrame + packHeadSize;
    strncpy(msg, sendmsg.c_str(), sendmsg.length());

    phd->length = sendmsg.length();

    _connAdmin.Send(_bufFrame);
    DispInfo("Send over");

    int nret = _connAdmin.Recv(_bufFrame, g_recvBuf);

    DispInfo("Receive over");
    
    oss.str("");
    if (nret >= 0)
    {
        oss << "接收数据成功，接收" << nret << "个字节" << std::endl;

        // 读取数据
        std::vector<char> vod;
        int sendDataLen = nret - packHeadSize;
        char* pData = (char*)(_bufFrame + packHeadSize);

        vod.resize(sendDataLen);
        vod.assign(pData, pData + sendDataLen);

        // 从vector转为string
        std::string jsonStr;
        jsonStr.insert(jsonStr.begin(), vod.begin(), vod.end());

        oss << jsonStr;
    }
    else
    {
        oss << "接收数据失败，错误码" << nret;
    }

    DispInfo(oss.str());
    
    OnBnClickedBtnDisconnect();

    /*

    _event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    _handle = (HANDLE)_beginthreadex(NULL, 0, DataControlAnalyze, (LPVOID)this, 0, NULL);
    if (NULL == _event || _handle == NULL)
    {
        DispInfo("{Thread Init Error}");
        return;
    }

    ::CloseHandle(_handle);
    */
}


//unsigned int __stdcall  CClientDlg::DataControlAnalyze(void *pParam)
//{
//    unsigned int usRet = true;
//
//
//    CClientDlg *au = (CClientDlg*)pParam;
//
//    while (WAIT_OBJECT_0 == WaitForSingleObject(au->_event, INFINITE))
//    {
//        PPACKHEAD phd = (PPACKHEAD)au->_bufFrame;
//
//        
//    }
//
//    return usRet;
//}

