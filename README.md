# 👔 Job Seeker Recommender

기업이 중요하게 보는 항목에 **가중치를 매겨 구직자 10만 명의 점수를 계산하고, 퀵정렬로 상위 지원자를 추천**하는 C++/MFC 데스크톱 프로그램입니다.
**피벗 선택 전략에 따른 퀵정렬 성능 차이**를 실험했습니다.

`25-1` · 팀 프로젝트 (3인)

## 기능

- 구직자 9개 항목(경력, 학력, 자격증, 시험, 기술 스택, 프로젝트, 토익, 논문, 추천인)으로 종합 점수 계산
- 기업 성향별 가중치 프리셋: **밸런스형 / 실무중심형 / 잠재력중심형** (잠재력형은 경력에 음의 가중치)
- 최대 경력 연수 필터
- 퀵정렬(내림차순) 후 상위 10명 표시, 정렬 시간 측정, 전체 결과 CSV 저장

## 피벗 전략 실험

10만 명 데이터, 3가지 가중치 프리셋에서 **무작위 입력**과 **이미 정렬된 입력**(최악의 경우)을 비교했습니다. → [`benchmark/`](benchmark/)

| 피벗 전략 | 무작위 입력 | 정렬된 입력 |
|---|---|---|
| 첫 번째 원소 | 11~15 ms | **880 ~ 5,230 ms** |
| Median-of-Three | 12~14 ms | **4~6 ms** |
| 평균 점수 | 11~14 ms | 5~7 ms |

- 무작위 데이터에서는 차이가 거의 없지만, **정렬된(또는 거의 정렬된) 입력에서 첫 원소 피벗은 O(n²)으로 붕괴**해 최대 약 1,000배 느려짐
- Median-of-Three, 평균 피벗은 입력 순서와 관계없이 안정적

> 정리하며 발견한 점: 원래 MFC 코드의 Median-of-Three는 세 값을 정렬한 뒤 **최댓값**(`arr[low]`)을 피벗으로 써서 실제로는 개선 효과가 없었습니다(정렬된 입력 1,900ms). 중간값(`arr[mid]`)을 쓰도록 수정했고, 벤치마크에 원본 구현도 함께 남겨 비교했습니다.

## 구조

```text
.
├── mfc_app/              # MFC 다이얼로그 앱 (Visual Studio, C++)
│   ├── seeker_MFCDlg.cpp #   점수 계산, 퀵정렬 2종, UI, CSV 입출력
│   └── ...
├── prototype/            # 초기 콘솔 버전 (고정 가중치)
├── tools/
│   ├── data_add.cpp      # 랜덤 구직자 데이터 생성/추가
│   └── dup_check.cpp     # 속성 조합 중복 데이터 점검
├── benchmark/
│   ├── pivot_benchmark.cpp # 피벗 전략 비교 (표준 C++, 크로스플랫폼)
│   └── result.txt
├── data/
│   ├── job_seekers.csv   # 더미 구직자 10만 명
│   └── mysql_prototype.sql # 초기 MySQL 프로토타입
└── docs/                 # 발표 자료, MySQL 프로토타입 화면
```

## 실행

**MFC 앱 (Windows)**: `mfc_app/seeker_MFC.sln`을 Visual Studio로 열어 빌드 → 실행 폴더에 `data/job_seekers.csv`를 복사 → 프리셋 선택 후 출력

**벤치마크**

```bash
cd benchmark
g++ -O2 -std=c++17 pivot_benchmark.cpp -o pivot_benchmark
./pivot_benchmark ../data/job_seekers.csv
```

> 정렬된 입력에서 첫 원소 피벗은 재귀가 깊어지므로 스택 크기가 작으면 `ulimit -s unlimited`가 필요할 수 있습니다.

## 진행 방식

워터폴 + 프로토타이핑: MySQL 테이블·더미 데이터로 프로토타입 → C 콘솔 버전 → MFC GUI + 정렬 최적화

## 기술 스택

C++ · MFC (Visual Studio) · MySQL
