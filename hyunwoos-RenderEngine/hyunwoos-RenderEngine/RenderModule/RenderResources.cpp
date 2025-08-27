#include "RenderResources.h"


/*============================================================================================================
 *   지정한 위치의 픽셀을 얻습니다...
 *========*/
const hyunwoo::LinearColor hyunwoo::Texture2D::GetPixel(const hyunwoo::Vector2Int& pos) const 
{
	const int idx = (pos.y * Width) + pos.x;

	if (idx >= 0 && idx < (Width * Height)) {
		return Pixels[idx];
	}

	return LinearColor::White;
}