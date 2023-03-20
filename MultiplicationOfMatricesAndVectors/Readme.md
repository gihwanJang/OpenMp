# Lab2-1 Matrix X Vector

## Matrix X Vector

- 내용
    - 행렬과 벡터 곱을 구하는 프로그램 작성
#
- 실습 목표
    - Parallel construct, work-sharing construct 활용하기
    - 병렬처리에서 Read와 Write operation의 차이 알기
    - 병렬처리의 힘을 느껴 보기
#

- 입력
    - Matrix A의 크기 (row, column), Vector b의 길이 (line 10 - 11)
    - 입력 받은 크기에 맞추어 A,b 가 생성됨
        - Float type, 값은 random
#

- 출력
    - 결과 검증 결과
        - 직렬처리 결과와 병렬처리 결과 비교
    - 직렬처리 시간
    - 병렬처리 시간
#

- 작성 코드
    1. Serial algorithm (line 49 - 51)
    2. Parallel algorithm (line 65 - 71)
    3. 결과 검증 코드 (line 87 - 92)
        - 결과가 정상 일 시, isCorrect flag를 true로 변경