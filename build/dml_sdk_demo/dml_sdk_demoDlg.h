
// dml_sdk_demoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <string>
#include "interface/dml_sdk_interface.h"

struct DML_STATE{
	DMLEvent dmlEvent;
	int fps;
	int bitrate;
};
// Cdml_sdk_demoDlg 对话框
class Cdml_sdk_demoDlg : public CDialogEx
{
// 构造
public:
	Cdml_sdk_demoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DML_SDK_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	DML_STATE m_state;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	static void MsgCallBackHandle(int uType, int uMsg, void *param1, void *param2, void *pthis);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSdkMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnOK();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox cmbCamera1;
	CButton btnCamera1Start;
	afx_msg void OnBnClickedBtnCamera1Start();
	std::wstring utf8_to_unicode(const std::string &value);
	std::string unicode_to_utf8(const std::wstring &value);
	CEdit edtSeverIP;
	CEdit edtSeverPort;
	CEdit edtSSRC;
	CEdit edtMemberID;
	CEdit edtPushId;
};
