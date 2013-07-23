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

void Rebuild();
};
