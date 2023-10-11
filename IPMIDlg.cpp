
// IPMIDlg.cpp: 实现文件
//

#include "framework.h"
#include "IPMI.h"
#include "IPMIDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <atlstr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString StartupPath;
CString ipmitoolPath;
CString javaPath;
CString G_ip;
CString G_user;
CString G_paw;
BOOL G_autol = FALSE;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIPMIDlg 对话框

// 从配置文件中读取指定配置项的值
CString ReadConfigValue(const CString& key)
{
	CString value;
	CStdioFile configFile;
	if (configFile.Open(_T("config.ini"), CFile::modeRead | CFile::typeText))
	{
		CString line;
		while (configFile.ReadString(line))
		{
			line.Trim(); // 去除首尾空格

			// 查找配置项名称对应的行
			if (line.Find(key) == 0)
			{
				// 提取配置项的值
				value = line.Right(line.GetLength() - key.GetLength()-1);
				value.Trim(); // 去除首尾空格
				break;
			}
		}

		configFile.Close();
	}

	return value;
}

// 将指定配置项的值写入配置文件
void WriteConfigValue(const CString& key, const CString& value)
{
	CStdioFile configFile;
	if (configFile.Open(_T("config.ini"), CFile::modeReadWrite | CFile::typeText))
	{
		CString fileContent;
		CString line;
		bool hasKey = false;

		while (configFile.ReadString(line))
		{
			line.Trim();

			if (line.Find(key) == 0)
			{
				line = key + _T("=") + value;
				hasKey = true;
			}

			fileContent += line + _T("\n");
		}

		configFile.Close();

		if (!hasKey)
		{
			fileContent += key + _T("=") + value + _T("\n");
		}

		if (configFile.Open(_T("config.ini"), CFile::modeWrite | CFile::modeCreate | CFile::typeText))
		{
			configFile.WriteString(fileContent);
			configFile.Close();
		}
	}
	else
	{
		// 如果文件不存在，则创建一个空的配置文件并写入配置项
		if (configFile.Open(_T("config.ini"), CFile::modeWrite | CFile::modeCreate | CFile::typeText))
		{
			configFile.WriteString(key + _T("=") + value + _T("\n"));
			configFile.Close();
		}
	}
}


CIPMIDlg::CIPMIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IPMI_DIALOG, pParent)
	, m_ed_ip(_T(""))
	, m_ed_user(_T(""))
	, m_ed_paw(_T(""))
	, m_ed_ss(_T(""))
	, m_appoint(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIPMIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ed_ip);
	DDX_Text(pDX, IDC_EDIT2, m_ed_user);
	DDX_Text(pDX, IDC_EDIT3, m_ed_paw);
	DDX_Text(pDX, IDC_EDIT4, m_ed_ss);
	DDX_Control(pDX, IDC_EDIT5, m_sever_info);
	DDX_Control(pDX, IDC_LIST1, m_List);

	DDX_Control(pDX, IDC_STATIC_Server_Status, m_severst);
	DDX_Control(pDX, IDC_STATIC_m_Fan, m_Fan);
	DDX_Control(pDX, IDC_STATIC_Voltage, m_Voltage);
	DDX_Control(pDX, IDC_STATIC_Consumption, m_Consumption);
	DDX_Control(pDX, IDC_STATIC_Temp, m_Temp);
	DDX_Control(pDX, IDC_STATIC_Exhaust, m_Exhaust);
	DDX_Control(pDX, IDC_STATIC_Inlet, m_Inlet);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
	DDX_Control(pDX, IDC_CHECK1, m_check);
	DDX_Control(pDX, IDC_BUTTON_sxx, m_sxx);
	DDX_Control(pDX, IDC_CHECK_AUTOLOG, m_autolog);


	DDX_Control(pDX, IDC_BUTTON_Login, m_login);
	DDX_Text(pDX, IDC_EDIT_appoint, m_appoint);
}

BEGIN_MESSAGE_MAP(CIPMIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Get_Data, &CIPMIDlg::OnBnClickedButtonGetData)
	ON_BN_CLICKED(IDC_BUTTON_Login, &CIPMIDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_Setup, &CIPMIDlg::OnBnClickedButtonSetup)
	ON_BN_CLICKED(IDC_BUTTON_Restore, &CIPMIDlg::OnBnClickedButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_apper, &CIPMIDlg::OnBnClickedButtonapper)
	ON_BN_CLICKED(IDC_BUTTON_sxx, &CIPMIDlg::OnBnClickedButtonsxx)
	ON_BN_CLICKED(IDC_BUTTON_RACCE, &CIPMIDlg::OnBnClickedButtonRacce)
	ON_BN_CLICKED(IDC_BUTTON_DELLINFO, &CIPMIDlg::OnBnClickedButtonDellinfo)
	ON_BN_CLICKED(IDC_BUTTON_BMCINFO, &CIPMIDlg::OnBnClickedButtonBmcinfo)
	ON_BN_CLICKED(IDC_BUTTON_USERINFO, &CIPMIDlg::OnBnClickedButtonUserinfo)
	ON_BN_CLICKED(IDC_BUTTON_IDRestart, &CIPMIDlg::OnBnClickedButtonIdrestart)
	ON_BN_CLICKED(IDC_BUTTON_IDRestart2, &CIPMIDlg::OnBnClickedButtonIdrestart2)
	ON_BN_CLICKED(IDC_BUTTON_USERSSEION, &CIPMIDlg::OnBnClickedButtonUsersseion)
	ON_BN_CLICKED(IDC_BUTTON_NETWORKINFO, &CIPMIDlg::OnBnClickedButtonNetworkinfo)
	ON_BN_CLICKED(IDC_BUTTON_CHANNELINFO, &CIPMIDlg::OnBnClickedButtonChannelinfo)
	ON_COMMAND(IDC_CHECK_AUTOLOG, &CIPMIDlg::OnCheckBoxClicked)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTONGet_Data1, &CIPMIDlg::OnBnClickedButtongetData1)
	ON_BN_CLICKED(IDC_BUTTON9, &CIPMIDlg::OnBnClickedButton9)
END_MESSAGE_MAP()


// CIPMIDlg 消息处理程序

UINT CIPMIDlg::ThreadEntryPoint(LPVOID pParam)
{
	// 获取 m_check 控件的指针
	CButton* pCheck = (CButton*)pParam;

	// 获取 m_check 控件的选中状态
	BOOL bChecked = pCheck->GetCheck();

	// 如果 m_check 控件被选中，则执行相应操作
	if (bChecked)
	{
		
		Sleep(5000);
		// TODO: 在这里添加 m_check 被选中时的处理逻辑
	}

	return 0;
}



BOOL CIPMIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	CString strPath;
	GetModuleFileName(NULL, strPath.GetBuffer(MAX_PATH), MAX_PATH);
	strPath.ReleaseBuffer();
	StartupPath = strPath.Left(strPath.ReverseFind('\\'));
	ipmitoolPath = StartupPath + L"\\ipmitool.exe";
	javaPath = StartupPath + L"\\jre\\bin\\java.exe";
	m_ed_ip = ReadConfigValue(L"IP");
	m_ed_user = ReadConfigValue(L"Username");
	m_ed_paw = ReadConfigValue(L"Password");
	m_ed_ss = ReadConfigValue(L"SS");

	CString m_che_autol = ReadConfigValue(L"AUTOLOG");

	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_AUTOLOG);

	if (m_che_autol == L"TRUE")
	{
		pCheckBox->SetCheck(IDC_CHECK_AUTOLOG);
		G_autol = TRUE;
	}
	else
	{
		G_autol = FALSE;
	}

	UpdateData(FALSE); // 将变量的值更新到对应的编辑框控件


	DWORD dwstyle = m_List.GetExtendedStyle();
	dwstyle |= LVS_EX_GRIDLINES;
	m_List.SetExtendedStyle(dwstyle);


	m_List.InsertColumn(0, L"探测器", LVCFMT_LEFT,100);
	m_List.InsertColumn(1, L"当前读数", LVCFMT_LEFT, 100);
	m_List.InsertColumn(2, L"单位", LVCFMT_LEFT, 80);
	m_List.InsertColumn(3, L"状态", LVCFMT_LEFT, 80);
	m_List.InsertColumn(4, L"未知", LVCFMT_LEFT, 80);
	m_List.InsertColumn(5, L"严重下限", LVCFMT_LEFT, 80);
	m_List.InsertColumn(6, L"警告下限", LVCFMT_LEFT, 80);
	m_List.InsertColumn(7, L"警告上限", LVCFMT_LEFT, 80);
	m_List.InsertColumn(8, L"故障上限", LVCFMT_LEFT, 80);

	m_combo.SetCurSel(0);

	GetDlgItem(IDC_BUTTON_Get_Data)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Setup)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Restore)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_apper)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RACCE)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_DELLINFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BMCINFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_USERINFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_IDRestart)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_IDRestart2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_USERSSEION)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_NETWORKINFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHANNELINFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_sxx)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTONGet_Data1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_APPOINT)->EnableWindow(FALSE);
	
	SetTimer(1, 500, NULL);


	
	// 创建线程
	//m_pCheck = &m_check;
	//m_pThread = AfxBeginThread(ThreadEntryPoint, (LPVOID)m_pCheck);
	//if (m_pThread == nullptr)
	//{
	//	// 处理线程创建失败的情况
	//}
	//else
	//{
	//	// 启动线程
	//	m_pThread->ResumeThread();
	//}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIPMIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIPMIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIPMIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





CString RunCommandAndGetOutput(const CString& command)
{
	CString formatSetSpeed;
	formatSetSpeed.Format(_T(" -I lanplus -H %s -U %s -P %s %s"), G_ip.GetString(), G_user.GetString(), G_paw.GetString(), command.GetString());
	
	CString fullExecuteSetSpeed = ipmitoolPath + " " + formatSetSpeed;
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;

	// 创建管道
	HANDLE hReadPipe, hWritePipe;
	if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0))
	{
		return CString(); // 创建管道失败
	}

	// 设置命令行执行信息
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);


	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE; // 隐藏窗口
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;

	// 启动cmd进程
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	//CString exePath = _T("C:\\Program Files (x86)\\Dell\\SysMgt\\bmc\\ipmitool.exe");


	BOOL bResult = CreateProcess(ipmitoolPath, (LPTSTR)(LPCTSTR)fullExecuteSetSpeed, nullptr, nullptr,
		TRUE, 0, nullptr, nullptr, &si, &pi);
	if (!bResult)
	{
		// 启动进程失败
		CloseHandle(hWritePipe);
		CloseHandle(hReadPipe);
		return CString();
	}

	// 等待命令运行完成
	WaitForSingleObject(pi.hProcess, 6000);



	// 关闭读取管道的写端
	CloseHandle(hWritePipe);

	// 读取命令输出
	CHAR buffer[4096];
	DWORD bytesRead;
	CString output;
	while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0)
	{
		buffer[bytesRead] = '\0';
		output += buffer;
	}

	// 关闭管道和进程句柄
	CloseHandle(hReadPipe);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return output;
}



void CIPMIDlg::OnBnClickedButtonGetData()
{
	CString text = RunCommandAndGetOutput(_T("sensor"));
	m_List.DeleteAllItems();

	//MessageBox(text);

	CString delimiter = _T("|");
	int start = 0;
	int end = text.Find(_T("\n"));
	while (end != -1) {
		CString row = text.Mid(start, end - start);
		int colIdx = 0;
		int pos = 0;
		CString cell = row.Tokenize(delimiter, pos);
		while (!cell.IsEmpty()) {
			// 如果是第一列，插入新行
			if (colIdx == 0) {
				m_List.InsertItem(m_List.GetItemCount(), cell);
			}
			else {
				m_List.SetItemText(m_List.GetItemCount() - 1, colIdx, cell);
			}
			cell = row.Tokenize(delimiter, pos);
			colIdx++;
		}
		start = end + 1;
		end = text.Find(_T("\n"), start);
	}

	// 最后一行
	CString row = text.Mid(start);
	int colIdx = 0;
	int pos = 0;
	CString cell = row.Tokenize(delimiter, pos);
	while (!cell.IsEmpty()) {
		if (colIdx == 0) {
			m_List.InsertItem(m_List.GetItemCount(), cell);
		}
		else {
			m_List.SetItemText(m_List.GetItemCount() - 1, colIdx, cell);
		}
		cell = row.Tokenize(delimiter, pos);
		colIdx++;
	}

	// TODO: 在此添加控件通知处理程序代码
}




void CIPMIDlg::RefreshStatus()
{
	CString title;
	CString text;
	text = RunCommandAndGetOutput(_T("power status"));

	title.Format(_T("服务器状态：%s"), text.GetString());
	m_severst.SetWindowText(title);

	text = RunCommandAndGetOutput(_T("sensor reading \"Fan1 RPM\" \"Voltage 1\" \"Pwr Consumption\" \"Temp\" \"Exhaust Temp\" \"Inlet Temp\""));

	CString fanRPM, voltage, powerConsumption, temp, exhaustTemp, inletTemp;
	CString line;

	// 将结果按行拆分成字符串数组
	CStringArray lines;
	int start = 0;
	int pos;
	while ((pos = text.Find('\n', start)) != -1) {
		line = text.Mid(start, pos - start);
		lines.Add(line);
		start = pos + 1;
	}
	if (start < text.GetLength()) {
		line = text.Mid(start);
		lines.Add(line);
	}

	// 解析每一行，并赋值给对应的变量
	for (int i = 0; i < lines.GetCount(); i++) {
		line = lines[i];
		int pos = line.Find('|');
		if (pos >= 0) {
			CString key = line.Left(pos);
			CString value = line.Mid(pos + 1);

			// 去除空格并赋值给对应变量
			key.Trim();
			value.Trim();

			if (key == "Fan1 RPM") {
				fanRPM = value;
			}
			else if (key == "Voltage 1") {
				voltage = value;
			}
			else if (key == "Pwr Consumption") {
				powerConsumption = value;
			}
			else if (key == "Temp") {
				temp = value;
			}
			else if (key == "Exhaust Temp") {
				exhaustTemp = value;
			}
			else if (key == "Inlet Temp") {
				inletTemp = value;
			}
		}
	}



	title.Format(_T("风扇转速：%sRPM"), fanRPM.GetString());
	m_Fan.SetWindowText(title);

	title.Format(_T("电源电压：%sV"), voltage.GetString());
	m_Voltage.SetWindowText(title);

	title.Format(_T("服务器功耗：%sW"), powerConsumption.GetString());
	m_Consumption.SetWindowText(title);

	title.Format(_T("CPU温度：%s°C"), temp.GetString());
	m_Temp.SetWindowText(title);

	title.Format(_T("主板温度：%s°C"), exhaustTemp.GetString());
	m_Exhaust.SetWindowText(title);

	title.Format(_T("进气温度：%s°C"), inletTemp.GetString());
	m_Inlet.SetWindowText(title);


}



void CIPMIDlg::OnBnClickedButtonLogin()
{
	GetDlgItem(IDC_BUTTON_Login)->EnableWindow(FALSE);
	m_login.SetWindowText(L"验证中");

	UpdateData(TRUE);
	G_ip = m_ed_ip.GetString();
	G_user = m_ed_user.GetString();
	G_paw = m_ed_paw.GetString();

	WriteConfigValue(L"IP", m_ed_ip.GetString());
	WriteConfigValue(L"Username", m_ed_user.GetString());
	WriteConfigValue(L"Password", m_ed_paw.GetString());
	WriteConfigValue(L"SS", m_ed_ss.GetString());

	CString text = RunCommandAndGetOutput(_T("session info active"));
	CString consoleIP;
	int ipPosition = text.Find(_T("console ip"));
	if (ipPosition != -1) {
		
		GetDlgItem(IDC_BUTTON_Get_Data)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_Setup)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_Restore)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_apper)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RACCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DELLINFO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BMCINFO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_USERINFO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_IDRestart)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_IDRestart2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_USERSSEION)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_NETWORKINFO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CHANNELINFO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_sxx)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTONGet_Data1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_APPOINT)->EnableWindow(TRUE);
		
		int colonPosition = text.Find(_T(":"), ipPosition);
		int newlinePosition = text.Find(_T("\n"), colonPosition);
		consoleIP = text.Mid(colonPosition + 2, newlinePosition - colonPosition - 2);
		consoleIP.Trim();
	}
	else
	{
		MessageBox(text+ _T("\r\n") + _T("请检查用户名或密码"));
		GetDlgItem(IDC_BUTTON_Login)->EnableWindow(TRUE);
		m_login.SetWindowText(L"登录");
		return;
	}

	CString consolePort;
	int portPosition = text.Find(_T("console port"));
	if (portPosition != -1) {
		int colonPosition = text.Find(_T(":"), portPosition);
		int newlinePosition = text.Find(_T("\n"), colonPosition);
		consolePort = text.Mid(colonPosition + 2, newlinePosition - colonPosition - 2);
		consolePort.Trim();
	}

	CString title;
	GetWindowText(title);
	title += _T("           -----     登录IP：" + consoleIP + "	" + "         登录端口：" + consolePort);
	SetWindowText(title);

	text = RunCommandAndGetOutput(_T("power status"));



	text = RunCommandAndGetOutput(_T("delloem sysinfo"));
	text.Replace(_T("\n"), _T("\r\n"));
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	pEdit->SetWindowText(text);

	RefreshStatus();
	m_login.SetWindowText(L"已登录");
}


void CIPMIDlg::OnBnClickedButtonSetup()
{
	UpdateData(TRUE);
	CString l_ss = m_ed_ss.GetString();
	CString text;
	CString hexString;
	hexString.Format(_T("raw 0x30 0x30 0x02 0xff 0x%X"), int(_ttoi(l_ss)));
	text = RunCommandAndGetOutput(_T("raw 0x30 0x30 0x01 0x00"));
	text = RunCommandAndGetOutput(hexString);
	
}


void CIPMIDlg::OnBnClickedButtonRestore()
{
	CString text;
	text = RunCommandAndGetOutput(_T("raw 0x30 0x30 0x01 0x01"));
}


void CIPMIDlg::OnBnClickedButtonapper()
{

	CString text;
	int l_ss = m_combo.GetCurSel();

	m_combo.GetLBText(l_ss, text);
	int result = MessageBox(_T("是否进行 ") + text, _T("确认"), MB_OKCANCEL);

	if (result == IDOK) {
		if (l_ss == 0)
			text = RunCommandAndGetOutput(_T("chassis power on"));
		if (l_ss == 1)
			text = RunCommandAndGetOutput(_T("chassis power soft"));
		if (l_ss == 2)
			text = RunCommandAndGetOutput(_T("chassis power off"));
		if (l_ss == 3)
			text = RunCommandAndGetOutput(_T("chassis power reset"));
		if (l_ss == 3)
			text = RunCommandAndGetOutput(_T("chassis power cycle"));

		MessageBox(text);
	}
	else if (result == IDCANCEL) {
		return;
	}

}


void CIPMIDlg::OnBnClickedButtonsxx()
{

	RefreshStatus();

}

void CIPMIDlg::OnBnClickedButtonRacce()
{


	CString jarFilePath = StartupPath + _T("\\avctKVM.jar");
	if (_waccess(jarFilePath, 0) == -1)
	{
		CString command;





		command.Format(_T("curl -o \"%s\\avctKVM.jar\" https://%s:443/software/avctKVM.jar --insecure"), StartupPath.GetString(), G_ip.GetString());
		STARTUPINFO si = { sizeof(si) };
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		PROCESS_INFORMATION pi;
		if (CreateProcess(NULL, command.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
	if (_waccess(jarFilePath, 0) == -1)
	{
		MessageBox(_T("文件下载失败 请手动下载"));
	}
	CString command;
	command.Format(_T("-cp avctKVM.jar -Djava.library.path=lib com.avocent.idrac.kvm.Main ip=%s vm=%d title=%s kmport=%d vport=%d user=%s passwd=%s apcp=%d reconnect=%d chat=%d F1=%d custom=%d scaling=%d minwinheight=%d minwinwidth=%d videoborder=%d version=%d"),
		G_ip.GetString(), 1, _T("A_lot"), 5900, 5900, G_user.GetString(), G_paw.GetString(), 1, 2, 1, 1, 0, 15, 100, 100, 0, 2);

	CString fullExecuteSetSpeed = javaPath + " " + command;

	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	if (CreateProcess(javaPath, (LPTSTR)(LPCTSTR)fullExecuteSetSpeed, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		// 关闭主程序对新进程的等待
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

}


void CIPMIDlg::OnBnClickedButtonIdrestart()
{
	CString text;
	text = RunCommandAndGetOutput(_T("bmc reset cold"));
	MessageBox(text);
}


void CIPMIDlg::OnBnClickedButtonIdrestart2()
{
	CString text;
	text = RunCommandAndGetOutput(_T("bmc reset warmBMC"));
	MessageBox(text);
}


void CIPMIDlg::OnBnClickedButtonDellinfo()
{
	CString text;
	text = RunCommandAndGetOutput(_T("delloem sysinfo"));
	text.Replace(_T("\n"), _T("\r\n"));
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	pEdit->SetWindowText(text);
}


void CIPMIDlg::OnBnClickedButtonBmcinfo()
{
	CString text;
	text = RunCommandAndGetOutput(_T("mc info"));
	text.Replace(_T("\n"), _T("\r\n"));
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	pEdit->SetWindowText(text);
}


void CIPMIDlg::OnBnClickedButtonUserinfo()
{
	CString text;
	text = RunCommandAndGetOutput(_T("user list 1"));
	text.Replace(_T("\n"), _T("\r\n"));
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	pEdit->SetWindowText(text);
}




void CIPMIDlg::OnBnClickedButtonUsersseion()
{
	CString text;
	text = RunCommandAndGetOutput(_T("session info active"));
	text.Replace(_T("\n"), _T("\r\n"));
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	pEdit->SetWindowText(text);
}


void CIPMIDlg::OnBnClickedButtonNetworkinfo()
{
	CString text;
	text = RunCommandAndGetOutput(_T("lan print 1"));
	text.Replace(_T("\n"), _T("\r\n"));
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	pEdit->SetWindowText(text);
}


void CIPMIDlg::OnBnClickedButtonChannelinfo()
{
	CString text;
	text = RunCommandAndGetOutput(_T("channel info"));
	text.Replace(_T("\n"), _T("\r\n"));
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	pEdit->SetWindowText(text);
}

void CIPMIDlg::OnCheckBoxClicked()
{
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_AUTOLOG);
	BOOL bChecked = pCheckBox->GetCheck();
	if (bChecked == BST_CHECKED)
	{
		WriteConfigValue(L"AUTOLOG", L"TRUE");
	}
	else
	{
		WriteConfigValue(L"AUTOLOG", L"FALSE");
	}

}

void CIPMIDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		// 关闭计时器
		KillTimer(1);
		if (G_autol ==TRUE) 
		{
			OnBnClickedButtonLogin();

		}

		// 执行您需要在对话框显示完成后执行的代码
		// ...
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CIPMIDlg::OnBnClickedButtongetData1()
{
	CString text = RunCommandAndGetOutput(_T("sdr list full"));
	m_List.DeleteAllItems();

	//MessageBox(text);

	CString delimiter = _T("|");
	int start = 0;
	int end = text.Find(_T("\n"));
	while (end != -1) {
		CString row = text.Mid(start, end - start);
		int colIdx = 0;
		int pos = 0;
		CString cell = row.Tokenize(delimiter, pos);
		while (!cell.IsEmpty()) {
			// 如果是第一列，插入新行
			if (colIdx == 0) {
				m_List.InsertItem(m_List.GetItemCount(), cell);
			}
			else {
				m_List.SetItemText(m_List.GetItemCount() - 1, colIdx, cell);
			}
			cell = row.Tokenize(delimiter, pos);
			colIdx++;
		}
		start = end + 1;
		end = text.Find(_T("\n"), start);
	}

	// 最后一行
	CString row = text.Mid(start);
	int colIdx = 0;
	int pos = 0;
	CString cell = row.Tokenize(delimiter, pos);
	while (!cell.IsEmpty()) {
		if (colIdx == 0) {
			m_List.InsertItem(m_List.GetItemCount(), cell);
		}
		else {
			m_List.SetItemText(m_List.GetItemCount() - 1, colIdx, cell);
		}
		cell = row.Tokenize(delimiter, pos);
		colIdx++;
	}
}


void CIPMIDlg::OnBnClickedButton9()
{
	UpdateData(TRUE);
	CString text;
	text = RunCommandAndGetOutput(m_appoint.GetString());
	MessageBox(text);
	UpdateData(FALSE); // 将变量的值更新到对应的编辑框控件
}
