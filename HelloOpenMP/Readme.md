# Lab 1-1. Hello OpenMP

- 사용하는 스레드 수 만큼 "Hello OpenMP"를 출력하는 프로그램을 작성하기

- 목표
    - OpenMP 컴파일 환경 구축하기
    - OpenMP와 인사하기!

## OpenMP 기초
- Parallel region 만들기
- 사용할 스레드의 수 지정
- 기초 함수
    - int omp_get_num_threads() -> 전체 스레드의 수 반환
    - int omp_get_thread_num() -> 자신의 스레드 ID 반환