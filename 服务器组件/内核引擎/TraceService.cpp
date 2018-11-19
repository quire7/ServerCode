#include "StdAfx.h"
#include "TraceService.h" // #include "EventService.h"
#include <strsafe.h>

//////////////////////////////////////////////////////////////////////////

//��ɫ����
#define COLOR_TIME						RGB(0,0,0)
#define COLOR_NORMAL					RGB(125,125,125)
#define COLOR_WARN						RGB(255,128,0)
#define COLOR_EXCEPTION					RGB(200,0,0)
#define COLOR_DEBUG						RGB(0,128,128)

#define IDM_MENU0						100
#define IDM_MENU1						110
#define IDM_MENU2						120
#define IDM_MENU3						130
#define IDM_MENU4						140
#define IDM_MENU5						150

#define TRACE_LEVEL_COUNT				5
//////////////////////////////////////////////////////////////////////////
struct tagTraceString
{
	enTraceLevel	enTL;
	CString			strMsg;
};
//׷�ٷ���
//////////////////////////////////////////////////////////////////////////
static ITraceService				*g_pITraceService = NULL;
static bool							g_bEnableTrace=true;
static CYSArray<tagTraceString *>	g_aryTraceString;
static CYSArray<tagTraceString *>	g_aryTraceStringStorage;
static CCriticalSection				g_CS;
//��������
//////////////////////////////////////////////////////////////////////////
//���캯��
CTraceService::CTraceService()
{

}

//��������
//////////////////////////////////////////////////////////////////////////
//���÷���
bool CTraceService::SetTraceService(IUnknownEx * pIUnknownEx)
{
	g_pITraceService = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITraceService);
	return NULL != g_pITraceService;
}

//��ȡ����
VOID * CTraceService::GetTraceService(const IID & Guid, DWORD dwQueryVer)
{
	ASSERT(g_pITraceService);
	if (g_pITraceService) return NULL;

	return (VOID*)g_pITraceService->QueryInterface(Guid, dwQueryVer);
}

//��ȡ����
VOID * CTraceService::GetTraceServiceManager(const IID & Guid, DWORD dwQueryVer)
{
	return NULL;
}

//״̬����
//////////////////////////////////////////////////////////////////////////
//׷��״̬
bool CTraceService::IsEnableTrace(enTraceLevel TraceLevel)
{
	return false;
}

//׷�ٿ���
bool CTraceService::EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace)
{
	return false;
}
//׷�ٿ���
bool CTraceService::EnableAllTrace( bool bEnableTrace )
{
	if (g_bEnableTrace!=bEnableTrace)
	{
		g_bEnableTrace=bEnableTrace;
		return true;
	}
	return false;
}
//���ܺ���
//////////////////////////////////////////////////////////////////////////
//׷����Ϣ
bool CTraceService::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	if (g_pITraceService!=NULL && pszString!=NULL && *pszString!=0)
	{
		return g_pITraceService->TraceString(pszString, TraceLevel);	
	}
	return false;
}

bool CTraceService::TraceStringEx( enTraceLevel TraceLevel, LPCTSTR pszFormat,... )
{
	if (g_bEnableTrace && g_pITraceService!=NULL && pszFormat!=NULL && *pszFormat!=0)
	{
		CString str;
		va_list ptr;
		va_start(ptr, pszFormat);
		str.FormatV(pszFormat,ptr);
		va_end(ptr);
		return g_pITraceService->TraceString(str, TraceLevel);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//׷�ٷ���
//////////////////////////////////////////////////////////////////////////
#define WM_TRACE_STRING	WM_USER+1

BEGIN_MESSAGE_MAP(CTraceServiceControl, CRichEditCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_TRACE_STRING,OnTraceServiceMessage)
	ON_COMMAND(IDM_MENU0, OnCopyString)
	ON_COMMAND(IDM_MENU1, OnSelectAll)
	ON_COMMAND(IDM_MENU2, OnDeleteString)
	ON_COMMAND(IDM_MENU3, OnClearAll)
	ON_COMMAND(IDM_MENU4, OnSaveString)
END_MESSAGE_MAP()

//��������

//д����־
void AutoSaveLog(CTraceServiceControl *pRichEditTrace)
{
	static CString	strLogPath;
	if (strLogPath.IsEmpty())
	{
		TCHAR szBuf[MAX_PATH];
		GetModuleFileName(NULL,szBuf,CountString(szBuf));
		PathRemoveExtension(szBuf);
		LPTSTR pszFileName=::PathFindFileName(szBuf);
		CString strFileName(pszFileName);		
		StringCchCopy(pszFileName,CountString(szBuf)-(pszFileName-szBuf),_T("ServerLog"));
		if (!::PathFileExists(szBuf))
		{
			return;
		}

		if (strFileName.CompareNoCase(_T("GameServer"))==0)
		{
			AfxGetMainWnd()->GetWindowText(strFileName);
			strFileName.MakeLower();
			strFileName.Replace(_T(".rop"),_T(""));
			strFileName.Replace(_T("[���سɹ�]"),_T(""));
			strFileName.Replace(_T("[����ʧ��]"),_T(""));
			strFileName.Replace(_T("[ֹͣ]"),_T(""));
		}
		//��ȡʱ��
		SYSTEMTIME st;
		GetLocalTime(&st);
		strLogPath.Format(_T("%s\\%s%04d%02d%02d%02d%02d%02d%03d.log"),szBuf,strFileName,
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,st.wMilliseconds);
	}
	try
	{
		CString strInput;
		pRichEditTrace->GetWindowText(strInput);
		if (!strInput.IsEmpty())
		{
			CFile file;
			if (file.Open(strLogPath, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary|CFile::modeNoTruncate))
			{
				file.SeekToEnd();
				file.Write(strInput, strInput.GetLength()*sizeof(TCHAR));
				file.Close();
			}
		}
	}
	catch(...)
	{
		ASSERT(0);
	}

}
//////////////////////////////////////////////////////////////////////////
//���캯��
CTraceServiceControl::CTraceServiceControl()
{
	CTraceService::SetTraceService((IUnknownEx*)QueryInterface(IID_IUnknownEx, VER_IUnknownEx));
}

//��������
CTraceServiceControl::~CTraceServiceControl()
{
	CTraceService::SetTraceService(NULL);
	g_aryTraceStringStorage.Append(g_aryTraceString);
	for ( INT_PTR i=g_aryTraceStringStorage.GetCount()-1; i>=0; i-- )
	{
		tagTraceString *pTraceString=g_aryTraceStringStorage[i];
		SafeDelete(pTraceString);
	}
}

//�����ӿ�
//////////////////////////////////////////////////////////////////////////
//�ӿڲ�ѯ
VOID * CTraceServiceControl::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITraceService, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITraceService, Guid, dwQueryVer);
	return NULL;
}

//��Ϣ�ӿ�
//////////////////////////////////////////////////////////////////////////
//׷����Ϣ
bool CTraceServiceControl::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//Ч�����
	ITraceService *pITraceService = QUERY_ME_INTERFACE(ITraceService);
	VERIFY_RETURN_FALSE(pITraceService != NULL);
	//�������
	tagTraceString *pTraceString=NULL;
	g_CS.Lock();	
	INT_PTR iIndex=g_aryTraceStringStorage.GetCount()-1;
	if (iIndex>=0)
	{
		pTraceString=g_aryTraceStringStorage[iIndex];
		g_aryTraceStringStorage.RemoveAt(iIndex);
	}
	g_CS.Unlock();

	if (NULL==pTraceString)
	{
		pTraceString=new tagTraceString;
	}
	VERIFY_RETURN_FALSE(pTraceString!=NULL);
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	//��������
	pTraceString->enTL=TraceLevel;
	pTraceString->strMsg.Format(TEXT("�� %04d-%02d-%02d %02d:%02d:%02d ��%s\n"), SystemTime.wYear, \
		SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond,pszString);
	//��ӵ�������
	iIndex=-1;
	g_CS.Lock();
	iIndex=g_aryTraceString.Add(pTraceString);
	g_CS.Unlock();
	if (0==iIndex)
	{
		PostMessage(WM_TRACE_STRING,0,0);
	}
	return true;
}

//���غ���
//////////////////////////////////////////////////////////////////////////
//�󶨺���
VOID CTraceServiceControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	CTraceService::SetTraceService((IUnknownEx*)QueryInterface(IID_IUnknownEx, VER_IUnknownEx));
}

//���ܺ���
//////////////////////////////////////////////////////////////////////////
//������Ϣ
bool CTraceServiceControl::LoadMessage(LPCTSTR pszFileName)
{
	if (static_cast<CTraceServiceControl*>(g_pITraceService) == NULL) return false;
	return static_cast<CTraceServiceControl*>(g_pITraceService)->LoadMessage(pszFileName);
}

//������Ϣ
bool CTraceServiceControl::SaveMessage(LPCTSTR pszFileName)
{
	if (static_cast<CTraceServiceControl*>(g_pITraceService) == NULL) return false;
	return static_cast<CTraceServiceControl*>(g_pITraceService)->SaveMessage(pszFileName);
}

//���ò���
bool CTraceServiceControl::SetParameter(LONG lMaxLineCount, LONG lReserveLineCount)
{
	m_lMaxLineCount = lMaxLineCount;					//�������
	m_lReserveLineCount = lReserveLineCount;			//��������

	return true;
}

//��������
//////////////////////////////////////////////////////////////////////////
//���÷���
VOID CTraceServiceControl::InitializeService()
{

}

//��ȡ��ɫ
COLORREF CTraceServiceControl::GetTraceColor(enTraceLevel TraceLevel)
{
	switch (TraceLevel)
	{
	case TraceLevel_Info		:	return RGB(15, 130, 248);
	case TraceLevel_Normal		:	return RGB(133, 124, 129);
	case TraceLevel_Warning		:	return RGB(232,100,27);
	case TraceLevel_Exception	:	return RGB(255,0,0);
	case TraceLevel_Debug		:	return RGB(19, 127, 140);
	}
	return RGB(0, 0, 0);
}

//�ַ��ж�
bool EfficacyUrlChar(TCHAR chChar)
{
	//�����ַ�
	if (chChar == TEXT('.')) return true;
	if (chChar == TEXT('=')) return true;
	if (chChar == TEXT('+')) return true;
	if (chChar == TEXT('?')) return true;
	if (chChar == TEXT('#')) return true;
	if (chChar == TEXT('%')) return true;
	if (chChar == TEXT('/')) return true;
	if (chChar == TEXT(':')) return true;
	if (chChar == TEXT('&')) return true;

	//�ַ���Χ
	if ((chChar >= TEXT('a')) && (chChar <= TEXT('z'))) return true;
	if ((chChar >= TEXT('A')) && (chChar <= TEXT('Z'))) return true;
	if ((chChar >= TEXT('0')) && (chChar <= TEXT('9'))) return true;

	return false;
}

//��ַ�ж�
bool EfficacyUrlString(LPCTSTR pszUrl)
{
	for (WORD i = 0; i < lstrlen(pszUrl); i++)
	{
		if (!EfficacyUrlChar(pszUrl[i])) return false;
	}

	return true;
}

//�����ִ�
bool CTraceServiceControl::InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat)
{
	if (GetTextLength() >= 0x80000)//1024 * 512
	{
		OnClearAll();
	}

	//������Ϣ
	//��������
	bool bResumeSelect;
	CHARRANGE CharRange;

	//����״̬
	SetSel(-1L, -1L);
	GetSel(CharRange.cpMin, CharRange.cpMax);
	bResumeSelect = (CharRange.cpMax != CharRange.cpMin);

	//��������
	LPCTSTR pszHttp = TEXT("http://");
	const INT nHttpLength = lstrlen(pszHttp);
	const INT nStringLength = lstrlen(pszString);

	//��������
	INT nStringStart = 0;
	INT nStringPause = 0;

	//�ַ�����
	for (INT i = 0; i < nStringLength; i++)
	{
		//��������
		INT nUrlPause = i;

		//��ַ�ж�
		if (((i + nHttpLength) < nStringLength) && (memcmp(&pszString[i], pszHttp, nHttpLength*sizeof(TCHAR)) == 0))
		{
			//��������
			nUrlPause = i + nHttpLength;

			//��ַ����
			while (nUrlPause < nStringLength)
			{
				//�ַ��ж�
				if (EfficacyUrlChar(pszString[nUrlPause]))
				{
					nUrlPause++;
					continue;
				}

				break;
			}
		}

		//��ֹ�ַ�
		if (nUrlPause <= (i + nHttpLength)) nStringPause = (i + 1);

		//�����ַ�
		if ((i == (nStringLength - 1)) || (nUrlPause > (i + nHttpLength)))
		{
			//��ͨ�ַ�
			if (nStringPause > nStringStart)
			{
				//��ȡ����
				CString strNormalString;
				LPTSTR pszNormalString = strNormalString.GetBuffer(nStringPause - nStringStart + 1);

				//�����ַ�
				pszNormalString[nStringPause-nStringStart] = 0;
				CopyMemory(pszNormalString, &pszString[nStringStart], (nStringPause - nStringStart)*sizeof(TCHAR));

				//�ͷŻ���
				strNormalString.ReleaseBuffer();

				//������Ϣ
				SetSel(-1L, -1L);
				SetWordCharFormat(CharFormat);
				ReplaceSel((LPCTSTR)strNormalString);
			}

			//���ӵ�ַ
			if (nUrlPause > (i + nHttpLength))
			{
				//��ȡ����
				CString strUrlString;
				LPTSTR pszUrlString = strUrlString.GetBuffer((nUrlPause - i) + 1);

				//�����ַ�
				pszUrlString[nUrlPause-i] = 0;
				CopyMemory(pszUrlString, &pszString[i], (nUrlPause - i)*sizeof(TCHAR));

				//�ͷŻ���
				strUrlString.ReleaseBuffer();

				//�����ʽ
				CHARFORMAT2 CharFormatHyper;
				CharFormatHyper = CharFormat;

				//Ч���ַ
				if (EfficacyUrlString(strUrlString))
				{
					CharFormatHyper.dwMask |= CFM_LINK;
					CharFormatHyper.dwEffects |= CFE_LINK;
				}

				//������Ϣ
				SetSel(-1L, -1L);
				SetWordCharFormat(CharFormatHyper);
				ReplaceSel((LPCTSTR)strUrlString);
			}

			//��������
			nStringStart = __max(i, nUrlPause);
		}

		//��������
		i += (nUrlPause - i);
	}

	//״̬����
	if (bResumeSelect)
	{
		SetSel(CharRange);
	}
	else
	{
		PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	return true;
}

//�ص�����
//////////////////////////////////////////////////////////////////////////
//���ػص�
DWORD CALLBACK CTraceServiceControl::LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	return 0;
}

//����ص�
DWORD CALLBACK CTraceServiceControl::SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	return 0;
}

//�˵�����
//////////////////////////////////////////////////////////////////////////
//ɾ����Ϣ
VOID CTraceServiceControl::OnClearAll()
{
	AutoSaveLog(this);
	SetWindowText(NULL);
}

//ȫ��ѡ��
VOID CTraceServiceControl::OnSelectAll()
{
	__super::SetSel(0, __super::GetTextLength());
}

//�����ַ�
VOID CTraceServiceControl::OnCopyString()
{
	__super::Copy();
}

//������Ϣ
VOID CTraceServiceControl::OnSaveString()
{
	CFileDialog dlg(FALSE, _T("*.txt"), NULL, 4 | 2, _T("��Ϣ|*.txt||"));
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		if (file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			CString strInput;
			GetWindowText(strInput);
			file.Write(strInput, strInput.GetLength());
			file.Close();
		}
	}
}

//ɾ���ַ�
VOID CTraceServiceControl::OnDeleteString()
{
	ReplaceSel(NULL, true);
}

//��Ϣӳ��
//////////////////////////////////////////////////////////////////////////
//������Ϣ
INT CTraceServiceControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1)
	{
		return -1;
	}
	return 0;
}

//�Ҽ���Ϣ
VOID CTraceServiceControl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;
	ClientToScreen(&point);

	menu.CreatePopupMenu();

	CHARRANGE sl;
	GetSel(sl);
	menu.AppendMenu(MF_STRING | (sl.cpMax != sl.cpMin) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU0, _T("����(&C)\tCtrl+C"));
	menu.AppendMenu(MF_STRING | (GetTextLength() > 0 && sl.cpMax - sl.cpMin < GetTextLength()) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU1, _T("ȫѡ(&A)\tCtrl+A"));
	menu.AppendMenu(MF_STRING | (false) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU2, _T("ɾ��(&D)"));
	menu.AppendMenu(MF_STRING | (GetTextLength() > 0) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU3, _T("�����Ϣ"));
	menu.AppendMenu(MF_SEPARATOR, 0);
	menu.AppendMenu(MF_STRING | (GetTextLength() > 0) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU4, _T("������Ϣ..."));

	TrackPopupMenu(menu.m_hMenu, nFlags, point.x, point.y, 0, m_hWnd, NULL);
}

//׷����Ϣ
LRESULT CTraceServiceControl::OnTraceServiceMessage(WPARAM wParam, LPARAM lParam)
{
	if (g_aryTraceString.IsEmpty())
	{
		return 0;
	}
	CYSArray<tagTraceString *> ary;
	//��ȡ��ǰ׷����Ϣ
	g_CS.Lock();
	ary.Append(g_aryTraceString);
	g_aryTraceString.RemoveAll();
	g_CS.Unlock();
	
	//�����Ϣ

	//��������
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat, sizeof(CharFormat));

	//��������
	CharFormat.cbSize = sizeof(CharFormat);
	CharFormat.dwMask = CFM_COLOR | CFM_BACKCOLOR;
	CharFormat.crTextColor = RGB(0, 0, 0);
	CharFormat.crBackColor = RGB(255, 255, 255);

	StringCchCopy(CharFormat.szFaceName, CountString(CharFormat.szFaceName), TEXT("����"));

	for ( INT_PTR i=0,n=ary.GetCount(); i<n; i++ )
	{
		tagTraceString *pTraceString=ary[i];
		CharFormat.crTextColor = GetTraceColor(pTraceString->enTL);
		InsertString(pTraceString->strMsg, CharFormat);
	}

	//�ڴ����
	g_CS.Lock();
	g_aryTraceStringStorage.Append(ary);
	g_CS.Unlock();
	return 0;
}

void CTraceServiceControl::OnDestroy()
{
	AutoSaveLog(this);
	return __super::OnDestroy();
}
