#pragma once
#include <Windows.h>
#include <vector>
#include "../MathModule/Vector.h"
#include "../MathModule/Color.h"


namespace hyunwoo {
	struct Texture2D;

}



/*========================================================================================
 *    2D 텍스쳐가 정의된 구조체입니다.....
 *=========*/
struct hyunwoo::Texture2D
{
	uint32_t				 Width;
	uint32_t				 Height;
	std::vector<LinearColor> Pixels;

	const LinearColor GetPixel(const Vector2Int& pos) const;
};