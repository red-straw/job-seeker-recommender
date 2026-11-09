#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <cstdio>
#include <algorithm>
#include <limits> // std::numeric_limits 사용

// JobSeeker 구조체는 이전과 동일합니다.
struct JobSeeker {
    int id;
    char name[64];
    int experience_years;
    int education_level;
    int certifications;
    int test_score;
    int tech_stack_count;
    int project_experience;
    int toeic_score;
    int paper_publications;
    int has_recommender;

    JobSeeker() : id(0), experience_years(0), education_level(0), certifications(0),
        test_score(0), tech_stack_count(0), project_experience(0),
        toeic_score(0), paper_publications(0), has_recommender(0) {
        name[0] = '\0';
    }
};

// --- 랜덤 데이터 생성 도구 및 함수 ---
std::random_device rd;
std::mt19937 gen(rd());

// 각 필드에 대한 균등 분포 정의 (기존과 동일)
std::uniform_int_distribution<> dist_exp(0, 20);
std::uniform_int_distribution<> dist_edu(1, 5);
std::uniform_int_distribution<> dist_cert(0, 5);
std::uniform_int_distribution<> dist_test_grade(1, 9);
std::uniform_int_distribution<> dist_tech(0, 5);
std::uniform_int_distribution<> dist_proj(0, 10);
std::uniform_int_distribution<> dist_toeic_grade(1, 9);
std::uniform_int_distribution<> dist_paper(0, 5);
std::uniform_int_distribution<> dist_rec(0, 1);

// 'zaaa'부터 시작하여 다음 알파벳 조합 이름을 생성하는 함수
std::string generateNextName(int index) {
    std::string base = "aaaa"; // 임시로 'aaaa'로 초기화 (계산 후 'zaaa'부터 시작)

    // 각 자릿수에 대한 인덱스를 계산하여 알파벳으로 변환합니다.
    // 'a' (아스키 97)부터 시작합니다.
    int i0 = index % 26; // 마지막 문자 (a-z)
    index /= 26;
    int i1 = index % 26; // 세 번째 문자 (a-z)
    index /= 26;
    int i2 = index % 26; // 두 번째 문자 (a-z)
    index /= 26;
    int i3 = index % 26; // 첫 번째 문자 (a-z)

    base[3] = 'a' + i0;
    base[2] = 'a' + i1;
    base[1] = 'a' + i2;
    // 첫 번째 자리는 'z'로 고정됩니다.
    // 참고: 10,000개의 데이터로는 'zaaa'부터 'zzzz'까지의 범위를 벗어나지 않습니다.
    // 26*26*26 = 17576 이므로, zaaa부터 zzzz까지는 충분히 커버 가능합니다.
    base[0] = 'z';

    return base;
}

// 하나의 랜덤 JobSeeker 데이터를 생성하는 함수 (기존과 동일)
JobSeeker generateRandomJobSeeker(int current_data_index, int start_id_from) {
    JobSeeker js;
    js.id = start_id_from + current_data_index;

    std::string generated_name = generateNextName(current_data_index);
    strncpy_s(js.name, sizeof(js.name), generated_name.c_str(), _TRUNCATE);

    js.experience_years = dist_exp(gen);
    js.education_level = dist_edu(gen);
    js.certifications = dist_cert(gen);
    js.test_score = dist_test_grade(gen);
    js.tech_stack_count = dist_tech(gen);
    js.project_experience = dist_proj(gen);
    js.toeic_score = dist_toeic_grade(gen);
    js.paper_publications = dist_paper(gen);
    js.has_recommender = dist_rec(gen);

    return js;
}

// CSV 파일에서 구직자 데이터를 읽어오는 함수 (기존과 동일)
std::vector<JobSeeker> readJobSeekersFromCsv(const std::string& filename) {
    std::vector<JobSeeker> seekers;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "정보: 파일 '" << filename << "'을(를) 찾을 수 없습니다. 새로운 데이터를 생성합니다." << std::endl;
        return seekers;
    }

    std::string line;
    std::getline(file, line); // 헤더 라인 건너뛰기

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        JobSeeker js;

        if (!std::getline(ss, token, ',')) { std::cerr << "경고: ID 필드 누락 - 라인: '" << line << "'" << std::endl; continue; }
        try { js.id = std::stoi(token); }
        catch (const std::exception& e) { std::cerr << "경고: ID 파싱 오류 - 라인: '" << line << "', 오류: " << e.what() << std::endl; continue; }

        if (!std::getline(ss, token, ',')) { std::cerr << "경고: 이름 필드 누락 - 라인: '" << line << "'" << std::endl; continue; }
        strncpy_s(js.name, sizeof(js.name), token.c_str(), _TRUNCATE);

        try {
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: experience_years"); js.experience_years = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: education_level"); js.education_level = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: certifications"); js.certifications = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: test_score_grade"); js.test_score = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: tech_stack_count"); js.tech_stack_count = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: project_experience"); js.project_experience = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: toeic_score_grade"); js.toeic_score = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: paper_publications"); js.paper_publications = std::stoi(token);
            if (!std::getline(ss, token, ',')) throw std::runtime_error("파싱 오류: has_recommender"); js.has_recommender = std::stoi(token);

            // Score 필드가 있을 수 있으나, 여기서는 읽지 않고 버립니다.
            std::string remaining_token;
            std::getline(ss, remaining_token);

            seekers.push_back(js);
        }
        catch (const std::exception& e) {
            std::cerr << "경고: CSV 파싱 중 오류 발생 - 라인: '" << line << "', 오류: " << e.what() << std::endl;
        }
    }
    file.close();
    return seekers;
}

// 구직자 데이터를 CSV 파일로 쓰는 함수 (기존과 동일)
void writeJobSeekersToCsv(const std::string& filename, const std::vector<JobSeeker>& seekers) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "오류: 파일 '" << filename << "'을(를) 저장할 수 없습니다." << std::endl;
        return;
    }

    // CSV 헤더 작성 (score 컬럼 제외)
    file << "id,name,experience_years,education_level,certifications,test_score_grade,tech_stack_count,project_experience,toeic_score_grade,paper_publications,has_recommender\n";

    for (const auto& js : seekers) {
        file << js.id << ","
            << js.name << ","
            << js.experience_years << ","
            << js.education_level << ","
            << js.certifications << ","
            << js.test_score << ","
            << js.tech_stack_count << ","
            << js.project_experience << ","
            << js.toeic_score << ","
            << js.paper_publications << ","
            << js.has_recommender << "\n";
    }
    file.close();
    std::cout << "저장 완료: " << filename << std::endl;
}

// --- 메인 함수 ---
int main() {
    const std::string input_file = "sorted_job_seekers.csv"; // 기존 10만 명 데이터 파일
    const std::string output_file = "new_job_seekers.csv";   // 새로운 1만 명 추가 데이터가 저장될 파일
    const int num_random_data_to_add = 10000; // 새로 추가할 랜덤 데이터의 수 (1만 명)

    std::vector<JobSeeker> all_seekers;

    // 1. sorted_job_seekers.csv 파일을 읽어옵니다.
    all_seekers = readJobSeekersFromCsv(input_file);

    if (all_seekers.empty()) {
        std::cout << "경고: 기존 파일 '" << input_file << "'을(를) 찾을 수 없습니다. 새 데이터만 생성하여 '" << output_file << "'에 저장합니다." << std::endl;
    }
    else {
        std::cout << "기존 데이터 " << all_seekers.size() << "개 읽음." << std::endl;
    }

    // 2. 새로운 랜덤 데이터의 ID 시작점을 결정합니다.
    int next_id_start = 1;
    if (!all_seekers.empty()) {
        auto max_id_seeker = std::max_element(all_seekers.begin(), all_seekers.end(),
            [](const JobSeeker& a, const JobSeeker& b) {
                return a.id < b.id;
            });
        next_id_start = max_id_seeker->id + 1;
    }
    std::cout << "새로운 데이터는 ID " << next_id_start << "부터 시작됩니다." << std::endl;

    // 3. 지정된 개수만큼 랜덤 데이터를 생성하여 벡터에 추가합니다.
    for (int i = 0; i < num_random_data_to_add; ++i) {
        all_seekers.push_back(generateRandomJobSeeker(i, next_id_start));
    }
    std::cout << "랜덤 데이터 " << num_random_data_to_add << "개 생성 완료." << std::endl;

    // 4. 모든 데이터를 (기존 데이터 + 새로 생성된 데이터) new_job_seekers.csv에 저장합니다.
    writeJobSeekersToCsv(output_file, all_seekers);

    std::cout << "총 " << all_seekers.size() << "개의 데이터가 '" << output_file << "'에 저장되었습니다." << std::endl;

    return 0;
}