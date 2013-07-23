#include "StdAfx.h"
#include <Windows.h>
#include "ComFileExtr.h"
#include "resource.h"


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
		IStream *  pStream;
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
