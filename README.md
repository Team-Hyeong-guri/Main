# 3차원 의료 서비스
openGL 기반 3차원 의료 서비스

[고광연](https://github.com/KoKwangYeon)

[이기현](https://github.com/looloolalaa)

[최승훈](https://github.com/owvwo)

[최형진](https://github.com/Dalgoon)


### Architecture

openGL / Gnuplot / imgui

------

## 코드 스타일

- 디렉토리 구성

  - FragmentShader 및 VertexShader 텍스트 파일을 2개씩 갖는다.
  - main.cpp 파일이 주 소스이다.
  - imgui 디렉토리 내 GUI 소스이다.


- 실행법

  - main 내 변수를 활용하여 비교하고 싶은 두 환자의 데이터.obj 입력
  
  
  ```markdown
    string filename = " ";
    string filename2 = " ";
  ```

- Mouse Input

  - LEFT BUTTON CLICK && MOVE : 화면 회전

  - Wheel : 화면 확대 및 축소

- KeyBoard Input

  - q,a : x축 회전
  - w,s : y축 회전
  - e,d ; z축 회전
  - / 선택 영역 지정
  - . 선택 영역 지정 후 이동

- 기본 활용
  - 3차원 메쉬 모델을 통해 이산 곡률에 근거하여 각 vertex들의 점의 색을 지정함.
