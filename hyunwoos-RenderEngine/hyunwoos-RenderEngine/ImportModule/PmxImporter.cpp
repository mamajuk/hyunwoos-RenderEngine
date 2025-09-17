#include "PmxImporter.h"
#include <fstream>
#include <vector>

/*=======================================================================================================================
 *  지정한 주소의 Png파일로부터, 텍스쳐 데이터를 읽어들입니다....
 *  reference: https://gist.github.com/felixjones/f8a06bd48f9da9a4539f
 *=============*/
hyunwoo::PmxImporter::ImportResult hyunwoo::PmxImporter::Import(Mesh& outMesh, const std::wstring& path)
{
    ImportResult ret = { 0, };


    /**********************************************************************************
     *  지정한 주소에 해당 파일이 존재하는가? 없다면
     *  결과를 갱신하고, 함수를 종료한다...
     *******/
    std::ifstream in(path, std::ios::binary);

    if (in.is_open()==false) {
        ret.Failed_OpenFile = true;
        return ret;
    }




    /**********************************************************************************
     *   Pmx 헤더를 읽어들인다....
     ********/
    PmxImporter::Header header = { 0, };

    /*---------------------------------------
     *  Pmx시그니처가 아니라면, 결과를 갱신하고
     *  함수를 종료한다...
     *-------*/
    in.read((char*)&header.Signature, 4);

    if (header.Signature!=PmxImporter::Signature) {
        ret.IsNotPmxFile = true;
        return ret;
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



    /*********************************************************************************
     *   메시의 버텍스 목록을 읽어들인다....
     *********/
    uint32_t vertex_count = 0;

    in.read((char*)&vertex_count, 4);
    outMesh.Vertices.resize(vertex_count);


    for (uint32_t i=0; i<vertex_count; i++) 
    {
        /*--------------------------------------
         *  Position, Normal, UV를 읽어들인다..
         *----*/
        Vertex& vertex = outMesh.Vertices[i];
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
            case(WeightDeformType::BDEF1): {
                in.seekg((int)header.Globals.Vertex_Index_Size, std::ios::cur);
                break;
            }

            //BDEF2
            case(WeightDeformType::BDEF2): {
                in.seekg((int)header.Globals.Vertex_Index_Size * 2, std::ios::cur);
                in.seekg(4, std::ios::cur);
                break;
            }

            //BDEF4
            case(WeightDeformType::BDEF4): {
                in.seekg((int)header.Globals.Vertex_Index_Size * 4, std::ios::cur);
                in.seekg(16, std::ios::cur);
                break;
            }

            //SDEF
            case(WeightDeformType::SDEF): {
                in.seekg((int)header.Globals.Vertex_Index_Size * 2, std::ios::cur);
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


    /*************************************************************************************
     *   메시의 인덱스 정보들을 읽어들인다....
     *********/
    int32_t surface_count  = 0;
    int32_t triangle_count = 0;

    in.read((char*)&surface_count, 4);
    triangle_count = (surface_count / 3);
    outMesh.Triangles.resize(triangle_count);

    for (uint32_t i = 0; i < triangle_count; i++)
    {
        switch (header.Globals.Vertex_Index_Size)
        {
            /*----------------------------------------------------------
             *  ubyte 형식의 인덱스값 3개들을 묶어 하나의 삼각형으로 만든다...
             *-------*/
            case(VertexIndexType::Type1_ubyte): {
                    uint8_t idx1 = 0;
                    uint8_t idx2 = 0;
                    uint8_t idx3 = 0;

                    in.read((char*)&idx1, 1);
                    in.read((char*)&idx2, 1);
                    in.read((char*)&idx3, 1);

                    outMesh.Triangles[i] = {
                        (uint32_t)idx1, (uint32_t)idx2, (uint32_t)idx3
                    };

                    break;
            }


            /*--------------------------------------------------------
             *  ushort 형식의 인덱스값 3개를 묶어 하나의 삼각형으로 만든다..
             *------*/
            case(VertexIndexType::Type2_ushort): {
                    unsigned short idx1 = 0;
                    unsigned short idx2 = 0;
                    unsigned short idx3 = 0;

                    in.read((char*)&idx1, 2);
                    in.read((char*)&idx2, 2);
                    in.read((char*)&idx3, 2);

                    outMesh.Triangles[i] = {
                        (uint32_t)idx1, (uint32_t)idx2, (uint32_t)idx3
                    };

                    break;
            }


            /*-------------------------------------------------------
             *  int 형식의 인덱스값 3개를 묶어 하나의 삼각형으로 만든다...
             *--------*/
            case(VertexIndexType::Type4_int): {
                    int32_t idx1 = 0;
                    int32_t idx2 = 0;
                    int32_t idx3 = 0;

                    in.read((char*)&idx1, 4);
                    in.read((char*)&idx2, 4);
                    in.read((char*)&idx3, 4);

                    outMesh.Triangles[i] = {
                        (uint32_t)idx1, (uint32_t)idx2, (uint32_t)idx3
                    };

                    break;
            }
        }
    }



    /************************************************************************************
     *   해당 메시에 사용된 텍스쳐들의 경로들을 읽어들인다....
     **********/
    int32_t texture_count = 0;
    in.read((char*)&texture_count, 4);

    std::vector<wchar_t> texture_name;
    for (int32_t i = 0; i < texture_count; i++)
    {
        int32_t path_size = 0;
        int32_t prev_size = texture_name.size();
        in.read((char*)&path_size, 4);

        texture_name.resize(prev_size + path_size);
        in.read((char*)&texture_name[prev_size], path_size);
    }



    /**************************************************************************************
     *   해당 메시에 사용된 머터리얼 정보들을 읽어들인다....
     **********/
    int32_t material_count = 0;
    in.read((char*)&material_count, 4);

    std::vector<int32_t> tex_idx_list;
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
        in.seekg(1, std::ios::cur);

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

        tex_idx_list.push_back(tex_idx);

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
        outMesh.SubMesh_Triangle_Counts.push_back(subMesh_index_count / 3);
    }


    ret.Success = true;
    return ret;
}
