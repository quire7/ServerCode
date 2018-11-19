#ifndef DLG_SERVER_WIZARD_HEAD_FILE
#define DLG_SERVER_WIZARD_HEAD_FILE

#pragma once

#include "ModuleListControl.h"
#include "ModuleInfoManager.h"
#include "ServerInfoManager.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//类说明
class CDlgServerWizard;

//////////////////////////////////////////////////////////////////////////////////

//配置基类
class MODULE_MANAGER_CLASS CDlgServerOptionItem : public CDialog
{
	//友元定义
	friend class CDlgServerWizardItem2;

	//变量定义
protected:
	CDlgServerWizard *				m_pDlgServerWizard;					//向导指针
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//函数定义
protected:
	//构造函数
	CDlgServerOptionItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgServerOptionItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//功能函数
private:
	//保存数据
	bool SaveItemData();
	//显示配置
	bool ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//房间配置
class MODULE_MANAGER_CLASS CDlgServerOptionItem1 : public CDlgServerOptionItem
{
	//函数定义
public:
	//构造函数
	CDlgServerOptionItem1();
	//析构函数
	virtual ~CDlgServerOptionItem1();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//功能函数
protected:
	//构造控件
	VOID InitCtrlWindow();
	//更新数据
	VOID UpdateDataBaseName();

	//控件函数
protected:
	//选择改变
	VOID OnSelchangeServerType();
	//选择改变
	VOID OnSelchangeServerKind();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRevenueRadio();
	afx_msg void OnBnClickedServiceRadio();
};

//////////////////////////////////////////////////////////////////////////////////

//房间配置
class MODULE_MANAGER_CLASS CDlgServerOptionItem2 : public CDlgServerOptionItem
{
	//函数定义
public:
	//构造函数
	CDlgServerOptionItem2();
	//析构函数
	virtual ~CDlgServerOptionItem2();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//功能函数
protected:
	//构造控件
	VOID InitCtrlWindow();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//房间配置
class MODULE_MANAGER_CLASS CDlgServerOptionItem3 : public CDlgServerOptionItem
{
	//函数定义
public:
	//构造函数
	CDlgServerOptionItem3();
	//析构函数
	virtual ~CDlgServerOptionItem3();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////
//房间配置
class MODULE_MANAGER_CLASS CDlgServerOptionItem4 : public CDlgServerOptionItem
{
	//变量定义
protected:
	int							m_nSelectItem;						//选择索引

	//控件变量
protected:
	CComboBox					m_cbEnterHour;						//进入时间
	CComboBox					m_cbEnterMinute;					//进入时间
	CComboBox					m_cbEnterSecond;					//进入时间
	CComboBox					m_cbLeaveHour;						//离开时间
	CComboBox					m_cbLeaveMinute;					//离开时间
	CComboBox					m_cbLeaveSecond;					//离开时间

	//控件变量
protected:
	CEdit						m_edtAndroidCount;					//机器数目
	CEdit						m_edtTableScoreMin;					//携带分数
	CEdit						m_edtTableScoreMax;					//携带分数
	CEdit						m_edtEnterIntervalMin;				//进入间隔
	CEdit						m_edtEnterIntervalMax;				//进入间隔
	CEdit						m_edtLeaveIntervalMin;				//离开间隔
	CEdit						m_edtLeaveIntervalMax;				//离开间隔
	CEdit						m_edtSwitchTableMin;				//换桌局数
	CEdit						m_edtSwitchTableMax;				//换桌局数

	//按钮控件
protected:
	CButton						m_btAddItem;						//添加子项
	CButton						m_btModifyItem;						//修改子项
	CButton						m_btDeleteItem;						//删除子项

	//控件变量
protected:
	CListCtrl					m_ParemeterList;					//参数列表

	//函数定义
public:
	//构造函数
	CDlgServerOptionItem4();
	//析构函数
	virtual ~CDlgServerOptionItem4();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//功能函数
public:
	//添加修改
	VOID AddModifyParameter(bool bModify);
	//选中子项
	VOID SelectItem(int nIndex);
	//设置子项
	VOID SetItemToParameterList(int nItemIndex, tagAndroidParameter * pAndroidParameter);
	//机器参数
	VOID OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagAndroidParameter * pAndroidParameter);

	//消息函数
protected:
	//单击子项
	VOID OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	//删除子项
	VOID OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	//键盘按下
	VOID OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult);

	//事件函数
protected:
	//添加子项
	VOID OnBnClickedBtAdd();
	//编辑子项
	VOID OnBnClickedBtModify();
	//删除子项
	VOID OnBnClickedBtDelete();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//定制配置
class MODULE_MANAGER_CLASS CDlgServerOptionItemCustom : public CDlgServerOptionItem
{
	//控件变量
protected:
	HWND							m_hCustomRule;						//定制规则
	IGameServiceCustomRule *		m_pIGameServiceCustomRule;			//自定配置

	//函数定义
public:
	//构造函数
	CDlgServerOptionItemCustom();
	//析构函数
	virtual ~CDlgServerOptionItemCustom();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//消息函数
private:
	//消耗消息
	VOID OnNcDestroy();
	//焦点消息
	VOID OnSetFocus(CWnd * pNewWnd);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//向导基类
class MODULE_MANAGER_CLASS CDlgServerWizardItem : public CDialog
{
	//友元定义
	friend class CDlgServerWizard;

	//变量定义
protected:
	CDlgServerWizard *				m_pDlgServerWizard;					//向导指针
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//函数定义
protected:
	//构造函数
	CDlgServerWizardItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgServerWizardItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//功能函数
private:
	//保存数据
	bool SaveItemData();
	//创建向导
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//游戏选择
class MODULE_MANAGER_CLASS CDlgServerWizardItem1 : public CDlgServerWizardItem
{
	//友元定义
	friend class CDlgServerOptionItem1;

	//列表变量
protected:
	CStatic							m_StaticPrompt;						//提示控件
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//模块信息
	CModuleListControl				m_ModuleListControl;				//模块列表
	CModuleInfoManager				m_ModuleInfoManager;				//模块管理

	//服务组件
protected:
	CGameServiceManagerHelper		m_GameServiceManager;				//游戏模块

	//函数定义
public:
	//构造函数
	CDlgServerWizardItem1();
	//析构函数
	virtual ~CDlgServerWizardItem1();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//功能函数
protected:
	//加载模块
	bool LoadDBModuleItem();

	//控件消息
protected:
	//双击列表
	VOID OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//房间配置
class MODULE_MANAGER_CLASS CDlgServerWizardItem2 : public CDlgServerWizardItem
{
	//变量定义
protected:
	WORD							m_wItemCount;						//子项数目
	WORD							m_wActiveIndex;						//活动索引
	CDlgServerOptionItem *			m_pOptionItem[5];					//向导指针

	//控件变量
protected:
	CTabCtrl						m_TabCtrl;							//选择控件
	CDlgServerOptionItem1			m_ServerOptionItem1;				//设置步骤
	CDlgServerOptionItem2			m_ServerOptionItem2;				//设置步骤
	CDlgServerOptionItem3			m_ServerOptionItem3;				//设置步骤
	CDlgServerOptionItem4			m_ServerOptionItem4;				//设置步骤
	CDlgServerOptionItemCustom		m_ServerOptionItemCustom;			//定制配置

	//函数定义
public:
	//构造函数
	CDlgServerWizardItem2();
	//析构函数
	virtual ~CDlgServerWizardItem2();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//功能函数
public:
	//激活配置
	bool ActiveOptionItem(WORD wIndex);

	//控件消息
protected:
	//焦点消息
	VOID OnSetFocus(CWnd * pNewWnd);
	//选择改变
	VOID OnTcnSelchangeTabCtrl(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//创建向导
class MODULE_MANAGER_CLASS CDlgServerWizard : public CDialog
{
	//友元定义
	friend class CDlgServerWizardItem2;
	friend class CDlgServerOptionItem1;
	friend class CDlgServerOptionItemCustom;

	//变量定义
protected:
	WORD							m_wActiveIndex;						//活动索引
	CDlgServerWizardItem *			m_pWizardItem[2];					//向导指针

	//配置信息
public:
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//控件变量
protected:
	CDlgServerWizardItem1			m_ServerWizardItem1;				//设置步骤
	CDlgServerWizardItem2			m_ServerWizardItem2;				//设置步骤

	//接口变量
protected:
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomRule *		m_pIGameServiceCustomRule;			//自定配置

	//函数定义
public:
	//构造函数
	CDlgServerWizard();
	//析构函数
	virtual ~CDlgServerWizard();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//配置函数
public:
	//创建房间
	bool CreateGameServer();
	//设置配置
	VOID SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption);

	//辅助函数
private:
	//激活向导
	bool ActiveWizardItem(WORD wIndex);

	//消息映射
public:
	//上一步
	VOID OnBnClickedLast();
	//下一步
	VOID OnBnClickedNext();
	//完成按钮
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif