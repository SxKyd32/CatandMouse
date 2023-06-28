#pragma once
#include <afxwin.h>
#include "Map.h"

class Cat
{

public:
	CBitmap* upbody = new CBitmap;            //�����ƶ�ʱʹ�õ�è��ͼƬ
	CBitmap* downbody = new CBitmap;      //�����ƶ�ʱʹ�õ�è��ͼƬ
	CBitmap* leftbody = new CBitmap;          //�����ƶ�ʱʹ�õ�è��ͼƬ
	CBitmap* rightbody = new CBitmap;       //�����ƶ�ʱʹ�õ�è��ͼƬ

	CPoint Headpos;                                           //è��ͷ�����Ͻ�����
	//CPoint Bodypos;                                           //è���������Ͻ�����
	CPoint Bodypos[5];											// Сè�������Ͻ����꼯
	CPoint Tailpos;                                              //è��β�����Ͻ�����
	CPoint StartPos;                                       //è��������ؿ��еĳ�ʼλ��
	int Direction = 1;                                          //1==up.   2== down   3== left  4== right   ָʾè���ƶ����ĸ�����
	int CatSpeed = 23;

	Cat();
	~Cat();

	BOOL CatchMouse(Cat cat, CPoint pt);
	BOOL TakeWall(Cat cat, Map Now, int Stage);
	BOOL OutMap(Cat cat, Map Now, int Stage);
	BOOL CatchFish(Cat cat, CPoint pt);
	BOOL CatchApple(Cat cat, CPoint pt);
};
