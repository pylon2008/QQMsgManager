// QQMgrMsgDlg.h : ͷ�ļ�
//

#pragma once
#include "ComFileExtr.h"

// CQQMgrMsgDlg �Ի���
class CQQMgrMsgDlg : public CDialog
{
// ����
public:
	CQQMgrMsgDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_QQMGRMSG_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	////////////////////˽�ö���
	ComFileExtr * comFext;
	CString strInputFielPath;

	//static DWORD WINAPI ThreadPrc(LPVOID pParam);
	HANDLE ThreadHandle;
};
