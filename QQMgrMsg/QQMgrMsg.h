// QQMgrMsg.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQQMgrMsgApp:
// �йش����ʵ�֣������ QQMgrMsg.cpp
//

class CQQMgrMsgApp : public CWinApp
{
public:
	CQQMgrMsgApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CQQMgrMsgApp theApp;