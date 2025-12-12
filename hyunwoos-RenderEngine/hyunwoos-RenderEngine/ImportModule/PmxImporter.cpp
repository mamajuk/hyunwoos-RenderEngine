#include "PmxImporter.h"
#include <filesystem>
#include <vector>
#include "../UtilityModule/StringLamda.h"
#undef max

/*=======================================================================================================================
 *  지정한 주소의 Png파일로부터, 텍스쳐 데이터를 읽어들입니다....
 *=============*/
hyunwoo::PmxImporter::ImportResult hyunwoo::PmxImporter::Import(const StorageDescription& storageDesc, const wchar_t* path)
{
    PmxImporter::ImportResult outRet = { 0, };


    /**************************************************************************************************
     *   로드할 데이터가 존재하지 않다면, 임포트를 진행할 이유가 없으니, 결과를 갱신하고
     *   함수를 종료한다...
     *******/
    if (storageDesc.OutMesh==nullptr && storageDesc.OutTextures==nullptr && storageDesc.OutMaterials==nullptr) {
        outRet.LoadDataIsNothing = true;
        return outRet;
    }




    /***************************************************************************************************
     *  지정한 주소에 해당 파일이 존재하는가? 없다면
     *  결과를 갱신하고, 함수를 종료한다...
     *******/
    std::ifstream in(path, std::ios::binary);
    if (in.is_open()==false) {
        outRet.Failed_OpenFile = true;
        return outRet;
    }





    /***************************************************************************************************
     *   Pmx 헤더를 읽어들인다....
     ********/
    PmxImporter::Header header = { 0, };

    /*---------------------------------------
     *  Pmx시그니처가 아니라면, 결과를 갱신하고
     *  함수를 종료한다...
     *-------*/
    in.read((char*)&header.Signature, 4);

    if (header.Signature!=PmxImporter::Signature) {
        outRet.IsNotPmxFile = true;
        return outRet;
    }


    /*----------------------------------------
     *  Pmx버전값을 읽어들인다...
     *---------*/
    in.read((char*)&header.Version, 4);



    /*----------------------------------------
     *   전역 속성의 개수를 나타내는 필드를 읽어
     *   들이고, 각 속성들을 모두 읽어들인다...
     *--------*/
    in.read((char*)&header.Globals_Count, 1); //PMX 2.0에서는 8로 고정...
    in.read((char*)&header.Globals, header.Globals_Count);



    /*----------------------------------------
     *  모델 설명을 읽어들인다....
     *-------*/
    uint32_t textSize = 0;

    //모델의 로컬 이름...
    ReadText(in, header, nullptr);

    //모델의 유니버설 이름...
    ReadText(in, header, nullptr);

    //모델의 로컬 설명...
    ReadText(in, header, nullptr);

    //모델의 유니버설 이름...
    ReadText(in, header, nullptr);





    /******************************************************************************************************
     *   메시의 버텍스 정보와, 삼각형 정보들을 읽어들인다...
     *********/

    //메시 데이터를 저장해야하는가?
    if (storageDesc.OutMesh!=nullptr) {
        Import_StoreVertexData(in, header, storageDesc);
        Import_StoreTriangleData(in, header, storageDesc);
    }

    //메시 데이터를 무시하는 경우...
    else {
        Import_IgnoreVertexData(in, header);
        Import_IgnoreTriangleData(in, header);
    }







    /*****************************************************************************************
     *   해당 메시에 사용된 텍스쳐들을 처리한다....
     **********/
    
    //텍스쳐 데이터를 저장해야하는가?
    if (storageDesc.OutTextures!=nullptr) 
    {
        Import_StoreTextureData(in, header, path, outRet, storageDesc);
        if (outRet.Failed_TextureStorage==true) {
            return outRet;
        }
    }

    //텍스쳐 데이터를 무시하는가?
    else {
        Import_IgnoreTextureData(in, header);
    }





    /**************************************************************************************
     *   해당 메시에 사용된 머터리얼 정보들을 읽어들인다....
     **********/

    //머터리얼 데이터를 저장해야하는가?
    if (storageDesc.OutMaterials!=nullptr) 
    {
        Import_StoreMaterialData(in, header, outRet, storageDesc);
        if (outRet.Failed_MaterialStorage==true) {
            return outRet;
        }
    }

    //머터리얼 데이터를 무시하는가?
    else {
        Import_IgnoreMaterialData(in, header);
    }





    /*****************************************************************************************************
     *   메시의 본 정보들을 처리한다...
     *********/
    
    //본 데이터를 저장해야하는가?
    if (storageDesc.OutMesh!=nullptr) {
        Import_StoreBoneData(in, header, storageDesc);
    }

    outRet.Success = true;
    return outRet;
}














int32_t hyunwoo::PmxImporter::ReadDefaultIndexType(std::ifstream& in, const DefaultIndexType defaultIdxType)
{
    if (defaultIdxType == DefaultIndexType::Type1_byte) {
        int8_t value;
        in.read((char*)&value, 1);
        return (int32_t)value;
    }

    if (defaultIdxType == DefaultIndexType::Type2_short) {
        int16_t value;
        in.read((char*)&value, 2);
        return (int32_t)value;
    }

    if (defaultIdxType == DefaultIndexType::Type4_int) {
        int32_t value;
        in.read((char*)&value, 4);
        return (int32_t)value;
    }

    return 0;
}

















/*=======================================================================================================================
 *   파일 스트림으로부터, Text 타입을 읽어들입니다. 문자열을 저장할 포인터를 지정하지 않으면 무시합니다....
 **************/
void hyunwoo::PmxImporter::ReadText(std::ifstream& in, const Header& header, std::wstring* out_u16_text)
{
    int32_t text_size;
    in.read((char*)&text_size, 4);

    /*******************************************************************
     *  텍스트를 저장할 위치가 유효하지 않다면, 텍스트를 읽어들이고 무시한다..
     *******/
    if (out_u16_text==nullptr) {
        in.seekg(text_size, std::ios::cur);
        return;
    }



    /*******************************************************************
     *  pmx파일이 UTF8 인코딩을 사용한다면, UTF16으로 변환해서 저장한다...
     ********/
    if (header.Globals.Text_Encoding == TextEncodingType::UTF8) {
        std::string u8_text;
        u8_text.resize(text_size + 1);

        in.read((char*)u8_text.data(), text_size);
        u8_text[text_size] = '\0';

        //UTF8 -> UTF16 길이 계산을 진행한다...
        uint32_t size_needed = MultiByteToWideChar(
            CP_UTF8, 0, u8_text.c_str(), -1, nullptr, 0
        );

        //빈 문자열이라면, u16_texPath를 빈 문자열로 설정한다...
        if (size_needed <= 0) {
            *out_u16_text = L"";
        }

        //빈 문자열이 아니라면, 변환을 시도한다....
        else {
            out_u16_text->resize(size_needed, 0);
            MultiByteToWideChar(
                CP_UTF8, 0, u8_text.c_str(), -1, out_u16_text->data(), size_needed
            );
        }

        return;
    }


    /********************************************************************
     *   pmx파일이 UTF8 인코딩을 사용한다면, 그대로 읽어들여서 저장한다...
     ********/
    else {
        const uint32_t u16_text_size = (text_size / 2);
        out_u16_text->resize(u16_text_size + 1);
        in.read((char*)out_u16_text->data(), text_size);
        (*out_u16_text)[u16_text_size] = L'\0';
    }
}














/*=======================================================================================================================
 *   메시 데이터를 읽어들이고, StorageDesc에 담습니다....
 **************/
void hyunwoo::PmxImporter::Import_StoreVertexData(std::ifstream& in, const Header& header, const StorageDescription& storageDesc)
{
    uint32_t vertex_count = 0;
    in.read((char*)&vertex_count, 4);


   /************************************************************
    *   읽어들인 메시 데이터들을, 지정한 mesh에 담는다...
    ******/
    Mesh& mesh = *storageDesc.OutMesh;
    mesh.Vertices.resize(vertex_count);

    for (uint32_t i = 0; i < vertex_count; i++)
    {
        /*------------------------------------------
         *  Position, Normal, UV를 읽어들인다..
         *----*/
        Vertex& vertex = mesh.Vertices[i];
        in.read((char*)&vertex.ObjPos, 12);
        in.seekg(12, std::ios::cur);
        in.read((char*)&vertex.UvPos, 8);




        /*--------------------------------------
         *  추가로 붙는 Vector4를 읽어들인다..
         *  ( 0이 될 수도 있다.. )
         *-----*/
        in.seekg((int)header.Globals.Additional_Vec4_Count * 4, std::ios::cur);



        /*---------------------------------------
         *  해당 버텍스의 스키닝 데이터를 읽어들인다..
         *-----*/
        WeightDeformType weight_deform_type;
        in.read((char*)&weight_deform_type, 1);


        switch (weight_deform_type)
        {
            //BDEF1
            case(WeightDeformType::BDEF1): 
            {
                vertex.SkinDeformDesc.DeformType      = SkinDeformType::BDEF;
                vertex.SkinDeformDesc.Weight_StartIdx = mesh.DeformWeights.size();
                vertex.SkinDeformDesc.Weight_Count    = 1;

                SkinDeformWeight new_weight;
                new_weight.Weight           = 1.f;
                new_weight.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                mesh.DeformWeights.push_back(new_weight);
                break;
            }

            //BDEF2
            case(WeightDeformType::BDEF2): 
            {
                vertex.SkinDeformDesc.DeformType      = SkinDeformType::BDEF;
                vertex.SkinDeformDesc.Weight_StartIdx = mesh.DeformWeights.size();
                vertex.SkinDeformDesc.Weight_Count    = 2;

                SkinDeformWeight new_weight1;
                SkinDeformWeight new_weight2;

                new_weight1.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight2.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);

                in.read((char*)&new_weight1.Weight, 4);
                new_weight2.Weight = (1.f - new_weight1.Weight);

                mesh.DeformWeights.push_back(new_weight1);
                mesh.DeformWeights.push_back(new_weight2);
                break;
            }

            //BDEF4
            case(WeightDeformType::BDEF4): {
                vertex.SkinDeformDesc.DeformType      = SkinDeformType::BDEF;
                vertex.SkinDeformDesc.Weight_StartIdx = mesh.DeformWeights.size();
                vertex.SkinDeformDesc.Weight_Count    = 4;

                SkinDeformWeight new_weight1;
                SkinDeformWeight new_weight2;
                SkinDeformWeight new_weight3;
                SkinDeformWeight new_weight4;

                new_weight1.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight2.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight3.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight4.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);

                in.read((char*)&new_weight1.Weight, 4);
                in.read((char*)&new_weight2.Weight, 4);
                in.read((char*)&new_weight3.Weight, 4);
                in.read((char*)&new_weight4.Weight, 4);

                mesh.DeformWeights.push_back(new_weight1);
                mesh.DeformWeights.push_back(new_weight2);
                mesh.DeformWeights.push_back(new_weight3);
                mesh.DeformWeights.push_back(new_weight4);
                break;
            }

            //SDEF
            case(WeightDeformType::SDEF): 
            {
                vertex.SkinDeformDesc.DeformType      = SkinDeformType::SDEF;
                vertex.SkinDeformDesc.Weight_StartIdx = mesh.DeformWeights.size();
                vertex.SkinDeformDesc.Weight_Count    = 2;

                SkinDeformWeight new_weight1;
                SkinDeformWeight new_weight2;

                new_weight1.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight2.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);

                in.read((char*)&new_weight1.Weight, 4);
                in.seekg(sizeof(Vector3) * 3, std::ios::cur);

                new_weight2.Weight = (1.f - new_weight1.Weight);
                mesh.DeformWeights.push_back(new_weight1);
                mesh.DeformWeights.push_back(new_weight2);
                break;
            }

            //QDEF
            case(WeightDeformType::QDEF): 
            {
                vertex.SkinDeformDesc.DeformType      = SkinDeformType::QDEF;
                vertex.SkinDeformDesc.Weight_StartIdx = mesh.DeformWeights.size();
                vertex.SkinDeformDesc.Weight_Count    = 4;

                SkinDeformWeight new_weight1;
                SkinDeformWeight new_weight2;
                SkinDeformWeight new_weight3;
                SkinDeformWeight new_weight4;

                new_weight1.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight2.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight3.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                new_weight4.BoneTransformIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);

                in.read((char*)&new_weight1.Weight, 4);
                in.read((char*)&new_weight2.Weight, 4);
                in.read((char*)&new_weight3.Weight, 4);
                in.read((char*)&new_weight4.Weight, 4);

                mesh.DeformWeights.push_back(new_weight1);
                mesh.DeformWeights.push_back(new_weight2);
                mesh.DeformWeights.push_back(new_weight3);
                mesh.DeformWeights.push_back(new_weight4);
                break;
            }
        }


        /*---------------------------------------
         *  Edge Scale값을 읽어들인다...
         *-----*/
        in.seekg(4, std::ios::cur);
    }
}















/*=======================================================================================================================
 *   메시 데이터를 읽어들이고, 무시합니다...
 ***************/
void hyunwoo::PmxImporter::Import_IgnoreVertexData(std::ifstream& in, const Header& header)
{
    uint32_t vertex_count = 0;
    in.read((char*)&vertex_count, 4);

    for (uint32_t i = 0; i < vertex_count; i++) 
    {
        /*------------------------------------------
         *  Position, Normal, UV를 읽어들인다..
         *----*/
        in.seekg(sizeof(Vector3) * 2 + sizeof(Vector2), std::ios::cur);


        /*--------------------------------------
         *  추가로 붙는 Vector4를 읽어들인다..
         *  ( 0이 될 수도 있다.. )
         *-----*/
        in.seekg((int)header.Globals.Additional_Vec4_Count * 4, std::ios::cur);


        /*---------------------------------------
         *  해당 버텍스의 스키닝 데이터를 읽어들인다..
         *-----*/
        WeightDeformType weight_deform_type;
        in.read((char*)&weight_deform_type, 1);

        switch (weight_deform_type)
        {
            //BDEF1
            case(WeightDeformType::BDEF1): {
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                break;
            }

             //BDEF2
            case(WeightDeformType::BDEF2): {
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                in.seekg(4, std::ios::cur);
                break;
            }

            //BDEF4
            case(WeightDeformType::BDEF4): {
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                in.seekg(16, std::ios::cur);
                break;
            }

            //SDEF
            case(WeightDeformType::SDEF): {
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                in.seekg(4, std::ios::cur);
                in.seekg(12 * 3, std::ios::cur);
                break;
            }

            //QDEF
            case(WeightDeformType::QDEF): {
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                in.seekg(4, std::ios::cur);
                break;
            }
        }

        //Edge Scale...
        in.seekg(4, std::ios::cur);
    }
}














/*=======================================================================================================================
 *   인덱스 데이터를 읽어들이고, 사전에 지정된 공간에 담습니다....
 *****************/
void hyunwoo::PmxImporter::Import_StoreTriangleData(std::ifstream& in, const Header& header, const StorageDescription& storageDesc)
{
    int32_t surface_count = 0;
    int32_t triangle_count = 0;

    /**************************************************************
     *   삼각형의 총 개수를 읽어들이고, 삼각형 데이터를 저장할 vector를
     *   확장한다....
     *******/
    in.read((char*)&surface_count, 4);
    triangle_count = (surface_count / 3);
    storageDesc.OutMesh->Triangles.resize(triangle_count);



    /****************************************************************
     *   데이터 크기에 따라 적절한 삼각형을 구성하고, 저장한다...
     ********/
    for (uint32_t i = 0; i < triangle_count; i++)
    {
       //ubyte 형식의 인덱스값 3개를 묶어 하나의 삼각형으로 만든다..
       if (header.Globals.Vertex_Index_Size == VertexIndexType::Type1_ubyte) {
           uint8_t idx1 = 0;
           uint8_t idx2 = 0;
           uint8_t idx3 = 0;

           in.read((char*)&idx1, 1);
           in.read((char*)&idx2, 1);
           in.read((char*)&idx3, 1);

           storageDesc.OutMesh->Triangles[i] = {
               (uint32_t)idx1, (uint32_t)idx2, (uint32_t)idx3
           };
       }


       //ushort 형식의 인덱스값 3개를 묶어 하나의 삼각형으로 만든다..
       else if (header.Globals.Vertex_Index_Size == VertexIndexType::Type2_ushort) {
           unsigned short idx1 = 0;
           unsigned short idx2 = 0;
           unsigned short idx3 = 0;

           in.read((char*)&idx1, 2);
           in.read((char*)&idx2, 2);
           in.read((char*)&idx3, 2);

           storageDesc.OutMesh->Triangles[i] = {
               (uint32_t)idx1, (uint32_t)idx2, (uint32_t)idx3
           };
       }


       //int 형식의 인덱스값 3개를 묶어 하나의 삼각형으로 만든다...
       else if (header.Globals.Vertex_Index_Size == VertexIndexType::Type4_int) {
           int32_t idx1 = 0;
           int32_t idx2 = 0;
           int32_t idx3 = 0;

           in.read((char*)&idx1, 4);
           in.read((char*)&idx2, 4);
           in.read((char*)&idx3, 4);

           storageDesc.OutMesh->Triangles[i] = {
               (uint32_t)idx1, (uint32_t)idx2, (uint32_t)idx3
           };
       }
    }
}
















/*=====================================================================================================================================
 *   인덱스 데이터를 읽어들이고, 무시합니다....
 ***************/
void hyunwoo::PmxImporter::Import_IgnoreTriangleData(std::ifstream& in, const Header& header)
{
    int32_t surface_count = 0;
    int32_t triangle_count = 0;

    /********************************************************************
     *   삼각형의 총 개수를 읽어들이고, 그만큼의 바이트 스트림을 무시한다....
     *******/
    in.read((char*)&surface_count, 4);
    triangle_count = (surface_count / 3);

    const int typeSize = (int)header.Globals.Vertex_Index_Size;
    in.seekg((typeSize * 3 * triangle_count), std::ios::cur);
}
















/*=====================================================================================================================================
 *   텍스쳐 데이터를 읽어들이고, 사전에 지정된 공간에 담습니다....
 ***************/
void hyunwoo::PmxImporter::Import_StoreTextureData(std::ifstream& in, const Header& header, const wchar_t* path, ImportResult& outRet, const StorageDescription& storageDesc)
{
    int32_t prevTex_buf_size = 0;
    int32_t texture_count    = 0;


    /***************************************************************
     *   총 텍스쳐 개수를 읽어들이고, 텍스쳐를 저장할 공간을 확보한다...
     *******/
    in.read((char*)&texture_count, 4);
    prevTex_buf_size = storageDesc.OutTextures->size();
    storageDesc.OutTextures->resize(prevTex_buf_size + texture_count);



    /****************************************************************
     *  pmx파일이 있는 곳을 기반으로, 텍스쳐 경로를 구성한 후 불러온다...
     *  윈도우즈 환경 전용이기 때문에, wstring은 utf16으로 취급한다. 
     *******/
    std::wstring u16_texPath;
    const std::wstring parent_path = std::filesystem::path(path).parent_path().wstring();

    for (uint32_t i = prevTex_buf_size; i < storageDesc.OutTextures->size(); i++) 
    {
        ReadText(in, header, &u16_texPath);

        /*--------------------------------------------------
         *   pmx파일의 텍스쳐 파일 형식을 png로 교체한다...
         *******/
        u16_texPath.replace(
            u16_texPath.begin() + u16_texPath.rfind(L'.'),
            u16_texPath.end(),
            L".png\0"
        );


        /*----------------------------------------------------------
         *   최종 텍스쳐의 경로를 구성하고, 텍스쳐를 읽어들이고 저장한다..
         *******/
        uint32_t            prev_importCount = 0;
        Texture2D&          outTex           = (*storageDesc.OutTextures)[i];
        const std::wstring& texture_path     = w$(parent_path.c_str(), L"/", u16_texPath.c_str());

        prev_importCount         = outRet.TextureLoadResult.ImportCount;
        outRet.TextureLoadResult = PngImporter::Import(outTex, texture_path);
        outRet.TextureLoadResult.ImportCount += prev_importCount;

        if (outRet.TextureLoadResult.Success == false) {
            outRet.Failed_TextureStorage = true;
            return;
        }
    }
}















/*=====================================================================================================================================
 *   텍스쳐 데이터를 읽어들이고, 무시합니다....
 ***************/
void hyunwoo::PmxImporter::Import_IgnoreTextureData(std::ifstream& in, const Header& header)
{
    int32_t prevTex_buf_size = 0;
    int32_t texture_count    = 0;
    in.read((char*)&texture_count, 4);

    for (int32_t i = 0; i < texture_count; i++) {
        int32_t path_size = 0;
        in.read((char*)&path_size, 4);
        in.seekg(path_size, std::ios::cur);
    }
}














/*=====================================================================================================================================
 *   머터리얼 데이터를 읽어들이고, 사전에 지정된 공간에 저장합니다...
 ***************/
void hyunwoo::PmxImporter::Import_StoreMaterialData(std::ifstream& in, const Header& header, ImportResult& outRet, const StorageDescription& storageDesc)
{
    int32_t material_count = 0;
    int32_t prev_tex_size  = (storageDesc.OutTextures->size() - outRet.TextureLoadResult.ImportCount);
    in.read((char*)&material_count, 4);


    for (int32_t i = 0; i < material_count; i++)
    {
        /*--------------------------------------
         *   머터리얼의 이름을 읽어들인다...
         *------*/

        //material name local...
        ReadText(in, header, nullptr);

        //material name universal...
        ReadText(in, header, nullptr);


        /*-------------------------------------
         *  Diffuse colour를 읽어들인다...
         *  ( Vector4 )
         *------*/
        in.seekg(16, std::ios::cur);


        /*-------------------------------------------
         *  Specular colour, strength를 읽어들인다..
         * ( Vector3, float )
         *------*/
        in.seekg(12, std::ios::cur);
        in.seekg(4, std::ios::cur);


        /*-------------------------------------
         *  Ambient colour를 읽어들인다...
         *  ( Vector3 )
         *------*/
        in.seekg(12, std::ios::cur);


        /*-------------------------------------
         *  Drawing Flags를 읽어들인다....
         * ( MaterialFlag )
         *------*/
        MaterialFlags drawing_flags;
        in.read((char*)&drawing_flags, 1);


        /*-------------------------------------
         *  Edge colour, scale를 읽어들인다...
         *  ( Vector4, float )
         *------*/
        in.seekg(16, std::ios::cur);
        in.seekg(4, std::ios::cur);


        /*-------------------------------------
         *  Texture index를 읽어들인다...
         *  ( byte, short, int )
         *------*/
        int32_t tex_idx = ReadDefaultIndexType(in, header.Globals.Texture_Index_Size);

        if (storageDesc.OutMaterials != nullptr && tex_idx >= 0)
        {
            //머터리얼을 저장하기위해서는, 텍스쳐 데이터의 저장도 필요하다...
            if (storageDesc.OutTextures == nullptr) {
                outRet.Failed_MaterialStorage = true;
                outRet.Require_TextureStorage = true;
                return;
            }

            Material newMaterial;
            newMaterial.MappedTexture = &(*storageDesc.OutTextures)[prev_tex_size + tex_idx];
            storageDesc.OutMaterials->push_back(newMaterial);
        }


        /*-----------------------------------------------
         *  Enviroment index, blend mode를 읽어들인다..
         *  ( index, byte )
         *------*/
        in.seekg((int)header.Globals.Texture_Index_Size, std::ios::cur);
        in.seekg(1, std::ios::cur);


        /*-------------------------------------
         *  Toon reference, value를 읽어들인다..
         *  ( byte, index/byte )
         *------*/
        ToonReferenceType toon_refType;
        in.read((char*)&toon_refType, 1);

        if (toon_refType == ToonReferenceType::Texture_Reference) {
            in.seekg((int)header.Globals.Texture_Index_Size, std::ios::cur);
        }
        else in.seekg(1, std::ios::cur);


        /*------------------------------------
         *   메타 데이터를 읽어들인다...
         *------*/
        int32_t metaData_size = 0;
        in.read((char*)&metaData_size, 4);
        in.seekg(metaData_size, std::ios::cur);


        /*-------------------------------------
         *   해당 머터리얼이 적용되는 삼각형들의
         *   개수를 읽어들인다..( 서브메시 하나를
         *   의미한다.. )
         *------*/
        uint32_t subMesh_index_count = 0;
        in.read((char*)&subMesh_index_count, 4);

        if (storageDesc.OutMesh != nullptr){
            storageDesc.OutMesh->SubMeshs.push_back(SubMesh{ (subMesh_index_count / 3) });
        }
    }

}
















/*=====================================================================================================================================
 *   머터리얼 데이터를 읽어들이고, 무시합니다....
 ***************/
void hyunwoo::PmxImporter::Import_IgnoreMaterialData(std::ifstream& in, const Header& header)
{
    int32_t material_count = 0;
    in.read((char*)&material_count, 4);


    for (int32_t i = 0; i < material_count; i++)
    {
        /*--------------------------------------
         *   머터리얼의 이름을 읽어들인다...
         *------*/
        int32_t name_size = 0;

        //material name local...
        in.read((char*)&name_size, 4);
        in.seekg(name_size, std::ios::cur);

        //material name universal...
        in.read((char*)&name_size, 4);
        in.seekg(name_size, std::ios::cur);


        /*-------------------------------------
         *  Diffuse colour를 읽어들인다...
         *  ( Vector4 )
         *------*/
        in.seekg(16, std::ios::cur);


        /*-------------------------------------------
         *  Specular colour, strength를 읽어들인다..
         * ( Vector3, float )
         *------*/
        in.seekg(12, std::ios::cur);
        in.seekg(4, std::ios::cur);


        /*-------------------------------------
         *  Ambient colour를 읽어들인다...
         *  ( Vector3 )
         *------*/
        in.seekg(12, std::ios::cur);


        /*-------------------------------------
         *  Drawing Flags를 읽어들인다....
         * ( MaterialFlag )
         *------*/
        MaterialFlags drawing_flags;
        in.read((char*)&drawing_flags, 1);


        /*-------------------------------------
         *  Edge colour, scale를 읽어들인다...
         *  ( Vector4, float )
         *------*/
        in.seekg(16, std::ios::cur);
        in.seekg(4, std::ios::cur);


        /*-------------------------------------
         *  Texture index를 읽어들인다...
         *  ( byte, short, int )
         *------*/
        int32_t tex_idx = 0;
        in.read((char*)&tex_idx, (int)header.Globals.Texture_Index_Size);


        /*-----------------------------------------------
         *  Enviroment index, blend mode를 읽어들인다..
         *  ( index, byte )
         *------*/
        in.seekg((int)header.Globals.Texture_Index_Size, std::ios::cur);
        in.seekg(1, std::ios::cur);


        /*-------------------------------------
         *  Toon reference, value를 읽어들인다..
         *  ( byte, index/byte )
         *------*/
        ToonReferenceType toon_refType;
        in.read((char*)&toon_refType, 1);

        if (toon_refType == ToonReferenceType::Texture_Reference) {
            in.seekg((int)header.Globals.Texture_Index_Size, std::ios::cur);
        }
        else in.seekg(1, std::ios::cur);


        /*------------------------------------
         *   메타 데이터를 읽어들인다...
         *------*/
        int32_t metaData_size = 0;
        in.read((char*)&metaData_size, 4);
        in.seekg(metaData_size, std::ios::cur);


        /*-------------------------------------
         *   해당 머터리얼이 적용되는 삼각형들의
         *   개수를 읽어들인다..( 서브메시 하나를
         *   의미한다.. )
         *------*/
        uint32_t subMesh_index_count = 0;
        in.read((char*)&subMesh_index_count, 4);
    }
}
















/*=====================================================================================================================================
 *   본 데이터를 읽어들이고, 사전에 지정된 공간에 저장합니다....
 ***************/
void hyunwoo::PmxImporter::Import_StoreBoneData(std::ifstream& in, const Header& header, const StorageDescription& storageDesc)
{
    int32_t bone_count;
    in.read((char*)&bone_count, 4);


    /********************************************************************
     *   본 데이터들을 모두 읽어들인다...
     *******/
    std::wstring bone_name;
    for (uint32_t i = 0; i < bone_count; i++) 
    {
        Bone new_bone;

        /*---------------------------------------------
         *   본의 로컬/유니버설 이름을 읽어들인다...
         *****/
        ReadText(in, header, &bone_name);
        ReadText(in, header, nullptr);

        new_bone.Name = bone_name.c_str();

       

        /*---------------------------------------------
         *   본의 월드 위치를 읽어들인다...
         *   (Pmx Editor)
         *******/
        in.read((char*)&new_bone.BindingPose.Position, sizeof(Vector3));



        /*----------------------------------------------
         *   부모 본의 인덱스를 읽어들인다...
         ********/
        new_bone.Parent_BoneIdx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);



        /*------------------------------------------------
         *  레이어값을 읽어들인다. PMX에는 본을 화면에서 켜고
         *  끄는 "표시 패널"이 있는데, 여기서 레이어별로 묶기
         *  위해서 존재하는 값이다...
         ******/
        int32_t layer;
        in.read((char*)&layer, 4);



        /*--------------------------------------------------
         *  본의 플래그들을 읽어들이고 처리한다....
         *******/
        BoneFlags flags;
        in.read((char*)&flags, 2);



        /*-------------------------------------------------
         *  Tail position을 나타내는 Vector3를 읽어들인다.
         *  만약 Indexed_tail_position 플래그가 설정되어 
         *  있다면, Vector3값 대신 Bone 인덱스값을 읽어들인다..
         *****/
        if (flags.Indexed_tail_position) {
            uint32_t bone_tail_idx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
        }
        else in.seekg(sizeof(Vector3), std::ios::cur);



        /*-----------------------------------------------
         *   상속받은 본이 있는가? 상속받은 회전/이동이 유효
         *   할 경우에만 처리한다...
         ******/
        int32_t inherit_parent_idx = -1;
        float   parent_influence   = 0.f;

        if (flags.Inherit_rotation || flags.Inherit_translation) {
            inherit_parent_idx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
            in.read((char*)&parent_influence, 4);
        }


        /*--------------------------------------------------
         *   고정된 축을 사용한다면, 해당 본이 가리키는 방향을
         *   나타내는 Vector3값을 읽어들인다..이는 팔의 동그란
         *   갈고리 본에 쓰인다...
         *******/
        Vector3 fixed_BoneDir;

        if (flags.Fixed_axis) {
            in.read((char*)&fixed_BoneDir, sizeof(Vector3));

            new_bone.useFixedAxis = flags.Fixed_axis;
            new_bone.FixedAxis    = fixed_BoneDir;
        }


        /*-----------------------------------------------------
         *  로컬 축을 사용한다면, 로컬 X/Z축 Vector3를 읽어들인다..
         *  (Y축은 주어진 X/Z축을 외적하면 구할 수 있다.) 
         *******/
        Vector3 axisX;
        Vector3 axisZ;
        Vector3 axisY;

        if (flags.Local_coordinate) {
            in.read((char*)&axisX, sizeof(Vector3));
            in.read((char*)&axisZ, sizeof(Vector3));
            axisY = Vector3::Cross(axisX, axisZ);
        }


        /*---------------------------------------------
         *  외부 보모의 트랜스폼을 사용한다면, 부모 본의
         *  인덱스값을 읽어들인다....
         *******/
        int32_t external_parent_idx = -1;

        if (flags.External_parent_deform) {
            external_parent_idx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
        }


        /*---------------------------------------------
         *   IK를 사용한다면, 본 정보들을 읽어들인다...
         *******/
        IKDescriptor::IKInfo ik_info;

        if (flags.IK)
        {
            ik_info.ik_bone_idx        = i;
            ik_info.ik_target_bone_idx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);

            in.read((char*)&ik_info.ik_loop_count, 4);
            in.read((char*)&ik_info.limit_radian, 4);
            in.read((char*)&ik_info.link_count, 4);


            //모든 IK 링크들을 읽어들인다...
            for (uint32_t j = 0; j < ik_info.link_count; j++)
            {
                IKLink new_link;

                new_link.Bone_idx = ReadDefaultIndexType(in, header.Globals.Bone_Index_Size);
                in.read((char*)&new_link.Has_limits, 1);

                if (new_link.Has_limits) {
                    in.read((char*)&new_link.AngleLimit.Limit_Min, sizeof(Vector3));
                    in.read((char*)&new_link.AngleLimit.Limit_Max, sizeof(Vector3));
                }

                if (storageDesc.OutIKDesc != nullptr) {
                    storageDesc.OutIKDesc->m_links.push_back(new_link);
                }
            }

            if (storageDesc.OutIKDesc != nullptr) {
                ik_info.link_start_idx = (storageDesc.OutIKDesc->m_links.size() - ik_info.link_count);
                storageDesc.OutIKDesc->m_infos.push_back(ik_info);
            }
        }

        storageDesc.OutMesh->Bones.push_back(new_bone);
    }



    /*****************************************************************************
     *   ik용 본들과 대응되는 디스플레이용 본이 있다면, 교체한다....
     *********/
    if (storageDesc.OutIKDesc == nullptr) {
        return;
    }

    IKDescriptor&      ik_desc = *storageDesc.OutIKDesc;
    std::vector<Bone>& bones   = storageDesc.OutMesh->Bones;

    for (uint32_t i=0; i<storageDesc.OutIKDesc->m_infos.size(); i++)
    {
        IKDescriptor::IKInfo& info        = storageDesc.OutIKDesc->m_infos[i];
        const Bone&           endEff_bone = bones[info.ik_target_bone_idx];

        /*--------------------------------------------------
         *  해당 endEffect 본과 대응되는 디스플레이 본을 찾는다..
         *  ( 가장 가까운 곳에 배치되었다는 가정하에 적용한다... )
         ******/
        float    min_dst = std::numeric_limits<float>::max();
        uint32_t min_idx = info.ik_target_bone_idx;

        for (uint32_t j = 0; j < bones.size(); j++) {
            const Bone& bone = bones[j];

            const float dst = (bone.BindingPose.Position - endEff_bone.BindingPose.Position).GetSqrMagnitude();
            if (dst <= min_dst && j!=info.ik_target_bone_idx) {
                min_dst = dst;
                min_idx = j;
            }
        }

        info.ik_target_bone_idx = min_idx;


        /*--------------------------------------------------
         *  각 링크 본들과 대응되는 디스플레이 본을 찾는다..
         *  ( 가장 가까운 곳에 배치되었다는 가정하에 적용한다... )
         *****/
        min_dst = std::numeric_limits<float>::max();

        for (uint32_t j = 0; j < info.link_count; j++) 
        {
            const uint32_t link_idx  = (ik_desc.m_links[info.link_start_idx+j].Bone_idx);
            const Bone&    link_bone = bones[link_idx];

            min_idx = link_idx;
            min_dst = std::numeric_limits<float>::max();
            {
                for (uint32_t k = 0; k < bones.size(); k++) {
                    const Bone& bone = bones[k];
                    const float dst  = (bone.BindingPose.Position - link_bone.BindingPose.Position).GetSqrMagnitude();

                    if (dst < min_dst && link_idx!=k) {
                        min_dst = dst;
                        min_idx = k;
                    }
                }
            }
            ik_desc.m_links[info.link_start_idx + j].Bone_idx = min_idx;
        } 
    }
}