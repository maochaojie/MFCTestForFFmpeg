
// MFCTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCTest.h"
#include "MFCTestDlg.h"
#include "afxdialogex.h"
#include "FFmpegPlayer.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

FFmpegPlayer player;

int status = player.init();


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


// CMFCTestDlg 对话框



CMFCTestDlg::CMFCTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_play, &CMFCTestDlg::OnBnClickedplay)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMFCTestDlg 消息处理程序

BOOL CMFCTestDlg::OnInitDialog()
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

	ShowWindow(SW_MAXIMIZE);

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCTestDlg::OnPaint()
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
HCURSOR CMFCTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int zanting = 0;


UINT Thread_Play(LPVOID lpParam) {
	CMFCTestDlg *dlg = (CMFCTestDlg *)lpParam;
	
	
	int index = player.getVideoStreamIndex();
	int audioIndex = player.getAudioStreamIndex();
	player.getPictureSize(index);
	//player.decodeToYUV(outFileName, );
	player.createWindow(dlg, IDC_SCREEN);
	player.sdlRender();
	player.createSdlTexture();
	player.decodeToscreen(index,&zanting);
	//player.decodeVideoToscreen(index, audioIndex, &zanting);
	player.releaseFFmpeg();
	player.releaseSdl();
	return 0;

	// TODO: 在此添加控件通知处理程序代码

	return 0;
}

UINT Thread_Play_Audio(LPVOID lpParam) {
	CMFCTestDlg *dlg = (CMFCTestDlg *)lpParam;
	int index = player.getAudioStreamIndex();
	player.decodeAudioToDevice(index, &zanting);
	return 0;
}


void CMFCTestDlg::OnBnClickedplay()
{
	string videoName = "F:\\VSPRO\\MFCTest\\Titanic.ts";
	//string videoName = "F:\\视频编解码\\simplest_ffmpeg_audio_player\\simplest_ffmpeg_audio_player\\WavinFlag.aac";
	if (player.openVideo(videoName) != 0)
	{

		cout << "Couldn't open input stream.\n";
		return;


	}

	if (player.getFormatInfo() != 0) {


		cout << "Couldn't find stream information.\n";

		return;


	}
	cout << player.getStream() << endl;
	
	pThreadPlay = AfxBeginThread(Thread_Play, this);//开启线程
	//pAuidoThreadPlay = AfxBeginThread(Thread_Play_Audio, this);//开启线程
}







void CMFCTestDlg::OnBnClickedButton1()
{
	
	
	zanting = !zanting;
	//string a = to_string(zanting);
	//CString b(a.c_str());
	//AfxMessageBox(b);
	return ;
	
	// TODO: 在此添加控件通知处理程序代码
}
