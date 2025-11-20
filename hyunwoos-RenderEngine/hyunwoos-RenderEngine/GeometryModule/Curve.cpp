#include "Curve.h"

/*=======================================================================================================================================
  *    주어진 정규화된 시간값을 통해서, 1차 베지어 곡선에 대한 y값을 얻습니다....
  *********/
float hyunwoo::LinearBezierCurve::Evaluate(float normalizedTime) const
{
    const float A = Yvalues[0];
    const float B = Yvalues[1];

    const float P = A + ((B - A) * normalizedTime);

    return P;
}


/*=======================================================================================================================================
  *    주어진 정규화된 시간값을 통해서, 2차 베지어 곡선에 대한 y값을 얻습니다....
  *********/
float hyunwoo::QuadraticBezierCurve::Evaluate(float normalizedTime) const
{
    const float A = Yvalues[0];
    const float B = Yvalues[1];
    const float C = Yvalues[2];

    const float E = (A + (B - A) * normalizedTime);
    const float F = (B + (C - B) * normalizedTime);

    const float P = E + ((F - E) * normalizedTime);

    return P;
}



/*=======================================================================================================================================
  *    주어진 정규화된 시간값을 통해서, 3차 베지어 곡선에 대한 y값을 얻습니다....
  *********/
float hyunwoo::CubicBezierCurve::Evaluate(float normalizedTime) const
{
    const float A = Yvalues[0];
    const float B = Yvalues[1];
    const float C = Yvalues[2];
    const float D = Yvalues[3];

    const float E = (A + (B - A) * normalizedTime);
    const float F = (B + (C - B) * normalizedTime);
    const float G = (C + (D - C) * normalizedTime);

    const float Q = E + ((F - E) * normalizedTime);
    const float R = F + ((G - F) * normalizedTime);

    const float P = Q + ((R - Q) * normalizedTime);

    return P;
}

