# Lab3 Histogram

## Histogram

- 내용
    - Serialversion 구현
    - version 1,2,3 순으로 구현
        - v_1 : critical section에 대하여 lock을 걸고 진행
        - v_2 : 각 구간에 대하여 병렬 처리 후 각 처리 결과를 lock을 걸고 진행
        - v_3 : 각 구간에 대하여 병렬 처리후 각 처리 결과에 대하여 다시 병렬 처리
#

- 입력 : data_size, thread_num, bin_num
    - data_size : 입력 데이터의 갯수
    - thread_num : 사용 스레드의 갯수
    - bin_num : 히스트그램 구간의 갯수
#

- 출력
    - Histogram bins
#

- 작성 코드
    1. Serial algorithm
    2. Parallel algorithm
    3. 두 알고리즘의 성능 비교