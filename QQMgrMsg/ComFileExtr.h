#pragma once

class ComFileExtr
{
public:
	ComFileExtr(CDialog* pdlg);
	~ComFileExtr(void);
	
////////////////////˽�ö���
IStorage * m_pRootStg; // ���洢�Ľӿ�ָ�� 
CString strDBPath;
CString strExtrPath;
CString m_strOutLogMsg;
void ExtrDB(IStorage * Istg ,CString strSubPath);
//CArray <ULONG, ULONG> m_aryPosition; // ���������˺�Ŀ¼����ƫ�Ƶ�ַ������    

CDialog *cdlg_Main;
int OutLogMsg(CString strMsgLog);
void OnStart();
void SetInputDBFilePath(CString strPath);
void SetOutFilePath();
};
