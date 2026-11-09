// 피벗 선택 전략별 퀵정렬 성능 비교 (MFC 앱과 동일한 정렬·가중치 로직, 표준 C++)
//
// 빌드: g++ -O2 -std=c++17 pivot_benchmark.cpp -o pivot_benchmark
// 실행: ./pivot_benchmark ../data/job_seekers.csv
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct JobSeeker {
    std::string name;
    int f[9];  // 경력, 학력, 자격증, 시험, 기술스택, 프로젝트, 토익, 논문, 추천인
    double score;
};

struct Preset {
    const char* name;
    double w[9];
};

// MFC 앱의 가중치 프리셋
const Preset PRESETS[] = {
    {"밸런스형", {0.07, 0.20, 0.07, 0.04, 0.07, 0.07, 0.004, 0.07, 0.04}},
    {"실무중심형", {0.10, 0.1875, 0.0875, 0.0475, 0.12, 0.10, 0.006, 0.0475, 0.01}},
    {"잠재력중심형", {-0.15, 0.30, 0.10, 0.10, 0.10, 0.05, 0.01, 0.15, 0.05}},
};

using SortFn = std::function<void(std::vector<JobSeeker>&, int, int)>;

int partition(std::vector<JobSeeker>& a, int low, int high, double pivot, int& i_out) {
    int i = low, j = high;
    while (i <= j) {
        while (a[i].score > pivot) i++;
        while (a[j].score < pivot) j--;
        if (i <= j) std::swap(a[i++], a[j--]);
    }
    i_out = i;
    return j;
}

// 1) 첫 번째 원소를 피벗으로 (내림차순)
void quickSortFirst(std::vector<JobSeeker>& a, int low, int high) {
    if (low >= high) return;
    int i;
    int j = partition(a, low, high, a[low].score, i);
    if (low < j) quickSortFirst(a, low, j);
    if (i < high) quickSortFirst(a, i, high);
}

// 2) Median-of-Three: 처음/중간/끝 중 중간값을 피벗으로
void quickSortMedian3(std::vector<JobSeeker>& a, int low, int high) {
    if (low >= high) return;
    int mid = low + (high - low) / 2;
    if (a[low].score < a[mid].score) std::swap(a[low], a[mid]);
    if (a[low].score < a[high].score) std::swap(a[low], a[high]);
    if (a[mid].score < a[high].score) std::swap(a[mid], a[high]);
    int i;
    int j = partition(a, low, high, a[mid].score, i);
    if (low < j) quickSortMedian3(a, low, j);
    if (i < high) quickSortMedian3(a, i, high);
}

// 2-1) 원본 MFC 구현: 세 값을 정렬한 뒤 a[low](=셋 중 최댓값)를 피벗으로 사용하던 버전
void quickSortMedian3Original(std::vector<JobSeeker>& a, int low, int high) {
    if (low >= high) return;
    int mid = low + (high - low) / 2;
    if (a[low].score < a[mid].score) std::swap(a[low], a[mid]);
    if (a[low].score < a[high].score) std::swap(a[low], a[high]);
    if (a[mid].score < a[high].score) std::swap(a[mid], a[high]);
    int i;
    int j = partition(a, low, high, a[low].score, i);
    if (low < j) quickSortMedian3Original(a, low, j);
    if (i < high) quickSortMedian3Original(a, i, high);
}

// 3) 평균 점수를 피벗으로
void quickSortMean(std::vector<JobSeeker>& a, int low, int high) {
    if (low >= high) return;
    double sum = 0, mn = a[low].score, mx = a[low].score;
    for (int k = low; k <= high; k++) {
        sum += a[k].score;
        mn = std::min(mn, a[k].score);
        mx = std::max(mx, a[k].score);
    }
    if (mn == mx) return;  // 모두 같은 점수 → 이미 정렬됨
    double pivot = std::clamp(sum / (high - low + 1), mn, mx);  // 부동소수 오차로 범위를 벗어나지 않게
    int i;
    int j = partition(a, low, high, pivot, i);
    if (low < j) quickSortMean(a, low, j);
    if (i < high) quickSortMean(a, i, high);
}

std::vector<JobSeeker> load(const std::string& path) {
    std::ifstream in(path);
    std::vector<JobSeeker> v;
    std::string line;
    std::getline(in, line);  // header
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string tok;
        JobSeeker s{};
        std::getline(ss, tok, ',');  // id
        std::getline(ss, s.name, ',');
        for (int& x : s.f) {
            std::getline(ss, tok, ',');
            x = std::stoi(tok);
        }
        v.push_back(s);
    }
    return v;
}

double timeMs(std::vector<JobSeeker> data, const SortFn& fn, bool& ok) {
    auto t0 = std::chrono::high_resolution_clock::now();
    fn(data, 0, (int)data.size() - 1);
    auto t1 = std::chrono::high_resolution_clock::now();
    ok = std::is_sorted(data.begin(), data.end(),
                        [](const JobSeeker& x, const JobSeeker& y) { return x.score > y.score; });
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main(int argc, char** argv) {
    std::string path = argc > 1 ? argv[1] : "../data/job_seekers.csv";
    auto base = load(path);
    if (base.empty()) {
        std::cerr << "데이터를 읽을 수 없습니다: " << path << "\n";
        return 1;
    }
    std::printf("데이터: %zu명\n\n", base.size());

    const std::pair<const char*, SortFn> sorts[] = {
        {"첫 원소 피벗", quickSortFirst}, {"Median-of-3(원본)", quickSortMedian3Original},
        {"Median-of-3", quickSortMedian3}, {"평균 피벗", quickSortMean}};

    for (const auto& p : PRESETS) {
        auto data = base;
        for (auto& s : data) {
            s.score = 0;
            for (int k = 0; k < 9; k++) s.score += s.f[k] * p.w[k];
        }
        auto sorted = data;
        quickSortMedian3(sorted, 0, (int)sorted.size() - 1);

        std::printf("[%s] 1위: %s (%.3f)\n", p.name, sorted[0].name.c_str(), sorted[0].score);
        std::printf("  %-18s %12s %12s\n", "피벗 전략", "무작위 입력", "정렬된 입력");
        for (const auto& [name, fn] : sorts) {
            bool ok1, ok2;
            double r = timeMs(data, fn, ok1);
            double s = timeMs(sorted, fn, ok2);
            std::printf("  %-18s %9.2f ms %9.2f ms %s\n", name, r, s, (ok1 && ok2) ? "" : "(정렬 오류)");
        }
        std::printf("\n");
    }
    return 0;
}
