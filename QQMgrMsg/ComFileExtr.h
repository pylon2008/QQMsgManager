#pragma once

#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct QQMsgIndex 
{
	unsigned int m_id;
	unsigned int m_length;
};

class QQMsg
{
public:
	typedef std::vector< char > SignalMsgContent;

	QQMsg(IStorage* pRootStg);
	~QQMsg();

	void GetMsgContent(unsigned int idx, SignalMsgContent& content);
	void SetMsgContent(unsigned int idx, const SignalMsgContent& content);

protected:
	void readMsg();

	void save();

private:
	IStorage* m_pRootStg;
	std::vector< QQMsgIndex > m_index;
	std::vector< SignalMsgContent > m_content;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void Rebuild();
};
