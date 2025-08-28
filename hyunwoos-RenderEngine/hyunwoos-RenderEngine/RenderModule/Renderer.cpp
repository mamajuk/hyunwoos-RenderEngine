#include "Renderer.h"
#include <cstdint>
#include <emmintrin.h>
#include "../UtilityModule/StringLamda.h"

/*=====================================================================
 *    좌표계 변환 메소드...
 *============*/
hyunwoo::Vector2 hyunwoo::Renderer::WorldToScreen(const hyunwoo::Vector2& cartesianPos)
{
    return Vector2(
        (cartesianPos.x  + m_widthf_half), 
        (-cartesianPos.y + m_heightf_half)
    );
}

hyunwoo::Vector2 hyunwoo::Renderer::ScreenToWorld(const Vector2& screenPos)
{
    return Vector2(
        (screenPos.x - m_widthf_half),
        (screenPos.y - m_heightf_half)
    );
}









/*=====================================================================
 *   랜더러를 초기화하는 메소드.
 *===========*/
hyunwoo::Renderer::InitResult hyunwoo::Renderer::Init(HWND renderTargetHwnd, UINT initWidth, UINT initHeight)
{
    InitResult ret = { 0, };


    /******************************************
     *   이미 초기화가 되어있는가? 
         맞다면 결과를 갱신하고 함수를 종료한다..
     ******/
    if (m_isInit) {
        ret.IsAlreadyInit = true;
        return ret;
    }


    /***********************************************************
     *   윈도우 창 영역에 그래픽을 출력할 수 있는 화면 DC를 얻는다.
     *   그리고 해당 DC와 호환되는 메모리 전용 DC를 생성한다.
     *   이렇게 생성된 메모리 DC는 백버퍼 용으로 사용된다.
     *******/
    HDC hdc = GetDC(renderTargetHwnd);
    
    //메모리 DC를 생성하는데 실패했는가? 맞다면 결과 갱신 후 함수를 종료한다...
    if ((m_memDC = CreateCompatibleDC(hdc))==NULL) {
        ret.CreateMemDCIsFailed = true;
        return ret;
    }


    /*******************************************************
     *   메모리 DC에 대응되는 비트맵 GDI 오브젝트를 생성한다.
     *   생성한 비트맵의 픽셀값들을 수정해, 가공한 뒤 해당 비트맵을
     *   사용하는 메모리 DC를 화면 DC에 출력하는 용도로 쓰인다.
     *******/
    BITMAPINFO info              = { 0, };
    info.bmiHeader.biWidth       = initWidth;
    info.bmiHeader.biHeight      = initHeight;
    info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biBitCount    = 32;
    info.bmiHeader.biPlanes      = 1;
    info.bmiHeader.biCompression = BI_RGB;

    //비트맵 생성에 실패했는가? 맞다면 결과 갱신 후 함수를 종료한다...
    if ((m_backBufferBitmap = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, (void**)&m_backBufferBitmapPtr, NULL, 0))==NULL) {
        ret.CreateBitmapIsFailed = true;
        return ret;
    }


    /*****************************************************
     *    랜더링에 필요한 필드들을 초기화한다....
     ********/
    m_width               = initWidth;
    m_height              = initHeight;
    m_totalPixelNum       = (initWidth * initHeight);
    m_widthf              = float(m_width);
    m_heightf             = float(m_height);
    m_widthf_half         = (m_widthf * .5f);
    m_heightf_half        = (m_heightf * .5f);
    m_renderTargetHWND    = renderTargetHwnd;
    m_isInit              = true;
    ret.InitSuccess       = true;


    /***************************************************************
     *   생성한 비트맵 GDI 오브젝트를 메모리 DC가 사용하도록 설정한다.
     *   추후 메모리 DC를 파괴할 때, 기존 GDI오브젝트로 교체할 수 있도록
     *   기존 비트맵을 기록하고, 사용이 끝난 화면 DC를 반환한다...
     *******/
    m_oldBitmap = (HBITMAP)SelectObject(m_memDC, m_backBufferBitmap);
    ReleaseDC(renderTargetHwnd, hdc);

    return ret;
}








/*============================================================================
 *   가공한 비트맵을 화면 DC에 최종 제출합니다....
 *=============*/
void hyunwoo::Renderer::Present()
{
    if (m_isInit == false) return;


    /************************************************************
     *  GetDC() 대신 BeginPaint()를 사용하여 화면 DC를 얻어오는데,
     *  이는 WM_PAINT 메세지에서 무효화 영역을 처리할 때, 해당 영역만
     *  처리한다는 클립핑 정보가 담겨 효율적인 처리가 가능하기 때문이다.
     ******/
    HDC         hdc;
    RECT        rect;
    PAINTSTRUCT ps;
    GetClientRect(m_renderTargetHWND, &rect);
    hdc = BeginPaint(m_renderTargetHWND, &ps);

    /************************************************************
     *   화면DC를 가져온 후, 메모리 DC의 내용을 고속복사해 출력한다...
     ******/
    StretchBlt(hdc, 0, 0, rect.right, rect.bottom, m_memDC, 0, 0, m_width, m_height, SRCCOPY);
    EndPaint(m_renderTargetHWND, &ps);
}









/*============================================================================
 *   화면을 특정 색깔로 초기화합니다...
 *==========*/
void hyunwoo::Renderer::ClearScreen()
{
    if (m_isInit == false) return;

    DWORD* endPtr       = m_backBufferBitmapPtr + m_totalPixelNum;
    DWORD* alignedBegin = reinterpret_cast<DWORD*>(reinterpret_cast<uintptr_t>(m_backBufferBitmapPtr) + 15 & -16);
    DWORD* alignedEnd   = reinterpret_cast<DWORD*>(reinterpret_cast<uintptr_t>(endPtr) & -16);

    const DWORD clearColor = ClearColor.ARGB;


    /******************************************************
     *   SIMD 코드는 주소값이 16으로 정렬되어 있어야 한다.
     *   따라서 비트맵의 시작 주소부터 16으로 정렬된 주소까지는
     *   SIMD 연산을 사용하지 않고 일반 for문으로 처리한다..
     ********/
    for (DWORD* i = endPtr; i < alignedBegin; i++) {
        *i = clearColor;
    }


    /***************************************************
     *   정렬된 주소에 진입했다면, 마지막 정렬 주소까지
     *    SIMD 연산으로 한 번에 16bytes씩 처리한다....
     ******/
    __m128i rgba = _mm_set1_epi32(clearColor);

    for (DWORD* i = alignedBegin; i < alignedEnd; i+=4 ) {
        _mm_store_si128((__m128i*)i, rgba);
    }


    /*****************************************************
     *  정렬된 주소 이후, 남은 원소들을 처리한다...
     *******/
    for (DWORD* i = alignedEnd; i < endPtr; i++) {
        *i = clearColor;
    }
}










/*=============================================================================
 *   백버퍼에서 주어진 두 점 사이의 선을 그립니다....
 *==================*/
void hyunwoo::Renderer::DrawLine(const Color& color, const Vector2& startScreenPos, const Vector2& endScreenPos, bool useClipping)
{
    float   w        = m_widthf;
    float   h        = m_heightf;
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
            SetPixel(color, cur);

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
        SetPixel(color, cur);

        if (d < 0) d += dTrue;
        else
        {
            d += dFalse;
            cur.x += wDir;
        }

        cur.y += hDir;
    }
}







/*=============================================================================
 *   백버퍼의 특정 위치의 픽셀을, 지정한 색깔로 바꿉니다.
 *================*/
void hyunwoo::Renderer::SetPixel(const Color& color, const Vector2Int& screenPos)
{
    /*********************************************************
     *   초기화가 안되어있거나, 인덱스를 벗어나면 함수를 종료한다...
     *****/
    const int idx = ((m_height - screenPos.y) * m_width + screenPos.x);
    if (m_isInit == false || idx<0 || idx>=m_totalPixelNum) {
        return;
    }

    m_backBufferBitmapPtr[idx] = color.ARGB;
}







/*===========================================================================
 *    백버퍼의 지정한 위치에 문자열을 출력합니다.... 
 *=============*/
void hyunwoo::Renderer::DrawTextField(const std::wstring& out, const hyunwoo::Vector2Int& screenPos)
{
    if (m_isInit == false) return;

    RECT rc = { screenPos.x, screenPos.y };

    /******************************************************************
     *   출력할 텍스트의 전체 크기를 계산하고, 백버퍼에 텍스트를 그린다...
     *   (계산할 때, memDC가 사용하는 기본 폰트 GDI 오브젝트를 사용한다.)
     ******/
    DrawTextW(m_memDC, out.c_str(), -1, &rc, (DT_LEFT | DT_TOP | DT_CALCRECT));
    DrawTextW(m_memDC, out.c_str(), out.size(), &rc, (DT_LEFT | DT_TOP));
}










/*==============================================================================
 *    지정된 색상으로 삼각형을 그립니다....
 *=============*/
void hyunwoo::Renderer::DrawTriangle(const Color& color, const Vector2& screenPos1, const Vector2& screenPos2, const Vector2& screenPos3)
{
    /*****************************************************
     *    와이어 프레임 모드일 경우, 선만 그리고 종료한다...
     *******/
    if (UseWireFrameMode) {
        DrawLine(WireFrameColor, screenPos1, screenPos2);
        DrawLine(WireFrameColor, screenPos1, screenPos3);
        DrawLine(WireFrameColor, screenPos2, screenPos3);
        return;
    }


    /*******************************************************
     *    컨벡스 결합을 위해 필요한 값들을 계산한다...
     *******/
    const Vector2 u = (screenPos1 - screenPos3);
    const Vector2 v = (screenPos2 - screenPos3);

    const float uv   = Vector2::Dot(u, v);
    const float uu   = Vector2::Dot(u, u);
    const float vv   = Vector2::Dot(v, v);
    const float deno = (uv * uv - uu * vv);


    
    /**********************************************************
     *     퇴화 삼각형인가? ( 분모가 0 ) 맞다면 함수를 종료한다...
     **********/
    if (deno==0.0f) {
        return;
    }



    /*********************************************************
     *   세 점의 크기를 구한 후, 해당 범위에 있는 점들을 순회한다..
     ********/
    const int   xMin = Math::Min(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   xMax = Math::Max(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   yMin = Math::Min(screenPos1.y, screenPos2.y, screenPos3.y);
    const int   yMax = Math::Max(screenPos1.y, screenPos2.y, screenPos3.y);
    const float div  = (1.f / deno);

    for (int y = yMin; y <= yMax; y++){
        for (int x = xMin; x <= xMax; x++) {

            const Vector2 p   = Vector2(x, y);
            const Vector2 w   = (p - screenPos3);
            const float   wu  = Vector2::Dot(w, u);
            const float   wv  = Vector2::Dot(w, v);
            const float   s   = (wv*uv - wu*vv) * div;
            const float   t   = (wu*uv - wv*uu) * div;
            const float   r   = (1.f - s - t);

            /*-----------------------------------------
             *   삼각형 범위 안에 있다면 점을 찍는다..
             *-----*/
            if ((s >= 0.f && s <= 1.f) && (t >= 0.f && t <= 1.f) && (r >= 0.f && r <= 1.f)) {
                SetPixel(color, p);
            }
        }
    }
}










/*==============================================================================
 *    지정된 텍스쳐로 삼각형을 그립니다....
 *=============*/
void hyunwoo::Renderer::DrawTriangleWithTexture(const Texture2D& texture, const Vector2& screenPos1, const Vector2& uvPos1, const Vector2& screenPos2, const Vector2& uvPos2, const Vector2& screenPos3, const Vector2& uvPos3)
{
    /*****************************************************
     *    와이어 프레임 모드일 경우, 선만 그리고 종료한다...
     *******/
    if (UseWireFrameMode) {
        DrawLine(WireFrameColor, screenPos1, screenPos2);
        DrawLine(WireFrameColor, screenPos1, screenPos3);
        DrawLine(WireFrameColor, screenPos2, screenPos3);
        return;
    }


    /*******************************************************
     *    컨벡스 결합을 위해 필요한 값들을 계산한다...
     *******/
    const Vector2 u = (screenPos1 - screenPos3);
    const Vector2 v = (screenPos2 - screenPos3);

    const float uv = Vector2::Dot(u, v);
    const float uu = Vector2::Dot(u, u);
    const float vv = Vector2::Dot(v, v);
    const float deno = (uv * uv - uu * vv);



    /**********************************************************
     *     퇴화 삼각형인가? ( 분모가 0 ) 맞다면 함수를 종료한다...
     **********/
    if (deno == 0.0f) {
        return;
    }



    /*********************************************************
     *   세 점의 크기를 구한 후, 해당 범위에 있는 점들을 순회한다..
     ********/
    const int   xMin = Math::Min(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   xMax = Math::Max(screenPos1.x, screenPos2.x, screenPos3.x);
    const int   yMin = Math::Min(screenPos1.y, screenPos2.y, screenPos3.y);
    const int   yMax = Math::Max(screenPos1.y, screenPos2.y, screenPos3.y);
    const float div = (1.f / deno);

    for (int y = yMin; y <= yMax; y++) {
        for (int x = xMin; x <= xMax; x++) {

            const Vector2 p = Vector2(x, y);
            const Vector2 w = (p - screenPos3);
            const float   wu = Vector2::Dot(w, u);
            const float   wv = Vector2::Dot(w, v);
            const float   s = (wv * uv - wu * vv) * div;
            const float   t = (wu * uv - wv * uu) * div;
            const float   r = (1.f - s - t);

            /*-----------------------------------------
             *   삼각형 범위 안에 있다면 점을 찍는다..
             *-----*/
            if ((s >= 0.f && s <= 1.f) && (t >= 0.f && t <= 1.f) && (r >= 0.f && r <= 1.f)) {
                Vector2 uvPos = (uvPos1 * s) + (uvPos2 * t) + (uvPos3 * r);
                uvPos.x *= (texture.Width -1);
                uvPos.y *= (texture.Height-1);

                SetPixel(texture.GetPixel(uvPos), p);
            }
        }
    }
}



















/*====================================================================
 *   랜더러가 생성한 비트맵의 크기를 반환합니다...
 *=============*/
UINT hyunwoo::Renderer::GetWidth()  const {
    return m_width;
}

UINT hyunwoo::Renderer::GetHeight() const {
    return m_height;
}







/*=====================================================================
 *    랜더러가 초기화 되었는지의 여부를 확인합니다...
 *============*/
bool hyunwoo::Renderer::IsInit() const {
    return m_isInit;
}