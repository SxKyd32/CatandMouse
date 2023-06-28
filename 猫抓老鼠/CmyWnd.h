#pragma once
//#include <afxwin.h>
#include "Cat.h"
#include "Map.h"
#include "DirectSound.h"
#include <string>
#include <string.h>

class CMyWnd : public CFrameWnd
{
	LPDIRECTSOUND pDS;	// ���ڴ���Direct Sound
	LPDIRECTSOUNDBUFFER pDSB_BK, pDSB_MG, pDSB_W, pDSB_L;		// ���ڴ����洢ds�Ĵλ�����
	HRESULT result;		// �����ж��Ƿ�ִ�гɹ�
	LPDIRECTSOUNDBUFFER CreateSecondaryBuffer(LPSTR filename);	// ������Ƶ�ļ����λ�����
private:
	CDC* mdc = new CDC;
	Cat* cat = new Cat;
	Map* StageMap = new Map;
	int Stage = 1;                          //����ʶ��ؿ�
	CBitmap* msbmp = new CBitmap;
	CBitmap* fibmp = new CBitmap;
	CBitmap* apbmp = new CBitmap;

	int GameState = 1;              //0��ʾ��Ϸ�У�1��ʾ��ʼ���棬2��ʾ��Ϸʤ����3��ʾ��Ϸʧ��
	
	int gp = 0;				// �÷�
	char str[30];
	CString gpc;

	int bodylength = 0;		// Сè���峤��-1�ĳ���

	BOOL hasfish = TRUE;	// ���Ƿ񱻳Ե�
	BOOL hasapple = TRUE;	// ƻ���Ƿ񱻳Ե�

	CBitmap* BaseStage = new CBitmap;
	CBitmap* Title = new CBitmap;
	CBitmap* VicStage = new CBitmap;
	CBitmap* FailedStage = new CBitmap;
	CBitmap* AgainStage = new CBitmap;


public:
	CMyWnd();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};