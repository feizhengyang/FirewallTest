
// FirewallUI.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFirewallUIApp: 
// �йش����ʵ�֣������ FirewallUI.cpp
//

class CFirewallUIApp : public CWinApp
{
public:
	CFirewallUIApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFirewallUIApp theApp;