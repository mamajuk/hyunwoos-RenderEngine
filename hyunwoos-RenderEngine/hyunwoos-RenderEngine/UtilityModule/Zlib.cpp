#include "Zlib.h"

/*=====================================================================================
 *   주어진 코드 길이 테이블을 이용하여, 동적 허프만 트리를 구축한다....
 *==========*/
void hyunwoo::Zlib::HuffmanTree::Build_Dynamic(const uint32_t* code_length_table_ptr, uint32_t code_length_table_len)
{
    /********************************************************
     *   각 코드 길이들의 발생 빈도를 구한다...
     *   e.g: code_len_table	   = (3,3,3,3,3,2,4,4)
     *        code_len_frequencies = (0,1,5,2)
     *
     * 	 ※Deflate나 Jpg등은 효율을 위해 코드의 최대 비트 수를 보통
     *	  15 또는 16으로 제한한다. 해당 메소드는 16으로 제한한다.※
     ******/
    uint32_t max_code_length             = 0;
    uint32_t code_length_frequencies[16] = { 0, };

    for (uint32_t i = 0; i < code_length_table_len; i++) {
        const uint32_t code_length = code_length_table_ptr[i];

        if (code_length > 0) {
            code_length_frequencies[code_length - 1]++;
        }

        //현재 코드 길이가 가장 긴 코드인가?
        if (max_code_length < code_length) {
            max_code_length = code_length;
        }
    }


    /**********************************************************
     *   각 코드 길이들의 시작 코드값을 구한다....
     *   e.g: code_len_frequencies    = (0,1,5,2)
     *		  code_length_start_value = (0,0,2,14)
     ******/
    uint32_t code                         = 0;
    uint32_t code_length_start_values[16] = { 0, };

    for (uint32_t bits = 1; bits <= max_code_length; bits++) {

        code = (code + code_length_frequencies[bits - 1]) << 1;
        code_length_start_values[bits] = code;
    }



    /*******************************************************
     *   시작값들을 기반으로, 허프만 트리를 구축한다...
     *******/

     /*---------------------------------
      *  트리를 초기화하고, 루트 노드를
      *  삽입한다...
      *----*/
    m_nodeList.clear();
    m_nodeList.reserve(code_length_table_len);
    m_nodeList.push_back(Node{ -1, -1, 0 });


    /*----------------------------------
     *  트리에 각 코드들을 구성하는
     *  노드들을 삽입한다. 사전순으로 값이
     *  
     *-----*/
    for (uint32_t i = 0; i < code_length_table_len; i++) {
        const uint32_t code_len = code_length_table_ptr[i];

        if (code_len > 0) {
            InsertNode(i, code_length_start_values[code_len - 1]++, code_len);
        }
    }

}












/*==========================================================================================
 *   RFC1951에서 정의되어 있는 고정 허프만 트리를 구축한다.....
 *===========*/
void hyunwoo::Zlib::HuffmanTree::Build_Fixed(const SymbolType symbolType)
{
    /***********************************************************
     *   리터럴/원본 문자열의 길이값이 심볼값인 트리를 구축하는가?
     *******/

     /*---------------------------------
      *  트리를 초기화하고, 루트 노드를
      *  삽입한다...
      *----*/
    m_nodeList.clear();
    m_nodeList.push_back(Node{ -1, -1, 0 });


    if (symbolType==SymbolType::Literal_Length) 
    {
        /**lit Value (0-143)**/
        for (uint32_t i = 0; i < 143; i++) {
            InsertNode(i, (0b00110000+i), 8);
        }

        /**lit Value (144-255)**/
        for (uint32_t i = 0; i < 111; i++) {
            InsertNode(i, (0b110010000 + i), 9);
        }

        /**lit Value (256-279)**/
        for (uint32_t i = 0; i < 23; i++) {
            InsertNode(i, (0b0000000 + i), 7);
        }

        /**lit Value (280-287)**/
        for (uint32_t i = 0; i < 7; i++) {
            InsertNode(i, (0b11000000 + i), 8);
        }

        return;
    }


    /*******************************************************
     *    역방향 거리값이 심볼값인 트리를 구축하는가?
     *********/

    /**Dist Value (0-31)**/
    for (uint32_t i = 0; i < 31; i++) {
        InsertNode(i, i, 5);
    }
}















/*==========================================================================================
 *   비트스트림으로부터 허프만 코드를 읽어들이고, 이에 대응되는 심볼을 반환합니다....
 *========*/
uint32_t hyunwoo::Zlib::HuffmanTree::GetSymbol(BitStream& bitStream) const
{
    uint32_t curNodeIdx = 0;

    while (curNodeIdx >= 0 && curNodeIdx < m_nodeList.size()) 
    {
        /******************************************
         * 현재 노드의 자식 노드가 존재하지 않는가?
         *******/
        const Node& node = m_nodeList[curNodeIdx];
        if (node.Left < 0 && node.Right < 0) {

            return node.Symbol;
        }


        /*********************************************
         *  허프만 코드값에 따라 다음 노드로 이동시킨다..
         *******/
        curNodeIdx = (bitStream.ReadBits(1) > 0 ? node.Right : node.Left);
    }

    return 0;
}













/*==========================================================================================
 *    트리에 지정한 코드를 구성하는 노드들을 삽입합니다.....
 *=======*/
void hyunwoo::Zlib::HuffmanTree::InsertNode(uint32_t symbol, uint32_t code, uint32_t code_length)
{
    uint32_t curNodeIdx = 0;

    for (uint32_t bitIdx = 0; bitIdx < code_length; bitIdx++) 
    {
        /************************************************************
         *  현재 코드 비트값에 따라서, 적절한 방향으로
         *  노드를 삽입한다...
         *******/

        /*--------------------------------------
         *  노드를 현재 노드의 우측으로 삽입하는가?
         *----*/
        if ((code & (1<<(code_length-bitIdx-1)))>0){

            //해당 방향이 비어있는가?
            if (m_nodeList[curNodeIdx].Right < 0) {
                m_nodeList[curNodeIdx].Right = m_nodeList.size();
                m_nodeList.push_back(Node{ -1, -1, 0 });
            }

            curNodeIdx = m_nodeList[curNodeIdx].Right;
        }

        /*--------------------------------------
         *  노드를 현재 노드의 좌측으로 삽입하는가?
         *----*/
        else{

            //해당 방향이 비어있는가?
            if (m_nodeList[curNodeIdx].Left < 0) {
                m_nodeList[curNodeIdx].Left = m_nodeList.size();
                m_nodeList.push_back(Node{ -1, -1, 0 });
            }

            curNodeIdx = m_nodeList[curNodeIdx].Left;
        }
    }


    /**************************************************************
     *   최종 노드에 코드와 대응되는 심볼값을 넣는다...
     *******/
    m_nodeList[curNodeIdx].Symbol = symbol;
}












/*=============================================================================================================================================
 *     Zlib로 압축된 데이터의 압축을 해제합니다.....
 *=========*/
hyunwoo::Zlib::InflateResult hyunwoo::Zlib::Inflate(std::vector<uint8_t>& inZlibDeflateStream, std::vector<uint8_t>& outInflateStream)
{
    Zlib::Data          data;
    Zlib::InflateResult ret       = { 0, };


    /**************************************************************************************************
     *    인자로 받은 압축된 데이터 스트림으로부터, Zlib::Data를 읽어들인다...
     *******/
    uint32_t stream_idx  = 0;


    /*-----------------------------------------
     *   Zlib Data의 압축 방식과, 추가 정보가
     *   담긴 엑스트라 플래그값을 읽어들인다...
     *------*/
    data.CompressionMethod.data = inZlibDeflateStream[stream_idx++];
    data.Extra_flags.data       = inZlibDeflateStream[stream_idx++];


    /*--------------------------------------------
     *   Zlib는 무조건 Deflate 압축 방식을 사용한다.
     *   아니라면 결과를 갱신하고, 함수를 종료한다....
     *------*/
    if (data.CompressionMethod.cm != Zlib::CompressionMethod::CMType::Deflate) {
        ret.CMIsNotDeflate = true;
        return ret;
    }


    /*-------------------------------------------------
     *   CMF, FLG값이 손상되었는가? 
     *   (FCHECK로 CMFx256+FLG값이 31의 배수임을 확인...)
     *------*/
    if ((((data.CompressionMethod.data * 256) + data.Extra_flags.data) % 31)!=0) {
        ret.CMF_Or_FLG_ValueIsCorrupted = true;
        return ret;
     }


    /*--------------------------------------------------
     *   FLG 직후 따라오는 32bits의 프리셋 Dictionary가
     *   존재하는가? 있다면 4bytes를 추가로 읽어들인다...
     *   ( 왠만해서는 사용되지 않는다... )
     *-------*/
    if (data.Extra_flags.fdict >= 1) {
        data.fdict              = *(uint32_t*)inZlibDeflateStream[stream_idx];
        stream_idx             += 4;
        ret.UsePresetDictionary = true;
    }


    /*-----------------------------------------------
     *   압축되어있는 Zlib 블럭 데이터들을 읽어들인다..
     *-------*/
    const uint32_t zlib_block_size = (inZlibDeflateStream.size() - stream_idx - 4);

    data.DeflateBlockData.resize(zlib_block_size);
    memcpy(&data.DeflateBlockData[0], &inZlibDeflateStream[stream_idx], zlib_block_size);
    stream_idx += zlib_block_size;


    /*-----------------------------------------------
     *   확인값을 읽는다....
     *-------*/
    data.checkSum_alder32 = *(uint32_t*)&inZlibDeflateStream[stream_idx];






    /*****************************************************************************************************
     *   읽어들인 Zlib 데이터의 압축을 해제한다.....
     *******/
    bool                       isFinalBlock;
    BitStream                  bitStream(&data.DeflateBlockData[0], data.DeflateBlockData.size());
    Zlib::BlockCompressionType blockType;


    /*-----------------------------------------------
     *  LZ77 압축해제를 위한 허프만 트리들과 테이블.
     *  하나의 블럭마다 허프만 트리들을 재구축해야 하기 
     *  때문에, 재할당이 빈번하게 발생한다. 따라서 블럭들을
     *  처리하는 반복문 밖에서 정의한다...
     *----*/
    Zlib::HuffmanTree     code_len_code_len_tree;
    Zlib::HuffmanTree     literal_length_tree;
    Zlib::HuffmanTree     distance_tree;

    Zlib::HuffmanTree     fixed_literal_length_tree;
    Zlib::HuffmanTree     fixed_distance_tree;
    std::vector<uint32_t> litLength_and_dist_code_len_table;

    //고정 허프만 트리들을 구축한다...
    fixed_literal_length_tree.Build_Fixed(Zlib::HuffmanTree::SymbolType::Literal_Length);
    fixed_distance_tree.Build_Fixed(Zlib::HuffmanTree::SymbolType::Distance);

    do {
       /*+------------------------------------------------------------+
         | Zlib(Deflate)로 압축된 데이터는, 여러개의 블럭으로 구성되어      |
         | 있다. 하나의 블럭은 해당 블럭 고유의 압축 방식으로 압축되어 있다.  |
         | 이를 알기 위해서, 블럭의 최상단 3bits를 읽어야 한다.             |
         | 각 블럭의 고유 압축 방식으로 압축을 해제하여 얻은 결과물을 결과    |
         | 스트림에 차례대로 쌓아가면 Zlib 압축이 완전히 해제된 원본 데이터를 |
         | 얻을 수 있다...                                             |
         +------------------------------------------------------------+



        /*---------------------------------------------------------
         *   해당 블럭의 3 bits 헤더를 읽는다....  
         **------*/

        /*--------------------------------------------------------+
         *   isFinalBlock(1) | 마지막 블럭인가? ( true:1, false:0 ) |
         * ------------------+------------------------------------+
         *   hummanType  (2) | 사용된 허프만 압축 유형은?            |
         *                   | (00): 사용되지 않음.                |
         *                   | (01): 고정 Huffman 코드를 사용함.    |
         *                   | (10): 동적 Huffman 코드를 사용함.    |
         *                   | (11): 무효.                        |
         **------------------+-----------------------------------|*/
        isFinalBlock = (bitStream.ReadBits(1) > 0);
        blockType    = Zlib::BlockCompressionType(bitStream.ReadBits(2));



        /*------------------------------------------------------------
         *   읽어들인 블럭에서 사용된 압축 방법에 따라서 적절히 처리한다.....
         *-------*/
        switch (blockType)
        {
            /*----------------------------------------------------+
             |                      압축되지 않음                   |
             +-----------+----------------------------------------+
             |  INFO     |  3bit 헤더 이후, 바이트 경계까지의 비트를  |
             |           |  모두 버려야한다. LEN, NLEN을 읽어들인 후  |
             |           |  엔 LEN 바이트 만큼의 해당 블럭에 포함된   |
             |           |  리터럴 데이터(원본 데이터)가 나온다.      |
             +-----------+----------------------------------------+
             |   LEN(16) |  블럭 내의 데이터 바이트 수를 나타낸다. 즉  |
             |           |  해당 블럭 내의 리터럴 데이터의 길이다.     |
             +-----------+----------------------------------------+
             |  NLEN(16) |  LEN의 보수를 나타낸다. 즉 LEN의 비트를    |
             |           |  반전시킨 값이다. 이 값은 데이터의 무결성을 |
             |           |  검사하기 위해 사용된다...                |
             +-----------+----------------------------------------+*/
            case(Zlib::BlockCompressionType::None): {

                bitStream.MoveOffsetToByteBoundary();

                uint32_t LEN  = bitStream.ReadBits(16);
                uint32_t NLEN = bitStream.ReadBits(16);

                //리터럴 바이트 수만큼, 결과 스트림에 복사한다...
                for (int i = 0; i < LEN; i++) {
                    outInflateStream.push_back(uint8_t(bitStream.ReadBits(8)));
                }

                break;
            }


           /*+--------------------------------------------------------+                                                     
             | 압축되어 있는 블럭들은, 원본 데이터를 LZ77 알고리즘으로 중복  |
             | 되는 부분들을 <중복 문자열의 길이, 역방향 거리>의 짝으로 치환 |
             | 시킨다. 그리고 '원본 문자열/중복 문자열의 길이'들만을 대상으로|
             | 허프만 코딩으로 압축시켜서 얻은 허프만 코드로 원본 문자열,    |
             | 중복 문자열의 길이 부분을 치환시킨다. 또 '역방향 거리'들만을  |
             | 대상으로 허프만 코딩으로 압축시켜서 얻은 허프만 코드로 역방향 |
             | 거리 부분을 치환한다. 따라서 바이트 스트림에서 바이트 순으로  |
             | 봤을 때, 블럭의 구성은 다음과 같이 된다:                   |
             |                                                       |
             | ①: 원본 문자열/중복 문자열의 길이 허프만 코딩을 해제하는데  |
             |     필요한 코드 길이 테이블의 바이트 스트림.               |
             | ②: 중복 문자열에서 원본 문자열까지의 역방향 거리 허프만 코딩 |
             |     을 해제하는데 필요한 코드 길이 테이블의 바이트 스트림.   |
             | ③: LZ77로 압축되어 있는 바이트 스트림.                   |
             |                                                       |
             |  원본 데이터를 얻고 싶다면, ①②를 통해서 '중복 문자열의 길이'|
             |  , '역방향 거리'의 허프만 트리를 만들어서 ③의 허프만 코드들을|
             |  원래값으로 다시 복구시키고, LZ77 알고리즘을 따라 원본 문자열|
             |  로 바꾸면 된다.                                        |
             +-------------------------------------------------------+
             | LZ77 알고리즘은, 중복된 문자열을 다른 문자로 교체해 중복된   |
             | 문자열을 제거하는 압축 알고리즘이다. 교체할 문자열이 시작하는 |
             | 위치보다 이전에 있던 중복 문자열까지의 '중복 문자열의 길이' 와|
             | '역방향 거리'의 짝으로 중복 문자열을 치환하는 것이다.        |
             | 압축된 데이터를 해제하기 위해서는, '역방향 거리'와 '중복 문자 |
             | 열의 길이', '압축이 안된 리터럴 문자'를 판별할 수 있어야 한다.|
             | 일반 문자열이 딱 1bytes임을 이용해서, 1bytes를 읽고 바이트  |
             | 경계를 무시하고 1bits를 더 읽는다. 총 9bits 정수를 읽어서   |
             | 값이 256보다 작으면, 다음 값이 리터럴 문자가 나오고, 클 경우 |
             | 다음 값이 중복 문자열 길이/역방향 거리의 짝이 나옴을 알 수   |
             | 있다는 것이다. 그리고 값이 256이면 압축 해제 과정이 마무리   |
             | 되었다는 '정지토큰'이기에 그대로 디코딩 과정을 끝내면 된다.  |
             | 단, LZ77을 통해 치환된 요소들을 허프만 코딩으로 허프만 코드로|
             | 치환시켜 압축하는 Deflate 알고리즘에서는 항상 9bits를 읽는게|
             | 아니다. 비트스트림으로부터 1bits씩 읽어들이면서 허프만 코드와|
             | 대응되는 값이 '압축이 안된 리터럴 문자 또는 중복 문자열의 길이|
             | '인지, '역방향 거리'인지를 파악해 나가기 때문이다.          |
             +-------------------------------------------------------+
             | 허프만 코딩은 중복으로 발생하는 심볼을, 발생빈도에 따라서    |
             | 더 적은 '허프만 코드( 절대 겹치지 않는 비접두사 코드 )'를   |
             | 부여해 압축하는 알고리즘이다. (e.g: A=1, B=01, C=001)     |
             | Deflate 사양에서 허프만 코딩의 '비접두사 코드'는 다음과     |
             | 같은 규칙을 따른다:                                     |
             | ①: 주어진 모든 허프만 코드들은 사전 순서대로 연속적인      |
             |     값들을 가진다. (같은 길이의 허프만 코드는, 늦게 등장한  |
             |     허프만 코드의 크기의 값이 더 커야한다.)               |
             | ②: 짧은 허프만 코드들이 사전 순서대로 긴 허프만 코드들보다 |
             |     먼저 등장한다.                                     |
             |                                                      |
             | ※여기서 사전 순이라는 것은, 말 그대로 첫번째부터 심볼들이   |
             |   A,B,C,D...식으로 연속적으로 구성되는 식이라는 것이다.※  |
             |                                                      |
             | 이 방식으로 압축된 데이터를 해제하기 위해서는, 이 비접두사  |
             | 코드들을 해독할 수 있는 '허프만 트리'가 있어야 한다. 그리고  |
             | 이 '허프만 트리'는 사용된 '비두사 코드'들의 코드 길이       |
             | 테이블을 통해 언제든지 구축이 가능하다.                    |
             | (코드 길이 테이블 또한 사전순으로 차례대로 구성되어 있다.)   |
             |                                                       |
             | 따라서 허프만 코딩으로 압축된 데이터가 있다면, 반드시 압축에 |
             | 사용된 코드 길이들이 담긴 '코드 길이 테이블', '허프만 트리' |
             | 를 구축할 수 있는 데이터 중 하나가 제공된다.               |
             +-------------------------------------------------------+*/


            /*------------------------------------------------------+
             |                    고정 허프만 코드                    |
             +-----------+------------------------------------------+
             |  INFO     |  사전에 정해진 허프만 트리를 사용해야 한다.   |
             |           |  rfc1951 문서에 허프만 트리를 구축할 수 있는 |
             |           |  테이블을 별도로 제공한다. 한 번 구축한 고정  |
             |           |  허프만 트리는 반복해서 사용하기 때문에, 블럭 |
             |           |  들을 처리하기전에 미리 구축해놓는다.        |
             |           |                                         |
             |           |  미리 구축해놓은 LZ77의 '리터럴/원본 문자열  |
             |           |  의 길이', '역방향 거리'를 압축하는데 사용한 |
             |           |  허프만 코딩을 해제하는데 필요한 고정 허프만 |
             |           |  트리로 곧바로 LZ77을 해독하면 된다...     |
             +-----------+-----------------------------------------+*/
            case(Zlib::BlockCompressionType::Fixed_Huffman): {

                Inflate_LZ77(bitStream, outInflateStream, fixed_literal_length_tree, fixed_distance_tree);
                break;
            }




            /*--------------------------------------------------------+
             |                    동적 허프만 코드                      |
             +-----------+--------------------------------------------+
             |  INFO     |  LZ77의 '리터럴/중복 문자열의 길이', '역방향 거 |
             |           |  리'의 코드 길이 테이블에서 같은 길이의 허프만 코|
             |           |  드가 많다면 압축 효율이 떨어질 것이다. 따라서   |
             |           |  반복 되는 코드 길이들을 특정 값으로 대체시키는  |
             |           |  것으로 압축률을 개선한다. 이에 대한 압축을 풀기 |
             |           |  위해선 아래의 방법을 통해서 압축된 코드 길이 테 |
             |           |  이블로부터 온전한 코드 길이 테이블을 얻어내야 한|
             |           |  다:                                       |
             |           |                                            |
             |           |  ①: 코드 길이 테이블에서 코드 길이값을 인덱스  |
             |           |      순으로 하나 꺼내서 확인한다.             |
             |           |                                            |
             |           |  ②: ①에서 확인한 코드 길이값이 (0-15)면 해당 |
             |           |      코드 값은 제대로 된 코드 길이 값이다.     |
             |           |                                            |
             |           |  ③: ①에서 확인한 코드 길이값이 (16)이면 이전에|
             |           |      확인한 온전한 코드 길이값이 3-6번 반복해서 |
             |           |      나타난 다는 것을 의미한다. 반복 횟수는 스트|
             |           |      림으로 부터 2bits 정수값을 추가로 읽는 것  |
             |           |      으로 알 수 있다.                        |
             |           |                                            |
             |           |  ④: ①에서 확인한 코드 길이값이 (17)이면 코드  |
             |           |      길이값 0이 3-10번 반복됨을 의미한다. 반복  |
             |           |      횟수는 스트림으로부터 3bits 정수값을 추가  |
             |           |      로 읽는 것으로 알 수 있다.               |
             |           |                                            |
             |           |  ⑤: ①에서 확인한 코드 길이값이 (18)이면 코드  |
             |           |      길이값 0이 11-138번 반복함을 의미한다.    |
             |           |      반복 횟수는 스트림으로부터 7bits 정수값을  |
             |           |      추가로 읽어들이는 것으로 알 수 있다.       |
             |           |                                            |
             |           |  ⑥: ①~⑤까지의 과정을 (HLIT+257) + (HDIST+1)|
             |           |      번 반복한다. 이는 LZ77의 '리터럴/중복 문자열|
             |           |      의 길이', '역방향 거리'의 온전한 코드 길이  |
             |           |      테이블들을 얻기 위함이다.                |
             |           |                                            |
             |           |  위의 방법으로 압축되어 있는 LZ77의 '리터럴/중복|
             |           |  문자열의 길이', '역방향 거리'의 코드 길이 테이블|
             |           |  은 압축률을 더욱 더 개선하기 위해서, 허프만 코딩|
             |           |  으로 또다시 압축한다. 이 허프만 코딩 압축을 해제|
             |           |  하기 위한 코드 길이 테이블이 또 앞에 붙는다는   |
             |           |  이야기다.                                  |
             |           |                                            |
             |           |  이 코드 길이 테이블은 총 원소 수가 19개다. 만약|
             |           |  이 테이블의 16,17번째 코드 길이와 대응되는 허프|
             |           |  만 코드로만 압축되어 있다면, 이 테이블에 쓰지도 |
             |           |  않는 17개의 데이터를 넣는 것은 낭비다.        |
             |           |                                            |
             |           |  따라서 각 원소들을 경험적으로 봤을 때, 가장 사용|
             |           |  될 확률이 높은 코드 길이가 앞으로 오도록 RFC에서|
             |           |  정의한 고정된 순서대로 코드 길이 테이블의 원소들 |
             |           |  의 순서 재배치하고, ( 기존의 사전순서대로라는 규 |
             |           |  을 무시하고 )쓰지 않는 값들의 코드 길이는       |
             |           |  0으로 취급하는 것으로 불필요한 데이터를 코드 길이|
             |           |  테이블에서 생략하여 압축 효율을 높인다.         |
             |           |                                             |
             |           |  예를 들어서, 가장 사용될 확률이 높은 허프만 코드 |
             |           |  의 인덱스는 차례대로 16,17,18,0 인데           |
             |           |  코드 길이 테이블의 원소가 4개가 있다면 해당 원소 |
             |           |  의 각 코드 길이값은 각각 16,17,18,0번째 인덱스 |
             |           |  허프만 코드 것이 되는거다.                   |
             |           |                                            |
             |           | LZ77의 '리터럴/원본 문자열의 길이'와 '역방향 거리|
             |           | 를 압축하는데 사용된 허프만 코딩을 해제하기 위한 |
             |           | 코드 길이 테이블을 압축하는데 사용된 허프만 코딩 |
             |           | 을 해제하기 위한 코드 길이 테이블은 (HCLEN + 4)|
             |           | 개의 원소를 가지고 있다. 그리고 해당 테이블의 원|
             |           | 소들은 3bits 정수값으로 (0-7)의 코드 길이값을  |
             |           | 나타낸다. 이 값들을 위에서 설명한대로 처리하자.  |
             +-----------+--------------------------------------------+
             |  HLIT(5)  | LZ77의 '원본 문자열의 길이'를 압축하는데 사용된 |
             |           | 허프만 코딩을 해제하기 위한 코드 길이 테이블의   |
             |           | 원소 수가 (HLIT + 257)개가 있음을 의미.       |
             +-----------+--------------------------------------------+
             |  HDIST(5) | LZ77의 '역방향 거리'를 압축하는데 사용된 허프만 |
             |           | 코딩을 해제하기 위한 코드 길이 테이블의 원소 수가|
             |           | (HDIST + 1)개가 있음을 의미.                 |
             +-----------+--------------------------------------------+
             |  HCLEN(4) | LZ77의 '원본 문자열의 길이', '역방향 거리'를    |
             |           | 압축하는데 사용된 허프만 코딩을 해제하기 위한    |
             |           | 코드 길이 테이블을 압축하는데 사용한 허프만 코딩  |
             |           | 을 해제하기 위한 코드 길이 테이블의 원소 수가    |
             |           | (HCLEN + 4)개가 있음을 의미. 이 코드 길이 테이블|
             |           | 의 원소는 3bits 정수값이다.                   |                     
             +-----------+---------------------------------------------+*/
            case(Zlib::BlockCompressionType::Dynamic_Huffman): {

                uint32_t hlit  = bitStream.ReadBits(5) + 257;
                uint32_t hdist = bitStream.ReadBits(5) + 1;
                uint32_t hclen = bitStream.ReadBits(4) + 4;


                /*-----------------------------------------------------------
                 *  LZ77의 '리터럴/원본 문자열의 길이', '역방향 거리'를 압축하는데
                 *  사용한 허프만 코딩을 해제하는데 필요한 코드 길이 테이블을 압축하는
                 *  데 사용한 허프만 코딩을 해제하는데 필요한 코드 길이 테이블을
                 *  읽어들이고, 이에 대한 허프만 트리를 구축한다.....
                 *------*/
                uint32_t code_len_code_len_table[19] = { 0, };

                for (uint32_t i = 0; i < hclen; i++) {
                    code_len_code_len_table[Zlib::CodeLengthCodesOrder[i]] = bitStream.ReadBits(3);
                }

                code_len_code_len_tree.Build_Dynamic(code_len_code_len_table, 19);
               


                /*---------------------------------------------------------
                 *  직전에 구축한 허프만 트리를 사용해, LZ77의 '리터럴/원본 문자
                 *  열의 길이', '역방향 거리'를 압축하는데 사용한 허프만 코딩을
                 *  해제하는데 필요한 코드 길이 테이블을 읽어들이고, 이에 대한
                 *  허프만 트리들을 구축한다....
                 *--------*/
                const uint32_t loopCount = (hlit + hdist);

                litLength_and_dist_code_len_table.clear();
                litLength_and_dist_code_len_table.reserve(loopCount);


                while (litLength_and_dist_code_len_table.size() < loopCount)
                {
                    const uint32_t symbol = code_len_code_len_tree.GetSymbol(bitStream);

                    /**코드 길이값에 대한 리터럴이다...**/
                    if (symbol <= 15) {
                        litLength_and_dist_code_len_table.push_back(symbol);
                    }

                    /**이전 코드 길이값을 3-6번 반복한다. 2bits를 추가로 읽는다..**/
                    else if (symbol == 16) {
                        const uint32_t loopCount = bitStream.ReadBits(2) + 3;
                        const uint32_t prevCodeLen = litLength_and_dist_code_len_table[litLength_and_dist_code_len_table.size() - 1];

                        for (uint32_t i = 0; i < loopCount; i++) {

                            litLength_and_dist_code_len_table.push_back(prevCodeLen);
                        }
                    }

                    /***코드 길이값 0을 3-10번 반복한다. 3bits를 추가로 읽는다...**/
                    else if (symbol == 17) {
                        const uint32_t loopCount = bitStream.ReadBits(3) + 3;

                        for (uint32_t i = 0; i < loopCount; i++) {

                            litLength_and_dist_code_len_table.push_back(0);
                        }
                    }

                    /***코드 길이값 0을 11-138번 반복한다. 7bits를 추가로 읽는다...**/
                    else if (symbol == 18) {
                        const uint32_t loopCount = bitStream.ReadBits(7) + 11;

                        for (uint32_t i = 0; i < loopCount; i++) {

                            litLength_and_dist_code_len_table.push_back(0);
                        }
                    }

                    else return ret;

                }

                literal_length_tree.Build_Dynamic(&litLength_and_dist_code_len_table[0], hlit);
                distance_tree.Build_Dynamic(&litLength_and_dist_code_len_table[hlit], hdist);


                /*--------------------------------------------------------
                 *   LZ77을 해제한다....
                 *------*/
                Inflate_LZ77(bitStream, outInflateStream,literal_length_tree, distance_tree);


                break;
            }

        }

    } while (!isFinalBlock); //마지막 블럭이 아니라면 다음 블럭으로 넘어간다...


    ret.Success = true;
    return ret;
}









/*============================================================================================================
 *   주어진 리터럴/원본 문자열 길이 코드의 허프만 트리와, 역방향 거리의 허프만 트리로 LZ77을 해독한다...
 *=======*/
void hyunwoo::Zlib::Inflate_LZ77(BitStream& bitStream, std::vector<uint8_t>& outInflateStream, const Zlib::HuffmanTree& literal_length_code_tree, const Zlib::HuffmanTree& distance_code_tree)
{
    /*******************************************************
     *   중지 코드가 등장할 때까지, 디코딩을 진행한다...
     *******/
    while (true) 
    {
        uint32_t symbol = literal_length_code_tree.GetSymbol(bitStream);

        /*--------------------------------------
         *  리터럴 문자인가? 맞다면 심볼을
         *  결과 스트림에 넣는다...
         *----*/
        if (symbol < 256) {
            outInflateStream.push_back(symbol);
        }


        /*--------------------------------------
         *  중지 코드인가? 맞다면 함수를 종료한다..
         *------*/
        else if (symbol==256) {
            return;
        }

        

        /*------------------------------------------
         *  길이 코드인가? 맞다면 역방향 거리코드도
         *  읽어 들이고, 원본값을 구한 후 결과 스트림에
         *  넣는다...
         *-----*/
        else {

            //길이 코드를 읽어들인다....
            const uint32_t len_idx        = (symbol - 257);
            const uint32_t len_extra_bits = bitStream.ReadBits(Zlib::LZ77::LengthCodeTable[len_idx].extra_bits);
            const uint32_t length         = (Zlib::LZ77::LengthCodeTable[len_idx].length_start + len_extra_bits);

            //역방향 거리 코드를 읽어들인다...
            const uint32_t dst_idx        = distance_code_tree.GetSymbol(bitStream);
            const uint32_t dst_extra_bits = bitStream.ReadBits(Zlib::LZ77::DistCodeTable[dst_idx].extra_bits);
            const uint32_t distance       = (Zlib::LZ77::DistCodeTable[dst_idx].length_start + dst_extra_bits);


            //원본값으로 해독한다...
            const uint32_t originStrIdx = (outInflateStream.size() - distance);
            for (uint32_t i = 0; i < length; i++) {

                outInflateStream.push_back(outInflateStream[originStrIdx+i]);
            }

        }


    }
}
