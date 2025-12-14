#include "Renderer.h"
#include <cstdint>
#include <emmintrin.h>
#include <xmmintrin.h>
#include "../UtilityModule/StringLamda.h"


/*============================================================================================================
 *
 ********************/
inline void hyunwoo::Renderer::TriangleDescription::SetScreenPositions(const Vector2& screenPos1, const Vector2& screenPos2, const Vector2& screenPos3) {
    ScreenPositions[0] = screenPos1;
    ScreenPositions[1] = screenPos2;
    ScreenPositions[2] = screenPos3;
}

inline void hyunwoo::Renderer::TriangleDescription::SetUvPositions(const Vector2& uvPos1, const Vector2& uvPos2, const Vector2& uvPos3) {
    Uvs[0] = uvPos1;
    Uvs[1] = uvPos2;
    Uvs[2] = uvPos3;
}

inline void hyunwoo::Renderer::TriangleDescription::SetDepths(const float depth1, const float depth2, const float depth3) {
    Depths[0] = depth1;
    Depths[1] = depth2;
    Depths[2] = depth3;
}










/*=============================================================================================================
 *    좌표계 변환 메소드...
 *============*/
hyunwoo::Vector2 hyunwoo::Renderer::NDCToScreen(const Vector4& ndcPos, const ViewPort& vp)
{
    /*****************************************************************
     *   모든 좌표가 -1 ~ 1 사이인 NDC 좌표에, 스크린의 절반 크기를 곱해
     *   스크린 좌표로 변환시킨다....
     ******/
    return WorldToScreen(Vector2(
        (ndcPos.x * vp.RenderTarget.GetBackBufferWidthf() * .5f),
        (ndcPos.y * vp.RenderTarget.GetBackBufferHeightf() * .5f)), vp
    );
}

hyunwoo::Vector3 hyunwoo::Renderer::ClipToNDC(const Vector4& clipPos)
{
    /******************************************************************
     *   클립좌표계로 변환하기 전의 z값이 보존된 w값으로, 클립 좌표의 모든
     *   원소를 나누어 모든 축이 -1~1로 정규화된 NDC 좌표계로 변환한다...
     ******/
    const float wDiv = (clipPos.w==0.f? 0.f:(1.f / clipPos.w));
    return Vector3((clipPos.x * wDiv), (clipPos.y * wDiv), (clipPos.z * wDiv));
}

hyunwoo::Vector2 hyunwoo::Renderer::WorldToScreen(const hyunwoo::Vector2& cartesianPos, const ViewPort& vp)
{
    /****************************************************************
     *   화면 정중앙이 원점인 카타시안 좌표계 기반의 '월드' 좌표를,
     *   화면 맨 좌측 상단이 원점인 스크린 좌표계로 변환합니다....
     *******/
    return Vector2(
        (cartesianPos.x  + vp.RenderTarget.GetBackBufferWidthf() * .5f), 
        (-cartesianPos.y + vp.RenderTarget.GetBackBufferHeightf() * .5f)
    );
}

hyunwoo::Vector2 hyunwoo::Renderer::ScreenToWorld(const hyunwoo::Vector2& screenPos, const ViewPort& vp)
{
    /****************************************************************
     *   화면 맨 좌측 상단이 원점인 스크린 좌표계를, 화면 정중앙이
     *   원점인 카타시안 좌표계 기반의 '월드' 좌표로 변환합니다....
     *******/
    return Vector2(
        (screenPos.x - vp.RenderTarget.GetBackBufferWidthf() * .5f),
        -(screenPos.y - vp.RenderTarget.GetBackBufferHeightf() * .5f)
    );
}















/*=====================================================================================================================
 *   주어진 삼각형들의 클립핑 결과를 clipTriangleList에 담습니다....
 *****************/
void hyunwoo::Renderer::ClippingTriangle(ClipTriangleList& clipTriangleList, ClippingTestFunc* clippingTestFunc, SolveTFunc* solveTFunc)
{
    struct ClippingDescription
    {
        uint32_t from, to1, to2;
    };


    uint32_t		    clipping_desc_count = 0;
    const uint32_t      clipping_test_count = clipTriangleList.triangleCount;
    ClippingDescription clipping_descs[3];

    for (uint32_t i = 0; i < clipping_test_count; i++)
    {
        ClipTriangle& cur_triangle = clipTriangleList.Triangles[i];

        /****************************************************************
         *   클립핑이 적용되어야할 점에 대한 서술자를 작성한다....
         *******/
        clipping_desc_count = 0;

        //첫번째 점에 대한 클립핑 서술자를 작성한다...
        if (clippingTestFunc(cur_triangle.Vertices[0].ClipPos) == true) {
            clipping_descs[clipping_desc_count++] = ClippingDescription{ 0, 1, 2 };
        }

        //두번째 점에 대한 클립핑 서술자를 작성한다...
        if (clippingTestFunc(cur_triangle.Vertices[1].ClipPos) == true) {
            clipping_descs[clipping_desc_count++] = ClippingDescription{ 1, 0, 2 };
        }

        //세번째 점에 대한 클립핑 서술자를 작성한다...
        if (clippingTestFunc(cur_triangle.Vertices[2].ClipPos) == true) {
            clipping_descs[clipping_desc_count++] = ClippingDescription{ 2, 0, 1 };
        }



        /***************************************************************
         *   클립핑할 점이 하나일 경우의 처리...
         *******/
        if (clipping_desc_count == 1) {
            const ClippingDescription& desc = clipping_descs[0];

            const ClipVertex  fromVertex = cur_triangle.Vertices[desc.from];
            const ClipVertex& toVertex1  = cur_triangle.Vertices[desc.to1];
            const ClipVertex& toVertex2  = cur_triangle.Vertices[desc.to2];

            const float t1 = solveTFunc(fromVertex.ClipPos, toVertex1.ClipPos);
            const float t2 = solveTFunc(fromVertex.ClipPos, toVertex2.ClipPos);

            //첫번째 삼각형의 버텍스를 갱신한다...
            cur_triangle.Vertices[desc.from].ClipPos = (fromVertex.ClipPos * t1) + (toVertex1.ClipPos * (1.f - t1));
            cur_triangle.Vertices[desc.from].UvPos   = (fromVertex.UvPos * t1) + (toVertex1.UvPos * (1.f - t1));

            //두번째 삼각형의 버텍스를 갱신한다...
            ClipTriangle& nxt_triangle = clipTriangleList.Triangles[clipTriangleList.triangleCount++];
            nxt_triangle.Vertices[0]         = cur_triangle.Vertices[desc.from];
            nxt_triangle.Vertices[1]         = toVertex2;
            nxt_triangle.Vertices[2].ClipPos = (fromVertex.ClipPos * t2) + (toVertex2.ClipPos * (1.f - t2));
            nxt_triangle.Vertices[2].UvPos   = (fromVertex.UvPos * t2) + (toVertex2.UvPos * (1.f - t2));
        }



        /*****************************************************************
         *   클립핑할 점이 두 개일 경우의 처리...
         *******/
        else if (clipping_desc_count == 2) {
            const ClippingDescription& desc1 = clipping_descs[0];
            const ClippingDescription& desc2 = clipping_descs[1];

            ClipVertex&       fromVertex1 = cur_triangle.Vertices[desc1.from];
            ClipVertex&       fromVertex2 = cur_triangle.Vertices[desc2.from];
            const ClipVertex& toVertex    = cur_triangle.Vertices[(3 - desc1.from - desc2.from)];

            const float t1 = solveTFunc(fromVertex1.ClipPos, toVertex.ClipPos);
            const float t2 = solveTFunc(fromVertex2.ClipPos, toVertex.ClipPos);

            //기존 삼각형의 버텍스를 갱신한다...
            fromVertex1.ClipPos = (toVertex.ClipPos * (1.f - t1)) + (fromVertex1.ClipPos * t1);
            fromVertex1.UvPos   = (toVertex.UvPos * (1.f - t1)) + (fromVertex1.UvPos * t1);

            fromVertex2.ClipPos = (toVertex.ClipPos * (1.f - t2)) + (fromVertex2.ClipPos * t2);
            fromVertex2.UvPos   = (toVertex.UvPos * (1.f - t2)) + (fromVertex2.UvPos * t2);
        }


        /*******************************************************************
         *  클립핑할 점이 세 개일 경우, 해당 삼각형을 목록에서 제거한다..
         *******/
        else if (clipping_desc_count == 3) {
            cur_triangle = clipTriangleList.Triangles[clipTriangleList.triangleCount - 1];
            clipTriangleList.triangleCount--;
        }

    }
}












/*=================================================================================================================
 *   주어진 클립 좌표가 클립핑이 되어야하는지를 판별하는 메소드들....
 **************/
bool hyunwoo::Renderer::ClippingTest_Far(const Vector4& clipPos)
{
    return (clipPos.z > clipPos.w);
}

bool hyunwoo::Renderer::ClippingTest_Near(const Vector4& clipPos)
{
    return (clipPos.z < -clipPos.w);
}

bool hyunwoo::Renderer::ClippingTest_Right(const Vector4& clipPos)
{
    return (clipPos.x > clipPos.w);
}

bool hyunwoo::Renderer::ClippingTest_Left(const Vector4& clipPos)
{
    return (clipPos.x < -clipPos.w);
}

bool hyunwoo::Renderer::ClippingTest_Up(const Vector4& clipPos)
{
    return (clipPos.y > clipPos.w);
}

bool hyunwoo::Renderer::ClippingTest_Down(const Vector4& clipPos)
{
    return (clipPos.y < -clipPos.w);
}













/*=================================================================================================================
 *   두 클립 좌표 간의 선형보간을 했을 때, 특정 클립 좌표가 나오도록하는 상수 T를 구하는 메소드들....
 **************/
float hyunwoo::Renderer::SolveT_Far(const Vector4& fromClipPos, const Vector4& toClipPos)
{
    return (toClipPos.w - toClipPos.z) / (-toClipPos.z + fromClipPos.z + toClipPos.w - fromClipPos.w);
}

float hyunwoo::Renderer::SolveT_Near(const Vector4& fromClipPos, const Vector4& toClipPos)
{
    return (-toClipPos.w - toClipPos.z) / (-toClipPos.z + fromClipPos.z - toClipPos.w + fromClipPos.w);
}

float hyunwoo::Renderer::SolveT_Right(const Vector4& fromClipPos, const Vector4& toClipPos)
{
    return (toClipPos.w - toClipPos.x) / (-toClipPos.x + fromClipPos.x + toClipPos.w - fromClipPos.w);
}

float hyunwoo::Renderer::SolveT_Left(const Vector4& fromClipPos, const Vector4& toClipPos)
{
    return (-toClipPos.w - toClipPos.x) / (-toClipPos.x + fromClipPos.x - toClipPos.w + fromClipPos.w);
}

float hyunwoo::Renderer::SolveT_Up(const Vector4& fromClipPos, const Vector4& toClipPos)
{
    return (toClipPos.w - toClipPos.y) / (-toClipPos.y + fromClipPos.y + toClipPos.w - fromClipPos.w);
}

float hyunwoo::Renderer::SolveT_Down(const Vector4& fromClipPos, const Vector4& toClipPos)
{
    return (-toClipPos.w - toClipPos.y) / (-toClipPos.y + fromClipPos.y - toClipPos.w + fromClipPos.w);
}









/*=================================================================================================================
 *   가공한 비트맵을 화면 DC에 최종 제출합니다....
 *=============*/
void hyunwoo::Renderer::Present(HWND clientHwnd, const ViewPort& vp)
{
    if (vp.RenderTarget.IsInit() == false) {
        return;
    }

    /************************************************************
     *  GetDC() 대신 BeginPaint()를 사용하여 화면 DC를 얻어오는데,
     *  이는 WM_PAINT 메세지에서 무효화 영역을 처리할 때, 해당 영역만
     *  처리한다는 클립핑 정보가 담겨 효율적인 처리가 가능하기 때문이다.
     ******/
    HDC         hdc;
    RECT        rect;
    PAINTSTRUCT ps;
    GetClientRect(clientHwnd, &rect);
    hdc = BeginPaint(clientHwnd, &ps);

    /************************************************************
     *   화면DC를 가져온 후, 메모리 DC의 내용을 고속복사해 출력한다...
     ******/
    const UINT width  = (vp.ClientRect.RightBottom.x - vp.ClientRect.LeftTop.x);
    const UINT height = (vp.ClientRect.RightBottom.y - vp.ClientRect.LeftTop.y);

    StretchBlt(
        hdc, 
        vp.ClientRect.LeftTop.x, vp.ClientRect.LeftTop.y, 
        width, height, vp.RenderTarget.GetMemoryDC(), 0, 0,
        vp.RenderTarget.GetBackBufferWidth(), 
        vp.RenderTarget.GetBackBufferHeight(), SRCCOPY
    );

    EndPaint(clientHwnd, &ps);
}









/*===================================================================================================================
 *   화면을 특정 색깔로 초기화합니다...
 *==========*/
void hyunwoo::Renderer::ClearScreen(const ViewPort& vp)
{
    if (vp.RenderTarget.IsInit() == false) {
        return;
    }

    /***********************************************************************************
     *   백버퍼를 초기화합니다...
     *********/
    {
        DWORD* endPtr       = vp.RenderTarget.GetBackBufferPixels() + vp.RenderTarget.GetTotalPixelNum();
        DWORD* alignedBegin = reinterpret_cast<DWORD*>(reinterpret_cast<uintptr_t>(vp.RenderTarget.GetBackBufferPixels()) + 15 & -16); //메모리 영역에서 16으로 정렬이 시작되는 주소값..
        DWORD* alignedEnd   = reinterpret_cast<DWORD*>(reinterpret_cast<uintptr_t>(endPtr) & -16);  //메모리 영역에서 16으로 정렬된 구간이 끝나는 주소값...

        const DWORD clearColor = ClearColor.ARGB;


        /*-------------------------------------------------------
         *   SIMD 코드는 주소값이 16으로 정렬되어 있어야 한다.
         *   따라서 비트맵의 시작 주소부터 16으로 정렬된 주소까지는
         *   SIMD 연산을 사용하지 않고 일반 for문으로 처리한다..
         *------*/
        for (DWORD* i = endPtr; i < alignedBegin; i++) {
            *i = clearColor;
        }


        /*--------------------------------------------------------
         *   정렬된 주소에 진입했다면, 마지막 정렬 주소까지
         *    SIMD 연산으로 한 번에 16bytes씩 처리한다....
         *-------*/
        __m128i rgba = _mm_set1_epi32(clearColor);

        for (DWORD* i = alignedBegin; i < alignedEnd; i += 4) {
            _mm_store_si128((__m128i*)i, rgba);
        }


        /*-----------------------------------------------
         *  정렬된 주소 이후, 남은 원소들을 처리한다...
         *---------*/
        for (DWORD* i = alignedEnd; i < endPtr; i++) {
            *i = clearColor;
        }
    }



    /***************************************************************************************
     *   깊이 버퍼를 초기화합니다....
     *******/
    {
        float* endPtr       = vp.RenderTarget.GetDepthBufferValues() + vp.RenderTarget.GetTotalPixelNum();
        float* alignedBegin = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(vp.RenderTarget.GetDepthBufferValues()) + 15 & -16); //메모리 영역에서 16으로 정렬이 시작되는 주소값..
        float* alignedEnd   = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(endPtr) & -16);  //메모리 영역에서 16으로 정렬된 구간이 끝나는 주소값...

        constexpr float clearValue = std::numeric_limits<float>::infinity();

        /*-------------------------------------------------------
         *   SIMD 코드는 주소값이 16으로 정렬되어 있어야 한다.
         *   따라서 비트맵의 시작 주소부터 16으로 정렬된 주소까지는
         *   SIMD 연산을 사용하지 않고 일반 for문으로 처리한다..
         *------*/
        for (float* i = endPtr; i < alignedBegin; i++) {
            *i = clearValue;
        }


        /*--------------------------------------------------------
         *   정렬된 주소에 진입했다면, 마지막 정렬 주소까지
         *    SIMD 연산으로 한 번에 16bytes씩 처리한다....
         *-------*/
        __m128 inf = _mm_set1_ps(clearValue);

        for (float* i = alignedBegin; i < alignedEnd; i += 4) {
            _mm_store_ps(i, inf);
        }


        /*-----------------------------------------------
         *  정렬된 주소 이후, 남은 원소들을 처리한다...
         *---------*/
        for (float* i = alignedEnd; i < endPtr; i++) {
            *i = clearValue;
        }
    }
}










/*===================================================================================================================
 *   백버퍼에서 주어진 두 점 사이의 선을 그립니다....
 *==================*/
void hyunwoo::Renderer::DrawLine(const Color& color, const Vector2& startScreenPos, const Vector2& endScreenPos, const ViewPort& vp, bool useClipping)
{
    float   w        = vp.RenderTarget.GetBackBufferWidthf();
    float   h        = vp.RenderTarget.GetBackBufferHeightf();
    Vector2 startPos = startScreenPos;
    Vector2 endPos   = endScreenPos;

    /************************************************************
     *   클립핑을 사용하지 않는다면, 바로 선 긋기 과정으로 이동한다...
     *******/
    if (useClipping) 
    {
        /*----------------------------------------------
         *   두 점 사이의 선이 스크린에 포함되지 않는가?
         *   맞다면 해당 함수를 종료시킨다....
         *---------*/

        //두 점이 모두 스크린 하측 이후에 있는가?
        if (startPos.y > h && endPos.y > h) {
            return;
        }

        //두 점이 모두 스크린 상측 이후에 있는가?
        if (startPos.y < 0 && endPos.y < 0) {
            return;
        }

        //두 점이 모두 스크린 좌측 이후에 있는가?
        if (startPos.x < 0 && endPos.y < 0) {
            return;
        }

        //두 점이 모두 스크린 우측 이후에 있는가?
        if (startPos.x > w && endPos.x > w) {
            return;
        }
        


        /*-----------------------------------------------
         *   시작점에 대한 클립핑을 진행한다....
         *-------*/

        //해당 점이 스크린 맨 우측을 넘어섰을 경우의 클립핑...
        if (startPos.x > w) {
            float div = (startPos.x - endPos.x);
            float s   = ((w - endPos.x) / div);

            startPos = (startPos * s) + (endPos * (1.f - s));
        }

        //해당 점이 스크린 맨 좌측을 넘어섰을 경우의 클립핑...
        if (startPos.x < 0.f) {
            float div = (startPos.x - endPos.x);
            float s   = (-endPos.x / div);

            startPos = (startPos * s) + (endPos * (1.f - s));
        }

        //해당 점이 스크린 맨 상측을 넘어섰을 경우의 클립핑...
        if (startPos.y > h) {
            float div = (startPos.y - endPos.y);
            float s   = ((h - endPos.y) / div);

            startPos = (startPos * s) + (endPos * (1.f - s));
        }

        //해당 점이 스크린 맨 상측을 넘어섰을 경우의 클립핑...
        if (startPos.y < 0.f) {
            float div = (startPos.y - endPos.y);
            float s   = (-endPos.y / div);

            startPos = (startPos * s) + (endPos * (1.f - s));
        }


        /*-----------------------------------------------
         *   끝점에 대한 클립핑을 진행한다....
         *---------*/

         //해당 점이 스크린 맨 우측을 넘어섰을 경우의 클립핑...
        if (endPos.x > w) {
            float div = (endPos.x - startPos.x);
            float s   = ((w - startPos.x) / div);

            endPos = (endPos * s) + (startPos * (1.f - s));
        }

        //해당 점이 스크린 맨 좌측을 넘어섰을 경우의 클립핑...
        if (endPos.x < 0.f) {
            float div = (endPos.x - startPos.x);
            float s   = (-startPos.x / div);

            endPos = (endPos * s) + (startPos * (1.f - s));
        }

        //해당 점이 스크린 맨 상측을 넘어섰을 경우의 클립핑...
        if (endPos.y > h) {
            float div = (endPos.y - startPos.y);
            float s   = ((h - startPos.y) / div);

            endPos = (endPos * s) + (startPos * (1.f - s));
        }

        //해당 점이 스크린 맨 상측을 넘어섰을 경우의 클립핑...
        if (endPos.y < 0.f) {
            float div = (endPos.y - startPos.y);
            float s   = (-startPos.y / div);

            endPos = (endPos * s) + (startPos * (1.f - s));
        }
    }



    /************************************************************
     *     클립핑 된 두 점 사이의 선을 그린다....
     *******/
    const Vector2Int dst = (endPos - startPos);

    const int w2   = Math::Abs(dst.x);
    const int h2   = Math::Abs(dst.y);
    const int wDir = (dst.x >= 0 ? 1 : -1);
    const int hDir = (dst.y >= 0 ? 1 : -1);


    /*-------------------------------------------------
     *    w가 더 클경우....
     **-------*/
    Vector2Int cur = startPos;

    if (w2 >= h2) {
        int       d      = (2 * h2 - w2);
        const int dTrue  = (2 * h2);
        const int dFalse = (2 * (h2 - w2));

        /**선을 찍는다....*/
        for (int i = 0; i < w2; i++)
        {
            SetPixel(color, cur, vp);

            if (d < 0) d += dTrue;
            else
            {
                d     += dFalse;
                cur.y += hDir;
            }

            cur.x += wDir;
        }

        return;
    }


    /*----------------------------------------
     *   h가 더 클경우....
     **---------*/
    int       d      = (2 * w2 - h2);
    const int dTrue  = (2 * w2);
    const int dFalse = (2 * (w2 - h2));

    /**선을 찍는다....*/
    for (int i = 0; i < h2; i++)
    {
        SetPixel(color, cur, vp);

        if (d < 0) d += dTrue;
        else
        {
            d += dFalse;
            cur.x += wDir;
        }

        cur.y += hDir;
    }
}







/*=====================================================================================================================
 *   백버퍼의 특정 위치의 픽셀을, 지정한 색깔로 바꿉니다.
 *================*/
void hyunwoo::Renderer::SetPixel(const Color& color, const Vector2Int& screenPos, const ViewPort& vp)
{
    /***************************************************************
     *   초기화가 안되어있거나, 인덱스를 벗어나면 함수를 종료한다.
     *   백버퍼의 인덱스에서 y축이 뒤집어져 있기 때문에, 스크린좌표에서
     *   y축만 뒤집어서 계산한다....
     *****/
    const int idx = ((vp.RenderTarget.GetBackBufferHeight() - screenPos.y) * vp.RenderTarget.GetBackBufferWidth() + screenPos.x);
    if (vp.RenderTarget.IsInit() == false || idx<0 || idx >= vp.RenderTarget.GetTotalPixelNum()) {
        return;
    }

    SetPixel_internal(color, idx, 0, vp);
}

void hyunwoo::Renderer::SetPixel_internal(const Color& color, const uint32_t index, const float depth, const ViewPort& vp)
{
    /*************************************************************
     *    뒤쪽에 그려져야하는 경우...
     *********/
    float& cur_depth = vp.RenderTarget.GetDepthBufferValues()[index];

    if (cur_depth < depth) {
        //if (UseAlphaBlending == false) return;

        ////배경색과의 알파블랜딩을 진행한다....
        //LinearColor frontColor = color;
        //LinearColor backColor  = ClearColor;
        //
        ////직전의 결과 색상과 앞쪽 색상과의 알파블랜딩을 진행한다...
        //LinearColor frontColor2 = m_backBufferBitmapPtr[index];
        //LinearColor backColor2  = (backColor + (frontColor - backColor) * frontColor.A);
        //LinearColor finalColor = (backColor2 + (frontColor2 - backColor2) * frontColor2.A);

        //m_backBufferBitmapPtr[index] = Color(finalColor).ARGB;
        return;
    }




    /*************************************************************
     *   앞쪽에 그려져야하는 경우....
     *********/

    //깊이값을 갱신한다....
    cur_depth = depth;

    /*------------------------------------------
     *  알파 블랜드를 사용할 경우, 알파값에 따라서
     *  뒤쪽이 비치도록 한다....
     *-----*/
    //if (UseAlphaBlending && color.A < 255) {

    //    LinearColor goalColor = LinearColor(color);
    //    LinearColor prevColor = LinearColor(m_backBufferBitmapPtr[index]);
    //    m_backBufferBitmapPtr[index] = Color(prevColor + (goalColor - prevColor) * goalColor.A).ARGB;
    //}

    vp.RenderTarget.GetBackBufferPixels()[index] = color.ARGB;
}










/*=====================================================================================================================
 *    백버퍼의 지정한 위치에 문자열을 출력합니다.... 
 *=============*/
void hyunwoo::Renderer::DrawTextField(const std::wstring& out, const hyunwoo::Vector2Int& screenPos, const ViewPort& vp)
{
    if (vp.RenderTarget.IsInit() == false) {
        return;
    }

    RECT rc = { screenPos.x, screenPos.y };

    /******************************************************************
     *   출력할 텍스트의 전체 크기를 계산하고, 백버퍼에 텍스트를 그린다...
     *   (계산할 때, memDC가 사용하는 기본 폰트 GDI 오브젝트를 사용한다.)
     ******/
    DrawTextW(vp.RenderTarget.GetMemoryDC(), out.c_str(), -1, &rc, (DT_LEFT | DT_TOP | DT_CALCRECT));
    DrawTextW(vp.RenderTarget.GetMemoryDC(), out.c_str(), out.size(), &rc, (DT_LEFT | DT_TOP));
}












/*===================================================================================================================
 *    주어진 삼각형 서술 정보를 기반으로, 삼각형을 그립니다...
 *=============*/
void hyunwoo::Renderer::DrawTriangle(const TriangleDescription& triangleDesc, const ViewPort& vp)
{
    const Vector2& screenPos1 = triangleDesc.ScreenPositions[0];
    const Vector2& screenPos2 = triangleDesc.ScreenPositions[1];
    const Vector2& screenPos3 = triangleDesc.ScreenPositions[2];


    /*******************************************************************
     *    와이어 프레임 모드일 경우, 선만 그리고 종료한다...
     *******/
    if (UseWireFrameMode) {
        DrawLine(WireFrameColor, screenPos1, screenPos2, vp);
        DrawLine(WireFrameColor, screenPos1, screenPos3, vp);
        DrawLine(WireFrameColor, screenPos2, screenPos3, vp);
        return;
    }



    /*********************************************************************
     *    컨벡스 결합을 위해 필요한 값들을 계산한다...
     *******/
    const Vector2 u = (screenPos1 - screenPos3);
    const Vector2 v = (screenPos2 - screenPos3);

    const float uv   = Vector2::Dot(u, v);
    const float uu   = Vector2::Dot(u, u);
    const float vv   = Vector2::Dot(v, v);
    const float deno = (uv * uv - uu * vv);



    /**********************************************************************
     *     퇴화 삼각형인가? ( 분모가 0 ) 맞다면 함수를 종료한다...
     **********/
    if (deno == 0.0f) {
        return;
    }



    /************************************************************************
     *   두 점에 곱해지는 양의 스칼라값의 합이 1이 되도록 제한하면,
     *   두 점 사이의 직선 사이의 점들만 만들어진다. 그럼 세 점에 곱해지는
     *   양의 스칼라값의 합이 1이 되도록 제한하면, 세 점들에서 그려질 수 있는
     *   직선 사이의 점들만 그려지되, 스칼라의 가중치에 따라 특정 직선에
     *   치우져진 점이 만들어진다. 화면 해상도에 알맞는 픽셀들만 골라내어
     *   삼각형을 찍기 위해서, 세 점들이 모두 포함되는 범위를 구한 후, 해당
     *   범위에 있는 픽셀드을 순회한다. 그리고 한 점에서 다른 두 점을 향하는
     *   벡터의 내적을 통한 연립으로 세 점에 가해지는 스칼라값(무게중심좌표)
     *   를 구한다. 현재 처리할 점의 세 스칼라값의 범위가 0~1 사이면 삼각형
     *   범위 안에 있는 점이니, 점을 찍으면 된다..
     ********/
    const UINT totalPixelNum = vp.RenderTarget.GetTotalPixelNum();
    const UINT width         = vp.RenderTarget.GetBackBufferWidth();
    const UINT height        = vp.RenderTarget.GetBackBufferHeight();

    const int   xMin = Math::Min(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   xMax = Math::Max(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   yMin = Math::Min(screenPos1.y, screenPos2.y, screenPos3.y);
    const int   yMax = Math::Max(screenPos1.y, screenPos2.y, screenPos3.y);
    const float div = (1.f / deno);

    for (int y = yMin; y <= yMax; y++)
    {
        for (int x = xMin; x <= xMax; x++)
        {
            const Vector2  p  = Vector2(x, y);
            const Vector2  w  = (p - screenPos3);
            const float    wu = Vector2::Dot(w, u);
            const float    wv = Vector2::Dot(w, v);
            const float    s  = (wv * uv - wu * vv) * div;
            const float    t  = (wu * uv - wv * uu) * div;
            const float    r  = (1.f - s - t);

            /*-------------------------------------------------
             *   삼각형 범위 안에 있다면 점을 찍는다..
             *-----*/
            if ((s >= 0.f && s <= 1.f) && (t >= 0.f && t <= 1.f) && (r >= 0.f && r <= 1.f)) {
                const uint32_t idx   = ((height - p.y) * width + p.x);

                //점의 인덱스가 유효한 범위 안에 없다면 넘어간다...
                if (idx < 0 || idx>totalPixelNum) {
                    continue;
                }


                //깊이버퍼 안의 깊이값과 비교했을 때, 가려진다면 넘어간다...
                const float depth = (triangleDesc.Depths[0] * s) + (triangleDesc.Depths[1] * t) + (triangleDesc.Depths[2] * r);

                //텍스쳐 맵핑으로 삼각형을 채운다....
                if (triangleDesc.MappedTexture!=nullptr) {
                    Vector2 uvPos = (triangleDesc.Uvs[0] * s) + (triangleDesc.Uvs[1] * t) + (triangleDesc.Uvs[2] * r);
                    SetPixel_internal(triangleDesc.MappedTexture->GetPixel(uvPos), idx, depth, vp);
                }

                //주어진 색상으로 삼각형을 채운다...
                else SetPixel_internal(triangleDesc.FillUpColor, idx, depth, vp);
            }
        }
    }


}

void hyunwoo::Renderer::DrawClipTriangle_internal(const ClipTriangle& clipTriangle, const Vector3& normal, Shader::FragmentShaderFunc* fragmentShader, const Texture2D& tex, const ViewPort& vp)
{
    /*******************************************************************
     *   인자로 받은 클립 삼각형들을 스크린 좌표계로 변환시킨다...
     ********/
    const ClipVertex& clip_vertex1 = clipTriangle.Vertices[0];
    const ClipVertex& clip_vertex2 = clipTriangle.Vertices[1];
    const ClipVertex& clip_vertex3 = clipTriangle.Vertices[2];

    const Vector3 ndcPos1 = ClipToNDC(clip_vertex1.ClipPos);
    const Vector3 ndcPos2 = ClipToNDC(clip_vertex2.ClipPos);
    const Vector3 ndcPos3 = ClipToNDC(clip_vertex3.ClipPos);

    const Vector2 screenPos1 = NDCToScreen(ndcPos1, vp);
    const Vector2 screenPos2 = NDCToScreen(ndcPos2, vp);
    const Vector2 screenPos3 = NDCToScreen(ndcPos3, vp);



    /*******************************************************************
     *   삼각형의 노멀값을 표시해야한다면 표시한다....
     *******/
    if (DrawTriangleNormal) {
        const Vector2 goal_screenPos1 = NDCToScreen(ndcPos1 + (normal * 100.f), vp);
        const Vector2 goal_screenPos2 = NDCToScreen(ndcPos2 + (normal * 100.f), vp);
        const Vector2 goal_screenPos3 = NDCToScreen(ndcPos3 + (normal * 100.f), vp);

        DrawLine(Color::Red, screenPos1, goal_screenPos1, vp);
        DrawLine(Color::Red, screenPos2, goal_screenPos2, vp);
        DrawLine(Color::Red, screenPos3, goal_screenPos3, vp);
    }


    /*******************************************************************
     *    와이어 프레임 모드일 경우, 선만 그리고 종료한다...
     *******/
    if (UseWireFrameMode) {
        DrawLine(WireFrameColor, screenPos1, screenPos2, vp);
        DrawLine(WireFrameColor, screenPos1, screenPos3, vp);
        DrawLine(WireFrameColor, screenPos2, screenPos3, vp);
        return;
    }




    /*********************************************************************
     *    컨벡스 결합을 위해 필요한 값들을 계산한다...
     *******/
    const Vector2 u = (screenPos1 - screenPos3);
    const Vector2 v = (screenPos2 - screenPos3);

    const float uv = Vector2::Dot(u, v);
    const float uu = Vector2::Dot(u, u);
    const float vv = Vector2::Dot(v, v);
    const float deno = (uv * uv - uu * vv);



    /**********************************************************************
     *     퇴화 삼각형인가? ( 분모가 0 ) 맞다면 함수를 종료한다...
     **********/
    if (deno == 0.0f) {
        return;
    }



    /************************************************************************
     *   두 점에 곱해지는 양의 스칼라값의 합이 1이 되도록 제한하면,
     *   두 점 사이의 직선 사이의 점들만 만들어진다. 그럼 세 점에 곱해지는
     *   양의 스칼라값의 합이 1이 되도록 제한하면, 세 점들에서 그려질 수 있는
     *   직선 사이의 점들만 그려지되, 스칼라의 가중치에 따라 특정 직선에
     *   치우져진 점이 만들어진다. 화면 해상도에 알맞는 픽셀들만 골라내어
     *   삼각형을 찍기 위해서, 세 점들이 모두 포함되는 범위를 구한 후, 해당
     *   범위에 있는 픽셀드을 순회한다. 그리고 한 점에서 다른 두 점을 향하는
     *   벡터의 내적을 통한 연립으로 세 점에 가해지는 스칼라값(무게중심좌표)
     *   를 구한다. 현재 처리할 점의 세 스칼라값의 범위가 0~1 사이면 삼각형
     *   범위 안에 있는 점이니, 점을 찍으면 된다..
     ********/
    const UINT totalPixelNum = vp.RenderTarget.GetTotalPixelNum();
    const UINT width         = vp.RenderTarget.GetBackBufferWidth();
    const UINT height        = vp.RenderTarget.GetBackBufferHeight();

    const int   xMin = Math::Min(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   xMax = Math::Max(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   yMin = Math::Min(screenPos1.y, screenPos2.y, screenPos3.y);
    const int   yMax = Math::Max(screenPos1.y, screenPos2.y, screenPos3.y);
    const float div  = (1.f / deno);

    for (int y = yMin; y <= yMax; y++)
    {
        for (int x = xMin; x <= xMax; x++)
        {
            const Vector2  p = Vector2(x, y);
            const Vector2  w = (p - screenPos3);
            const float    wu = Vector2::Dot(w, u);
            const float    wv = Vector2::Dot(w, v);
            const float    s = (wv * uv - wu * vv) * div;
            const float    t = (wu * uv - wv * uu) * div;
            const float    r = (1.f - s - t);

            /*----------------------------------------------------
             *   삼각형 범위 안에 있다면 fragment Shader를 적용 후,
             *   최종 색상을 찍는다...
             *-----*/
            if ((s >= 0.f && s <= 1.f) && (t >= 0.f && t <= 1.f) && (r >= 0.f && r <= 1.f)) {
                const uint32_t idx = ((height - p.y) * width + p.x);

                //점의 인덱스가 유효한 범위 안에 없다면 넘어간다...
                if (idx < 0 || idx>totalPixelNum) {
                    continue;
                }


                //깊이버퍼 안의 깊이값과 비교했을 때, 가려진다면 넘어간다...
                const float   depth      = (clip_vertex1.ClipPos.w * s) + (clip_vertex2.ClipPos.w * t) + (clip_vertex3.ClipPos.w * r);
                const Vector2 uvPos      = (clip_vertex1.UvPos * s) + (clip_vertex2.UvPos * t) + (clip_vertex3.UvPos * r);
                const Color   finalColor = fragmentShader(uvPos, Vector3::Zero, tex);

                SetPixel_internal(finalColor, idx, depth, vp);
            }
        }
    }
}















/*==========================================================================================================================================================
 *    주어진 Render Mesh를 그립니다....
 *=============*/
void hyunwoo::Renderer::DrawRenderMesh(const RenderMesh& renderMesh, const ViewPort& vp)
{
    /*********************************************************************************************************
     *   RenderMesh가 Transform에 부착되지 않았거나, 참조하고 있는 Mesh/Camera가 없는 상태라면 함수를 종료한다..
     *******/
    Mesh*      mesh      = renderMesh.GetMesh().Get();
    Camera*    cam       = vp.RenderCamera.Get();
    Transform* transform = renderMesh.GetAttachedTransform().Get();

    static Texture2D              invalid_tex;
    static std::vector<Matrix4x4> skinning_mats;
    static std::vector<Matrix4x4> blending_mats;

    if (transform==nullptr || mesh==nullptr || cam==nullptr) {
        return;
    }


    /**********************************************************************************************************
     *   ViewPort의 RenderTarget이 초기화되지 않았다면, 함수를 종료한다..
     *******/
    if (vp.RenderTarget.IsInit()==false) {
        return;
    }


    /***********************************************************************************************************
     *   메시의 회전과 위치를 구성하는 행렬을 만든다...
     ********/
    const Matrix4x4 M   = transform->GetTRS();
    const Matrix4x4 V   = cam->GetViewMatrix();
    const Matrix4x4 P   = cam->GetPerspectiveMatrix(vp.RenderTarget.GetAspectRatio());
    const Matrix4x4 PV  = (P*V);
    const Matrix4x4 PVM = (P*V*M);



    /*************************************************************************************************************
     *   메시의 바운딩 박스가 절두체를 벗어났는지를 판별하고, 맞다면 삼각형 그리기를 넘어간다...
     *******/
    Frustum frustum = Frustum(PVM);

    if (frustum.IsOverlapped(mesh->BoundBox)==false) {
        return;
    }


    /**************************************************************************************************************
     *   스켈레탈 메시일 경우, 스키닝 행렬과 블랜딩 행렬을 계산한 후 랜더링을 진행한다....
     ********/
    if (mesh->Bones.size() > 0) 
    {
        /*-------------------------------------------------------------------------
         *   스키닝 행렬을 계산하고, 캐싱한다...
         ******/
        const uint32_t bone_size = mesh->Bones.size();
        skinning_mats.resize(bone_size);

        for (uint32_t i = 0; i < bone_size; i++) 
        {
            const Bone& bone    = mesh->Bones[i];
            Transform*  bone_tr = renderMesh.GetBoneTransformAt(i).Get();

            //유효하지 않은 본이라면, 단위 행렬으로 설정한다...
            if (bone_tr==nullptr) {
                skinning_mats[i] = Matrix4x4::Identity;
                continue;
            }

            //스키닝 행렬을 계산하고 캐싱한다...
            skinning_mats[i] = (bone_tr->GetTRS() * bone.BindingPose.GetTRS_Inverse());
        }


        /*------------------------------------------------------------------------
         *   모든 버텍스마다 적절한 블랜딩 행렬을 계산하고, 캐싱한다...
         *******/
        const uint32_t vertices_size = mesh->Vertices.size();
        blending_mats.resize(vertices_size);

        for (uint32_t i = 0; i < vertices_size; i++) 
        {
            const Vertex&                       vertex = mesh->Vertices[i];
            const Vertex::SkinDeformDescriptor& desc   = vertex.SkinDeformDesc;

            //블랜딩 행렬을 초기값으로 초기화한다...
            Matrix4x4& blending_mat = blending_mats[i];
            blending_mat = Matrix4x4(Vector4::Zero, Vector4::Zero, Vector4::Zero, Vector4::Zero);

            for (uint32_t j = 0; j < desc.Weight_Count; j++) {
                const SkinDeformWeight& weight = mesh->DeformWeights[desc.Weight_StartIdx + j];
                blending_mat += (skinning_mats[weight.BoneTransformIdx] * weight.Weight);
            }

            blending_mat = (PV * blending_mat);
        }


        /*-----------------------------------------------------------------------
         *   서브 메시별로 삼각형들을 그린다....
         ********/
        const uint32_t   subMesh_size = mesh->SubMeshs.size();
        ClipTriangleList clip_triangle_list;


        for (uint32_t subMeshIdx = 0, triangleIdx = 0; subMeshIdx < subMesh_size; subMeshIdx++ ) 
        {
            const SubMesh& subMesh  = mesh->SubMeshs[subMeshIdx];
            const uint32_t goal_idx = (triangleIdx + subMesh.Triangle_Count);

            Texture2D*                  tex = &invalid_tex;
            Shader::VertexShaderFunc*   vs  = Shader::VertexShader_MulFinalMat;
            Shader::FragmentShaderFunc* fs  = Shader::FragmentShader_InvalidTex;


            /*---------------------------------------------------
              *  해당 서브메시에 배정된 머터리얼이 유효한 경우,
              *  쉐이더 및 텍스쳐 주소값을 갱신한다....
              *******/
            if (Material* material = renderMesh.GetMaterialList()[subMeshIdx].Get(); material!=nullptr) {

                //해당 머터리얼의 버텍스 쉐이더가 유효할 경우 갱신한다...
                if (material->Shaders.VertexShader!=nullptr) {
                    vs = material->Shaders.VertexShader;
                }

                //해당 머터리얼의 프래그먼트 쉐이더가 유효할 경우 갱신한다...
                if (material->Shaders.FragmentShader != nullptr) {
                    fs = material->Shaders.FragmentShader;
                }
                
                //해당 머터리얼의 텍스쳐가 유효할 경우 갱신한다...
                if (Texture2D* mat_tex = material->MappedTexture.Get(); mat_tex!=nullptr) {
                    tex = mat_tex;
                }
            }


            /*---------------------------------------------------
             *   해당 서브메시에 소속된 모든 삼각형들을 랜더링한다..
             *******/
            while (triangleIdx < goal_idx) {
                const IndexedTriangle& triangle = mesh->Triangles[triangleIdx++];

                const Vertex& vertex1 = mesh->Vertices[triangle.Indices[0]];
                const Vertex& vertex2 = mesh->Vertices[triangle.Indices[1]];
                const Vertex& vertex3 = mesh->Vertices[triangle.Indices[2]];

                const Matrix4x4& blending_mat1 = blending_mats[triangle.Indices[0]];
                const Matrix4x4& blending_mat2 = blending_mats[triangle.Indices[1]];
                const Matrix4x4& blending_mat3 = blending_mats[triangle.Indices[2]];

                const Vector4 clipPos1 = vs(vertex1, blending_mat1);
                const Vector4 clipPos2 = vs(vertex2, blending_mat2);
                const Vector4 clipPos3 = vs(vertex3, blending_mat3);

                const Vector3 ndcPos1 = ClipToNDC(clipPos1);
                const Vector3 ndcPos2 = ClipToNDC(clipPos2);
                const Vector3 ndcPos3 = ClipToNDC(clipPos3);


                /*------------------------------------
                 *   삼각형이 카메라와 같은 방향을 보고 
                 *   있다면, 삼각형 그리기를 넘어간다...
                 ********/
                const Vector3 u      = (ndcPos1 - ndcPos3);
                const Vector3 v      = (ndcPos2 - ndcPos3);
                const Vector3 normal = Vector3::Cross(u, v);

                if (UseBackfaceCulling && Vector3::Dot(Vector3::Forward, normal)>=0.f) {
                    continue;
                }


                /*-------------------------------------
                 *   삼각형 클립핑을 진행한다...
                 ********/
                clip_triangle_list.triangleCount = 1;
                clip_triangle_list.Triangles[0]  = ClipTriangle(
                    ClipVertex( clipPos1, vertex1.UvPos ),
                    ClipVertex( clipPos2, vertex2.UvPos ),
                    ClipVertex( clipPos3, vertex3.UvPos )
                );

                //+Z, -Z 평면에 대한 클립핑을 적용한다..
                ClippingTriangle(clip_triangle_list, ClippingTest_Far, SolveT_Far);
                ClippingTriangle(clip_triangle_list, ClippingTest_Near, SolveT_Near);

                //+X, -X 평면에 대한 클립핑을 적용한다...
                ClippingTriangle(clip_triangle_list, ClippingTest_Right, SolveT_Right);
                ClippingTriangle(clip_triangle_list, ClippingTest_Left, SolveT_Left);

                //+Y, -Y 평면에 대한 클립핑을 적용한다...
                ClippingTriangle(clip_triangle_list, ClippingTest_Up, SolveT_Up);
                ClippingTriangle(clip_triangle_list, ClippingTest_Down, SolveT_Down);


                /*----------------------------------
                 *  분할된 삼각형들을 랜더링한다...
                 *******/
                for (uint32_t i = 0; i < clip_triangle_list.triangleCount; i++) {
                    const ClipTriangle& triangle = clip_triangle_list.Triangles[i];
                    DrawClipTriangle_internal(triangle, normal, fs, *tex, vp);
                }
            }
        }

        return;
    }



    /**************************************************************************************************************
     *   스타틱 메시일 경우, PVM 행렬로 서브 메시별 삼각형들을 그린다....
     ********/
    const uint32_t   subMesh_size = mesh->SubMeshs.size();
    ClipTriangleList clip_triangle_list;


    for (uint32_t subMeshIdx = 0, triangleIdx = 0; subMeshIdx < subMesh_size; subMeshIdx++)
    {
        const SubMesh& subMesh  = mesh->SubMeshs[subMeshIdx];
        const uint32_t goal_idx = (triangleIdx + subMesh.Triangle_Count);

        Texture2D*                  tex = &invalid_tex;
        Shader::VertexShaderFunc*   vs  = Shader::VertexShader_MulFinalMat;
        Shader::FragmentShaderFunc* fs  = Shader::FragmentShader_InvalidTex;


        /*---------------------------------------------------
          *  해당 서브메시에 배정된 머터리얼이 유효한 경우,
          *  쉐이더를 갱신한다....
          *******/
        if (Material* material = renderMesh.GetMaterialList()[subMeshIdx].Get(); material != nullptr) {

            //해당 머터리얼의 버텍스 쉐이더가 유효할 경우 갱신한다...
            if (material->Shaders.VertexShader != nullptr) {
                vs = material->Shaders.VertexShader;
            }

            //해당 머터리얼의 프래그먼트 쉐이더가 유효할 경우 갱신한다...
            if (material->Shaders.FragmentShader != nullptr) {
                fs = material->Shaders.FragmentShader;
            }

            //해당 머터리얼의 텍스쳐가 유효할 경우 갱신한다...
            if (Texture2D* mat_tex = material->MappedTexture.Get(); mat_tex != nullptr) {
                tex = mat_tex;
            }
        }


        /*---------------------------------------------------
         *   해당 서브메시에 소속된 모든 삼각형들을 랜더링한다..
         *******/
        while (triangleIdx < goal_idx) {
            const IndexedTriangle& triangle = mesh->Triangles[triangleIdx++];

            const Vertex& vertex1 = mesh->Vertices[triangle.Indices[0]];
            const Vertex& vertex2 = mesh->Vertices[triangle.Indices[1]];
            const Vertex& vertex3 = mesh->Vertices[triangle.Indices[2]];

            const Vector4 clipPos1 = vs(vertex1, PVM);
            const Vector4 clipPos2 = vs(vertex2, PVM);
            const Vector4 clipPos3 = vs(vertex3, PVM);

            const Vector3 ndcPos1 = ClipToNDC(clipPos1);
            const Vector3 ndcPos2 = ClipToNDC(clipPos2);
            const Vector3 ndcPos3 = ClipToNDC(clipPos3);


            /*------------------------------------
             *   삼각형이 카메라와 같은 방향을 보고
             *   있다면, 삼각형 그리기를 넘어간다...
             ********/
            const Vector3 u      = (ndcPos1 - ndcPos3);
            const Vector3 v      = (ndcPos2 - ndcPos3);
            const Vector3 normal = Vector3::Cross(u, v);

            if (UseBackfaceCulling && Vector3::Dot(Vector3::Forward, normal) >= 0.f) {
                continue;
            }


            /*-------------------------------------
             *   삼각형 클립핑을 진행한다...
             ********/
            clip_triangle_list.triangleCount = 1;
            clip_triangle_list.Triangles[0] = ClipTriangle(
                ClipVertex(clipPos1, vertex1.UvPos),
                ClipVertex(clipPos2, vertex2.UvPos),
                ClipVertex(clipPos3, vertex3.UvPos)
            );

            //+Z, -Z 평면에 대한 클립핑을 적용한다..
            ClippingTriangle(clip_triangle_list, ClippingTest_Far, SolveT_Far);
            ClippingTriangle(clip_triangle_list, ClippingTest_Near, SolveT_Near);

            //+X, -X 평면에 대한 클립핑을 적용한다...
            ClippingTriangle(clip_triangle_list, ClippingTest_Right, SolveT_Right);
            ClippingTriangle(clip_triangle_list, ClippingTest_Left, SolveT_Left);

            //+Y, -Y 평면에 대한 클립핑을 적용한다...
            ClippingTriangle(clip_triangle_list, ClippingTest_Up, SolveT_Up);
            ClippingTriangle(clip_triangle_list, ClippingTest_Down, SolveT_Down);


            /*----------------------------------
             *  분할된 삼각형들을 랜더링한다...
             *******/
            for (uint32_t i = 0; i < clip_triangle_list.triangleCount; i++) {
                const ClipTriangle& triangle = clip_triangle_list.Triangles[i];
                DrawClipTriangle_internal(triangle, normal, fs, *tex, vp);
            }
        }
    }

}
