
// IPMIDlg.h: 头文件
//

#pragma once


// CIPMIDlg 对话框
class CIPMIDlg : public CDialogEx
{
// 构造
public:
	CIPMIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPMI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	CWinThread* m_pThread;  // 线程对象指针
	CButton* m_pCheck;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_ed_ip;
	CString m_ed_user;
	CString m_ed_paw;
	CString m_ed_ss;
	CListCtrl m_List;
	CStatic m_severst;
	CEdit m_sever_info;
	CStatic m_Fan;
	CStatic m_Voltage;
	CStatic m_Consumption;
	CStatic m_Temp;
	CStatic m_Exhaust;
	CStatic m_Inlet;
	CComboBox m_combo;
	CButton m_sxx;
	CButton m_check;
	CButton m_autolog;
	CButton m_login;
	afx_msg void OnBnClickedButtonGetData();
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonSetup();
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnBnClickedButtonapper();
	afx_msg void RefreshStatus();

	static UINT ThreadEntryPoint(LPVOID pParam);  // 线程入口函数
	
	afx_msg void OnBnClickedButtonsxx();
	afx_msg void OnBnClickedButtonRacce();
	afx_msg void OnBnClickedButtonDellinfo();
	afx_msg void OnBnClickedButtonBmcinfo();
	afx_msg void OnBnClickedButtonUserinfo();
	afx_msg void OnBnClickedButtonIdrestart();
	afx_msg void OnBnClickedButtonIdrestart2();
	afx_msg void OnBnClickedButtonUsersseion();
	afx_msg void OnBnClickedButtonNetworkinfo();
	afx_msg void OnBnClickedButtonChannelinfo();




	afx_msg void OnCheckBoxClicked();


	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	afx_msg void OnBnClickedButtongetData1();
	CString m_appoint;
	afx_msg void OnBnClickedButton9();
};
