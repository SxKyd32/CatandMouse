#include "CMyApp.h"
#include "CMyWnd.h"

BOOL CMyApp::InitInstance()
{
	CMyWnd* pFrame = new CMyWnd;
	pFrame->MoveWindow(0, 0, 1080, 600);
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();
	this->m_pMainWnd = pFrame;
	return TRUE;
}

