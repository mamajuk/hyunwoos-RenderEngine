#include "Matrix.h"

//===================================================================
////////               Matrix2x2 methods defines..          /////////
//===================================================================
const hyunwoo::Matrix2x2 hyunwoo::Matrix2x2::Identity = Matrix2x2(Vector2::BasisX, Vector2::BasisY);



/*==============================================================
 *    해당 2x2 Matrix 객체의 전치에 대한 메소드들...
 *==========*/
hyunwoo::Matrix2x2& hyunwoo::Matrix2x2::Transpose()
{
    float temp = BasisY.x;
    BasisY.x   = BasisX.y;
    BasisX.y   = temp;
    return *this;
}

hyunwoo::Matrix2x2 hyunwoo::Matrix2x2::GetTranspose() const
{
    return Matrix2x2{
        Vector2(BasisX.x, BasisY.x),
        Vector2(BasisX.y, BasisY.y)
    };
}


/*====================================================================
 *   해당 2x2 Matrix 객체의 캐스팅 관련 메소드들...
 *===========*/
hyunwoo::Matrix2x2::operator hyunwoo::Matrix3x3() const
{
    return Matrix3x3(BasisX, BasisY, Vector3::BasisZ);
}

hyunwoo::Matrix2x2::operator hyunwoo::Matrix4x4() const
{
    return Matrix4x4(BasisX, BasisY, Vector4::BasisZ, Vector4::BasisW);
}















//===================================================================
////////               Matrix3x3 methods defines..          /////////
//===================================================================
const hyunwoo::Matrix3x3 hyunwoo::Matrix3x3::Identity = Matrix3x3(Vector3::BasisX, Vector3::BasisY, Vector3::BasisZ);


 /*=================================================================
  *    해당 3x3 Matrix 객체의 전치 관련 메소드...
  *==========*/
hyunwoo::Matrix3x3& hyunwoo::Matrix3x3::Transpose()
{
    const Matrix3x3 temp = *this;

    BasisX = Vector3(BasisX.x, BasisY.x, BasisZ.x);
    BasisY = Vector3(BasisX.y, BasisY.y, BasisZ.y);
    BasisZ = Vector3(BasisX.z, BasisY.z, BasisZ.z);
    return *this;
}

hyunwoo::Matrix3x3 hyunwoo::Matrix3x3::GetTranspose() const
{
    return Matrix3x3{
        Vector3(BasisX.x, BasisY.x, BasisZ.x),
        Vector3(BasisX.y, BasisY.y, BasisZ.y),
        Vector3(BasisX.z, BasisY.z, BasisZ.z)
    };
}


/*====================================================================
 *   해당 3x3 Matrix 객체의 캐스팅 관련 메소드들...
 *===========*/
hyunwoo::Matrix3x3::operator hyunwoo::Matrix4x4() const
{
    return Matrix4x4(BasisX, BasisY, BasisZ, Vector4::BasisW);
}

hyunwoo::Matrix3x3::operator hyunwoo::Matrix2x2() const
{
    return Matrix2x2(BasisX, BasisY);
}












//===================================================================
////////               Matrix4x4 methods defines..          /////////
//===================================================================
const hyunwoo::Matrix4x4 hyunwoo::Matrix4x4::Identity = Matrix4x4(Vector4::BasisX, Vector4::BasisY, Vector4::BasisZ, Vector4::BasisW);


 /*=================================================================
  *    해당 4x4 Matrix 객체의 전치 관련 메소드들...
  *==========*/
hyunwoo::Matrix4x4& hyunwoo::Matrix4x4::Transpose()
{
    const Matrix4x4 temp = *this;

    BasisX = Vector4(BasisX.x, BasisY.x, BasisZ.x, BasisW.x);
    BasisY = Vector4(BasisX.y, BasisY.y, BasisZ.y, BasisW.y);
    BasisZ = Vector4(BasisX.z, BasisY.z, BasisZ.z, BasisW.z);
    BasisW = Vector4(BasisX.w, BasisY.w, BasisZ.w, BasisW.w);
    return *this;
}

hyunwoo::Matrix4x4 hyunwoo::Matrix4x4::GetTranspose() const
{
    return Matrix4x4{
        Vector4(BasisX.x, BasisY.x, BasisZ.x, BasisW.x),
        Vector4(BasisX.y, BasisY.y, BasisZ.y, BasisW.y),
        Vector4(BasisX.z, BasisY.z, BasisZ.z, BasisW.z),
        Vector4(BasisX.w, BasisY.w, BasisZ.w, BasisW.w)
    };
}


/*====================================================================
 *   해당 4x4 Matrix 객체의 캐스팅 관련 메소드들...
 *===========*/
hyunwoo::Matrix4x4::operator hyunwoo::Matrix2x2() const
{
    return Matrix2x2(BasisX, BasisY);
}

hyunwoo::Matrix4x4::operator hyunwoo::Matrix3x3() const
{
    return Matrix3x3(BasisX, BasisY, BasisZ);
}