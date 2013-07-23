// QQMgrMsgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "QQMgrMsg.h"
#include "QQMgrMsgDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQQMgrMsgDlg �Ի���




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
END_MESSAGE_MAP()


// CQQMgrMsgDlg ��Ϣ�������

BOOL CQQMgrMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ThreadHandle = NULL;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CQQMgrMsgDlg::OnPaint()
{
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(L"��ָ��db�ļ�·��");	
	}
	else
	{
		//�����߳�
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
		//AfxMessageBox(L"�趨DB·��:" + strDBPath);
	}

	else
		AfxMessageBox(L"��ָ��db�ļ�·��");		
}

void CQQMgrMsgDlg::OnBnClickedButton3()
{

}
