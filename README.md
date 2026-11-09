# Job Seeker Recommender

기업 성향에 따라 가중치를 정해 구직자 10만 명의 점수를 계산하고, 퀵정렬로 상위 지원자를 보여주는 C++/MFC 데스크톱 프로그램입니다.

25-1, 팀 프로젝트 (3인)

## 기능

- 구직자 9개 항목(경력, 학력, 자격증, 시험, 기술 스택, 프로젝트, 토익, 논문, 추천인)으로 점수 계산
- 가중치 프리셋: 밸런스형, 실무중심형, 잠재력중심형 (잠재력중심형은 경력 가중치가 음수)
- 최대 경력 연수 필터
- 퀵정렬(내림차순) 후 상위 10명 표시, 정렬 시간 표시, 전체 결과 CSV 저장
- 퀵정렬은 첫 번째 원소 피벗과 Median-of-Three 피벗 두 가지를 구현

## 구조

```text
.
├── mfc_app/              # MFC 다이얼로그 앱 (Visual Studio)
├── prototype/            # 콘솔 버전
├── tools/
│   ├── data_add.cpp      # 랜덤 구직자 데이터 생성/추가
│   └── dup_check.cpp     # 속성 조합 중복 확인
├── benchmark/            # 피벗 방식별 정렬 시간 비교 (정리하면서 추가)
├── data/
│   ├── job_seekers.csv   # 가상 구직자 10만 명
│   └── mysql_prototype.sql
└── docs/                 # 발표 자료, MySQL 프로토타입 화면
```

## 실행

MFC 앱: `mfc_app/seeker_MFC.sln`을 Visual Studio로 빌드하고, 실행 폴더에 `data/job_seekers.csv`를 복사한 뒤 프리셋을 고르고 출력 버튼을 누릅니다.

벤치마크

```bash
cd benchmark
g++ -O2 -std=c++17 pivot_benchmark.cpp -o pivot_benchmark
./pivot_benchmark ../data/job_seekers.csv
```

## 정리 내역

포트폴리오로 올리면서 다음을 정리했습니다.

- Median-of-Three 함수가 세 값을 정렬한 뒤 가장 큰 값(`arr[low]`)을 피벗으로 쓰고 있어, 중간값(`arr[mid]`)을 쓰도록 고쳤습니다.
- 에러 메시지 버퍼 크기 인자 오류를 고치고, 주석 처리돼 있던 미사용 코드를 지웠습니다.
- `benchmark/`를 추가했습니다. 같은 데이터와 가중치로 피벗 방식별 정렬 시간을 잽니다. 측정 결과는 `benchmark/result.txt`에 있습니다. 무작위 순서에서는 세 방식 모두 10ms대였고, 이미 정렬된 입력에서는 첫 번째 원소 피벗이 0.9~5.2초, 수정한 Median-of-Three가 4~6ms였습니다.

## 기술 스택

C++, MFC (Visual Studio), MySQL

## 라이선스

팀 프로젝트 결과물로 저작권은 팀원 공동에 있습니다. 포트폴리오 열람 목적으로 공개하며 별도의 사용 허가는 없습니다. 구직자 데이터는 프로그램으로 만든 가상 데이터입니다.
