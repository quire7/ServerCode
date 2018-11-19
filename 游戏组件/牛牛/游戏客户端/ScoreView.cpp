#include "StdAfx.h"
#include "ScoreView.h"
#include ".\scoreview.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CScoreView::CScoreView() : CDialog(IDD_GAME_SCORE)
{
	//��������
	ZeroMemory(m_lTax,sizeof(m_lTax));
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//������Դ
	m_ImageBack.LoadFromResource(AfxGetInstanceHandle(),IDB_SCORE_VIEW);
	m_ImageWinLose.LoadFromResource(AfxGetInstanceHandle(),IDB_WIN_LOSE);

	return;
}

//��������
CScoreView::~CScoreView()
{
}
//�ؼ���
void CScoreView::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

}
//������Ϣ
BOOL CScoreView::OnInitDialog()
{
	__super::OnInitDialog();

	//ɾ�����ڱ���ͻ�ȡ����
	ModifyStyle(WS_CAPTION,0,0);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//��������
	CRgn RgnControl;
	m_ImageBack.CreateImageRegion(RgnControl,RGB(255,0,255));

	//���ô���
	if (RgnControl.GetSafeHandle()!=NULL)
	{
		//�ƶ�����
		SetWindowRgn(RgnControl,TRUE);
		SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);
	}
	//����͸��
	ModifyStyleEx(0,0x00080000);
	HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL")); 
	if(hInst) 
	{ 
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//ȡ��SetLayeredWindowAttributes��������ָ�� 
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)fun(this->GetSafeHwnd(),0,220,2); 
		FreeLibrary(hInst); 
	}

	return 0;
}

//�����Ϣ
void CScoreView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//��Ϣģ��
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//�ػ�����
void CScoreView::OnPaint() 
{
	CPaintDC dc(this); 


	//�滭����
	m_ImageBack.TransDrawImage(&dc,0,0,RGB(255,0,255));

	//���� DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	//dc.SelectObject(m_pSkinResourceManager->GetDefaultFont());

	//��ʾ��Ϣ
	TCHAR szBuffer[128]=TEXT("");

	//��ʾ����
	INT nWinLoseWidth = m_ImageWinLose.GetWidth()/3;
	INT nWinLostHeight = m_ImageWinLose.GetHeight();
	WORD wCount = 0;
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		if (m_szUserName[i][0]!=0)
		{
			//�������
			INT nScPos = 80;
			CRect rcName(26,wCount*29+nScPos,126,(wCount+1)*29+nScPos);
			dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcName,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//����ɼ�
			CRect rcScore(95,wCount*29+nScPos,195,(wCount+1)*29+nScPos);
			_sntprintf(szBuffer,sizeof(szBuffer),SCORE_STRING,m_lScore[i]);
			dc.DrawText(szBuffer,lstrlen(szBuffer),&rcScore,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//ʤ����־
			m_ImageWinLose.TransDrawImage(&dc,218-nWinLoseWidth/2,wCount*29+nScPos+13-nWinLostHeight/2,nWinLoseWidth,nWinLostHeight,
				(m_lScore[i]>0)?0:((m_lScore[i]<0)?nWinLoseWidth:nWinLoseWidth*2),0,RGB(255,0,255));

			//���˰��
			CRect rcTax(235,wCount*29+nScPos,335,(wCount+1)*29+nScPos);
			_sntprintf(szBuffer,sizeof(szBuffer),SCORE_STRING,m_lTax[i]);
			dc.DrawText(szBuffer,lstrlen(szBuffer),&rcTax,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			
			wCount++;
		}
	}

	return;
}

//���û���
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore)
{
	if (wChairID<CountArray(m_lScore))
	{
		m_lScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		Invalidate(FALSE);
	}
	return;
}

//����˰��
void CScoreView::SetGameTax(LONGLONG lTax,WORD wChairID)
{
	m_lTax[wChairID]=lTax;

	return;
}

//��������
void CScoreView::ResetScore()
{
	//��������
	ZeroMemory(m_lTax,sizeof(m_lTax));
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	return;
}

//////////////////////////////////////////////////////////////////////////

BOOL CScoreView::OnEraseBkgnd(CDC* pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

