#include "Vector.h"
using namespace hyunwoo;


/*********************************************************
 *     Vector2 관련 정의...
 *********/
const Vector2 Vector2::Zero   = { 0.f, 0.f  };
const Vector2 Vector2::One    = { 1.f, 1.f  };
const Vector2 Vector2::BasisX = { 1.f, 0.f  };
const Vector2 Vector2::BasisY = { 0.f, 1.f  };
const Vector2 Vector2::Up     = { 0.f, 1.f  };
const Vector2 Vector2::Down   = { 0.f, -1.f };
const Vector2 Vector2::Right  = { 1.f, 0.f  };
const Vector2 Vector2::Left   = { -1.f, 0.f };

const float Vector2::GetMagnitude() const {
	return Math::Sqrt(GetSqrMagnitude());
}

hyunwoo::Vector2::operator hyunwoo::Vector3() const
{
	return Vector3(x, y, 0.f);
}

hyunwoo::Vector2::operator hyunwoo::Vector4() const
{
	return Vector4(x, y, 0.f, 0.f);
}

hyunwoo::Vector2::operator hyunwoo::Vector2Int() const
{
	return Vector2Int(std::lround(x), std::lround(y));
}

hyunwoo::Vector2::operator hyunwoo::Vector3Int() const
{
	return Vector3Int(std::lround(x), std::lround(y));
}


hyunwoo::Vector2::operator hyunwoo::Vector4Int() const
{
	return Vector4Int(std::lround(x), std::lround(y));
}


/**정수버전....**/
const Vector2Int Vector2Int::Zero    = { 0, 0 };
const Vector2Int Vector2Int::One     = { 1, 1 };
const Vector2Int Vector2Int::BasisX  = { 1, 0 };
const Vector2Int Vector2Int::BasisY  = { 0, 1 };
const Vector2Int Vector2Int::Up      = { 0, 1 };
const Vector2Int Vector2Int::Down    = { 0, -1 };
const Vector2Int Vector2Int::Right   = { 1, 0 };
const Vector2Int Vector2Int::Left    = { -1, 0 };

const int Vector2Int::GetMagnitude() const {
	return Math::Sqrt(GetSqrMagnitude());
}

hyunwoo::Vector2Int::operator hyunwoo::Vector2() const
{
	return Vector2(x,y);
}

hyunwoo::Vector2Int::operator hyunwoo::Vector3() const
{
	return Vector3(x, y);
}

hyunwoo::Vector2Int::operator hyunwoo::Vector4() const
{
	return Vector4(x, y);
}

hyunwoo::Vector2Int::operator hyunwoo::Vector3Int() const
{
	return Vector3Int(x, y);
}

hyunwoo::Vector2Int::operator hyunwoo::Vector4Int() const
{
	return Vector4Int(x, y);
}




/*********************************************************
 *     Vector3 관련 정의...
 *********/
const Vector3 Vector3::Zero    = { 0.f, 0.f, 0.f  };
const Vector3 Vector3::One     = { 1.f, 1.f, 1.f  };
const Vector3 Vector3::BasisX  = { 1.f, 0.f, 0.f  };
const Vector3 Vector3::BasisY  = { 0.f, 1.f, 0.f  };
const Vector3 Vector3::BasisZ  = { 0.f, 0.f, 1.f  };
const Vector3 Vector3::Up      = { 0.f, 1.f, 0.f  };
const Vector3 Vector3::Down    = { 0.f, -1.f, 0.f };
const Vector3 Vector3::Right   = { 1.f, 0.f, 0.f  };
const Vector3 Vector3::Left    = { -1.f, 0.f, 0.f };
const Vector3 Vector3::Forward = { 0.f, 0.f, 1.f  };
const Vector3 Vector3::Back    = { 0.f, 0.f, -1.f };

const float Vector3::GetMagnitude() const {
	return Math::Sqrt(GetSqrMagnitude());
}

hyunwoo::Vector3::operator hyunwoo::Vector2() const
{
	return Vector2(x, y);
}

hyunwoo::Vector3::operator hyunwoo::Vector4() const
{
	return Vector4(x, y, z, 0.f);
}

hyunwoo::Vector3::operator hyunwoo::Vector2Int() const
{
	return Vector2Int(std::lround(x), std::lround(y));
}

hyunwoo::Vector3::operator hyunwoo::Vector3Int() const
{
	return Vector3Int(std::lround(x), std::lround(y), std::lround(z));
}

hyunwoo::Vector3::operator hyunwoo::Vector4Int() const
{
	return Vector4Int(std::lround(x), std::lround(y), std::lround(z));
}


/**정수버전....**/
const Vector3Int Vector3Int::Zero	 = { 0, 0, 0  };
const Vector3Int Vector3Int::One     = { 1, 1, 1  };
const Vector3Int Vector3Int::BasisX  = { 1, 0, 0  };
const Vector3Int Vector3Int::BasisY  = { 0, 1, 0  };
const Vector3Int Vector3Int::BasisZ  = { 0, 0, 1  };
const Vector3Int Vector3Int::Up      = { 0, 1, 0  };
const Vector3Int Vector3Int::Down    = { 0, -1, 0 };
const Vector3Int Vector3Int::Right   = { 1, 0, 0  };
const Vector3Int Vector3Int::Left    = { -1, 0, 0 };
const Vector3Int Vector3Int::Forward = { 0, 0, 1  };
const Vector3Int Vector3Int::Back    = { 0, 0, -1 };

const int Vector3Int::GetMagnitude() const {
	return Math::Sqrt(GetSqrMagnitude());
}

hyunwoo::Vector3Int::operator hyunwoo::Vector2() const
{
	return Vector2(x, y);
}

hyunwoo::Vector3Int::operator hyunwoo::Vector3() const
{
	return Vector3(x, y, z);
}

hyunwoo::Vector3Int::operator hyunwoo::Vector4() const
{
	return Vector4(x, y, z);
}

hyunwoo::Vector3Int::operator hyunwoo::Vector2Int() const
{
	return Vector2Int(x, y);
}

hyunwoo::Vector3Int::operator hyunwoo::Vector4Int() const
{
	return Vector4Int(x, y, z);
}



/*********************************************************
 *     Vector4 관련 정의...
 *********/
const Vector4 Vector4::Zero   = { 0.f, 0.f, 0.f, 0.f };
const Vector4 Vector4::One    = { 1.f, 1.f, 1.f, 1.f };
const Vector4 Vector4::BasisX = { 1.f, 0.f, 0.f, 0.f };
const Vector4 Vector4::BasisY = { 0.f, 1.f, 0.f, 0.f };
const Vector4 Vector4::BasisZ = { 0.f, 0.f, 1.f, 0.f };
const Vector4 Vector4::BasisW = { 0.f, 0.f, 0.f, 1.f };

const float Vector4::GetMagnitude() const {
	return Math::Sqrt(GetSqrMagnitude());
}

hyunwoo::Vector4::operator hyunwoo::Vector2() const
{
	return Vector2(x,y);
}

hyunwoo::Vector4::operator hyunwoo::Vector3() const
{
	return Vector3(x,y,z);
}

hyunwoo::Vector4::operator hyunwoo::Vector2Int() const
{
	return Vector2Int(std::lround(x), std::lround(y));
}

hyunwoo::Vector4::operator hyunwoo::Vector3Int() const
{
	return Vector3Int(std::lround(x), std::lround(y), std::lround(z));
}

hyunwoo::Vector4::operator hyunwoo::Vector4Int() const
{
	return Vector4Int(std::lround(x), std::lround(y), std::lround(z), std::lround(w));
}


/**정수버전....**/
const Vector4Int Vector4Int::Zero    = { 0, 0, 0, 0 };
const Vector4Int Vector4Int::One     = { 1, 1, 1, 1 };
const Vector4Int Vector4Int::BasisX  = { 1, 0, 0, 0 };
const Vector4Int Vector4Int::BasisY  = { 0, 1, 0, 0 };
const Vector4Int Vector4Int::BasisZ  = { 0, 0, 1, 0 };
const Vector4Int Vector4Int::BasisW  = { 0, 0, 0, 1 };

const int Vector4Int::GetMagnitude() const {
	return Math::Sqrt(GetSqrMagnitude());
}

hyunwoo::Vector4Int::operator hyunwoo::Vector2() const
{
	return Vector2(x,y);
}

hyunwoo::Vector4Int::operator hyunwoo::Vector3() const
{
	return Vector3(x, y, z);
}

hyunwoo::Vector4Int::operator hyunwoo::Vector4() const
{
	return Vector4(x, y, z, w);
}

hyunwoo::Vector4Int::operator hyunwoo::Vector2Int() const
{
	return Vector2Int(x, y);
}

hyunwoo::Vector4Int::operator hyunwoo::Vector3Int() const
{
	return Vector3Int(x, y);
}