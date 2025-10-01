#include "PmxImporter.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include "../UtilityModule/StringLamda.h"

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
    in.read((char*)&textSize, 4);
    in.seekg(textSize, std::ios::cur);

    //모델의 유니버설 이름...
    in.read((char*)&textSize, 4);
    in.seekg(textSize, std::ios::cur);

    //모델의 로컬 설명...
    in.read((char*)&textSize, 4);
    in.seekg(textSize, std::ios::cur);

    //모델의 유니버설 이름...
    in.read((char*)&textSize, 4);
    in.seekg(textSize, std::ios::cur);







    /******************************************************************************************************
     *   메시의 버텍스 데이터를 읽어들인다....
     *********/
    uint32_t vertex_count = 0;
    in.read((char*)&vertex_count, 4);

    //메시 데이터를 저장해야한다면, 버텍스 버퍼를 초기화한다....
    if (storageDesc.OutMesh!=nullptr) {
        storageDesc.OutMesh->Vertices.resize(vertex_count);
    }


    for (uint32_t i = 0; i < vertex_count; i++)
    {
        /*------------------------------------------
         *  Position, Normal, UV를 읽어들인다..
         *----*/
        if (storageDesc.OutMesh != nullptr) {
            Vertex& vertex = storageDesc.OutMesh->Vertices[i];
            in.read((char*)&vertex.ObjPos, 12);
            in.seekg(12, std::ios::cur);
            in.read((char*)&vertex.UvPos, 8);
        }

        else in.seekg(sizeof(Vector3) * 2 + sizeof(Vector2), std::ios::cur);



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
                in.seekg((int)header.Globals.Bone_Index_Size, std::ios::cur);
                break;
            }

            //BDEF2
            case(WeightDeformType::BDEF2): {
                in.seekg((int)header.Globals.Bone_Index_Size * 2, std::ios::cur);
                in.seekg(4, std::ios::cur);
                break;
            }

            //BDEF4
            case(WeightDeformType::BDEF4): {
                in.seekg((int)header.Globals.Bone_Index_Size * 4, std::ios::cur);
                in.seekg(16, std::ios::cur);
                break;
            }

            //SDEF
            case(WeightDeformType::SDEF): {
                in.seekg((int)header.Globals.Bone_Index_Size * 2, std::ios::cur);
                in.seekg(4, std::ios::cur);
                in.seekg(12 * 3, std::ios::cur);
                break;
            }

            //QDEF
            case(WeightDeformType::QDEF): {
                in.seekg((int)header.Globals.Vertex_Index_Size * 4, std::ios::cur);
                in.seekg(4, std::ios::cur);
                break;
            }
        }

        //Edge Scale...
        in.seekg(4, std::ios::cur);
    }








    /*****************************************************************************************************
     *   메시의 인덱스 정보들을 처리한다...
     *********/
    int32_t surface_count = 0;
    int32_t triangle_count = 0;

    in.read((char*)&surface_count, 4);
    triangle_count = (surface_count / 3);

    /*------------------------------------------------------------
     *   메시 데이터를 저장해야하는 경우....
     *-------*/
    if (storageDesc.OutMesh!=nullptr) {
        storageDesc.OutMesh->Triangles.resize(triangle_count);

        for (uint32_t i = 0; i < triangle_count; i++) 
        {
            //ubyte 형식의 인덱스값 3개들을 묶어 하나의 삼각형으로 만든다...
            if (header.Globals.Vertex_Index_Size==VertexIndexType::Type1_ubyte) {
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
            else if(header.Globals.Vertex_Index_Size==VertexIndexType::Type2_ushort) {
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
            else if(header.Globals.Vertex_Index_Size==VertexIndexType::Type4_int) {
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

    /*-------------------------------------------------------
     *   메시 데이터를 무시하는 경우....
     *-----*/
    else {
        const int typeSize = (int)header.Globals.Vertex_Index_Size;
        in.seekg((typeSize*3*triangle_count), std::ios::cur);
    }







    /*****************************************************************************************
     *   해당 메시에 사용된 텍스쳐들을 처리한다....
     **********/
    int32_t prevTex_buf_size = 0;
    int32_t texture_count = 0;
    in.read((char*)&texture_count, 4);

    /*----------------------------------------------
     *   텍스쳐를 저장해야하는 경우....
     *-------*/
    if (storageDesc.OutTextures!=nullptr) 
    {
        prevTex_buf_size       = storageDesc.OutTextures->size();
        const int32_t new_size = (prevTex_buf_size + texture_count);
        storageDesc.OutTextures->resize(new_size);


        //텍스쳐 경로가 UTF16LE 인코딩을 사용하는 경우....
        if (header.Globals.Text_Encoding==TextEncodingType::UTF16LE)
        {
            int32_t            texPath_size;
            const std::wstring parent_path = std::filesystem::path(path).parent_path().wstring();
            std::wstring       tex_path;

            for (int32_t i = prevTex_buf_size; i < new_size; i++) {
                in.read((char*)&texPath_size, 4);
                tex_path.resize(texPath_size);

                in.read((char*)&tex_path[0], texPath_size);
                tex_path[texPath_size/2] = L'\0';

                tex_path.replace(
                    tex_path.begin() + tex_path.rfind(L'.'), 
                    tex_path.end(), 
                    L".png"
                );

                Texture2D&          outTex       = (*storageDesc.OutTextures)[i];
                const std::wstring& texture_path = w$(parent_path.c_str(), L"/", tex_path.c_str());

                if ((outRet.TextureLoadResult = PngImporter::Import(outTex, texture_path)).Success == false) {
                    outRet.Failed_TextureStorage = true;
                    return outRet;
                }
            }
        }


        //텍스쳐 경로가 UTF8 인코딩을 사용하는 경우....
        else if (header.Globals.Text_Encoding==TextEncodingType::UTF8)
        {

        }
    }


    /*---------------------------------------------
     *   텍스쳐를 무시해야하는 경우....
     *-------*/
    else 
    {
        for (int32_t i = 0; i < texture_count; i++){
            int32_t path_size = 0;
            in.read((char*)&path_size, 4);
            in.seekg(path_size, std::ios::cur);
        }
    }





    /**************************************************************************************
     *   해당 메시에 사용된 머터리얼 정보들을 읽어들인다....
     **********/
    int32_t material_count = 0;
    in.read((char*)&material_count, 4);

    for (int32_t i = 0; i < material_count; i++) {

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
        in.seekg(4,  std::ios::cur);


        /*-------------------------------------
         *  Ambient colour를 읽어들인다...
         *  ( Vector3 )
         *------*/
        in.seekg(12, std::ios::cur);


        /*-------------------------------------
         *  Drawing Flags를 읽어들인다....
         * ( MaterialFlag )
         *------*/
        uint8_t drawing_flags;
        in.read((char*)&drawing_flags, 1);


        /*-------------------------------------
         *  Edge colour, scale를 읽어들인다...
         *  ( Vector4, float )
         *------*/
        in.seekg(16, std::ios::cur);
        in.seekg(4,  std::ios::cur);


        /*-------------------------------------
         *  Texture index를 읽어들인다...
         *  ( byte, short, int )
         *------*/
        int32_t tex_idx = 0;
        in.read((char*)&tex_idx, (int)header.Globals.Texture_Index_Size);

        if (storageDesc.OutMaterials!=nullptr) 
        {
            //머터리얼을 저장하기위해서는, 텍스쳐 데이터의 저장도 필요하다...
            if (storageDesc.OutTextures==nullptr) {
                outRet.Failed_MaterialStorage = true;
                outRet.Require_TextureStorage = true;
                return outRet;
            }

            Material newMaterial      = { 0, };
            newMaterial.TwoSide       = false;
            newMaterial.MappedTexture = &(*storageDesc.OutTextures)[prevTex_buf_size + tex_idx];
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

        if (storageDesc.OutMesh!=nullptr) 
        {
            storageDesc.OutMesh->SubMeshs.push_back(Mesh::SubMesh{ (subMesh_index_count / 3) });
        }
    }


    outRet.Success = true;
    return outRet;
}