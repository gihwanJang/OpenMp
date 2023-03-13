# Lab 1-2. EncryptedImage

## 암호화된 이미지 복원하기

```
목표
    - 병렬처리의 개념 이해 하기
    - 일 분배 및 스레드의 개념 잡기
    - 병렬처리의 유용성 확인하기
```
```
수행 방법
    - 아래의 함수들만 이용하여 해결하기
        - #pragma omp parallel num_threads(n)
        - int omp_get_num_threads()
        - in omp_get_thread_num()
```
```
입력 파일 : ./InputData/InputA.data, ./InputData/InputB.data
    - 암호화된 이미지 파일
        - ImageSize.txt에 이미지 크기가 있음
```
```
수행할 폴더 : ./Lab1-2
```
```
수행할 파일 : ./Lab1-2/main.cpp
```
```
작성 내용
    - 병렬처리 코드(line 49~60)
        - 결과는 parallelC[]에 저장 할 것
```
```
수행 절차
    - cd ./OpenMP/EncryptedImage/Lab1-2
    - ./main.out ../InputData/inputA.out ../InputData/inputB.out 7680 4320 out.data
```
```
복원 이미지 확인
    - window : Utility Programs 내의 Vierwer.exe [데이터파일][너비][높이][저장할 파일]
    - linux, mac : ./OpenMP/EncryptedImage/Viewer/Viewer.out 사용
        - ./Viewr.out ../Lab1-2/out.data 7680 4320 out.png
```