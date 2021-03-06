// QQMgrMsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QQMgrMsg.h"
#include "QQMgrMsgDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQQMgrMsgDlg 对话框




CQQMgrMsgDlg::CQQMgrMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQQMgrMsgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQQMgrMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQQMgrMsgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CQQMgrMsgDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CQQMgrMsgDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CQQMgrMsgDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_REBUILD, &CQQMgrMsgDlg::OnBnClickedButtonRebuild)
	ON_BN_CLICKED(IDC_BUTTON_SUB, &CQQMgrMsgDlg::OnBnClickedButtonSub)
END_MESSAGE_MAP()


// CQQMgrMsgDlg 消息处理程序

BOOL CQQMgrMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ThreadHandle = NULL;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CQQMgrMsgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQQMgrMsgDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CQQMgrMsgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI ThreadPrc(LPVOID pParam)
{
	CQQMgrMsgDlg* MgrDlg = (CQQMgrMsgDlg*)pParam;
	ComFileExtr comFext(MgrDlg);
	
	comFext.SetInputDBFilePath(MgrDlg->strInputFielPath);	
	
	comFext.OnStart();
	
	MgrDlg->ThreadHandle = NULL;
	
	return 0;
}


void CQQMgrMsgDlg::OnBnClickedOk()
{
	if(strInputFielPath == "")
	{
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(L"请指定db文件路径");	
	}
	else
	{
		//创建线程
		if(!ThreadHandle)
			ThreadHandle = CreateThread(NULL, 0, ThreadPrc, this, 0, NULL);	
	}

}

void CQQMgrMsgDlg::OnBnClickedButton1()
{
	// 
	CFileDialog fDlg(1);
	fDlg.DoModal();
	
	CString strPath = fDlg.GetPathName();
	
	if(!strPath.IsEmpty())
	{
		strInputFielPath = strPath;
		//AfxMessageBox(L"设定DB路径:" + strDBPath);
	}

	else
		AfxMessageBox(L"请指定db文件路径");		
}

void CQQMgrMsgDlg::OnBnClickedButton3()
{

}

void CQQMgrMsgDlg::OnBnClickedButtonRebuild()
{
	CQQMgrMsgDlg* MgrDlg = this;
	ComFileExtr comFext(MgrDlg);
	comFext.SetInputDBFilePath(MgrDlg->strInputFielPath);	
	comFext.Rebuild();
}

void CQQMgrMsgDlg::OnBnClickedButtonSub()
{
	CQQMgrMsgDlg* MgrDlg = this;
	ComFileExtr comFext(MgrDlg);
	comFext.SetInputDBFilePath(MgrDlg->strInputFielPath);	
	comFext.Remain();
}
