#pragma once
#include <vector>   // std::vector 사용
#include <string>   // std::string 사용 (선택 사항, CString 사용해도 무방)
#include <afxwin.h> // MFC 관련 클래스 (CString 등) 사용

#define MAX 200000 // 사용자 정의 MAX 값

// JobSeeker 구조체 정의
typedef struct {
    char name[64];             // 이름
    int experience_years;      // 경력 (연차)
    int education_level;       // 학벌 (수치화된 값)
    int certifications;        // 자격증 수
    int test_score;            // 시험 점수
    int tech_stack_count;      // 기술 스택 수
    int project_experience;    // 프로젝트 수
    int toeic_score;           // 토익 점수
    int paper_publications;    // 논문 발표 수
    int has_recommender;       // 추천인 유무 (0 또는 1)
    double score;              // 계산된 점수 저장
} JobSeeker;

// CseekerMFCDlg 대화 상자
class CseekerMFCDlg : public CDialogEx
{
public:
    CseekerMFCDlg(CWnd* pParent = nullptr);    // 표준 생성자입니다.

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SEEKER_MFC_DIALOG };
#endif

protected:
    HICON m_hIcon;

    virtual void DoDataExchange(CDataExchange* pDX);

    CStatic m_sortTimeText;

    CListCtrl m_resultList;

    CEdit m_editMaxYear;    // 최대 연차 입력 컨트롤 변수

    // 가중치 변수 (출력 시 사용하지 않음)
    double m_weightExperience;
    double m_weightEducation;
    double m_weightCertifications;
    double m_weightTestScore;
    double m_weightTechStack;
    double m_weightProject;
    double m_weightToeic;
    double m_weightPaper;
    double m_weightRecommender;

    JobSeeker seekers[MAX];
    int count;

    // 필터값 저장 변수
    int m_maxYear; // 최대 연차 필터

    // 사용자 정의 점수 계산 함수
    double calculateScore(JobSeeker js);

protected:
    DECLARE_MESSAGE_MAP()

public:
    // 이벤트 처리기 함수
    afx_msg void OnClickedBalanceButton();
    afx_msg void OnClickedPracticalButton();
    afx_msg void OnClickedPotentialButton();
    afx_msg void OnClickedOutputButton();

    // 재정의된 멤버 함수
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    void quickSort_DefaultPivot(JobSeeker arr[], int low, int high);
    void quickSort_MedianOfThree(JobSeeker arr[], int low, int high);

    bool ReadSeekers(const CString& filePath);
    // 구직자 데이터를 정렬하고 정렬 시간을 측정 UI에 표시
    void SortAndMeasure(bool useMedianOfThree);
    // 정렬된 상위 구직자 10명을 리스트 컨트롤에 표시
    void DisplayTop10();
    // 정렬된 모든 구직자 데이터를 CSV 파일로 저장
    bool SaveSortedData(const CString& filePath);
};