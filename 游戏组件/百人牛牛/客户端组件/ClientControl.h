#pragma once

//游戏控制基类
class IClientControlDlg : public CDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

public:
	//更新控制
	virtual void  OnAllowControl(bool bEnable) = NULL;
	//申请结果
	virtual bool  ReqResult(const void * pBuffer) = NULL;
	//更新库存
	virtual bool  UpdateStorage(const void * pBuffer) = NULL;
	//更新控件
	virtual void  UpdateControl() = NULL;
};
