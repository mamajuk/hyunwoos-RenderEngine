#include "RenderResources.h"
#include "../MathModule/Quaternion.h"
#undef min
#undef max

/***************************************************
 *   Texture2D 메소드들의 정의....
 ******/

/*============================================================================================================
 *   지정한 위치의 픽셀을 얻습니다...
 *========*/
const hyunwoo::Color hyunwoo::Texture2D::GetPixel(const hyunwoo::Vector2Int& pos) const 
{
	const int idx = (pos.y * Width) + pos.x;

	if (idx >= 0 && idx < (Width * Height)) {
		return Pixels[idx];
	}

 	return Color::Pink;
}










