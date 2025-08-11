#include "Math.h"
using namespace hyunwoo;

const float Math::Atan2(const hyunwoo::Vector2& rhs) {
	return atan2f(rhs.y, rhs.x);
}