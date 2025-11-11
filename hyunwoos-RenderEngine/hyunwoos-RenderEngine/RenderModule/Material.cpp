#include "Material.h"
#include "../MathModule/Quaternion.h"
#undef min
#undef max

/***************************************************
 *   Texture2D 메소드들의 정의....
 ******/

/*============================================================================================================
 *   지정한 위치의 픽셀을 얻습니다...
 *========*/
const hyunwoo::Color hyunwoo::Texture2D::GetPixel(const hyunwoo::Vector2& uvPos) const
{
	const Vector2Int texPos = Vector2((uvPos.x * (Width-1)), (uvPos.y * (Height-1)));
	const int		 idx    = (texPos.y * Width) + texPos.x;

	if (idx >= 0 && idx < (Width * Height)) {
		return Pixels[idx];
	}

	return Color::Pink;
}






/***************************************************
 *   Shader 메소드들의 정의....
 ******/

 /*============================================================================================================
  *   인자로 받은 finalMatrix를 vertex에 곱해 반환하는 버텍스 쉐이더입니다....
  *========*/
hyunwoo::Vector4 hyunwoo::Shader::VertexShader_MulFinalMat(const Vertex& inVertex, const Matrix4x4& inFinalMat)
{
	return (inFinalMat * Vector4(inVertex.ObjPos, 1.f));
}


/*============================================================================================================
 *    최종 색상을 핑크색으로 반환하는 프래그먼트 쉐이더입니다...
 *========*/
hyunwoo::Color hyunwoo::Shader::FragmentShader_InvalidTex(const Vector2& inUv, const hyunwoo::Vector3& inNormal, const hyunwoo::Texture2D& inTex)
{
	return Color::Pink;
}


/*============================================================================================================
 *    주어진 텍스쳐로부터 샘플링을 한 결과를 출력하는 프래그먼트 쉐이더입니다...
 *========*/
hyunwoo::Color hyunwoo::Shader::FragmentShader_Tex0Mapping(const Vector2& inUv, const hyunwoo::Vector3& inNormal, const hyunwoo::Texture2D& inTex)
{
	return inTex.GetPixel(inUv);
}
