#include <iostream> // 콘솔 출력
#include <fstream>  // 파일 스트림
#include <string>   // 문자열 처리
#include <sstream>  // 문자열 스트림 (파싱에 사용)
#include <map>      // 키-값 쌍 저장 (중복 카운트)
#include <vector>   // 벡터 사용 (선택 사항)

// JobSeeker 속성값을 저장할 구조체 (이름은 제외)
// CSV 파일에서 읽어올 필드 순서와 타입을 맞춰주세요.
// `calculateScore` 함수에서 사용하는 필드와 동일하게 구성해야 합니다.
struct JobSeekerAttributes {
    int experience_years;
    int education_level;
    int certifications;
    int test_score_grade; // test_score_grade로 변경
    int tech_stack_count;
    int project_experience;
    int toeic_score_grade; // toeic_score_grade로 변경
    int paper_publications;
    int has_recommender;

    // map의 키로 사용하기 위해 비교 연산자 오버로드
    // 이 연산자가 있어야 map이 JobSeekerAttributes 객체를 정렬할 수 있습니다.
    bool operator<(const JobSeekerAttributes& other) const {
        if (experience_years != other.experience_years) return experience_years < other.experience_years;
        if (education_level != other.education_level) return education_level < other.education_level;
        if (certifications != other.certifications) return certifications < other.certifications;
        if (test_score_grade != other.test_score_grade) return test_score_grade < other.test_score_grade;
        if (tech_stack_count != other.tech_stack_count) return tech_stack_count < other.tech_stack_count;
        if (project_experience != other.project_experience) return project_experience < other.project_experience;
        if (toeic_score_grade != other.toeic_score_grade) return toeic_score_grade < other.toeic_score_grade;
        if (paper_publications != other.paper_publications) return paper_publications < other.paper_publications;
        // has_recommender가 마지막이므로 이까지 같으면 같은 속성 조합
        return has_recommender < other.has_recommender;
    }
};

int main() {
    std::ifstream file("job_seekers.csv"); // CSV 파일 경로
    if (!file.is_open()) {
        std::cerr << "Error: job_seekers.csv 파일을 열 수 없습니다." << std::endl;
        return 1;
    }

    std::string line;
    std::getline(file, line); // 헤더 라인 건너뛰기

    std::map<JobSeekerAttributes, int> attributeCounts; // 속성 조합별 빈도수 저장
    int totalRecords = 0; // 총 레코드 수
    int duplicateRecords = 0; // 중복 레코드 수 (2개 이상인 경우부터 카운트)

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        JobSeekerAttributes currentAttributes;

        // 첫 번째 컬럼 (id) 건너뛰기
        std::getline(ss, token, ',');

        // 두 번째 컬럼 (name) 건너뛰기
        std::getline(ss, token, ',');

        // 나머지 속성값 파싱
        std::getline(ss, token, ',');
        currentAttributes.experience_years = std::stoi(token);
        std::getline(ss, token, ',');
        currentAttributes.education_level = std::stoi(token);
        std::getline(ss, token, ',');
        currentAttributes.certifications = std::stoi(token);
        std::getline(ss, token, ',');
        currentAttributes.test_score_grade = std::stoi(token); // test_score_grade
        std::getline(ss, token, ',');
        currentAttributes.tech_stack_count = std::stoi(token);
        std::getline(ss, token, ',');
        currentAttributes.project_experience = std::stoi(token);
        std::getline(ss, token, ',');
        currentAttributes.toeic_score_grade = std::stoi(token); // toeic_score_grade
        std::getline(ss, token, ',');
        currentAttributes.paper_publications = std::stoi(token);
        std::getline(ss, token); // 마지막 필드 (has_recommender)는 콤마 없이
        currentAttributes.has_recommender = std::stoi(token);

        totalRecords++; // 총 레코드 수 증가

        // 해당 속성 조합의 카운트를 증가시킵니다.
        attributeCounts[currentAttributes]++;
    }

    file.close(); // 파일 닫기

    // 중복 레코드 계산
    // map을 순회하며 빈도수가 1보다 큰 항목들을 찾습니다.
    for (const auto& pair : attributeCounts) {
        if (pair.second > 1) { // 해당 속성 조합이 2번 이상 나타났다면
            duplicateRecords += (pair.second - 1); // 첫 번째를 제외한 나머지 수만큼 중복으로 간주
        }
    }

    double duplicatePercentage = 0.0;
    if (totalRecords > 0) {
        duplicatePercentage = (static_cast<double>(duplicateRecords) / totalRecords) * 100.0;
    }

    std::cout << "총 레코드 수: " << totalRecords << std::endl;
    std::cout << "속성값 기준으로 중복된 레코드 수 (첫 번째 발생 제외): " << duplicateRecords << std::endl;
    std::cout << "중복된 레코드 비율: " << duplicatePercentage << "%" << std::endl;

    return 0;
}