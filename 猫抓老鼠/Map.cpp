#include "Map.h"

Map::Map()
{
	bk->m_hObject = (HBITMAP)::LoadImage(
		NULL,
		"bk.bmp",
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);

	Wall->m_hObject = (HBITMAP)::LoadImage(
		NULL,
		"wall1.bmp",
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);

}

Map::~Map()
{

}

