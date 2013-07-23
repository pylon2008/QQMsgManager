#pragma once

class ComFileExtr
{
public:
	ComFileExtr(CDialog* pdlg);
	~ComFileExtr(void);
	
////////////////////私用对象
IStorage * m_pRootStg; // 根存储的接口指针 
CString strDBPath;
CString strExtrPath;
CString m_strOutLogMsg;
void ExtrDB(IStorage * Istg ,CString strSubPath);
//CArray <ULONG, ULONG> m_aryPosition; // 保存流中账号目录所在偏移地址的数组    

CDialog *cdlg_Main;
int OutLogMsg(CString strMsgLog);
void OnStart();
void SetInputDBFilePath(CString strPath);
void SetOutFilePath();
};
