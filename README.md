# hyunwoo's RenderEngine

## Overview
<table><tr><td>
<img src="https://github.com/mamajuk/hyunwoos-RenderEngine/blob/main/Readmy_Data/Readmy_play.gif?raw=true">
</td></tr></table>

```hyunwoo's RenderEngine```은 게임 개발에 필요한 **게임 수학** 및 **3D 랜더링** 을 이해하는 것을 목표로, 외부 라이브러리 없이 직접 개발한 CPU 기반 3D 랜더러입니다. 개발 초기에는 "게임 엔진" 개발로의 확장까지 고려하며 설계했지만, 점점 원래 목표에서 벗어난 과도한 설계를 하고 있다는 생각이 들었습니다. 

때문에 개발의 중간시점부터 오직 ***3D모델링 랜더링*** 및 ***스키닝 애니메이션의 구현*** 에만 충실하도록 설계 범위를 최소화했습니다. 일단 목표는 달성했기에 당분간은 확장할 예정은 없지만, 추후 ```.pmx```, ```.vmd```의 모프/카메라 모션을 구현하거나, ```.fbx```, ```.obj``` 와 같은 파일의 임포트를 지원하거나, ```Hiererchy Window``` 및 ```Multi ViewPort```등을 천천히 구현해볼 예정입니다.

마지막으로 좋은 ```pmx``` 모델링 및 ```vmd``` 모션 작업물을 공유해주신 [Hoyoverse](https://www.mihoyo.com/), [tweekcrystal](http://tweekcrystal.deviantart.com/), [PiT_shan](https://bowlroll.net/file/33936). 그리고 ```pmx```, ```vmd```, ```png``` **file structure** 글을 공유해주신 [felixjones](https://gist.github.com/felixjones/f8a06bd48f9da9a4539f), [Nuthouse01](https://github.com/Nuthouse01/PMX-VMD-Scripting-Tools?utm_source=chatgpt.com), [pyokagan](https://pyokagan.name/blog/2019-10-14-png/). 특히 ```이득우의 게임수학``` 이라는 좋은 책을 써주신  [이득우 교수님](https://www.youtube.com/channel/UCt_fa1d5aHEl69_Bll0w1Ww/videos)과 개발하면서 많은 조언을 해준 [teumal](https://github.com/teumal)에게 정말 큰 감사를 드립니다.

## Notes
●```hyunwoo's RenderEngine```은 기본적으로 왼손좌표계와, 열기반 행렬을 사용합니다. 그리고 실제 수학 연산과 비슷하게 코드를 작성할 수 있도록, ```Matrix4x4::operator*(const Matrix4x4& rhs)```, ```Quaternion::operator*(const Quaternion& rhs)``` 등의 연산자 오버로딩은 우측에 있는 변환이 먼저 적용됩니다. 

●```Quaternion``` 의 회전 합성은 오른손 좌표계 규약을 따릅니다. 다만 ```Quaternion::AngleAxis()``` 는 내부적으로 사용자가 카타시안(Cartesian) 좌표계에서의 벡터 회전(active rotation)과 동일한 감각으로 사용할 수 있도록 인자로 받은 각도를 반전시켜 처리합니다. (e.g 30º -> -30º)

●```pmx``` 모델링은 ```IK bone```과 ```display bone```( 실제 랜더링되는 스킨이 영향받는 본 )이 따로 존재하며, IK 데이터의 각 링크본들은 IK 본을 사용합니다. 따라서 ```hyunwoo's RenderEngine```은 각 링크본에서 가장 가까운 다른 본을 찾아 교체하여, IK 적용시 디스플레이 본이 움직이도록 했습니다. 이는 ```pmx``` 모델링의 바인딩 포즈에서 display bone과 ik bone가 서로 비슷한 위치에 있다는 것을 가정하여 적용한 것이기 때문에, 일부 ```pmx``` 모델링에 IK를 적용할 경우 본이 뒤틀리는 경우가 있을 수 있습니다.

●```pmxImporter::Import()```를 통해 ```.pmx``` 모델링 파일을 로드할 때, 기본적으로 그 모델링에서 사용되는 모든 ```.png``` 확장명의 텍스쳐 파일을 읽어들이려고 시도합니다. 이 과정에서 ```.pmx``` 모델링이 요구하는 모든 텍스쳐 파일을 읽어들이지 않으면 실패하기 때문에, 사용자는 Import할 ```.pmx``` 모델링에서 사용하는 모든 텍스쳐 파일을 ```.png``` 확장명으로 미리 변환해놓아야 합니다.

●최종 개발한 각종 모듈들을 활용하여 구현된 컨텐츠의 소스는 ```hyunwoo-RenderEngine/RendererApp.h```,  ```hyunwoo-RenderEngine/RendererApp.cpp```를 통해 확인할 수 있으며, 빌드된 프로그램은 ```hyunwoo-RenderEngine/x64/Release/hyunwoos-RenderEngine.exe``` 를 실행하면 됩니다.


## How to Load Models and Animations
●원하는 ```.pmx``` 파일을 ```hyunwoo-s-RenderEngine.exe``` 클라이언트 창에 드래그 앤 드롭하여 3D 모델링을 읽어들입니다.<br>
●원하는 ```.vmd``` 파일을 ```hyunwoo-s-RenderEngine.exe``` 클라이언트 창에 드래그 앤 드롭하여 애니메이션 클립을 읽어들입니다.<br>( 먼저 모델링을 불러와야 정상적으로 로드됩니다. )<br>

## Controls
●애니메이션 재생/일시중지: ```P```<br>
●와이어 프레임 모드 on/off: ```1```<br>
●백 페이스 컬링 on/off: ```2```<br>
●IK 사용 on/off: ```3```
●조작 transform 대상을 ***AnimateMesh transform*** 로 전환: ```4```<br>
●조작 transform 대상을 ***Camera transform*** 로 전환: ```5```<br>
●모델링의 본 표시: ```6```<br>( 표시된 본을 클릭하여 조작 transform 대상을 해당 본 transform으로 전환할 수 있습니다.)<br>( 표시된 본이 클릭된 상태에서 ```Del```키를 눌러 해당 본 transform을 삭제할 수 있습니다..)<br>
●카메라의 FOV 수치 조절: ```NUMPAD 6``` ~ ```NUMPAD 9```<br>
●현재 선택된 조작 transform의 X축 이동: ```Left``` ~ ```Right```<br>
●현재 선택된 조작 transform의 Y축 이동: ```Down``` ~ ```Up```<br>
●현재 선택된 조작 transform의 Z축 이동: ```NUMPAD 2``` ~ ```NUMPAD 8```<br>
●현재 선택된 조작 transform의 yaw 회전: ```A``` ~ ```D```<br>
●현재 선택된 조작 transform의 pitch 회전: ```S``` ~ ```W```<br>
●현재 선택된 조작 transform의 roll 회전: ```Q``` ~ ```E```<br>
●0.2배속 모드: ```Space~```
