#include <cmath>
#include "Color.h"
using namespace hyunwoo;

const LinearColor LinearColor::White  = LinearColor(1.f, 1.f, 1.f, 1.f);
const LinearColor LinearColor::Black  = LinearColor(0.f, 0.f, 0.f, 1.f);
const LinearColor LinearColor::Red    = LinearColor(1.f, 0.f, 0.f, 1.f);
const LinearColor LinearColor::Blue   = LinearColor(0.f, 0.f, 1.f, 1.f);
const LinearColor LinearColor::Green  = LinearColor(0.f, 1.f, 0.f, 1.f);
const LinearColor LinearColor::Yellow = LinearColor(1.f, (192.f/255.f), 0.f, 1.f);
const LinearColor LinearColor::Pink   = LinearColor(1.f, (62.f/255.f), 1.f, 1.f);
const LinearColor LinearColor::Purple = LinearColor((165.f/255.f),0.f,(216.f/255.f),1.f);

DWORD hyunwoo::LinearColor::ToDWORD_rgba(int rOrder, int gOrder, int bOrder, int aOrder) const
{
    DWORD ret = 0;
    ret |= std::lround(r * 255.f) << (rOrder * 8);
    ret |= std::lround(g * 255.f) << (gOrder * 8);
    ret |= std::lround(b * 255.f) << (bOrder * 8);
    ret |= std::lround(a * 255.f) << (aOrder * 8);

    return ret;
}

DWORD hyunwoo::LinearColor::ToDWORD_rgb(int rOrder, int gOrder, int bOrder) const
{
    DWORD ret = 0;
    ret |= std::lround(r * 255.f) << (rOrder * 8);
    ret |= std::lround(g * 255.f) << (gOrder * 8);
    ret |= std::lround(b * 255.f) << (bOrder * 8);

    return ret;
}
