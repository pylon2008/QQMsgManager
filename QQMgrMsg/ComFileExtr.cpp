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
	if (hr != S_OK) throw(L"ö��ʧ��");
	
	//ö�������Ӵ洢
	while(ppenum->Next(celt,&SStg,&celt)==S_OK)
	{
		//������Ӵ洢 ���õݹ�
		if(SStg.type == 1 )
		{
			//CString strFoldName(L"mkdir ");
			CString tstrSubPath = strSubPath;
			tstrSubPath.Append(L"\\");
			tstrSubPath.Append(SStg.pwcsName);
			//strFoldName.Append(strSubPath);
			//�����洢ͬ���ļ���
			OutLogMsg(L"+------------------------------------------\
------------------------------------------------------+");
			if(!CreateDirectory(tstrSubPath,NULL))   
				OutLogMsg(L"�޷������ļ���:" + tstrSubPath);   
			else 
				{
					OutLogMsg(L"�����ļ���:" + tstrSubPath);
				}
			
			//�õ��Ӵ洢�ӿ�
			IStorage * tIstg;
			hr = Istg->OpenStorage(SStg.pwcsName, 
										NULL, 
										STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 
										NULL, 
										0, 
										&tIstg);
			//���õݹ�
			ExtrDB(tIstg, tstrSubPath);
		}//������� ֱ�ӽ�ѹ
		else if(SStg.type == 2)
		{
			ULONG cbReaded = 0;
			
			//�õ����Ľӿ�
			hr = Istg->OpenStream(SStg.pwcsName, 
									NULL, 
									STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 
									NULL, 
									&pStream);
			if (hr != S_OK) OutLogMsg(L"����ʱʧ��");
						
			//�����ڴ�		
			char *buf = (char *)malloc(ULONG(SStg.cbSize.QuadPart));
			
			//��ȡ��
			hr = pStream->Read(buf, ULONG(SStg.cbSize.QuadPart), &cbReaded);
			if (hr != S_OK) OutLogMsg(L"��ȡ��������ʱʧ��");
			
			//ת��
			CString tstrExtrPath = strSubPath + L"\\";
			tstrExtrPath .Append(SStg.pwcsName);
			CFile file;
			file.Open(tstrExtrPath,CFile::modeWrite|CFile::modeCreate|CFile::typeBinary);
			file.Seek(0,CFile::begin);
			file.Write(buf,SStg.cbSize.QuadPart);
			file.Close();
			CString strTmp(SStg.pwcsName);
			OutLogMsg(L"�����ļ�:" + strTmp);
			
			//�ͷ��ڴ�
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
		OutLogMsg(L"�޷������ļ���:" + strExtrPath);   
	}
	else 
	{
		OutLogMsg(L"�����ļ���:" + strExtrPath);
	}

	//strExtrPath.Append(L".\\MSGDB");
	CString strTemp(L"��ѹ�ļ����·��:");
	strTemp.Append(strExtrPath);
	OutLogMsg(strTemp);
	
	try
	{
		//�򿪸����ĵ�ȡ�ýӿ�
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
			OutLogMsg(L"��" + strDBPath + L"ʱ�����쳣");
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
	
	int len=edt->GetWindowTextLength();//GetLineCount();//��õ�ǰList�ؼ�һ��������
	edt->SetSel(-1,-1);//LineScroll(index,0);//����ֱ���������������һ��
	edt->ReplaceSel(strMsgLog); 
	//cdlg_Main->UpdateData(true);
	
	//���Log���ݴ���ָ������ʱ�� ">" Ϊ���ɾ��ǰ�������
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
	OutLogMsg(L"DB�ļ�����·��:" + strDBPath);
}

void ComFileExtr::SetOutFilePath()
{
	// 
}
