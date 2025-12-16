# hyunwoo's RenderEngine

## Overview
<table><tr><td>
<img src="https://github.com/mamajuk/hyunwoos-RenderEngine/blob/main/Readmy_Data/Readmy_play.gif?raw=true">
</td></tr></table>

```hyunwoo's RenderEngine```은 게임 개발에 필요한 **게임 수학** 및 **3D 랜더링** 을 이해하는 것을 목표로, 외부 라이브러리 없이 직접 개발한 CPU 기반 3D 랜더러입니다. 개발 초기에는 "게임 엔진" 개발로의 확장까지 고려하며 설계했지만, 점점 원래 목표에서 벗어난 과도한 설계를 하고 있다는 생각이 들었습니다. 때문에 개발의 중간시점부터 오직 ***3D모델링 랜더링*** 및 ***스키닝 애니메이션의 구현*** 에만 충실하도록 설계 범위를 최소화했습니다. (e.g ```GUI``` 및 ```Multi ViewPort``` 등의 미구현됨. ) 

일단 목표는 달성했기에 당장은 확장할 예정은 없지만, 추후 ```.fbx```, ```.obj``` 와 같은 파일의 임포트를 지원하거나, ```Hiererchy Window``` 및 ```Multi ViewPort```를 천천히 구현해볼 예정입니다.

## How to use
```hyunwoo-RenderEngine/x64/Release/hyunwoos-RenderEngine.exe``` 를 실행합니다.
