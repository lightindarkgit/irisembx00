
// NetServerApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNetServerAppApp: 
// �йش����ʵ�֣������ NetServerApp.cpp
//

class CNetServerAppApp : public CWinApp
{
public:
	CNetServerAppApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNetServerAppApp theApp;