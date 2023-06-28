#pragma once
//#include <afxwin.h>
#include "Cat.h"
#include "Map.h"
#include "DirectSound.h"
#include <string>
#include <string.h>

class CMyWnd : public CFrameWnd
{
	LPDIRECTSOUND pDS;	// 用于创建Direct Sound
	LPDIRECTSOUNDBUFFER pDSB_BK, pDSB_MG, pDSB_W, pDSB_L;		// 用于创建存储ds的次缓冲区
	HRESULT result;		// 用于判断是否执行成功
	LPDIRECTSOUNDBUFFER CreateSecondaryBuffer(LPSTR filename);	// 加载音频文件到次缓冲区
private:
	CDC* mdc = new CDC;
	Cat* cat = new Cat;
	Map* StageMap = new Map;
	int Stage = 1;                          //用于识别关卡
	CBitmap* msbmp = new CBitmap;
	CBitmap* fibmp = new CBitmap;
	CBitmap* apbmp = new CBitmap;

	int GameState = 1;              //0表示游戏中，1表示开始界面，2表示游戏胜利，3表示游戏失败
	
	int gp = 0;				// 得分
	char str[30];
	CString gpc;

	int bodylength = 0;		// 小猫身体长度-1的长度

	BOOL hasfish = TRUE;	// 鱼是否被吃掉
	BOOL hasapple = TRUE;	// 苹果是否被吃掉

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