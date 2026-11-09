#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 100000

typedef struct {
    char name[5];
    int experience_years;
    int education_level;
    int certifications;
    int test_score;
    int tech_stack_count;
    int project_experience;
    int toeic_score;
    int paper_publications;
    int has_recommender;
    double score;
} JobSeeker;

JobSeeker seekers[MAX];
int count = 0;

double calculateScore(JobSeeker js) {
    return
        js.experience_years * 1.2 +
        js.education_level * 2.0 +
        js.certifications * 1.5 +
        js.test_score * 1.0 +
        js.tech_stack_count * 1.3 +
        js.project_experience * 1.4 +
        js.toeic_score * 0.05 +
        js.paper_publications * 2.0 +
        js.has_recommender * 5.0;
}

// 간결한 퀵정렬
void quickSort(JobSeeker arr[], int low, int high) {
    if (low >= high) return;

    double pivot = arr[(low + high) / 2].score;
    int i = low, j = high;

    while (i <= j) {
        while (arr[i].score > pivot) i++;
        while (arr[j].score < pivot) j--;
        if (i <= j) {
            JobSeeker temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }

    quickSort(arr, low, j);
    quickSort(arr, i, high);
}

int main() {
    FILE* fp = fopen("job_seekers.csv", "r");
    if (!fp) {
        printf("파일 열기 실패\n");
        return 1;
    }

    char line[256];
    fgets(line, sizeof(line), fp); // 헤더 무시

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%*d,%4[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",
            seekers[count].name,
            &seekers[count].experience_years,
            &seekers[count].education_level,
            &seekers[count].certifications,
            &seekers[count].test_score,
            &seekers[count].tech_stack_count,
            &seekers[count].project_experience,
            &seekers[count].toeic_score,
            &seekers[count].paper_publications,
            &seekers[count].has_recommender) == 10) {

            seekers[count].score = calculateScore(seekers[count]);
            count++;
        }
    }

    fclose(fp);

    // 퀵정렬 시간 측정 시작
    clock_t start = clock();

    quickSort(seekers, 0, count - 1); // 퀵정렬 실행

    // 퀵정렬 시간 측정 종료
    clock_t end = clock();
    double time_spent_ms = (double)(end - start) / (CLOCKS_PER_SEC / 1000);

    printf("상위 10명 추천 구직자:\n");
    for (int i = 0; i < 10 && i < count; i++) {
        printf("%d. %s (점수: %.2f)\n", i + 1, seekers[i].name, seekers[i].score);
    }

    printf("퀵정렬에 걸린 시간: %.2fms\n", time_spent_ms);

    return 0;
}