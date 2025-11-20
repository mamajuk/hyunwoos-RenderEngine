#include "VmdImporter.h"
#include <windows.h>


/*============================================================================================================================================
 *    주어진 경로에 있는 Vmd파일로부터, 애니메이션 정보들을 읽어들입니다....
 *********/
hyunwoo::VmdImporter::ImportResult hyunwoo::VmdImporter::Import(AnimationClip& outAnimClip, const wchar_t* path)
{
    VmdImporter::ImportResult outRet = { 0, };


    /***************************************************************************************************
     *  지정한 주소에 해당 파일이 존재하는가? 없다면 결과를 갱신하고, 함수를 종료한다...
     *******/
    std::ifstream in(path, std::ios::binary);
    if (in.is_open() == false) {
        outRet.Failed_OpenFile = true;
        return outRet;
    }



    /*****************************************************************************************************
     *   해당 파일의 시그니처를 비교하고, Vmd 확장자가 아니라면 결과를 갱신하고 함수를 종료한다...
     *********/
    char file_signature[30] = { 0, };
    in.read(file_signature, 30);

    /*------------------------------------------------
     *   MMD 1.30 이후 버전에서 만들어진 Vmd파일인가?
     *******/
    if (strcmp(Signature_v1_30_later, file_signature)!=0) {
        outRet.CreatedWithMMD_1_30_later = true;
    }

    /*------------------------------------------------
     *   MMD 1.30 버전에서 만들어진 Vmd파일인가?
     *******/
    else if (strcmp(Signature_v1_30, file_signature) != 0) {
        outRet.CreatedWithMMD_1_30_later = true;
    }

    /*------------------------------------------------
     *   Vmd파일이 아니라면 함수를 종료한다....
     *******/
    else {
        outRet.IsNotVmdFile = true;
        return outRet;
    }





    /*****************************************************************************************************
     *   해당 Vmd파일에서 사용된 모델 이름을 읽어들인다....
     *********/
    const uint32_t name_size = (outRet.CreatedWithMMD_1_30 ? 10 : 20);
    in.seekg(name_size, std::ios::cur);





    /*****************************************************************************************************
     *   키프레임 데이터를 읽어들인다.....
     *********/
    uint32_t keyFrame_count;
    in.read((char*)&keyFrame_count, 4);


    /*--------------------------------------------
     *  본들의 키프레임 데이터들을 읽어들인다...
     ********/
    wchar_t      bone_name[100];
    BoneKeyFrame keyFrameDesc;

    for (uint32_t i = 0; i < keyFrame_count; i++) {
        ReadString(in, 15, bone_name);
        in.read((char*)&keyFrameDesc, sizeof(BoneKeyFrame));
    }


    outRet.Success = true;
    return outRet;
}

















/*============================================================================================================================================
 *    파일 스트림으로부터 주어진 크기의 ShiftJIS 인코딩 문자열을 읽어들이고, UTF16으로 변환하여 저장합니다...
 *********/
void hyunwoo::VmdImporter::ReadString(std::ifstream& in, uint32_t shiftJIS_StrSize, wchar_t* outUTF16Str)
{
    /***********************************************************
     *   ShiftJIS 문자열을 읽어들인다...
     *******/
    char shiftJIS_str[40] = { 0, };
    in.read(shiftJIS_str, shiftJIS_StrSize);


    /************************************************************
     *   UTF-16으로 변환되었을 때의 문자열 크기를 계산한다...
     *   932 = shift JIS 코드 페이지를 의미한다...
     ********/
    const uint32_t utf16_len = MultiByteToWideChar(932, 0, shiftJIS_str, -1, NULL, 0);

    //계산이 실패했다면, 기본값을 담고 함수를 종료한다...
    if (utf16_len==0) {
        outUTF16Str[0] = 0;
        return;
    }


    /*************************************************************
     *   결과를 담을 버퍼를 할당하고, 변환을 시도한다....
     *******/

    //변환이 실패했다면, 기본값을 담고 함수를 종료한다...
    if (MultiByteToWideChar(932, 0, shiftJIS_str, -1, &outUTF16Str[0], utf16_len)==0) {
        outUTF16Str[0] = 0;
        return;
    }
}
