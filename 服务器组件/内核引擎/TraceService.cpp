#include "StdAfx.h"
#include "TraceService.h" // #include "EventService.h"
#include <strsafe.h>

//////////////////////////////////////////////////////////////////////////

//颜色定义
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
//追踪服务
//////////////////////////////////////////////////////////////////////////
static ITraceService				*g_pITraceService = NULL;
static bool							g_bEnableTrace=true;
static CYSArray<tagTraceString *>	g_aryTraceString;
static CYSArray<tagTraceString *>	g_aryTraceStringStorage;
static CCriticalSection				g_CS;
//函数定义
//////////////////////////////////////////////////////////////////////////
//构造函数
CTraceService::CTraceService()
{

}

//服务配置
//////////////////////////////////////////////////////////////////////////
//设置服务
bool CTraceService::SetTraceService(IUnknownEx * pIUnknownEx)
{
	g_pITraceService = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITraceService);
	return NULL != g_pITraceService;
}

//获取服务
VOID * CTraceService::GetTraceService(const IID & Guid, DWORD dwQueryVer)
{
	ASSERT(g_pITraceService);
	if (g_pITraceService) return NULL;

	return (VOID*)g_pITraceService->QueryInterface(Guid, dwQueryVer);
}

//获取服务
VOID * CTraceService::GetTraceServiceManager(const IID & Guid, DWORD dwQueryVer)
{
	return NULL;
}

//状态管理
//////////////////////////////////////////////////////////////////////////
//追踪状态
bool CTraceService::IsEnableTrace(enTraceLevel TraceLevel)
{
	return false;
}

//追踪控制
bool CTraceService::EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace)
{
	return false;
}
//追踪控制
bool CTraceService::EnableAllTrace( bool bEnableTrace )
{
	if (g_bEnableTrace!=bEnableTrace)
	{
		g_bEnableTrace=bEnableTrace;
		return true;
	}
	return false;
}
//功能函数
//////////////////////////////////////////////////////////////////////////
//追踪信息
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
//追踪服务
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

//函数定义

//写入日志
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
			strFileName.Replace(_T("[加载成功]"),_T(""));
			strFileName.Replace(_T("[加载失败]"),_T(""));
			strFileName.Replace(_T("[停止]"),_T(""));
		}
		//获取时间
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
//构造函数
CTraceServiceControl::CTraceServiceControl()
{
	CTraceService::SetTraceService((IUnknownEx*)QueryInterface(IID_IUnknownEx, VER_IUnknownEx));
}

//析构函数
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

//基础接口
//////////////////////////////////////////////////////////////////////////
//接口查询
VOID * CTraceServiceControl::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITraceService, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITraceService, Guid, dwQueryVer);
	return NULL;
}

//信息接口
//////////////////////////////////////////////////////////////////////////
//追踪信息
bool CTraceServiceControl::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//效验参数
	ITraceService *pITraceService = QUERY_ME_INTERFACE(ITraceService);
	VERIFY_RETURN_FALSE(pITraceService != NULL);
	//构造变量
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
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	//生成数据
	pTraceString->enTL=TraceLevel;
	pTraceString->strMsg.Format(TEXT("【 %04d-%02d-%02d %02d:%02d:%02d 】%s\n"), SystemTime.wYear, \
		SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond,pszString);
	//添加到队列中
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

//重载函数
//////////////////////////////////////////////////////////////////////////
//绑定函数
VOID CTraceServiceControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	CTraceService::SetTraceService((IUnknownEx*)QueryInterface(IID_IUnknownEx, VER_IUnknownEx));
}

//功能函数
//////////////////////////////////////////////////////////////////////////
//加载消息
bool CTraceServiceControl::LoadMessage(LPCTSTR pszFileName)
{
	if (static_cast<CTraceServiceControl*>(g_pITraceService) == NULL) return false;
	return static_cast<CTraceServiceControl*>(g_pITraceService)->LoadMessage(pszFileName);
}

//保存信息
bool CTraceServiceControl::SaveMessage(LPCTSTR pszFileName)
{
	if (static_cast<CTraceServiceControl*>(g_pITraceService) == NULL) return false;
	return static_cast<CTraceServiceControl*>(g_pITraceService)->SaveMessage(pszFileName);
}

//设置参数
bool CTraceServiceControl::SetParameter(LONG lMaxLineCount, LONG lReserveLineCount)
{
	m_lMaxLineCount = lMaxLineCount;					//最大行数
	m_lReserveLineCount = lReserveLineCount;			//保留行数

	return true;
}

//辅助函数
//////////////////////////////////////////////////////////////////////////
//配置服务
VOID CTraceServiceControl::InitializeService()
{

}

//获取颜色
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

//字符判断
bool EfficacyUrlChar(TCHAR chChar)
{
	//特殊字符
	if (chChar == TEXT('.')) return true;
	if (chChar == TEXT('=')) return true;
	if (chChar == TEXT('+')) return true;
	if (chChar == TEXT('?')) return true;
	if (chChar == TEXT('#')) return true;
	if (chChar == TEXT('%')) return true;
	if (chChar == TEXT('/')) return true;
	if (chChar == TEXT(':')) return true;
	if (chChar == TEXT('&')) return true;

	//字符范围
	if ((chChar >= TEXT('a')) && (chChar <= TEXT('z'))) return true;
	if ((chChar >= TEXT('A')) && (chChar <= TEXT('Z'))) return true;
	if ((chChar >= TEXT('0')) && (chChar <= TEXT('9'))) return true;

	return false;
}

//地址判断
bool EfficacyUrlString(LPCTSTR pszUrl)
{
	for (WORD i = 0; i < lstrlen(pszUrl); i++)
	{
		if (!EfficacyUrlChar(pszUrl[i])) return false;
	}

	return true;
}

//插入字串
bool CTraceServiceControl::InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat)
{
	if (GetTextLength() >= 0x80000)//1024 * 512
	{
		OnClearAll();
	}

	//插入消息
	//变量定义
	bool bResumeSelect;
	CHARRANGE CharRange;

	//保存状态
	SetSel(-1L, -1L);
	GetSel(CharRange.cpMin, CharRange.cpMax);
	bResumeSelect = (CharRange.cpMax != CharRange.cpMin);

	//搜索变量
	LPCTSTR pszHttp = TEXT("http://");
	const INT nHttpLength = lstrlen(pszHttp);
	const INT nStringLength = lstrlen(pszString);

	//索引定义
	INT nStringStart = 0;
	INT nStringPause = 0;

	//字符解释
	for (INT i = 0; i < nStringLength; i++)
	{
		//变量定义
		INT nUrlPause = i;

		//地址判断
		if (((i + nHttpLength) < nStringLength) && (memcmp(&pszString[i], pszHttp, nHttpLength*sizeof(TCHAR)) == 0))
		{
			//设置索引
			nUrlPause = i + nHttpLength;

			//地址搜索
			while (nUrlPause < nStringLength)
			{
				//字符判断
				if (EfficacyUrlChar(pszString[nUrlPause]))
				{
					nUrlPause++;
					continue;
				}

				break;
			}
		}

		//终止字符
		if (nUrlPause <= (i + nHttpLength)) nStringPause = (i + 1);

		//插入字符
		if ((i == (nStringLength - 1)) || (nUrlPause > (i + nHttpLength)))
		{
			//普通字符
			if (nStringPause > nStringStart)
			{
				//获取缓冲
				CString strNormalString;
				LPTSTR pszNormalString = strNormalString.GetBuffer(nStringPause - nStringStart + 1);

				//拷贝字符
				pszNormalString[nStringPause-nStringStart] = 0;
				CopyMemory(pszNormalString, &pszString[nStringStart], (nStringPause - nStringStart)*sizeof(TCHAR));

				//释放缓冲
				strNormalString.ReleaseBuffer();

				//插入消息
				SetSel(-1L, -1L);
				SetWordCharFormat(CharFormat);
				ReplaceSel((LPCTSTR)strNormalString);
			}

			//连接地址
			if (nUrlPause > (i + nHttpLength))
			{
				//获取缓冲
				CString strUrlString;
				LPTSTR pszUrlString = strUrlString.GetBuffer((nUrlPause - i) + 1);

				//拷贝字符
				pszUrlString[nUrlPause-i] = 0;
				CopyMemory(pszUrlString, &pszString[i], (nUrlPause - i)*sizeof(TCHAR));

				//释放缓冲
				strUrlString.ReleaseBuffer();

				//构造格式
				CHARFORMAT2 CharFormatHyper;
				CharFormatHyper = CharFormat;

				//效验地址
				if (EfficacyUrlString(strUrlString))
				{
					CharFormatHyper.dwMask |= CFM_LINK;
					CharFormatHyper.dwEffects |= CFE_LINK;
				}

				//插入消息
				SetSel(-1L, -1L);
				SetWordCharFormat(CharFormatHyper);
				ReplaceSel((LPCTSTR)strUrlString);
			}

			//设置索引
			nStringStart = __max(i, nUrlPause);
		}

		//设置索引
		i += (nUrlPause - i);
	}

	//状态设置
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

//回调函数
//////////////////////////////////////////////////////////////////////////
//加载回调
DWORD CALLBACK CTraceServiceControl::LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	return 0;
}

//保存回调
DWORD CALLBACK CTraceServiceControl::SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	return 0;
}

//菜单命令
//////////////////////////////////////////////////////////////////////////
//删除信息
VOID CTraceServiceControl::OnClearAll()
{
	AutoSaveLog(this);
	SetWindowText(NULL);
}

//全部选择
VOID CTraceServiceControl::OnSelectAll()
{
	__super::SetSel(0, __super::GetTextLength());
}

//拷贝字符
VOID CTraceServiceControl::OnCopyString()
{
	__super::Copy();
}

//保存信息
VOID CTraceServiceControl::OnSaveString()
{
	CFileDialog dlg(FALSE, _T("*.txt"), NULL, 4 | 2, _T("信息|*.txt||"));
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

//删除字符
VOID CTraceServiceControl::OnDeleteString()
{
	ReplaceSel(NULL, true);
}

//消息映射
//////////////////////////////////////////////////////////////////////////
//建立消息
INT CTraceServiceControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1)
	{
		return -1;
	}
	return 0;
}

//右键消息
VOID CTraceServiceControl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;
	ClientToScreen(&point);

	menu.CreatePopupMenu();

	CHARRANGE sl;
	GetSel(sl);
	menu.AppendMenu(MF_STRING | (sl.cpMax != sl.cpMin) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU0, _T("复制(&C)\tCtrl+C"));
	menu.AppendMenu(MF_STRING | (GetTextLength() > 0 && sl.cpMax - sl.cpMin < GetTextLength()) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU1, _T("全选(&A)\tCtrl+A"));
	menu.AppendMenu(MF_STRING | (false) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU2, _T("删除(&D)"));
	menu.AppendMenu(MF_STRING | (GetTextLength() > 0) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU3, _T("清除信息"));
	menu.AppendMenu(MF_SEPARATOR, 0);
	menu.AppendMenu(MF_STRING | (GetTextLength() > 0) ? 0 : MF_DISABLED | MF_GRAYED, IDM_MENU4, _T("保存信息..."));

	TrackPopupMenu(menu.m_hMenu, nFlags, point.x, point.y, 0, m_hWnd, NULL);
}

//追踪消息
LRESULT CTraceServiceControl::OnTraceServiceMessage(WPARAM wParam, LPARAM lParam)
{
	if (g_aryTraceString.IsEmpty())
	{
		return 0;
	}
	CYSArray<tagTraceString *> ary;
	//获取当前追踪信息
	g_CS.Lock();
	ary.Append(g_aryTraceString);
	g_aryTraceString.RemoveAll();
	g_CS.Unlock();
	
	//输出信息

	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat, sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize = sizeof(CharFormat);
	CharFormat.dwMask = CFM_COLOR | CFM_BACKCOLOR;
	CharFormat.crTextColor = RGB(0, 0, 0);
	CharFormat.crBackColor = RGB(255, 255, 255);

	StringCchCopy(CharFormat.szFaceName, CountString(CharFormat.szFaceName), TEXT("宋体"));

	for ( INT_PTR i=0,n=ary.GetCount(); i<n; i++ )
	{
		tagTraceString *pTraceString=ary[i];
		CharFormat.crTextColor = GetTraceColor(pTraceString->enTL);
		InsertString(pTraceString->strMsg, CharFormat);
	}

	//内存回收
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
