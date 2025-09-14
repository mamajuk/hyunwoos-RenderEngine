#include <cmath>
#include "Color.h"
using namespace hyunwoo;


/******************************************************
 *   LinearColor의 필드/메소드들의 정의....
 *******/

/*===============================================================================================
 *    LinearColor의 정적 맴버들의 초기화....
 *=========*/
const LinearColor LinearColor::White  = LinearColor(1.f, 1.f, 1.f, 1.f);
const LinearColor LinearColor::Black  = LinearColor(0.f, 0.f, 0.f, 1.f);
const LinearColor LinearColor::Red    = LinearColor(1.f, 0.f, 0.f, 1.f);
const LinearColor LinearColor::Blue   = LinearColor(0.f, 0.f, 1.f, 1.f);
const LinearColor LinearColor::Green  = LinearColor(0.f, 1.f, 0.f, 1.f);
const LinearColor LinearColor::Yellow = LinearColor(1.f, (192.f/255.f), 0.f, 1.f);
const LinearColor LinearColor::Pink   = LinearColor(1.f, (62.f/255.f), 1.f, 1.f);
const LinearColor LinearColor::Purple = LinearColor((165.f/255.f),0.f,(216.f/255.f),1.f);



/*===============================================================================================
 *    LinearColor에서 Color로의 캐스트 메소드....
 *=========*/
hyunwoo::LinearColor::operator Color() const
{
    return Color(
        std::lround(r * 255.f),
        std::lround(g * 255.f),
        std::lround(b * 255.f),
        std::lround(a * 255.f)
    );
}











/*******************************************************
 *    Color의 필드/메소드들의 정의....
 *******/

/*===============================================================================================
 *    Color의 정적 맴버들의 초기화....
 *=========*/
const Color Color::White  = Color(255, 255, 255, 255);
const Color Color::Black  = Color(0, 0, 0, 255);
const Color Color::Red    = Color(255, 0, 0, 255);
const Color Color::Blue   = Color(0, 0, 255, 255);
const Color Color::Green  = Color(0, 255, 0, 255);
const Color Color::Yellow = Color(255, 192, 0, 255);
const Color Color::Pink   = Color(255, 62, 255, 255);
const Color Color::Purple = Color(165, 0, 216, 255);



/*===============================================================================================
 *    Color에서 LinearColor로의 캐스트 메소드....
 *=========*/
hyunwoo::Color::operator LinearColor() const
{
    return LinearColor(
        (R / 255.f),
        (G / 255.f),
        (B / 255.f),
        (A / 255.f)
    );
}
