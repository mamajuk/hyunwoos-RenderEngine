#include "VmdImporter.h"
#include <windows.h>


/*============================================================================================================================================
 *    주어진 경로에 있는 Vmd파일로부터, 애니메이션 정보들을 읽어들입니다....
 *********/
hyunwoo::VmdImporter::ImportResult hyunwoo::VmdImporter::Import(AnimationClip& outAnimClip, const Mesh& inPmxMesh, const wchar_t* path)
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
    if (strcmp(Signature_v1_30_later, file_signature)==0) {
        outRet.CreatedWithMMD_1_30_later = true;
    }

    /*------------------------------------------------
     *   MMD 1.30 버전에서 만들어진 Vmd파일인가?
     *******/
    else if (strcmp(Signature_v1_30, file_signature)==0) {
        outRet.CreatedWithMMD_1_30_later = false;
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
    std::wstring   outName;
    const uint32_t name_size = (outRet.CreatedWithMMD_1_30 ? 10 : 20);
    ReadString(in, name_size, outName);





    /*****************************************************************************************************
     *   빠른 bone의 바인딩포즈를 얻어오기위한 map을 만든다....
     *********/
    std::unordered_map<WStringKey, uint32_t> pose_map;

    for (uint32_t i = 0; i < inPmxMesh.Bones.size(); i++) 
    {
        const Bone& bone = inPmxMesh.Bones[i];
        pose_map.insert(std::pair<WStringKey, uint32_t>(bone.Name, i));
    }







    /*****************************************************************************************************
     *   본의 키프레임 데이터를 읽어들인다.....
     *********/
    uint32_t maxKeyFrame = 0;
    uint32_t bone_keyFrame_count;

    const float                              fpsDiv = (1.f / 30.f);
    std::wstring                             utf16_name;
    std::unordered_map<WStringKey, uint32_t> prop_startIdx_map;

    in.read((char*)&bone_keyFrame_count, 4);
    for (uint32_t i = 0; i < bone_keyFrame_count; i++) 
    {
        /*--------------------------------------------------------------
         *  해당 키프레임 데이터에 사용된 본의 이름을 읽어들인다....
         *******/
        ReadString(in, 15, utf16_name);
        

        /*--------------------------------------------------------------
         *  해당 키프레임 번호를 읽어들이고, 가장 마지막 키프레임인지를
         *  판별한다....
         *******/
        uint32_t frameNumber;
        in.read((char*)&frameNumber, 4);

        if (maxKeyFrame < frameNumber) {
            maxKeyFrame = frameNumber;
        }


        /*--------------------------------------------------------------
         *  해당 키프레임에서 사용된 본의 Local Position을 읽어들인다...
         *  해당 좌표는 Pmx Modeling의 본들의 바인딩 포즈(기본 포즈)에 상대적
         *  이기에, 최종 월드 위치는 바인딩 포즈에 vmd의 local_pos를 
         *  더한 값이다. 따라서 pmx 메시의 LocalPosition값을 미리 더해준다...
         *******/
        Vector3 local_pos;
        in.read((char*)&local_pos, sizeof(Vector3));



        /*--------------------------------------------------------------
         *  해당 키프레임에서 사용된 본의 Local Quat(XYZW)을 읽어들인다...
         *******/
        Vector4 local_quat;
        in.read((char*)&local_quat, sizeof(Vector4));





        /*-----------------------------------------------------------------
         *  보간 데이터를 읽어들인다. 여기서 disabled값은 단순히 최상위 16bytes
         *  를 왼쪽으로 1/2/3bytes씩 shift한 무의미한 값이다. 단, ControlPoints
         *  Z/R의 ax값이 정상적으로 남아있지 않고 덮어씌어졌을 가능성이 있다. 이는
         *  Physics Indicator(물리 사용 여부 플래그)를 비활성/활성화하는 플래그가
         *  기록되면서 덮어씌어지기 때문이다. 따라서 disbled값의 첫번째, 두번째
         *  인덱스에 기록되어 있다..
         *******/
        InterpolationData interpol;
        in.read((char*)&interpol, sizeof(InterpolationData));
        interpol.ControlPointZ[0] = interpol.disabled[1];
        interpol.ControlPointR[0] = interpol.disabled[2];


        /*----------------------------------------------------------------
         *  해당 이름을 가진 Property가 존재하지 않을 경우, 키를 삽입한다...
         ******/
        WStringKey  name_key = utf16_name.c_str();
        const Bone& bone     = inPmxMesh.Bones[pose_map[name_key]];

        if (prop_startIdx_map.contains(name_key) == false) {
            prop_startIdx_map.insert(std::pair<WStringKey, uint32_t>(
                name_key,
                outAnimClip.Properties.size()
            ));

            outAnimClip.Properties.push_back(AnimationClip::Property(AnimationClip::PropertyType::Local_Position, name_key));
            outAnimClip.Properties.push_back(AnimationClip::Property(AnimationClip::PropertyType::Local_Rotation_Quat, name_key));
        }



        /*---------------------------------------------------------------
         *   각 프로퍼티에 키프레임을 삽입한다....
         *********/
        const float              keyFrame_time  = (float(frameNumber) * fpsDiv);
        const uint32_t           prop_startIdx  = prop_startIdx_map[name_key];
        AnimationClip::Property& local_pos_prop = outAnimClip.Properties[prop_startIdx];
        AnimationClip::Property& local_rot_prop = outAnimClip.Properties[prop_startIdx + 1];


        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         *  각 키프레임에서 사용할 커브를 초기화한다...
         ******/
        const float uint8_tDiv = (1.f / 127.f);

        CurveVariant local_Xpos_curve;
        CurveVariant local_Ypos_curve;
        CurveVariant local_Zpos_curve;
        CurveVariant local_rot_curve;

        //local_xPosition에 대한 커브를 설정한다....
        local_Xpos_curve.Type = CurveVariant::VariantType::Cubic_Bezier;
        local_Xpos_curve.CubicBezier.Yvalues[0] = 0.f;
        local_Xpos_curve.CubicBezier.Yvalues[1] = float(interpol.ControlPointX[1]) * uint8_tDiv;
        local_Xpos_curve.CubicBezier.Yvalues[2] = float(interpol.ControlPointX[3]) * uint8_tDiv;
        local_Xpos_curve.CubicBezier.Yvalues[3] = 1.f;

        //local_yPosition에 대한 커브를 설정한다....
        local_Ypos_curve.Type = CurveVariant::VariantType::Cubic_Bezier;
        local_Ypos_curve.CubicBezier.Yvalues[0] = 0.f;
        local_Ypos_curve.CubicBezier.Yvalues[1] = float(interpol.ControlPointY[1]) * uint8_tDiv;
        local_Ypos_curve.CubicBezier.Yvalues[2] = float(interpol.ControlPointY[3]) * uint8_tDiv;
        local_Ypos_curve.CubicBezier.Yvalues[3] = 1.f;

        //local_zPosition에 대한 커브를 설정한다....
        local_Zpos_curve.Type = CurveVariant::VariantType::Cubic_Bezier;
        local_Zpos_curve.CubicBezier.Yvalues[0] = 0.f;
        local_Zpos_curve.CubicBezier.Yvalues[1] = float(interpol.ControlPointZ[1]) * uint8_tDiv;
        local_Zpos_curve.CubicBezier.Yvalues[2] = float(interpol.ControlPointZ[3]) * uint8_tDiv;
        local_Zpos_curve.CubicBezier.Yvalues[3] = 1.f;

        //local_rotation에 대한 커브를 설정한다....
        local_rot_curve.Type = CurveVariant::VariantType::Cubic_Bezier;
        local_rot_curve.CubicBezier.Yvalues[0] = 0.f;
        local_rot_curve.CubicBezier.Yvalues[1] = float(interpol.ControlPointR[1]) * uint8_tDiv;
        local_rot_curve.CubicBezier.Yvalues[2] = float(interpol.ControlPointR[3]) * uint8_tDiv;
        local_rot_curve.CubicBezier.Yvalues[3] = 1.f;
        


        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         *  Local Position 프로퍼티의 키프레임을 추가한다..
         ******/
        AnimationClip::KeyFrame local_pos_keyFrame;
        local_pos_keyFrame.Time          = keyFrame_time;
        local_pos_keyFrame.CurveStartIdx = local_pos_prop.Curves.size();
        local_pos_keyFrame.Vec3          = (local_pos + bone.BindingPose.LocalPosition);

        local_pos_prop.Curves.push_back(local_Xpos_curve);
        local_pos_prop.Curves.push_back(local_Ypos_curve);
        local_pos_prop.Curves.push_back(local_Zpos_curve);

        AddKeyFrame(local_pos_prop, local_pos_keyFrame);


        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         *  Local Rotation 프로퍼티의 키프레임을 추가한다..
         ******/
        AnimationClip::KeyFrame local_rot_keyFrame;
        local_rot_keyFrame.Time          = keyFrame_time;
        local_rot_keyFrame.CurveStartIdx = local_rot_prop.Curves.size();
        local_rot_keyFrame.Quat          = (Quaternion(local_quat.w, Vector3(local_quat.x, local_quat.y, local_quat.z)).GetNormalized() * bone.BindingPose.LocalRotation).GetNormalized();

        local_rot_prop.Curves.push_back(local_rot_curve);
        AddKeyFrame(local_rot_prop, local_rot_keyFrame);
    }



    /*-------------------------------------------------------------------------------------
     *  각 프로퍼티들의 마지막 키프레임의 시간값이 애니메이션의 최종 시간까지 존재하지 않는다면,
     *  최종 시간까지 이전 프레임부터 Constant Curve를 사용하는 키프레임을 삽입한다...
     ********/
    outAnimClip.TotalTime = (float(maxKeyFrame) * fpsDiv);

    for (uint32_t i = 0; i < outAnimClip.Properties.size(); i++) {

        AnimationClip::Property& prop          = outAnimClip.Properties[i];
        AnimationClip::KeyFrame& last_keyFrame = prop.KeyFrames.back();

        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         *  최종 시간까지 재생되는 키프레임이라면, 넘어간다..
         ******/
        if (last_keyFrame.Time >= outAnimClip.TotalTime) {
            continue;
        }

        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         *  아니라면, 마지막 키프레임의 커브를 Constant
         *  Curve로 설정한다. Vmd는 어차피 2 종류의
         *  PropertyType만을 사용하기에, 이를 통해 간단히
         *  커브 개수를 구할 수 있다...
         ******/
        const uint32_t curve_count = (prop.Type==AnimationClip::PropertyType::Local_Rotation_Quat? 1:3);
        ConstantCurve const_curve  = { 1.f };

        for (uint32_t j = 0; j < curve_count; j++) {
            CurveVariant& curve = prop.Curves[last_keyFrame.CurveStartIdx + j];
            curve.Constant = const_curve;
            curve.Type     = CurveVariant::VariantType::Constant;
        }


        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         *  최종 시간까지 재생되는 키프레임을 삽입한다...
         *  마지막이기 때문에, 커브 정보는 필요없다...
         *******/
        AnimationClip::KeyFrame new_keyFrame = last_keyFrame;
        new_keyFrame.Time          = outAnimClip.TotalTime;
        new_keyFrame.CurveStartIdx = 0;
        
        prop.KeyFrames.push_back(new_keyFrame);
    }




    /*****************************************************************************************************
     *   모프의 키프레임 데이터를 읽어들인다.....
     *********/
    uint32_t morph_keyFrame_count;
    in.read((char*)&morph_keyFrame_count, 4);

    for (uint32_t i=0; i<morph_keyFrame_count; i++) 
    {
        /*--------------------------------------------
         *   모프의 이름을 읽어들인다...
         *****/
        in.seekg(15, std::ios::cur);

        /*--------------------------------------------
         *  모프의 프레임 번호를 읽어들인다...
         ******/
        in.seekg(4, std::ios::cur);

        /*--------------------------------------------
         *  모프의 값 또는 무게를 읽어들인다...
         ******/
        in.seekg(4, std::ios::cur);
    }




    /*****************************************************************************************************
     *   카메라의 키프레임 데이터를 읽어들인다.....
     *********/
    uint32_t cam_keyFrame_count;
    in.read((char*)&cam_keyFrame_count, 4);

    for (uint32_t i=0; i<cam_keyFrame_count; i++)
    {
        /*--------------------------------------------
         *  카메라의 프레임 번호를 읽어들인다...
         ******/
        in.seekg(4, std::ios::cur);


        /*------------------------------------------------
         *  카메라가 바라보는 목표까지의 거리값을 읽어들인다..
         ******/
        in.seekg(4, std::ios::cur);


        /*--------------------------------------------
         *  카메라가 바라보는 목표의 위치값을 읽어들인다..
         ******/
        in.seekg(sizeof(Vector3), std::ios::cur);


        /*--------------------------------------------------
         *  카메라가 바라보는 목표의 xyz 라디안값을 읽어들인다..
         ******/
        in.seekg(sizeof(Vector3), std::ios::cur);


        /*--------------------------------------------
         *  카메라의 보간용 커브 데이터를 읽어들인다...
         ******/
        in.seekg(24, std::ios::cur);


        /*--------------------------------------------
         *  카메라의 FOV Angle값을 읽어들인다...
         ******/
        in.seekg(4, std::ios::cur);


        /*--------------------------------------------
         *  카메라의 Perspective 토글값을 읽어들인다..
         ******/
        in.seekg(1, std::ios::cur);
    }




    /*****************************************************************************************************
     *   광원의 키프레임 데이터를 읽어들인다.....
     *********/
    uint32_t light_keyFrame_count;
    in.read((char*)&light_keyFrame_count, 4);

    for (uint32_t i=0; i<light_keyFrame_count; i++)
    {
        /*--------------------------------------------
         *  광원의 프레임 번호를 읽어들인다...
         ******/
        in.seekg(4, std::ios::cur);


        /*--------------------------------------------
         *  광원의 rgb값을 읽어들인다....
         ******/
        in.seekg((4*3), std::ios::cur);


        /*--------------------------------------------
        *  광원의 위치값을 읽어들인다...
        ******/
        in.seekg(sizeof(Vector3), std::ios::cur);
    }




    /*****************************************************************************************************
     *   그림자의 키프레임 데이터를 읽어들인다.....
     *********/
    uint32_t shadow_keyFrame_count;
    in.read((char*)&shadow_keyFrame_count, 4);

    for (uint32_t i=0; i<shadow_keyFrame_count; i++)
    {
        /*--------------------------------------------
         *  그림자의 프레임 번호를 읽어들인다...
         ******/
        in.seekg(4, std::ios::cur);


        /*--------------------------------------------
         *  그림자의 모드를 읽어들인다...
         *  ( 0=off, 1=mode1, 2=mode2 )
         ******/
        in.seekg(1, std::ios::cur);


        /*--------------------------------------------
         *  그림자의 범위값을 읽어들인다...
         *  stored as 0.0 to 0.1 and also 
         *  range-inverted: [0,9999] -> [0.1, 0.0]
         ******/
        in.seekg(4, std::ios::cur);
    }




    /*****************************************************************************************************
     *   IK의 키프레임 데이터를 읽어들인다...
     *********/
    outRet.Success = true;
    return outRet;

    uint32_t                           ik_keyFrame_count;
    std::vector<IK_KeyFrame>           ik_keyFrames;
    std::vector<IK_KeyFrame::BoneDesc> ik_boneDescs;

    in.read((char*)&ik_keyFrame_count, 4);
    for (uint32_t i=0; i<ik_keyFrame_count; i++)
    {
        IK_KeyFrame new_ik_keyFrame;

        /*-------------------------------------------
         *  ik의 키프레임 번호를 읽어들인다....
         ******/
        in.read((char*)&new_ik_keyFrame.FrameCount, 4);


        /*-------------------------------------------
         *  ik의 표시 여부값을 읽어들인다...
         ******/
        in.read((char*)&new_ik_keyFrame.DisplayMode, 1);


        /*-------------------------------------------
         *  ik에서 사용될 본들의 정보들을 읽어들인다...
         *******/
        in.read((char*)&new_ik_keyFrame.IKBoneCount, 4);

        new_ik_keyFrame.IKBoneStartIdx = ik_boneDescs.size();
        ik_keyFrames.push_back(new_ik_keyFrame);


        /*---------------------------------------------
         *  해당 IK 키프레임에서 사용되는 본들의 이름과, ik 
         *  사용 여부를 읽어들인다...
         ******/
        IK_KeyFrame::BoneDesc bone_desc;
        for (uint32_t j = 0; j < new_ik_keyFrame.IKBoneCount; j++) {
            ReadString(in, 20, utf16_name);

            bone_desc.Name = utf16_name.c_str();
            in.read((char*)&bone_desc.Mode, 1);

            ik_boneDescs.push_back(bone_desc);
        }
    }


    outRet.Success = true;
    return outRet;
}
















/*============================================================================================================================================
 *    주어진 키프레임을, 시간 순으로 주어진 프로퍼티에 삽입합니다.
 *********/
void hyunwoo::VmdImporter::AddKeyFrame(AnimationClip::Property& prop, const AnimationClip::KeyFrame& keyFrame)
{
    /****************************************************
     *  주어진 키프레임을 시간값에 따라 순차적으로 정렬한다..
     *******/
    for (uint32_t i = 0; i < prop.KeyFrames.size(); i++) {
        if (prop.KeyFrames[i].Time < keyFrame.Time) {
            continue;
        }

        prop.KeyFrames.push_back(keyFrame);
        for (int32_t j = (prop.KeyFrames.size() - 1); j > i; j--) {
            std::swap(prop.KeyFrames[j], prop.KeyFrames[j - 1]);
        }

        return;
    }


    /**************************************************
     *  해당 프로퍼티에 키프레임이 없을 경우....
     ******/
    prop.KeyFrames.push_back(keyFrame);
}
















/*============================================================================================================================================
 *    파일 스트림으로부터 주어진 크기의 ShiftJIS 인코딩 문자열을 읽어들이고, UTF16으로 변환하여 저장합니다...
 *********/
void hyunwoo::VmdImporter::ReadString(std::ifstream& in, uint32_t shiftJIS_StrSize, std::wstring& outUTF16Str)
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
        outUTF16Str = L"";
        return;
    }


    /*************************************************************
     *   결과를 담을 버퍼를 할당하고, 변환을 시도한다....
     *******/
    outUTF16Str.resize(utf16_len);

    //변환이 실패했다면, 기본값을 담고 함수를 종료한다...
    if (MultiByteToWideChar(932, 0, shiftJIS_str, -1, &outUTF16Str[0], utf16_len)==0) {
        outUTF16Str = L"";
        return;
    }
}
