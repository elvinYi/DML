
// dml_sdk_demoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "dml_sdk_demo.h"
#include "dml_sdk_demoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cdml_sdk_demoDlg �Ի���

#define WM_USER_SDK_MSG WM_USER+1000

Cdml_sdk_demoDlg::Cdml_sdk_demoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cdml_sdk_demoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cdml_sdk_demoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA1, cmbCamera1);
	DDX_Control(pDX, IDC_BTN_CAMERA1_START, btnCamera1Start);
	DDX_Control(pDX, IDC_EDIT_IP, edtSeverIP);
	DDX_Control(pDX, IDC_EDIT_PORT, edtSeverPort);
	DDX_Control(pDX, IDC_EDIT_SSRC, edtSSRC);
	DDX_Control(pDX, IDC_EDIT_MEMID, edtMemberID);
	DDX_Control(pDX, IDC_EDT_PUSHID, edtPushId);
}

BEGIN_MESSAGE_MAP(Cdml_sdk_demoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE_VOID(WM_CLOSE, OnClose)
	ON_MESSAGE_VOID(WM_USER_SDK_MSG, OnSdkMsg)
	ON_BN_CLICKED(IDC_BTN_CAMERA1_START, &Cdml_sdk_demoDlg::OnBnClickedBtnCamera1Start)
END_MESSAGE_MAP()


BOOL Cdml_sdk_demoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	initialize();

	char videoList[20][256] = { 0 };

	get_device_list(videoList);

	for (int i = 0;i < 20; ++i){
		if (videoList[i][0] != 0){
			cmbCamera1.AddString((LPCTSTR)(utf8_to_unicode(videoList[i])).c_str());
		}else
			break;
	}
	edtSeverIP.SetWindowTextW(_T("222.73.146.226"));
	edtSeverPort.SetWindowTextW(_T("16385"));
	edtSSRC.SetWindowTextW(_T("200"));
	edtMemberID.SetWindowTextW(_T("123"));
	memset(&m_state, 0, sizeof(DML_STATE));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cdml_sdk_demoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cdml_sdk_demoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cdml_sdk_demoDlg::OnBnClickedBtnCamera1Start()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString sBtnText;
	btnCamera1Start.GetWindowTextW(sBtnText);
	if (!sBtnText.Compare(_T("Start"))){

		int nIndex = cmbCamera1.GetCurSel();

		if (nIndex < 0){
			MessageBox(_T("�������ô���"));
			return;
		}
		CString sCamera1, sIP, sPort, sSSRC, sMember;

		cmbCamera1.GetLBText(nIndex, sCamera1);
		edtSeverIP.GetWindowTextW(sIP);
		edtSeverPort.GetWindowTextW(sPort);
		edtSSRC.GetWindowTextW(sSSRC);
		edtMemberID.GetWindowTextW(sMember);

		std::wstring  camera1, ip, port, ssrc, memberid;
		camera1 = sCamera1.GetBuffer(0);
		ip = sIP.GetBuffer(0);
		port = sPort.GetBuffer(0);
		ssrc = sSSRC.GetBuffer(0);
		memberid = sMember.GetBuffer(0);

		DMLVideoConfig videoConfig;

		videoConfig.nFps = 15;
		videoConfig.nWidth = 720;
		videoConfig.nHeight = 960;
		videoConfig.nIndex = 1;
		videoConfig.sCameraName = unicode_to_utf8(camera1);
		videoConfig.sSeverAddr = unicode_to_utf8(ip);
		videoConfig.nSeverPort = _wtoi(port.c_str());
		videoConfig.nSSRC = _wtoi(ssrc.c_str());
		videoConfig.nMemberID = _wtoi(memberid.c_str());
		videoConfig.uiConfig.windHandle = GetDlgItem(IDC_SHOW_CAMERA1)->m_hWnd;
		videoConfig.uiConfig.pWind = this;
		videoConfig.uiConfig.windMsgCB = (CALLBACK_MSG)MsgCallBackHandle;

		start_video_stream(&videoConfig);
		btnCamera1Start.SetWindowTextW(_T("Stop"));
		cmbCamera1.EnableWindow(FALSE);
		edtSeverIP.EnableWindow(FALSE);
		edtSeverPort.EnableWindow(FALSE);
		edtMemberID.EnableWindow(FALSE);
		edtSSRC.EnableWindow(FALSE);
		edtPushId.EnableWindow(FALSE);
	}else{
		stop_video_stream(1);

		btnCamera1Start.SetWindowTextW(_T("Start"));
		cmbCamera1.EnableWindow(TRUE);
		edtSeverIP.EnableWindow(TRUE);
		edtSeverPort.EnableWindow(TRUE);
		edtMemberID.EnableWindow(TRUE);
		edtSSRC.EnableWindow(TRUE);
		edtPushId.EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_FPS)->SetWindowTextW(_T("0"));
		GetDlgItem(IDC_STATIC_STATE)->SetWindowTextW(_T("δ��ʼ"));
		GetDlgItem(IDC_STATIC_BITRATE)->SetWindowTextW(_T("0 KB/S"));
	}
}

void Cdml_sdk_demoDlg::OnClose()
{
	stop_video_stream(1);
	release();
	PostQuitMessage(0);
	//this->OnClose();
}

std::wstring Cdml_sdk_demoDlg::utf8_to_unicode(const std::string &value)
{
	wchar_t *result_buffer = NULL;
	std::wstring result_string;

	int size = ::MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, NULL, 0);
	if (size != 0)
	{
		result_buffer = new wchar_t[size];
		::MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, result_buffer, size);
		result_string = result_buffer;
		delete[] result_buffer;
	}
	return result_string;
}

std::string Cdml_sdk_demoDlg::unicode_to_utf8(const std::wstring &value)
{
	char *result_buffer = NULL;
	std::string result_string;
	int size = ::WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, NULL, 0, NULL, NULL);
	if (size != 0)
	{
		result_buffer = new char[size];
		::WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, result_buffer, size, NULL, NULL);
		result_string = result_buffer;
		delete[] result_buffer;
	}
	return result_string;
}

void Cdml_sdk_demoDlg::OnOK()
{

}

void Cdml_sdk_demoDlg::MsgCallBackHandle(int uType, int uMsg, void *param1, void *param2, void *pthis)
{
	if (NULL == pthis)
		return;
	Cdml_sdk_demoDlg* pDlg = (Cdml_sdk_demoDlg*)pthis;
	pDlg->PostMessage(WM_USER_SDK_MSG, 0, 0);
	switch (uMsg)
	{
	case DML_EVENT_PUSHER_STATE:
		pDlg->m_state.dmlEvent = DML_EVENT_PUSHER_STATE;
		pDlg->m_state.fps = *((int*)param1);
		pDlg->m_state.bitrate = *((int*)param2);
		pDlg->PostMessage(WM_USER_SDK_MSG, (WPARAM)&(pDlg->m_state), (LPARAM)param1);
		break;
	case DML_EVENT_PUSHER_START:
		pDlg->m_state.dmlEvent = DML_EVENT_PUSHER_START;
		pDlg->PostMessage(WM_USER_SDK_MSG, (WPARAM)&(pDlg->m_state), (LPARAM)param1);
		break;
	case DML_EVENT_PUSHER_STOP:
		pDlg->m_state.dmlEvent = DML_EVENT_PUSHER_STOP;
		pDlg->PostMessage(WM_USER_SDK_MSG, (WPARAM)&(pDlg->m_state), (LPARAM)param1);
		break;
	case DML_EVENT_CAPTURE_OPEN_FAILED:
		pDlg->m_state.dmlEvent = DML_EVENT_CAPTURE_OPEN_FAILED;
		pDlg->PostMessage(WM_USER_SDK_MSG, (WPARAM)&(pDlg->m_state), (LPARAM)param1);
		break;
	case DML_EVENT_PUSHER_OPEN_FAILED:
		pDlg->m_state.dmlEvent = DML_EVENT_PUSHER_OPEN_FAILED;
		pDlg->PostMessage(WM_USER_SDK_MSG, (WPARAM)&(pDlg->m_state), (LPARAM)param1);
		break;
	default:
		break;
	}


}

void Cdml_sdk_demoDlg::OnSdkMsg(WPARAM wParam, LPARAM lParam)
{
	if (!lParam){
		return;
	}
	DML_STATE dml_state = *((DML_STATE*)lParam);

	CString sFps,sBitrate;
	wchar_t fps[128],bitrate[128];
	wsprintf(fps, _T("%d"), dml_state.fps);
	wsprintf(bitrate, _T("%d KB/S"), dml_state.bitrate);
	sFps = fps;
	sBitrate = bitrate;
	switch (dml_state.dmlEvent)
	{
	case DML_EVENT_PUSHER_STATE:
		GetDlgItem(IDC_STATIC_FPS)->SetWindowTextW(sFps);
		GetDlgItem(IDC_STATIC_BITRATE)->SetWindowTextW(sBitrate);
		break;
	case DML_EVENT_PUSHER_START:
		GetDlgItem(IDC_STATIC_STATE)->SetWindowTextW(_T("�����С�����"));
		break;
	case DML_EVENT_PUSHER_STOP:
		GetDlgItem(IDC_STATIC_STATE)->SetWindowTextW(_T("δ��ʼ"));
		break;
	case DML_EVENT_CAPTURE_OPEN_FAILED:
		GetDlgItem(IDC_STATIC_STATE)->SetWindowTextW(_T("������ͷʧ�ܣ�"));
		break;
	case DML_EVENT_PUSHER_OPEN_FAILED:
		GetDlgItem(IDC_STATIC_STATE)->SetWindowTextW(_T("������ʼ��ʧ�ܣ�"));
		break;
	default:
		break;
	}
}
