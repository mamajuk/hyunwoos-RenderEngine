#pragma once
#include <Windows.h>
#include <vector>
#include "../MathModule/Vector.h"
#include "../MathModule/Color.h"

namespace hyunwoo {
	struct Texture2D;

}



/*========================================================================================================
 *    2D 텍스쳐가 정의된 구조체입니다. 텍스쳐의 uv좌표계는 스크린좌표계를 따릅니다...
 *=========*/
struct hyunwoo::Texture2D
{
	uint32_t				 Width;
	uint32_t				 Height;
	std::vector<Color>		 Pixels;

	const Color GetPixel(const Vector2Int& pos) const;
};



/*===========================================================================================================
 *
 *=========*/