
// dml_sdk_demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cdml_sdk_demoApp: 
// �йش����ʵ�֣������ dml_sdk_demo.cpp
//

class Cdml_sdk_demoApp : public CWinApp
{
public:
	Cdml_sdk_demoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cdml_sdk_demoApp theApp;