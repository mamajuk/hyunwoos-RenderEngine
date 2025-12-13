#include "RenderTarget.h"

/*======================================================================================================================================
 *   해당 RenderTarger이 생성했던 리소스들을 정리한다....
 *********/
void hyunwoo::RenderTarget::Clear()
{
    /*************************************************
     *  초기화가 된 상태가 아니라면 함수를 종료한다...
     *******/
    if (m_isInit==false) {
        return;
    }


    /*************************************************
     *  사용하던 MemoryDC와 관련 리소스를 정리한다...
     *  memDC가 기존에 사용하던 비트맵 GDI Object로 교체하고,
     *  사용중이 아닌 bitmap Section을 삭제한다. 그 후,
     *  memDC도 제거한다....
     *******/
    SelectObject(m_memDC, m_oldBitmap);
    DeleteObject(m_backBufferBitmap);
    DeleteDC(m_memDC);
    delete[] m_depthBufferPtr;

    m_isInit           = false;
    m_memDC            = NULL;
    m_backBufferBitmap = NULL;
    m_depthBufferPtr   = nullptr;       
}











/*======================================================================================================================================
 *   해당 RenderTarger에 인자로 받은 RenderTarget의 내용을 이동시킨다...
 *********/
void hyunwoo::RenderTarget::operator=(RenderTarget&& prev) noexcept
{
    Clear();

    m_isInit = prev.m_isInit;

    m_totalPixelNum     = prev.m_totalPixelNum;
    m_backBufferWidth   = prev.m_backBufferWidth;
    m_backBufferHeight  = prev.m_backBufferHeight;
    m_backBufferWidthf  = prev.m_backBufferHeightf;
    m_backBufferHeightf = prev.m_backBufferHeightf;

    m_memDC               = prev.m_memDC;
    m_backBufferBitmap    = prev.m_backBufferBitmap;
    m_oldBitmap           = prev.m_oldBitmap;
    m_backBufferBitmapPtr = prev.m_backBufferBitmapPtr;
    m_depthBufferPtr      = prev.m_depthBufferPtr;
}












/*======================================================================================================================================
 *   주어진 인자의 정보대로, RenderTarget의 비트맵과 깊이 버퍼를 생성합니다....
 *********/
hyunwoo::RenderTarget::InitResult hyunwoo::RenderTarget::Init(HWND clientHwnd, const Vector2Int& backBufferSize)
{
    InitResult ret           = { 0, };
    const UINT new_totalSize = (backBufferSize.x * backBufferSize.y);


    /**************************************************************************
     *   인자로 주어진 백버퍼의 크기가 유효하지 않다면, 결과 갱신 후 함수를 종료한다..
     *******/
    if (new_totalSize <= 0) {
        ret.InvalidBackBufferSize = true;
        return ret;
    }



    /**************************************************************************
     *   이미 초기화된 상태라면, 기존 자원을 해제한다...
     ********/
    Clear();




    /********************************************************************
     *   윈도우 창 영역에 그래픽을 출력할 수 있는 화면 DC를 얻는다.
     *   그리고 해당 DC와 호환되는 메모리 전용 DC를 생성한다.
     *   이렇게 생성된 메모리 DC는 백버퍼 용으로 사용된다.
     *******/
    HDC hdc = GetDC(clientHwnd);

    //메모리 DC를 생성하는데 실패했는가? 맞다면 결과 갱신 후 함수를 종료한다...
    if ((m_memDC = CreateCompatibleDC(hdc)) == NULL) {
        ret.CreateMemDCIsFailed = true;
        return ret;
    }


    /**********************************************************************
     *   메모리 DC에 대응되는 비트맵 GDI 오브젝트를 생성한다.
     *   생성한 비트맵의 픽셀값들을 수정해, 가공한 뒤 해당 비트맵을
     *   사용하는 메모리 DC를 화면 DC에 출력하는 용도로 쓰인다.
     *******/
    BITMAPINFO info              = { 0, };
    info.bmiHeader.biWidth       = backBufferSize.x;
    info.bmiHeader.biHeight      = backBufferSize.y;
    info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biBitCount    = 32;
    info.bmiHeader.biPlanes      = 1;
    info.bmiHeader.biCompression = BI_RGB;

    //비트맵 생성에 실패했는가? 맞다면 결과 갱신 후 함수를 종료한다...
    if ((m_backBufferBitmap = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, (void**)&m_backBufferBitmapPtr, NULL, 0)) == NULL) {
        ret.CreateBackBufferIsFailed = true;
        return ret;
    }


    /*************************************************************************
     *  새로운 깊이 버퍼의 크기보다, 기존 버퍼가 작을 때에만 새로 할당한다...
     ********/
    if (m_totalPixelNum < new_totalSize) {

        //기존에 사용하던 버퍼가 있다면 삭제한다...
        if (m_depthBufferPtr!=nullptr) {
            delete[] m_depthBufferPtr;
        }

        m_depthBufferPtr = new float[new_totalSize];
    }


    /************************************************************************
     *    랜더링에 필요한 필드들을 초기화한다....
     ********/
    m_isInit            = true;
    m_backBufferWidth   = backBufferSize.x;
    m_backBufferHeight  = backBufferSize.y;
    m_backBufferWidthf  = float(backBufferSize.x);
    m_backBufferHeightf = float(backBufferSize.y);
    m_aspectRatio       = (m_backBufferWidthf / m_backBufferHeightf);
    m_totalPixelNum     = new_totalSize;
    ret.InitSuccess     = true;


    /***************************************************************
     *   생성한 비트맵 GDI 오브젝트를 메모리 DC가 사용하도록 설정한다.
     *   추후 메모리 DC를 파괴할 때, 기존 GDI오브젝트로 교체할 수 있도록
     *   기존 비트맵을 기록하고, 사용이 끝난 화면 DC를 반환한다...
     *******/
    m_oldBitmap = (HBITMAP)SelectObject(m_memDC, m_backBufferBitmap);
    ReleaseDC(clientHwnd, hdc);

    return ret;
}