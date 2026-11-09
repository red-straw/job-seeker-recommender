#include "pch.h"
#include "framework.h"
#include "afxdialogex.h"
#include <afxwin.h>        // MFC 기본
#include "seeker_MFC.h"
#include "seeker_MFCDlg.h"
#include <fstream>         // 파일 입출력
#include <string>          // 문자열
#include <sstream>         // 문자열 스트림
#include <vector>          // 벡터
#include <atlconv.h>       // 문자열 변환 매크로
#include <stdio.h>         // C 스타일 파일 입출력
#include <chrono>          // 시간 측정
#include <afx.h>           // CFile::GetStatus, CFileStatus를 위해 필요

#ifdef _DEBUG
#endif

// CseekerMFCDlg 생성자
CseekerMFCDlg::CseekerMFCDlg(CWnd* pParent)
	: CDialogEx(IDD_SEEKER_MFC_DIALOG, pParent)
	, count(0)
	, m_weightExperience(0.0)
	, m_weightEducation(0.0)
	, m_weightCertifications(0.0)
	, m_weightTestScore(0.0)
	, m_weightTechStack(0.0)
	, m_weightProject(0.0)
	, m_weightToeic(0.0)
	, m_weightPaper(0.0)
	, m_weightRecommender(0.0)
	, m_maxYear(20)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	for (int i = 0; i < MAX; ++i) {
		memset(&seekers[i], 0, sizeof(JobSeeker));
	}
}

// 데이터 교환 (DDX/DDV)
void CseekerMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RESULT_LIST, m_resultList);
	DDX_Control(pDX, IDC_EDIT_MAX_YEAR, m_editMaxYear);
	DDX_Control(pDX, IDC_STATIC_SORT_TIME, m_sortTimeText);

	DDX_Text(pDX, IDC_EDIT_MAX_YEAR, m_maxYear);
	DDV_MinMaxInt(pDX, m_maxYear, 0, 20);
}

// 메시지 맵
BEGIN_MESSAGE_MAP(CseekerMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()  // 창의 최소화/최대화/닫기
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BALANCE_BUTTON, &CseekerMFCDlg::OnClickedBalanceButton)
	ON_BN_CLICKED(IDC_PRACTICAL_BUTTON, &CseekerMFCDlg::OnClickedPracticalButton)
	ON_BN_CLICKED(IDC_POTENTIAL_BUTTON, &CseekerMFCDlg::OnClickedPotentialButton)
	ON_BN_CLICKED(IDC_OUTPUT_BUTTON, &CseekerMFCDlg::OnClickedOutputButton)
END_MESSAGE_MAP()

// CseekerMFCDlg 초기화
BOOL CseekerMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_resultList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_resultList.SetView(LV_VIEW_DETAILS);

	m_resultList.ModifyStyle(0, LVS_REPORT);

	m_resultList.InsertColumn(0, _T("이름"), LVCFMT_LEFT, 100);
	m_resultList.InsertColumn(1, _T("경력"), LVCFMT_LEFT, 100);
	m_resultList.InsertColumn(2, _T("학력"), LVCFMT_LEFT, 100);
	m_resultList.InsertColumn(3, _T("자격증"), LVCFMT_LEFT, 100);
	m_resultList.InsertColumn(4, _T("시험 점수"), LVCFMT_LEFT, 140);
	m_resultList.InsertColumn(5, _T("기술 스택 수"), LVCFMT_LEFT, 150);
	m_resultList.InsertColumn(6, _T("프로젝트 수"), LVCFMT_LEFT, 150);
	m_resultList.InsertColumn(7, _T("토익"), LVCFMT_LEFT, 100);
	m_resultList.InsertColumn(8, _T("논문"), LVCFMT_LEFT, 100);
	m_resultList.InsertColumn(9, _T("추천인"), LVCFMT_LEFT, 100);
	m_resultList.InsertColumn(10, _T("점수"), LVCFMT_LEFT, 150);

	m_maxYear = 20;
	UpdateData(FALSE); // MaxYear 변수를 컨트롤에 반영

	return TRUE;
}

// 시스템 명령 처리(창 테두리)
void CseekerMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// 그리기 처리(창복원등)
void CseekerMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 드래그 아이콘 쿼리
HCURSOR CseekerMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 점수 계산 함수
double CseekerMFCDlg::calculateScore(JobSeeker js) {
	return
		js.experience_years * m_weightExperience +
		js.education_level * m_weightEducation +
		js.certifications * m_weightCertifications +
		js.test_score * m_weightTestScore +
		js.tech_stack_count * m_weightTechStack +
		js.project_experience * m_weightProject +
		js.toeic_score * m_weightToeic +
		js.paper_publications * m_weightPaper +
		js.has_recommender * m_weightRecommender;
}

// 퀵 정렬 함수 (기본형: 첫 번째 요소를 피벗으로 선정)
void CseekerMFCDlg::quickSort_DefaultPivot(JobSeeker arr[], int low, int high) {
	if (low >= high) return;

	double pivot = arr[low].score;
	int i = low;
	int j = high;

	while (i <= j) {
		while (arr[i].score > pivot) {
			i++;
		}
		while (arr[j].score < pivot) {
			j--;
		}

		if (i <= j) {
			JobSeeker temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			i++;
			j--;
		}
	}

	if (low < j) {
		quickSort_DefaultPivot(arr, low, j);
	}
	if (i < high) {
		quickSort_DefaultPivot(arr, i, high);
	}
}


// 퀵 정렬 함수 (Median-of-Three 피벗 선정)
void CseekerMFCDlg::quickSort_MedianOfThree(JobSeeker arr[], int low, int high) {
	if (low >= high) return;

	int mid = low + (high - low) / 2;

	if (arr[low].score < arr[mid].score) {
		JobSeeker temp = arr[low]; arr[low] = arr[mid]; arr[mid] = temp;
	}
	if (arr[low].score < arr[high].score) {
		JobSeeker temp = arr[low]; arr[low] = arr[high]; arr[high] = temp;
	}
	if (arr[mid].score < arr[high].score) {
		JobSeeker temp = arr[mid]; arr[mid] = arr[high]; arr[high] = temp;
	}

	double pivot = arr[mid].score; // 세 값 중 중간값
	int i = low;
	int j = high;

	while (i <= j) {
		while (arr[i].score > pivot) {
			i++;
		}
		while (arr[j].score < pivot) {
			j--;
		}

		if (i <= j) {
			JobSeeker temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			i++;
			j--;
		}
	}

	if (low < j) {
		quickSort_MedianOfThree(arr, low, j);
	}
	if (i < high) {
		quickSort_MedianOfThree(arr, i, high);
	}
}

// 밸런스형 가중치 적용 버튼 클릭
void CseekerMFCDlg::OnClickedBalanceButton() 
{
	m_weightExperience = 0.07;
	m_weightEducation = 0.20;
	m_weightCertifications = 0.07;
	m_weightTestScore = 0.04;
	m_weightTechStack = 0.07;
	m_weightProject = 0.07;
	m_weightToeic = 0.004;
	m_weightPaper = 0.07;
	m_weightRecommender = 0.04;
	AfxMessageBox(_T("밸런스형 가중치가 적용되었습니다."));
}

// 실무중심형 가중치 적용 버튼 클릭
void CseekerMFCDlg::OnClickedPracticalButton()
{
	m_weightExperience = 0.10;
	m_weightEducation = 0.1875;
	m_weightCertifications = 0.0875;
	m_weightTestScore = 0.0475;
	m_weightTechStack = 0.12;
	m_weightProject = 0.10;
	m_weightToeic = 0.006;
	m_weightPaper = 0.0475;
	m_weightRecommender = 0.01;
	AfxMessageBox(_T("실무중심형 가중치가 적용되었습니다."));
}

// 잠재력 중심형 가중치 적용 버튼 클릭
void CseekerMFCDlg::OnClickedPotentialButton()
{
	m_weightExperience = -0.15;
	m_weightEducation = 0.30;
	m_weightCertifications = 0.10;
	m_weightTestScore = 0.10;
	m_weightTechStack = 0.10;
	m_weightProject = 0.05;
	m_weightToeic = 0.01;
	m_weightPaper = 0.15;
	m_weightRecommender = 0.05;
	AfxMessageBox(_T("잠재력 중심형 가중치가 적용되었습니다."));
}


// 파일을 읽고 구직자 데이터를 파싱
bool CseekerMFCDlg::ReadSeekers(const CString& filePath)
{
	count = 0; // 데이터 개수 초기화

	FILE* fp = _wfopen(filePath, _T("r"));
	if (fp == nullptr) {
		return false; // 파일 열기 실패 시 false 반환
	}

	char line[256];
	fgets(line, sizeof(line), fp); // 헤더 라인 건너뛰기

	while (fgets(line, sizeof(line), fp) && count < MAX) { // MAX 초과 시 루프 중단
		line[strcspn(line, "\r\n")] = 0; // 줄바꿈 문자 제거

		int parsed_id;

		int parsed_items = sscanf(line, "%d,%63[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",
			&parsed_id,
			seekers[count].name,
			&seekers[count].experience_years,
			&seekers[count].education_level,
			&seekers[count].certifications,
			&seekers[count].test_score,
			&seekers[count].tech_stack_count,
			&seekers[count].project_experience,
			&seekers[count].toeic_score,
			&seekers[count].paper_publications,
			&seekers[count].has_recommender);

		if (parsed_items == 11) {
			seekers[count].score = calculateScore(seekers[count]);

			if (seekers[count].experience_years <= m_maxYear) {
				count++;
			}
		}
	}
	fclose(fp);
	return true;
}

// 구직자 데이터를 정렬하고 정렬 시간을 측정하여 UI에 표시
void CseekerMFCDlg::SortAndMeasure(bool useMedianOfThree)
{
	auto start = std::chrono::high_resolution_clock::now(); // 정렬 시간 측정 시작

	// useMedianOfThree 값에 따라 다른 퀵 정렬 함수 호출
	if (useMedianOfThree) {
		quickSort_MedianOfThree(seekers, 0, count - 1);
	}
	else {
		quickSort_DefaultPivot(seekers, 0, count - 1);
	}

	auto end = std::chrono::high_resolution_clock::now();  // 정렬 시간 측정 종료
	std::chrono::duration<double, std::milli> duration = end - start;

	// 정렬 시간을 UI 텍스트 컨트롤에 업데이트
	CString sortTimeStr;
	sortTimeStr.Format(_T("정렬 시간 : %.3f ms"), duration.count());
	m_sortTimeText.SetWindowText(sortTimeStr);
}

// 정렬된 상위 구직자 10명을 리스트 컨트롤에 표시
void CseekerMFCDlg::DisplayTop10()
{
	m_resultList.DeleteAllItems(); // 리스트 컨트롤 초기화

	// 정렬된 상위 10명의 데이터를 리스트 컨트롤에 표시
	for (int i = 0; i < 10 && i < count; i++) {
		CString strName(seekers[i].name); // char 배열을 CString으로 변환
		int nItem = m_resultList.InsertItem(i, strName); // 리스트에 새 항목 삽입

		if (nItem != -1) {
			m_resultList.SetItemText(nItem, 0, strName); // 이름

			CString tempStr;
			tempStr.Format(_T("%d"), seekers[i].experience_years);
			m_resultList.SetItemText(nItem, 1, tempStr); // 경력

			tempStr.Format(_T("%d"), seekers[i].education_level);
			m_resultList.SetItemText(nItem, 2, tempStr); // 학력

			tempStr.Format(_T("%d"), seekers[i].certifications);
			m_resultList.SetItemText(nItem, 3, tempStr); // 자격증

			tempStr.Format(_T("%d"), seekers[i].test_score);
			m_resultList.SetItemText(nItem, 4, tempStr); // 시험 점수

			tempStr.Format(_T("%d"), seekers[i].tech_stack_count);
			m_resultList.SetItemText(nItem, 5, tempStr); // 기술 스택 수

			tempStr.Format(_T("%d"), seekers[i].project_experience);
			m_resultList.SetItemText(nItem, 6, tempStr); // 프로젝트 수

			tempStr.Format(_T("%d"), seekers[i].toeic_score);
			m_resultList.SetItemText(nItem, 7, tempStr); // 토익 점수

			tempStr.Format(_T("%d"), seekers[i].paper_publications);
			m_resultList.SetItemText(nItem, 8, tempStr); // 논문

			tempStr.Format(_T("%d"), seekers[i].has_recommender);
			m_resultList.SetItemText(nItem, 9, tempStr); // 추천인

			tempStr.Format(_T("%.2f"), seekers[i].score);
			m_resultList.SetItemText(nItem, 10, tempStr); // 최종 점수
		}
	}
}

// 정렬된 구직자 데이터 CSV 파일로 저장
bool CseekerMFCDlg::SaveSortedData(const CString& filePath)
{
	CStdioFile file;
	CFileException ex;

	if (file.Open(filePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText, &ex))
	{
		// CSV 파일의 헤더 라인
		file.WriteString(
			_T("id,name,experience_years,education_level,certifications,test_score_grade,")
			_T("tech_stack_count,project_experience,toeic_score_grade,paper_publications,has_recommender,score\n")
		);

		// 정렬된 모든 구직자 데이터를 파일에 쓰기
		for (int i = 0; i < count; ++i)
		{
			CString line;
			line.Format(_T("%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.2f\n"), // ID 추가
				i + 1, // ID 부여
				CString(seekers[i].name),		// 이름
				seekers[i].experience_years,	// 경력
				seekers[i].education_level,		// 학력
				seekers[i].certifications,		// 자격증
				seekers[i].test_score,			// 시험 점수
				seekers[i].tech_stack_count,	// 기술 스택 수
				seekers[i].project_experience,	// 프로젝트 경험 수
				seekers[i].toeic_score,			// 토익 점수
				seekers[i].paper_publications,	// 논문 발표 수
				seekers[i].has_recommender,		// 추천인 여부
				seekers[i].score);				// 최종 점수

			file.WriteString(line); // 완성된 라인을 파일에 씀
		}
		file.Close(); // 파일 닫기

		CString msg;
		msg.Format(_T("정렬된 데이터가 '%s' 파일로 성공적으로 저장되었습니다."), (LPCTSTR)filePath);
		AfxMessageBox(msg);
		return true;
	}
	else // 파일 열기 또는 쓰기 실패 시
	{
		// 파일 관련 오류 메시지 출력
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, _countof(szError));
		AfxMessageBox(szError);
		return false;
	}
}

// 출력 버튼 클릭 이벤트
void CseekerMFCDlg::OnClickedOutputButton()
{
	UpdateData(TRUE);

	const CString jobSeekersFilePath = _T("job_seekers.csv");
	const CString newJobSeekersFilePath = _T("new_job_seekers.csv");

	CString inputFilePathToUse;
	bool useMedianOfThreeSort = false;
	CString outputFileName;
	
	CFileStatus fileStatus;

	// job_seekers.csv 파일이 존재하는지 먼저 확인
	if (CFile::GetStatus(jobSeekersFilePath, fileStatus))
	{
		inputFilePathToUse = jobSeekersFilePath;
		useMedianOfThreeSort = false; // job_seekers.csv에는 Default Pivot 정렬 사용
		outputFileName = _T("sorted_job_seekers.csv");
		AfxMessageBox(_T("job_seekers.csv 파일을 감지했습니다. Default Pivot 퀵 정렬을 사용합니다."));
	}
	// new_job_seekers.csv 파일이 존재하는지 확인
	else if (CFile::GetStatus(newJobSeekersFilePath, fileStatus))
	{
		inputFilePathToUse = newJobSeekersFilePath;
		useMedianOfThreeSort = true; // new_job_seekers.csv에는 Median-of-Three 정렬 사용
		outputFileName = _T("sorted_new_job_seekers.csv"); // new_sorted_job_seeker.csv로 변경
		AfxMessageBox(_T("new_job_seekers.csv 파일을 감지했습니다. Median-of-Three 퀵 정렬을 사용합니다."));
	}
	// 두 파일 모두 존재하지 않는 경우
	else
	{
		AfxMessageBox(_T("처리할 구직자 파일(job_seekers.csv 또는 new_job_seekers.csv)을 찾을 수 없습니다."));
		return; // 함수 종료
	}

	// 결정된 CSV 파일에서 구직자 데이터를 읽고 처리
	if (!ReadSeekers(inputFilePathToUse)) {
		AfxMessageBox(_T("파일 읽기 중 오류가 발생했습니다: ") + inputFilePathToUse);
		return; // 파일 읽기 실패 시 함수 종료
	}

	SortAndMeasure(useMedianOfThreeSort); // 결정된 정렬 방식의 SortAndMeasure 

	DisplayTop10(); // 정렬된 상위 10명 구직자 화면에 표시

	SaveSortedData(outputFileName); // 정렬된 구직자 데이터 파일로 저장
}