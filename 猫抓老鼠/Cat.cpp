#include "Cat.h"

Cat::Cat()                                                                              //导入各个图片数据，初始化初始位置
{
	upbody->m_hObject = (HBITMAP)::LoadImage(
		NULL,
		"catU.bmp",
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);

	downbody->m_hObject = (HBITMAP)::LoadImage(
		NULL,
		"catD.bmp",
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);

	leftbody->m_hObject = (HBITMAP)::LoadImage(
		NULL,
		"catL.bmp",
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);

	rightbody->m_hObject = (HBITMAP)::LoadImage(
		NULL,
		"catR.bmp",
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);


	StartPos.x = 1000; StartPos.y = 200;
}


Cat::~Cat()
{

}

BOOL Cat::CatchMouse(Cat cat, CPoint pt)
{
	if ((cat.Headpos.x + 25 >= pt.x) && (cat.Headpos.x <= pt.x + 60))
	{
		if ((cat.Headpos.y + 11 >= pt.y) && (cat.Headpos.y <= pt.y + 60))
		{
			return TRUE;
		}
		return FALSE;
	}
	else return FALSE;
}

BOOL Cat::TakeWall(Cat cat, Map map, int Stage)                //检查碰撞    cat用于获得位置    map用于获得关卡地图信息   Stage用于识别关卡
{
	switch (Stage)
	{
	case 1:

		for (int i = 0; i < 11; i++)
		{
			if (cat.Headpos.x >= map.MapMsg1[i][0] && cat.Headpos.x <= (map.MapMsg1[i][0] + map.MapMsg1[i][2]))
			{
				if (cat.Headpos.y >= map.MapMsg1[i][1] && cat.Headpos.y <= (map.MapMsg1[i][1] + map.MapMsg1[i][3]))
				{
					return TRUE;
				}
			}
		}
		break;
	case 2:
		for (int i = 0; i < 11; i++)
		{
			if (cat.Headpos.x >= map.MapMsg2[i][0] && cat.Headpos.x <= (map.MapMsg2[i][0] + map.MapMsg2[i][2]))
			{
				if (cat.Headpos.y >= map.MapMsg2[i][1] && cat.Headpos.y <= (map.MapMsg2[i][1] + map.MapMsg2[i][3]))
				{
					return TRUE;
				}
			}
		}
		break;
	case 3:
		for (int i = 0; i < 11; i++)
		{
			if (cat.Headpos.x >= map.MapMsg3[i][0] && cat.Headpos.x <= (map.MapMsg3[i][0] + map.MapMsg3[i][2]))
			{
				if (cat.Headpos.y >= map.MapMsg3[i][1] && cat.Headpos.y <= (map.MapMsg3[i][1] + map.MapMsg3[i][3]))
				{
					return TRUE;
				}
			}
		}
		break;
	case 4:
		for (int i = 0; i < 11; i++)
		{
			if (cat.Headpos.x >= map.MapMsg4[i][0] && cat.Headpos.x <= (map.MapMsg4[i][0] + map.MapMsg4[i][2]))
			{
				if (cat.Headpos.y >= map.MapMsg4[i][1] && cat.Headpos.y <= (map.MapMsg4[i][1] + map.MapMsg4[i][3]))
				{
					return TRUE;
				}
			}
		}
		break;
	case 5:
		for (int i = 0; i < 11; i++)
		{
			if (cat.Headpos.x >= map.MapMsg5[i][0] && cat.Headpos.x <= (map.MapMsg5[i][0] + map.MapMsg5[i][2]))
			{
				if (cat.Headpos.y >= map.MapMsg5[i][1] && cat.Headpos.y <= (map.MapMsg5[i][1] + map.MapMsg5[i][3]))
				{
					return TRUE;
				}
			}
		}
		break;
	default:break;
	}

	return FALSE;
}

BOOL Cat::OutMap(Cat cat, Map map, int Stage)
{
	if (cat.Headpos.x <= 0 || cat.Headpos.x >= 1080)
	{
		return TRUE;
	}
	else if (cat.Headpos.y <= 0 || cat.Headpos.y >= 600)
	{
		return TRUE;
	}
	else return FALSE;
}

BOOL Cat::CatchFish(Cat cat, CPoint pt)
{
	if ((cat.Headpos.x + 25 >= pt.x) && (cat.Headpos.x <= pt.x + 60))
	{
		if ((cat.Headpos.y + 11 >= pt.y) && (cat.Headpos.y <= pt.y + 60))
		{
			return TRUE;
		}
		return FALSE;
	}
	else return FALSE;
}

BOOL Cat::CatchApple(Cat cat, CPoint pt)
{
	if ((cat.Headpos.x + 25 >= pt.x) && (cat.Headpos.x <= pt.x + 80))
	{
		if ((cat.Headpos.y + 11 >= pt.y) && (cat.Headpos.y <= pt.y + 80))
		{
			return TRUE;
		}
		return FALSE;
	}
	else return FALSE;
}


