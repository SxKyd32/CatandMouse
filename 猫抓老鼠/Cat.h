#pragma once
#include <afxwin.h>
#include "Map.h"

class Cat
{

public:
	CBitmap* upbody = new CBitmap;            //向上移动时使用的猫咪图片
	CBitmap* downbody = new CBitmap;      //向下移动时使用的猫咪图片
	CBitmap* leftbody = new CBitmap;          //向左移动时使用的猫咪图片
	CBitmap* rightbody = new CBitmap;       //向右移动时使用的猫咪图片

	CPoint Headpos;                                           //猫咪头部左上角坐标
	//CPoint Bodypos;                                           //猫咪身体左上角坐标
	CPoint Bodypos[5];											// 小猫身体左上角坐标集
	CPoint Tailpos;                                              //猫咪尾部左上角坐标
	CPoint StartPos;                                       //猫咪在五个关卡中的初始位置
	int Direction = 1;                                          //1==up.   2== down   3== left  4== right   指示猫咪移动的四个方向
	int CatSpeed = 23;

	Cat();
	~Cat();

	BOOL CatchMouse(Cat cat, CPoint pt);
	BOOL TakeWall(Cat cat, Map Now, int Stage);
	BOOL OutMap(Cat cat, Map Now, int Stage);
	BOOL CatchFish(Cat cat, CPoint pt);
	BOOL CatchApple(Cat cat, CPoint pt);
};
