#include "CMyWnd.h"

int inc = 10;
CPoint Pos = (0, 0);

CPoint fis = CPoint(400, 300);
CPoint fis1 = CPoint(420, 300);
CPoint fis2 = CPoint(300, 400);
CPoint fis3 = CPoint(320, 200);
CPoint fis4 = CPoint(420, 280);

CPoint ap = CPoint(850, 230);
CPoint ap1 = CPoint(850, 400);
CPoint ap2 = CPoint(750, 200);
CPoint ap3 = CPoint(850, 200);
CPoint ap4 = CPoint(20, 450);

char bgMusic[20] = "bgm.wav";
char clkSound[20] = "点击鼠标.wav";
char winSound[20] = "胜利.wav";
char loseSound[20] = "失败.wav";

CMyWnd::CMyWnd()
{
	Create(NULL, "");
	CClientDC dc(this);
	mdc->CreateCompatibleDC(&dc);

	cat->Headpos = cat->StartPos;                                 //初始化猫咪位置                                            //关卡切换时在更改stage的同时要更改这个   
	cat->Bodypos[0].x = cat->Headpos.x;     cat->Bodypos[0].y = cat->Headpos.y + 30;
	for (int i = 0; i <= bodylength; i++)
	{
		if (i > 0)
		{
			cat->Bodypos[i].x = cat->Headpos.x;     cat->Bodypos[i].y = cat->Bodypos[i-1].y + 30;
		}
	}
	cat->Tailpos.x = cat->Headpos.x;        cat->Tailpos.y = cat->Headpos.y + 50 + 30 * bodylength;

	SetTimer(0, 100, NULL);

	msbmp->m_hObject = (HBITMAP)::LoadImage(NULL,		// 加载老鼠图片
		"mouse.bmp", IMAGE_BITMAP,
		120, 60, LR_LOADFROMFILE);

	fibmp->m_hObject = (HBITMAP)::LoadImage(NULL,		// 加载鱼图片
		"fish.bmp", IMAGE_BITMAP,
		120, 60, LR_LOADFROMFILE);

	apbmp->m_hObject = (HBITMAP)::LoadImage(NULL,		// 加载苹果图片
		"apple.bmp", IMAGE_BITMAP,
		80, 160, LR_LOADFROMFILE);

	//加载场景画面
	BaseStage->m_hObject = (HBITMAP)::LoadImage(                                              //开始界面
		NULL,
		"BaseStage.bmp",
		IMAGE_BITMAP,
		1080,
		600,
		LR_LOADFROMFILE);

	Title->m_hObject = (HBITMAP)::LoadImage(                                                      //游戏标题
		NULL,
		"Title.bmp",
		IMAGE_BITMAP,
		1080,
		600,
		LR_LOADFROMFILE);

	VicStage->m_hObject = (HBITMAP)::LoadImage(                                                  //胜利画面
		NULL,
		"VicStage.bmp",
		IMAGE_BITMAP,
		1080,
		600,
		LR_LOADFROMFILE);

	FailedStage->m_hObject = (HBITMAP)::LoadImage(                                          //失败画面
		NULL,
		"FailedStage.bmp",
		IMAGE_BITMAP,
		1080,
		600,
		LR_LOADFROMFILE);

	AgainStage->m_hObject = (HBITMAP)::LoadImage(                                          //失败/胜利画面菜单
		NULL,
		"AgainStage.bmp",
		IMAGE_BITMAP,
		1080,
		600,
		LR_LOADFROMFILE);

	// DirectSound部分
	result = DirectSoundCreate(NULL, &pDS, NULL);
	if (DS_OK != result)
		MessageBox("DirectSound对象创建失败！");
	result = pDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY);
	if (DS_OK != result)
		MessageBox("协作等级设定失败！");

	// 加载背景音乐
	pDSB_BK = CreateSecondaryBuffer(bgMusic);
	pDSB_BK->Play(0, 0, 1);	// 播放背景音乐

	// 加载鼠标单击音效
	pDSB_MG = CreateSecondaryBuffer(clkSound);

	// 加载游戏胜利音效
	pDSB_W = CreateSecondaryBuffer(winSound);

	// 加载游戏失败音效
	pDSB_L = CreateSecondaryBuffer(loseSound);
}

LPDIRECTSOUNDBUFFER CMyWnd::CreateSecondaryBuffer(LPSTR filename)
{
	HMMIO hmmio;
	MMRESULT mmresult;

	// 打开音频文件
	hmmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (NULL == hmmio)
		MessageBox("文件打开失败！");

	// 确认是否为riff文件
	MMCKINFO ckriff;
	ckriff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmresult = mmioDescend(hmmio, &ckriff, NULL, MMIO_FINDRIFF);
	if (MMSYSERR_NOERROR != mmresult)
		MessageBox("文件类型不是WAVE！");

	// 进入fmt区块，读取音频格式，然后退出fmt区块
	MMCKINFO ckinfo;
	ckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmresult = mmioDescend(hmmio, &ckinfo, &ckriff, MMIO_FINDCHUNK);
	if (MMSYSERR_NOERROR != mmresult)
		MessageBox("进入fmt区域错误！");
	WAVEFORMATEX swfmt;
	if (-1 == mmioRead(hmmio, (HPSTR)&swfmt, sizeof(swfmt)))	// 读取音频格式
		MessageBox("读取音频文件格式失败！");

	// 进入data区块，读取音频长度
	mmioAscend(hmmio, &ckinfo, 0);
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmresult = mmioDescend(hmmio, &ckinfo, &ckriff, MMIO_FINDCHUNK);
	if (MMSYSERR_NOERROR != mmresult)
		MessageBox("进入data区块失败！");

	// 创建音频次缓冲区
	LPDIRECTSOUNDBUFFER pTempBuf;
	DSBUFFERDESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.lpwfxFormat = &swfmt;
	desc.dwFlags = DSBCAPS_STATIC;	// 声音可重复播放
	desc.dwBufferBytes = ckinfo.cksize;
	result = pDS->CreateSoundBuffer(&desc, &pTempBuf, NULL);
	if (DS_OK != result)
		MessageBox("创建次缓冲区失败！");

	// 从文件读取音频数据存入次缓冲区
	LPVOID pAudio;
	DWORD BytesAudio;
	pTempBuf->Lock(0, ckinfo.cksize, &pAudio, &BytesAudio, NULL, NULL, NULL);
	if (-1 == mmioRead(hmmio, (HPSTR)pAudio, BytesAudio))
		MessageBox("读取音频数据失败！");
	pTempBuf->Unlock(pAudio, BytesAudio, NULL, NULL);

	// 关闭文件
	mmioClose(hmmio, 0);
	return pTempBuf;
}

BEGIN_MESSAGE_MAP(CMyWnd, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CMyWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CFrameWnd::OnPaint()
	switch (GameState)
	{
	case 1:

		mdc->SelectObject(BaseStage);
		dc.BitBlt(0, 0, 1080, 600, mdc, 0, 0, SRCCOPY);
		mdc->SelectObject(Title);
		dc.BitBlt(0, 0, 1080, 400, mdc, 0, 0, SRCCOPY);

		break;
	case 2:

		mdc->SelectObject(AgainStage);
		dc.BitBlt(0, 0, 1080, 600, mdc, 0, 0, SRCCOPY);
		mdc->SelectObject(VicStage);
		dc.BitBlt(0, 0, 1080, 400, mdc, 0, 0, SRCCOPY);

		pDSB_W->Play(0, 0, 0);	// 播放游戏胜利音效
		itoa(gp, str, 10);
		gpc = str;
		MessageBox("恭喜你，你的本次得分为：" + gpc, "游戏胜利");

		break;
	case 3:

		mdc->SelectObject(AgainStage);
		dc.BitBlt(0, 0, 1080, 600, mdc, 0, 0, SRCCOPY);
		mdc->SelectObject(FailedStage);
		dc.BitBlt(0, 0, 1080, 400, mdc, 0, 0, SRCCOPY);

		pDSB_L->Play(0, 0, 0);	// 播放游戏失败音效
		itoa(gp, str, 10);
		gpc = str;
		MessageBox("你的本次得分为：" + gpc, "游戏结束");

		break;
	case 0:
		mdc->SelectObject(StageMap->bk);                                //绘制背景
		dc.BitBlt(0, 0, 1080, 600, mdc, 0, 0, SRCCOPY);

		mdc->SelectObject(StageMap->Wall);
		switch (Stage)
		{
		case 1:
			for (int k = 0; k < 11; k++)                        //根据关卡绘制地图   （关卡由Stage指示）
			{
				dc.BitBlt(StageMap->MapMsg1[k][0], StageMap->MapMsg1[k][1], StageMap->MapMsg1[k][2], StageMap->MapMsg1[k][3], mdc, 0, 0, SRCCOPY);
			}

			// 苹果
			if (hasapple)
			{
				mdc->SelectObject(apbmp);
				dc.BitBlt(ap.x, ap.y, 80, 80, mdc, 0, 80, SRCAND);
				dc.BitBlt(ap.x, ap.y, 80, 80, mdc, 0, 0, SRCPAINT);
			}

			// 鱼
			if (hasfish)
			{
				if (fis.x < 600 && fis.y == 300)
				{
					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis.x += inc;
				}
				if (fis.x == 600 && fis.y > 260)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis.y -= inc;

				}
				if (fis.x > 400 && fis.y == 260)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis.x -= inc;

				}

				if (fis.x == 400 && fis.y < 300)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis.x, fis.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis.y += inc;

				}
			}

			// 老鼠
			if (Pos.x < 1 && Pos.y < 240)                   //绘制自动移动的老鼠
			{
				mdc->SelectObject(msbmp);

				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);

				Pos.y += inc;
			}
			if (Pos.x < 200 && Pos.y >= 240)
			{
				mdc->SelectObject(msbmp);

				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 200 && Pos.x <= 205 && Pos.y >= 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;
			}
			if (Pos.x >= 200 && Pos.x <= 310 && Pos.y <= 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 310 && Pos.x <= 320 && Pos.y <= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}
			if (Pos.x >= 310 && Pos.x <= 670 && Pos.y >= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 670 && Pos.x <= 700 && Pos.y >= 220)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;
			}
			if (Pos.x >= 670 && Pos.x <= 780 && Pos.y <= 220)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 780 && Pos.y <= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}
			if (Pos.x >= 780 && Pos.y >= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}

			break;
		case 2:
			for (int k = 0; k < 11; k++)
			{
				dc.BitBlt(StageMap->MapMsg2[k][0], StageMap->MapMsg2[k][1], StageMap->MapMsg2[k][2], StageMap->MapMsg2[k][3], mdc, 0, 0, SRCCOPY);
			}

			// 苹果
			if (hasapple)
			{
				mdc->SelectObject(apbmp);
				dc.BitBlt(ap1.x, ap1.y, 80, 80, mdc, 0, 80, SRCAND);
				dc.BitBlt(ap1.x, ap1.y, 80, 80, mdc, 0, 0, SRCPAINT);
			}

			// 鱼
			if (hasfish)
			{
				if (fis1.x < 600 && fis1.y == 300)
				{
					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis1.x += inc;
				}
				if (fis1.x == 600 && fis1.y > 200)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis1.y -= inc;

				}
				if (fis1.x > 420 && fis1.y == 200)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis1.x -= inc;

				}

				if (fis1.x == 420 && fis1.y < 300)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis1.x, fis1.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis1.y += inc;

				}
			}

			// 老鼠
			if (Pos.x < 1 && Pos.y < 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.y += inc;
			}
			if (Pos.x < 140 && Pos.y >= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 140 && Pos.x <= 210 && Pos.y >= 240)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.y -= inc;
			}
			if (Pos.x >= 140 && Pos.x <= 210 && Pos.y <= 240)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);

				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 210 && Pos.x < 400 && Pos.y <= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.y += inc;
			}
			if (Pos.x >= 210 && Pos.x < 400 && Pos.y >= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 400 && Pos.y >= 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;

			}

			if (Pos.x >= 400 && Pos.y <= 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.x += inc;

			}

			break;
		case 3:
			for (int k = 0; k < 11; k++)
			{
				dc.BitBlt(StageMap->MapMsg3[k][0], StageMap->MapMsg3[k][1], StageMap->MapMsg3[k][2], StageMap->MapMsg3[k][3], mdc, 0, 0, SRCCOPY);
			}

			// 苹果
			if (hasapple)
			{
				mdc->SelectObject(apbmp);
				dc.BitBlt(ap2.x, ap2.y, 80, 80, mdc, 0, 80, SRCAND);
				dc.BitBlt(ap2.x, ap2.y, 80, 80, mdc, 0, 0, SRCPAINT);
			}

			// 鱼
			if (hasfish)
			{
				if (fis2.x < 500 && fis2.y == 400)
				{
					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis2.x += inc;
				}
				if (fis2.x == 500 && fis2.y > 200)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis2.y -= inc;

				}
				if (fis2.x > 300 && fis2.y == 200)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis2.x -= inc;

				}

				if (fis2.x == 300 && fis2.y < 400)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis2.x, fis2.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis2.y += inc;

				}
			}

			// 老鼠
			if (Pos.x < 1 && Pos.y < 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}
			if (Pos.x < 300 && Pos.x < 500 && Pos.y >= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}

			if (Pos.x >= 300 && Pos.x <= 400 && Pos.y >= 200)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;
			}
			if (Pos.x >= 300 && Pos.x <= 530 && Pos.y <= 200)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}
			if (Pos.x >= 530 && Pos.y <= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}
			if (Pos.x >= 530 && Pos.x <= 1000 && Pos.y >= 490)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 53, 77, mdc, flag * 53, 0, SRCPAINT);
				Pos.x += inc;
			}

			break;
		case 4:
			for (int k = 0; k < 11; k++)
			{
				dc.BitBlt(StageMap->MapMsg4[k][0], StageMap->MapMsg4[k][1], StageMap->MapMsg4[k][2], StageMap->MapMsg4[k][3], mdc, 0, 0, SRCCOPY);
			}

			// 苹果
			if (hasapple)
			{
				mdc->SelectObject(apbmp);
				dc.BitBlt(ap3.x, ap3.y, 80, 80, mdc, 0, 80, SRCAND);
				dc.BitBlt(ap3.x, ap3.y, 80, 80, mdc, 0, 0, SRCPAINT);
			}

			// 鱼
			if (hasfish)
			{
				if (fis3.x < 600 && fis3.y == 200)
				{
					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis3.x += inc;
				}
				if (fis3.x == 600 && fis3.y > 0)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis3.y -= inc;

				}
				if (fis3.x > 320 && fis3.y == 0)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis3.x -= inc;

				}

				if (fis3.x == 320 && fis3.y < 200)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis3.x, fis3.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis3.y += inc;

				}
			}

			// 老鼠
			if (Pos.x < 1 && Pos.y < 290)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}
			if (Pos.x < 140 && Pos.y >= 290)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;

			}
			if (Pos.x >= 140 && Pos.x <= 520 && Pos.y >= 290 && Pos.y <= 490)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;

			}
			if (Pos.x >= 140 && Pos.x <= 540 && Pos.y >= 490)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;

			}
			if (Pos.x >= 540 && Pos.x < 780 && Pos.y >= 330)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;

			}
			if (Pos.x >= 540 && Pos.x <= 780 && Pos.y <= 330)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);

				Pos.x += inc;

			}

			if (Pos.x >= 780 && Pos.x <= 980 && Pos.y <= 490)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);

				Pos.y += inc;
			}


			if (Pos.x >= 780 && Pos.x <= 980 && Pos.y >= 490)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;

			}
			if (Pos.x >= 980 && Pos.y >= 0)
			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;

			}

			break;
		case 5:
			for (int k = 0; k < 11; k++)
			{
				dc.BitBlt(StageMap->MapMsg5[k][0], StageMap->MapMsg5[k][1], StageMap->MapMsg5[k][2], StageMap->MapMsg5[k][3], mdc, 0, 0, SRCCOPY);
			}

			// 苹果
			if (hasapple)
			{
				mdc->SelectObject(apbmp);
				dc.BitBlt(ap4.x, ap4.y, 80, 80, mdc, 0, 80, SRCAND);
				dc.BitBlt(ap4.x, ap4.y, 80, 80, mdc, 0, 0, SRCPAINT);
			}

			// 鱼
			if (hasfish)
			{
				if (fis4.x < 550 && fis4.y == 280)
				{
					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis4.x += inc;
				}
				if (fis4.x == 550 && fis4.y > 120)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis4.y -= inc;

				}
				if (fis4.x > 420 && fis4.y == 120)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis4.x -= inc;

				}

				if (fis4.x == 420 && fis4.y < 280)
				{

					mdc->SelectObject(fibmp);
					//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
						//dc.BitBlt(GirlPos.x, GirlPos.y, 200,200, mdc, 0, 0, SRCPAINT);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 60, 0, SRCAND);
					dc.BitBlt(fis4.x, fis4.y, 60, 60, mdc, 0, 0, SRCPAINT);
					fis4.y += inc;

				}
			}

			// 老鼠

			if (Pos.x < 300 && Pos.y < 300)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}

			else if (Pos.x < 300 && Pos.y >= 300)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;

			}
			else if (Pos.x >= 300 && Pos.x < 420 && Pos.y >= 0)

			{

				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;
			}
			else if (Pos.x >= 300 && Pos.x <= 420 && Pos.y <= 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);

				Pos.x += inc;
			}
			else if (Pos.x >= 420 && Pos.x <= 427 && Pos.y <= 300)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}
			else if (Pos.x > 420 && Pos.x < 427 && Pos.y > 300)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}

			else if (Pos.x >= 427 && Pos.x <= 680 && Pos.y > 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;
			}
			else if (Pos.x >= 427 && Pos.x <= 680 && Pos.y <= 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;

			}

			else if (Pos.x >= 427 && Pos.x <= 680 && Pos.y <= 0)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;

			}

			else if (Pos.x >= 680 && Pos.x <= 700 && Pos.y <= 280)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y += inc;
			}

			else if (Pos.x >= 680 && Pos.x < 980 && Pos.y >= 280)
			{
				mdc->SelectObject(msbmp);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.x += inc;
			}
			else if (Pos.x >= 680 && Pos.x >= 980 && Pos.y >= 0)
			{
				mdc->SelectObject(msbmp);
				//	dc.BitBlt(GirlPos.x, GirlPos.y, 200, 200, mdc, 0, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 60, 0, SRCAND);
				dc.BitBlt(Pos.x, Pos.y, 60, 60, mdc, 0, 0, SRCPAINT);
				Pos.y -= inc;
			}

			break;
		default:break;
		}

		switch (cat->Direction)                                                        // 小猫移动
		{
		case 1:
			mdc->SelectObject(cat->upbody);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 50, 23, mdc, 50, 0, SRCAND);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 50, 23, mdc, 0, 0, SRCPAINT);

			if (0 == bodylength)
			{
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 50, 23, mdc, 50, 30, SRCAND);
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 50, 23, mdc, 0, 30, SRCPAINT);
			}
			else
			{
				for (int i = 0; i <= bodylength; i++)
				{
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 50, 23, mdc, 50, 30, SRCAND);
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 50, 23, mdc, 0, 30, SRCPAINT);
				}
			}

			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 50, 23, mdc, 50, 50, SRCAND);
			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 50, 23, mdc, 0, 50, SRCPAINT);
			break;

		case 2:
			mdc->SelectObject(cat->downbody);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 50, 23, mdc, 0, 40, SRCAND);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 50, 23, mdc, 50, 40, SRCPAINT);

			if (0 == bodylength)
			{
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 50, 23, mdc, 0, 20, SRCAND);
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 50, 23, mdc, 50, 20, SRCPAINT);
			}
			else
			{
				for (int i = 0; i <= bodylength; i++)
				{
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 50, 23, mdc, 0, 20, SRCAND);
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 50, 23, mdc, 50, 20, SRCPAINT);
				}			
			}

			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 50, 23, mdc, 0, 0, SRCAND);
			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 50, 23, mdc, 50, 0, SRCPAINT);
			break;

		case 3:
			mdc->SelectObject(cat->leftbody);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 23, 50, mdc, 0, 0, SRCAND);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 23, 50, mdc, 0, 50, SRCPAINT);

			if (0 == bodylength)
			{
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 23, 50, mdc, 23, 0, SRCAND);
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 23, 50, mdc, 23, 50, SRCPAINT);
			}
			else
			{
				for (int i = 0; i <= bodylength; i++)
				{
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 23, 50, mdc, 23, 0, SRCAND);
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 23, 50, mdc, 23, 50, SRCPAINT);
				}
			}

			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 23, 50, mdc, 46, 0, SRCAND);
			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 23, 50, mdc, 46, 50, SRCPAINT);
			break;

		case 4:
			mdc->SelectObject(cat->rightbody);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 23, 50, mdc, 46, 50, SRCAND);
			dc.BitBlt(cat->Headpos.x, cat->Headpos.y, 23, 50, mdc, 46, 0, SRCPAINT);

			if (0 == bodylength)
			{
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 23, 50, mdc, 23, 50, SRCAND);
				dc.BitBlt(cat->Bodypos[0].x, cat->Bodypos[0].y, 23, 50, mdc, 23, 0, SRCPAINT);
			}
			else
			{
				for (int i = 0; i <= bodylength; i++)
				{
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 23, 50, mdc, 23, 50, SRCAND);
					dc.BitBlt(cat->Bodypos[i].x, cat->Bodypos[i].y, 23, 50, mdc, 23, 0, SRCPAINT);
				}		
			}

			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 23, 50, mdc, 0, 50, SRCAND);
			dc.BitBlt(cat->Tailpos.x, cat->Tailpos.y, 23, 50, mdc, 0, 0, SRCPAINT);
			break;
		default:break;
		}
		break;
	default:break;
	}


}

void CMyWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (GameState == 0)                                                        //当游戏进行时
	{
		switch (nChar)                                                              // 根据按键移动，并禁止移除屏幕外
		{
		case VK_LEFT:
			if (cat->Direction == 3 || cat->Direction == 4)
			{
				break;
			}
			else
			{
				cat->Direction = 3;
			}
			//Invalidate(FALSE);
			break;
		case VK_RIGHT:
			if (cat->Direction == 3 || cat->Direction == 4)
			{
				break;
			}
			else
			{
				cat->Direction = 4;
			}
			//Invalidate(FALSE);
			break;
		case VK_UP:
			if (cat->Direction == 1 || cat->Direction == 2)
			{
				break;
			}
			else
			{
				cat->Direction = 1;
			}
			//Invalidate(FALSE);
			break;
		case VK_DOWN:
			if (cat->Direction == 1 || cat->Direction == 2)
			{
				break;
			}
			else
			{
				cat->Direction = 2;
			}
			//Invalidate(FALSE);		
			break;
		default:
			break;
		}
	}

	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMyWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (GameState == 0)                               //当在游戏中时
	{
		//碰撞，捕捉，撞墙检测
		if (cat->TakeWall(*cat, *StageMap, Stage) || cat->OutMap(*cat, *StageMap, Stage))   //若碰撞墙壁或碰到地图边缘
		{
			GameState = 3;                                //游戏结束
		}

		if (cat->CatchMouse(*cat, Pos))                                 //若抓到老鼠
		{
			Stage++;                                                                           //关卡变化，进入下一关
			bodylength++;		// 小猫身体长度加1
			hasapple = TRUE;
			hasfish = TRUE;
			Pos.x = 0; Pos.y = 0;                                                     //初始化老鼠位置
			cat->Direction = 1;                                                        //初始化猫咪移动方向
			cat->Headpos = cat->StartPos;                                 //初始化猫咪位置                                                         
			cat->Bodypos[0].x = cat->Headpos.x;     cat->Bodypos[0].y = cat->Headpos.y + 30;
			for (int i = 0; i <= bodylength; i++)
			{
				if (i > 0)
				{
					cat->Bodypos[i].x = cat->Headpos.x;     cat->Bodypos[i].y = cat->Bodypos[i - 1].y + 30;
				}
			}
			cat->Tailpos.x = cat->Headpos.x;        cat->Tailpos.y = cat->Headpos.y + 50 + 30 * bodylength;
			gp += 3;
		}

		switch (Stage)
		{
		case 1:
			if (cat->CatchFish(*cat, fis))
			{
				hasfish = FALSE;
				gp += 2;
				fis = (999, 999);
			}
			if (cat->CatchApple(*cat, ap))
			{
				hasapple = FALSE;
				gp++;
				ap = (999, 999);
			}
			break;

		case 2:
			if (cat->CatchFish(*cat, fis1))
			{
				hasfish = FALSE;
				gp += 2;
				fis1 = (999, 999);
			}
			if (cat->CatchApple(*cat, ap1))
			{
				hasapple = FALSE;
				gp++;
				ap1 = (999, 999);
			}
			break;

		case 3:
			if (cat->CatchFish(*cat, fis2))
			{
				hasfish = FALSE;
				gp += 2;
				fis2 = (999, 999);
			}
			if (cat->CatchApple(*cat, ap2))
			{
				hasapple = FALSE;
				gp++;
				ap2 = (999, 999);
			}
			break;

		case 4:
			if (cat->CatchFish(*cat, fis3))
			{
				hasfish = FALSE;
				gp += 2;
				fis3 = (999, 999);
			}
			if (cat->CatchApple(*cat, ap3))
			{
				hasapple = FALSE;
				gp++;
				ap3 = (999, 999);
			}
			break;

		case 5:
			if (cat->CatchFish(*cat, fis4))
			{
				hasfish = FALSE;
				gp += 2;
				fis4 = (999, 999);
			}
			if (cat->CatchApple(*cat, ap4))
			{
				hasapple = FALSE;
				gp++;
				ap4 = (999, 999);
			}
			break;

		default:
			break;
		}

		// 处理移动
		cat->Tailpos = cat->Bodypos[bodylength];	// 处理小猫尾部位置变化

		for (int i = bodylength; i >= 0; i--)		// 处理小猫身体位置变化
		{
			if (i == 0)
				cat->Bodypos[0] = cat->Headpos;
			else
			{
				cat->Bodypos[i] = cat->Bodypos[i - 1];
			}
		}

		switch (cat->Direction)		// 处理小猫头部转向
		{
		case 1:
			cat->Headpos.y -= cat->CatSpeed;
			break;
		case 2:
			cat->Headpos.y += cat->CatSpeed;
			break;
		case 3:
			cat->Headpos.x -= cat->CatSpeed;
			break;
		case 4:
			cat->Headpos.x += cat->CatSpeed;
			break;
		default:
			break;
		}

		Invalidate(FALSE);
	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CMyWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	pDSB_MG->Play(0, 0, 0);
	
	switch (GameState)
	{
	case 1:
		if (point.x > 160 && point.x < 340)
		{
			if (point.y > 450 && point.y < 550)
			{
				//显示帮助信息                                               //点击help按钮

				MessageBox("游戏玩法：玩家通过键盘方向键上下左右来控制猫的移动;当碰撞到墙壁的时候猫死亡;当猫抓到老鼠加三分，并进入下一关;当猫吃到苹果加一分;当猫吃到鱼加两分;当成功通过5关或者猫死亡游戏结束，此时会显示玩家的积分", "玩法说明");
			}
		}
		if (point.x > 420 && point.x < 620)
		{
			if (point.y > 450 && point.y < 550)
			{
				GameState = 0;                                              //点击start按钮开始游戏
			}
		}
		if (point.x > 720 && point.x < 920)
		{
			if (point.y > 450 && point.y < 550)
			{
				PostQuitMessage(0);                                   //点击quit按钮退出游戏
			}
		}

		break;
	case 2:
		if (point.x > 90 && point.x < 500)
		{
			if (point.y > 500 && point.y < 560)
			{
				GameState = 0;                                                              //重开游戏
				Stage = 1;                                                                         //关卡从头开始
				bodylength = 0;															// 初始化小猫身长
				Pos.x = 0; Pos.y = 0;                                                     //初始化猫咪方向
				cat->Direction = 1;                                                       //初始化猫咪位置
				cat->Headpos = cat->StartPos;                                 //初始化猫咪位置                                                         
				cat->Bodypos[0].x = cat->Headpos.x;     cat->Bodypos[0].y = cat->Headpos.y + 30;
				for (int i = 0; i <= bodylength; i++)
				{
					if (i > 0)
					{
						cat->Bodypos[i].x = cat->Headpos.x;     cat->Bodypos[i].y = cat->Bodypos[i - 1].y + 30;
					}
				}
				cat->Tailpos.x = cat->Headpos.x;        cat->Tailpos.y = cat->Headpos.y + 50 + 30 * bodylength;
			}
		}
		if (point.x > 580 && point.x < 1000)                                  //点击退出按钮退出游戏
		{
			if (point.y > 500 && point.y < 560)
			{
				PostQuitMessage(0);
			}
		}
		break;
	case 3:
		if (point.x > 90 && point.x < 500)
		{
			if (point.y > 500 && point.y < 560)
			{
				GameState = 0;                                                              //重开游戏
				Stage = 1;                                                                         //关卡从头开始
				bodylength = 0;															// 初始化小猫身长
				Pos.x = 0; Pos.y = 0;                                                     //初始化猫咪方向
				cat->Direction = 1;                                                       //初始化猫咪位置
				cat->Headpos = cat->StartPos;                                 //初始化猫咪位置                                                         
				cat->Bodypos[0].x = cat->Headpos.x;     cat->Bodypos[0].y = cat->Headpos.y + 30;
				for (int i = 0; i <= bodylength; i++)
				{
					if (i > 0)
					{
						cat->Bodypos[i].x = cat->Headpos.x;     cat->Bodypos[i].y = cat->Bodypos[i - 1].y + 30;
					}
				}
				cat->Tailpos.x = cat->Headpos.x;        cat->Tailpos.y = cat->Headpos.y + 50 + 30 * bodylength;
			}
		}
		if (point.x > 580 && point.x < 1000)
		{
			if (point.y > 500 && point.y < 560)
			{
				PostQuitMessage(0);                                                  //点击退出按钮退出游戏
			}
		}
		break;
	default:break;
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}
