#include "StdAfx.h"
#include <Windows.h>
#include "ComFileExtr.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GetSubStorage(IStorage *Istg, std::wstring subName, IStorage **out)
{
	HRESULT    hr;
	IEnumSTATSTG *ppenum = NULL;
	STATSTG SStg;
	ULONG celt = 1;

	hr = Istg->EnumElements(NULL,NULL,NULL,&ppenum);
	if (hr != S_OK) throw(L"枚举失败");

	//枚举所有子存储
	while(ppenum->Next(celt,&SStg,&celt)==S_OK)
	{
		//如果是子存储 调用递归
		if(SStg.type == 1 && wcscmp(SStg.pwcsName, subName.c_str())==0 )
		{
			//得到子存储接口
			hr = Istg->OpenStorage(SStg.pwcsName, 
				NULL, 
				STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 
				NULL, 
				0, 
				out);
			break;
		}
	}

	if(ppenum)
	{
		ppenum->Release();
	}
}

void GetSubStreamData(IStorage *Istg, std::wstring subName, std::vector<char>& stream)
{
	HRESULT    hr;
	IEnumSTATSTG *ppenum = NULL;
	STATSTG SStg;
	ULONG celt = 1;

	hr = Istg->EnumElements(NULL,NULL,NULL,&ppenum);
	if (hr != S_OK) throw(L"枚举失败");

	//枚举所有子存储
	while(ppenum->Next(celt,&SStg,&celt)==S_OK)
	{
		//如果是子存储 调用递归
		if(SStg.type == 2 && wcscmp(SStg.pwcsName, subName.c_str())==0 )
		{
			IStream *  pStream;
			ULONG cbReaded = 0;
			//得到流的接口
			hr = Istg->OpenStream(SStg.pwcsName, 
				NULL, 
				STGM_READWRITE|STGM_SHARE_EXCLUSIVE|STGM_DIRECT_SWMR, 
				NULL, 
				&pStream);
			if (hr != S_OK)
			{
				//OutLogMsg(L"打开流时失败");
			}

			//分配内存	
			stream.resize(ULONG(SStg.cbSize.QuadPart));

			//读取流
			hr = pStream->Read(stream.begin(), ULONG(SStg.cbSize.QuadPart), &cbReaded);
			if (hr != S_OK)
			{
				//OutLogMsg(L"读取流的内容时失败");
			}
			pStream->Release();

			break;
		}
	}

	if(ppenum)
	{
		ppenum->Release();
	}
}

void SetSubStreamData(IStorage *Istg, std::wstring subName, const std::vector<char>& stream)
{
	HRESULT    hr;
	IEnumSTATSTG *ppenum = NULL;
	STATSTG SStg;
	ULONG celt = 1;

	hr = Istg->EnumElements(NULL,NULL,NULL,&ppenum);
	if (hr != S_OK) throw(L"枚举失败");

	//枚举所有子存储
	while(ppenum->Next(celt,&SStg,&celt)==S_OK)
	{
		//如果是子存储 调用递归
		if(SStg.type == 2 && wcscmp(SStg.pwcsName, subName.c_str())==0 )
		{
			IStream *  pStream;
			ULONG cbReaded = 0;
			//得到流的接口
			hr = Istg->OpenStream(SStg.pwcsName, 
				NULL, 
				STGM_READWRITE|STGM_SHARE_EXCLUSIVE|STGM_FAILIFTHERE, 
				NULL, 
				&pStream);
			if (hr != S_OK)
			{
				//OutLogMsg(L"打开流时失败");
			}
			ULARGE_INTEGER newSize;
			newSize.QuadPart = stream.size();
			pStream->SetSize(newSize);
			ULARGE_INTEGER setResult;
			LARGE_INTEGER seekResult;
			seekResult.QuadPart = 0;
			hr = pStream->Seek(seekResult, STREAM_SEEK_SET, &setResult);
			ULONG writeResult = 0xffffffff;
			hr = pStream->Write(stream.begin(), stream.size(), &writeResult);
			pStream->Release();
			break;
		}
	} 

	if(ppenum)
	{
		ppenum->Release();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QQMsg::QQMsg(IStorage* pRootStg)
{
	m_pRootStg = pRootStg;
	readMsg();
}

QQMsg::~QQMsg()
{
	;
}

void QQMsg::GetMsgContent(unsigned int idx, QQMsg::SignalMsgContent& content)
{
	content.resize( m_content[idx].size() );
	memcpy( content.begin(), m_content[idx].begin(), m_content[idx].size() );
}

void QQMsg::SetMsgContent(unsigned int idx, const QQMsg::SignalMsgContent& content)
{
	// replace all
	/*m_content[idx].resize( content.size() );
	memcpy( m_content[idx].begin(), content.begin(), content.size() );
	m_index[idx].m_length = content.size();*/

	// replace olny msg, not include TD
	SignalMsgContent oldContent;
	GetMsgContent(idx, oldContent);
	SignalMsgContent newContent;
	newContent.reserve(4096);
	unsigned long oldSize = 0;
	unsigned long newSize = 0;
	memcpy(&oldSize, oldContent.begin() + 20, 4);
	memcpy(&newSize, content.begin() + 20, 4);
	//newContent.insert( newContent.end(), oldContent.begin(), oldContent.begin()+20);
	newContent.insert( newContent.end(), content.begin(), content.begin()+20);
	memcpy( newContent.begin()+8, oldContent.begin()+8, 4 );
	newContent.insert( newContent.end(), content.begin()+20, content.begin()+20+4);

	// delete a mst
	/*char tdzero[5] = {0};
	tdzero[4] = 1;
	newContent.insert( newContent.end(), &tdzero[0], &tdzero[4]);*/

	newContent.insert( newContent.end(), content.begin()+24, content.begin()+24+newSize);
	newContent.insert( newContent.end(), oldContent.begin()+24+oldSize, oldContent.end());
	unsigned long realNewSize = newContent.size();
	memcpy(newContent.begin(), &realNewSize, 4);

	m_content[idx].resize( newContent.size() );
	memcpy( m_content[idx].begin(), newContent.begin(), newContent.size() );
	m_index[idx].m_length = newContent.size();
	save();
}

void QQMsg::readMsg()
{
	std::vector< char > indexdat;
	GetSubStreamData(m_pRootStg, L"index.dat", indexdat);
	std::vector< char > contentdat;
	GetSubStreamData(m_pRootStg, L"content.dat", contentdat);

	int numMsg = indexdat.size() / 8;
	m_index.resize(numMsg);
	for (int idx=0; idx<numMsg; ++idx)
	{
		unsigned long length = 0;
		unsigned long id = *(unsigned long*)(indexdat.begin() + idx*8);
		unsigned long offset = *(unsigned long*)(indexdat.begin() + idx*8 + 4);
		if (idx == numMsg-1)
		{
			length = contentdat.size() - offset;
		}
		else
		{
			length = *(unsigned long*)(indexdat.begin() + idx*8 + 8 + 4) - offset;
		}
		m_index[idx].m_id = id;
		m_index[idx].m_length = length;
	}

	m_content.resize(numMsg);
	unsigned long offset = 0;
	for (int idx=0; idx<numMsg; ++idx)
	{
		unsigned long length = m_index[idx].m_length;
		m_content[idx].resize(length);
		memcpy( m_content[idx].begin(), contentdat.begin()+offset, length);
		offset += length;
	}
}

void QQMsg::save()
{
	std::vector< char > indexdat;
	indexdat.resize( m_index.size() * 8 );
	unsigned long offset = 0;
	for (int idx=0; idx<m_index.size(); ++idx)
	{
		memcpy( indexdat.begin()+8*idx, &m_index[idx], 8 );
		memcpy( indexdat.begin()+8*idx + 4, &offset, 4 );
		offset += m_index[idx].m_length;
	}
	SetSubStreamData(m_pRootStg, L"index.dat", indexdat);

	std::vector< char > contentdat;
	contentdat.reserve( 1024 * 1024 );
	for (int idx=0; idx<m_content.size(); ++idx)
	{
		contentdat.insert( contentdat.end(), m_content[idx].begin(), m_content[idx].end() );
	}
	SetSubStreamData(m_pRootStg, L"content.dat", contentdat);
}

////////////////////////////////////////////////////////////////////////////////////////
namespace
{
	char g_AppPath[MAX_PATH] = ""; 
	char g_AppDir[MAX_PATH] = ""; 
}

int ToUnicode(wchar_t* dest, const char* src, int destCount)
{
	return MultiByteToWideChar(CP_ACP, NULL,
		src, -1,
		dest,
		destCount);
}

int ToMultibyte(char* dest, const wchar_t* src, int destCount)
{
	return WideCharToMultiByte(CP_ACP, NULL,
		src, -1,
		dest,
		destCount,NULL,NULL);
}

const char* GetApplicationPath()
{
	if (g_AppPath[0] == 0)
	{
		wchar_t path[MAX_PATH]; 

		if (GetModuleFileName( NULL, path, MAX_PATH ) > 0)
		{
			ToMultibyte(g_AppPath, path, MAX_PATH);
		}
	}

	return g_AppPath;
}

const char* GetApplicationDir()
{
	if (g_AppDir[0] == 0)
	{
		if (g_AppPath[0] == 0)
		{
			GetApplicationPath();
		}

		strcpy(g_AppDir, g_AppPath);

		int tlen = strlen(g_AppDir);
		for(int i = tlen; i >= 0; --i)
		{
			const char c = g_AppDir[i];
			if(c == '/' || c == '\\')
			{
				g_AppDir[i+1] = 0;
				break;
			}
		}
	}
	return g_AppDir;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ComFileExtr::ComFileExtr(CDialog* pdlg)
{
	cdlg_Main = pdlg;
}

ComFileExtr::~ComFileExtr(void)
{
}

void ComFileExtr::ExtrDB(IStorage * Istg, CString strSubPath)
{
	IStream *  pStream;
	HRESULT    hr;

	IEnumSTATSTG *ppenum = NULL;
	STATSTG SStg;
	ULONG celt = 1;

	hr = Istg->EnumElements(NULL,NULL,NULL,&ppenum);
	if (hr != S_OK) throw(L"枚举失败");
	
	//枚举所有子存储
	while(ppenum->Next(celt,&SStg,&celt)==S_OK)
	{
		//如果是子存储 调用递归
		if(SStg.type == 1 )
		{
			//CString strFoldName(L"mkdir ");
			CString tstrSubPath = strSubPath;
			tstrSubPath.Append(L"\\");
			tstrSubPath.Append(SStg.pwcsName);
			//strFoldName.Append(strSubPath);
			//创建存储同名文件夹
			OutLogMsg(L"+------------------------------------------\
------------------------------------------------------+");
			if(!CreateDirectory(tstrSubPath,NULL))   
				OutLogMsg(L"无法创建文件夹:" + tstrSubPath);   
			else 
				{
					OutLogMsg(L"创建文件夹:" + tstrSubPath);
				}
			
			//得到子存储接口
			IStorage * tIstg;
			hr = Istg->OpenStorage(SStg.pwcsName, 
										NULL, 
										STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 
										NULL, 
										0, 
										&tIstg);
			//调用递归
			ExtrDB(tIstg, tstrSubPath);
		}//如果是流 直接解压
		else if(SStg.type == 2)
		{
			ULONG cbReaded = 0;
			
			//得到流的接口
			hr = Istg->OpenStream(SStg.pwcsName, 
									NULL, 
									STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 
									NULL, 
									&pStream);
			if (hr != S_OK) OutLogMsg(L"打开流时失败");
						
			//分配内存		
			char *buf = (char *)malloc(ULONG(SStg.cbSize.QuadPart));
			
			//读取流
			hr = pStream->Read(buf, ULONG(SStg.cbSize.QuadPart), &cbReaded);
			if (hr != S_OK) OutLogMsg(L"读取流的内容时失败");
			
			//转存
			CString tstrExtrPath = strSubPath + L"\\";
			tstrExtrPath .Append(SStg.pwcsName);
			CFile file;
			file.Open(tstrExtrPath,CFile::modeWrite|CFile::modeCreate|CFile::typeBinary);
			file.Seek(0,CFile::begin);
			file.Write(buf,SStg.cbSize.QuadPart);
			file.Close();
			CString strTmp(SStg.pwcsName);
			OutLogMsg(L"创建文件:" + strTmp);
			
			//释放内存
			free(buf);
		}
	}
	
	if(ppenum)
	{
		ppenum->Release();
	}
}


void ComFileExtr::OnStart()
{
	TCHAR strCrPath[1024] = {0};
	//DWORD len = GetCurrentDirectory(1024,strCrPath);
	//strCrPath[len] = 0;
	ToUnicode(strCrPath, GetApplicationDir(), 1024);
	
	
	strExtrPath.Append(strCrPath);


	strExtrPath.Append(L"\\temp");

	if(!CreateDirectory(strExtrPath,NULL)) 
	{
		OutLogMsg(L"无法创建文件夹:" + strExtrPath);   
	}
	else 
	{
		OutLogMsg(L"创建文件夹:" + strExtrPath);
	}

	//strExtrPath.Append(L".\\MSGDB");
	CString strTemp(L"解压文件输出路径:");
	strTemp.Append(strExtrPath);
	OutLogMsg(strTemp);
	
	try
	{
		//打开复合文档取得接口
		HRESULT    hr;
		IStorage * Istg;
		hr = ::StgOpenStorage(strDBPath/*T2COLE(L".\\Msg2.0.db")*/,
								NULL, 
								STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 
								NULL, 
								0, 
								&m_pRootStg);
		if (hr != S_OK) 
		{
			OutLogMsg(L"打开" + strDBPath + L"时发生异常");
		}	
		else
		{
			Istg = m_pRootStg;
			ExtrDB(Istg,strExtrPath);
			m_pRootStg->Release();
		}

	}
	catch(CString strE)
	{
		OutLogMsg(strE);
	}
}

void ComFileExtr::Rebuild()
{
	try
	{
		//打开复合文档取得接口
		HRESULT    hr;
		hr = ::StgOpenStorage(strDBPath/*T2COLE(L".\\Msg2.0.db")*/,
			NULL, 
			STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 
			NULL, 
			0, 
			&m_pRootStg);
		if (hr != S_OK) 
		{
			OutLogMsg(L"打开" + strDBPath + L"时发生异常");
		}	
		else
		{
			std::wstring buddyName = L"buddy";
			IStorage* buddyISg = 0;
			GetSubStorage(m_pRootStg, buddyName, &buddyISg);
			std::wstring qqnumIn = L"20604395";
			int inIdx = 2;
			std::wstring qqnumOut = L"20604395";
			int outIdx = 3;
			IStorage* qqISgIn = 0;
			IStorage* qqISgOut = 0;
			GetSubStorage(buddyISg, qqnumIn, &qqISgIn);
			if (qqnumOut == qqnumIn)
			{
				qqISgOut = qqISgIn;
			}
			else
			{
				GetSubStorage(buddyISg, qqnumOut, &qqISgOut);
			}
			
			QQMsg msgIn(qqISgIn);
			QQMsg msgOut(qqISgOut);
			QQMsg::SignalMsgContent contentIn;
			msgIn.GetMsgContent(inIdx, contentIn);
			msgOut.SetMsgContent(outIdx, contentIn);
		}
	}
	catch(CString strE)
	{
		OutLogMsg(strE);
	}
}

int ComFileExtr::OutLogMsg(CString strMsgLog)
{
	strMsgLog = L">" + strMsgLog;
	strMsgLog.Append(L"\r\n");
	//m_strOutLogMsg += strMsgLog;
	
	CEdit *edt = (CEdit*)(cdlg_Main->GetDlgItem(IDC_EDIT1));
	edt->GetWindowTextW(m_strOutLogMsg);
	
	int len=edt->GetWindowTextLength();//GetLineCount();//获得当前List控件一共多少行
	edt->SetSel(-1,-1);//LineScroll(index,0);//将垂直滚动条滚动到最后一行
	edt->ReplaceSel(strMsgLog); 
	//cdlg_Main->UpdateData(true);
	
	//输出Log内容大于指定长度时以 ">" 为标记删除前面的内容
	//if(m_strOutLogMsg.GetLength() > 10000)
	//{
	//	int sub_End = m_strOutLogMsg.Find(L">",10);
	//	edt->SetSel(0,sub_End);
	//	edt->ReplaceSel(L"");
	//}
	return 0;
}

void ComFileExtr::SetInputDBFilePath(CString strPath)
{
	strDBPath = strPath;
	OutLogMsg(L"DB文件输入路径:" + strDBPath);
}

void ComFileExtr::SetOutFilePath()
{
	// 
}
