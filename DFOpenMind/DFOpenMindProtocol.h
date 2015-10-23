#ifndef __DFOPENMINDPROTOCOL_H__
#define __DFOPENMINDPROTOCOL_H__

#define USER_ID_LENGTH		11

/*---------------------------------------------------------------------------------
 검색 조건 및 우선순위.

  1. 사용자 ID 
  2. File 이름 일부.
  3. 형식(파일 확장자 입력으로)
  4. 날짜와 시간(생성/수정/접근 중 하나, 언제부터 언제까지)
  5. 파일 사이즈(최대/최소).

	대소문자 구분없고, 하위 디렉토리 검색 없음.
-----------------------------------------------------------------------------------*/

// definition of Search methods(5 bits)				// bit expression
#define	FIND_USER_ID			1					// 00001	
#define	FIND_NAME				2					// 00010
#define	FIND_EXTENSION			4					// 00100
#define	FIND_SIZE				8					// 01000
#define	FIND_TIME				16					// 10000

// definition of time search methods(3 bits)
#define FIND_CREATION_TIME		1					// 001
#define	FIND_ACCESS_TIME		2					// 010
#define	FIND_MODIFY_TIME		4					// 100


// definition of secondary search methods(4 bits)	// bit expression 
#define	FIND_MIN_SIZE			1					// 0001
#define	FIND_MAX_SIZE			2					// 0010
#define FIND_MIN_TIME			4					// 0100
#define FIND_MAX_TIME			8					// 1000


// definition of macros for search methods.
#define	SEARCH_USER_ID(a)			((a.base_methods) & FIND_USER_ID)			// 유저 아이디로 검색
#define SEARCH_FILE_NAME(a)			((a.base_methods) & FIND_NAME)				// 파일 이름으로 검색
#define SEARCH_FILE_EXTENSION(a)	((a.base_methods) & FIND_EXTENSION)		// 파일 확장자로 검색
#define SEARCH_FILE_TIME(a)			((a.base_methods) & FIND_TIME)				// 파일 시간으로 검색
#define	SEARCH_SIZE(a)				((a.base_methods) & FIND_SIZE)				// 파일 크기로 검색.

#define SEARCH_CREATION_TIME(b)		((b.time_methods) & FIND_CREATION_TIME)	// 파일 생성시간으로 검색
#define SEARCH_ACCESS_TIME(b)		((b.time_methods) & FIND_ACCESS_TIME)		// 파일 접근시간으로 검색
#define SEARCH_MODIFY_TIME(b)		((b.time_methods) & FIND_MODIFY_TIME)		// 파일 수정시간으로 검색

#define VALID_MIN_SIZE(c)			((c.valid_check) & FIND_MIN_SIZE)			// MinSize의 Validate.
#define VALID_MAX_SIZE(c)			((c.valid_check) & FIND_MAX_SIZE)			// MaxSize의 "
#define VALID_MIN_TIME(c)			((c.valid_check) & FIND_MIN_TIME)			// MinTime의 "
#define VALID_MAX_TIME(c)			((c.valid_check) & FIND_MAX_TIME)			// MaxTime의 "

// bit field for search methods.
typedef struct _Search_Methods
{
	unsigned base_methods	: 5;
	unsigned time_methods	: 3;
	unsigned valid_check	: 4;
} Search_Methods;


// 네트워크를 통해 클라이언트가 전송해 주는 메시지 구조체이다.
// 클라이언트는 이 정보를 관리한다.
// file infomation structure
typedef struct 
{
	TCHAR		tszUserID[USER_ID_LENGTH+1];	// UserID(HostID).
	TCHAR		tszFileName[MAX_PATH];			// File Name
//	TCHAR		tszAlternateFileName[14];		// Classic File Name 
	FILETIME	ftCreationTime;					// File Creation Time
	FILETIME	ftLastAccessTime;				// Last Access Time
	FILETIME	ftLastWriteTime;				// Last Write Time
	DWORD		dwFileSizeHigh;					// File Size(High DWORD)
	DWORD		dwFileSizeLow;					// File Size(Low DWORD)
} FILE_INFO, *PFILE_INFO;

// 서비스가 관리할 유저의 공유 파일들의 정보를 담을 구조체.
typedef struct 
{
	TCHAR		tszFileName[MAX_PATH+USER_ID_LENGTH+3];	// User ID + File Name
//	TCHAR		tszAlternateFileName[14];		// Classic File Name 
	FILETIME	ftCreationTime;					// File Creation Time
	FILETIME	ftLastAccessTime;				// Last Access Time
	FILETIME	ftLastWriteTime;				// Last Write Time
	DWORD		dwFileSizeHigh;					// File Size(High DWORD)
	DWORD		dwFileSizeLow;					// File Size(Low DWORD)
} FILE_INFO2, *PFILE_INFO2;

// filefind structure
typedef struct
{
	Search_Methods	Methods;					// Secondary Methods !!!!
	TCHAR		tszUserID[USER_ID_LENGTH+1];
	TCHAR		tszFileNamePortion[MAX_PATH];
	TCHAR		tszExtention[4];				// File Extension.
	FILETIME	ftMinTime;						// File Time(Min)
	FILETIME	ftMaxTime;						// File Time(Max)
	DWORD		dwMinFileSizeHigh;				// File Size(High DWORD)
	DWORD		dwMinFileSizeLow;				// File Size(Low DWORD)
	DWORD		dwMaxFileSizeHigh;				// File Size(High DWORD)
	DWORD		dwMaxFileSizeLow;				// File Size(Low DWORD)
} FILE_FIND_INFO, *PFILE_FIND_INFO;


// message type constants
#define DFOM_CS_LOGIN			1		// 새로 로긴함을 알림.
#define DFOM_CS_LOGOUT			2		// 로그 아웃을 알림
#define DFOM_CS_NEWUSER			3		// 새로운 유저 생성을 알림
#define DFOM_CS_SENDINFOS		4		// 현재 자신의 파일정보 알림
#define DFOM_CS_SEARCH			5		// 탐색 요청
#define DFOM_CS_ALIVE			6		// 살아 있음을 통보

#define DFOM_SC_LOGIN			101		// 로긴 응답 메시지
#define DFOM_SC_LOGINUSERS		102		// 기존 로긴된 사용자/새로 로긴한 사용자를 알려줌
#define DFOM_SC_LOGOUT			103		// 로그 아웃됨을 다른 사용자에게 알림.
#define DFOM_SC_NEWUSER			104		// 새로운 유저 생성 응답 메시지.
#define DFOM_SC_SEARCH_RESULTS	105		// 탐색 결과 정보를 송신(실패도 포함)
#define DFOM_SC_ALIVE			106		// 살아있는지 메시지 송신을 요구.

#define DFOM_CC_REQUEST			200		// 파일 송신을 요청
#define DFOM_CC_RESPONSE		201		// 파일 송신 요청에 대한 응답.
#define DFOM_CC_XFER_START		202		// 파일 전송 시작을 요청
#define DFOM_CC_XFER_FIN		203		// 파일 전송 종료를 알림
//#define DFOM_CC_FILE_TRANSFER	204		// 파일 데이터 송신..

// error code constants
#define FAIL_SUCCESS			200		// 성공~
#define FAIL_USER_ALREADY_LOGIN	201		// 로그인 실패
#define FAIL_ID_NOT_EXIST		202		//		"
#define FAIL_PASSWORD_MISMATCH	203		//		"
#define FAIL_MAX_USERS			204		// 아이디 생성 실패
#define FAIL_ID_ALREADY_EXIST	205		//		"


/*
 Message Protocol Definition

  모든 메시지의 첫번째 1바이트를 메시지 타입으로 한다(0 ~ 255).

  1. 클라이언트 -> 서버
  	(1)로그인 메시지 
		유저 ID				: 13 * sizeof(TCHAR)
		패스워드			:  9 * sizeof(TCHAR)
	(2) 로그 아웃 메시지
		유저 ID				: 13 * sizeof(TCHAR)
	(3) 새로운 유저 생성 메시지.
		새로운 유저 ID		: 13 * sizeof(TCHAR)
		패스워드			:  9 * sizeof(TCHAR)
		주소				: 61 * sizeof(TCHAR)
		전화				: 17 * sizeof(TCHAR)
		E-MAIL				: 41 * sizeof(TCHAR)
    (4) 공유 파일 정보 메시지(송신된 정보는 서버에 의해 모두 추가된다)
		FILE_INFO 구조체	: sizeof(FILE_INFO)
  	(5) 검색 요청 메시지
		최대 결과 셋		: 2바이트
		FILE_FIND_INFO구조체: sizeof(FILE_FIND_INFO)
	(6) 살아있음을 통보(별다른 메시지 내용이 없이 1바이트 메시지 타입만 송신)

  2. 서버 -> 클라이언트
	(1) 로그인 확인 메시지
		성공/실패 여부		: 1바이트(true/false)
		실패 원인			: 1바이트(...)
		유저 ID				: 13 * sizeof(TCHAR)
	(2) 로그인된 사용자 메시지
		사용자 ID 수		: 1바이트	(최대 50개로 한다)
		신규로긴 여부		: 1바이트 (1이면 1명의 유저가 신규 로긴함을 알린다)
		사용자 ip			: 4바이트!!!
		로그인된 사용자 ID	: 13 * sizeof(TCHAR)
	(3) 로그아웃 메시지
		사용자 ID			: 13 * sizeof(TCHAR)
	(4) 유저 생성 결과 메시지
		성공/실패			: 1바이트(true/false)
		실패원인			: 1바이트(...)
		유저 ID				: 13 * sizeof(TCHAR)
	(5) 검색 결과 메시지
		검색된 파일정보의 수: 2바이트(실패시 0)
		몇번째 정보인지		: 2바이트(실패시 0, 성공시 1부터 시작됨)
		파일 정보 구조체	: sizeof(FILE_INFO); 604 바이트
	(6) 살아있는지 문의(별다른 내용없음)

  3. 클라이언트 -> 클라이언트(메시지 길이를 표현하는 2바이트를 두지 않는다. 모두 길이를 아니깐)
	(1) 파일 송신 요청
		파일 이름			: (MAX_PATH) * sizeof(TCHAR)
		유저 이름			: 13 * sizeof(TCHAR)
	(2) 파일 송신 요청 응답
		파일 요청 결과		: 1 바이트 (실패 시 0, 성공 시 1)
		파일 크기			: UINT 4 바이트
	(3) 전송 시작 요청 메시지 
		파일 이름			: (MAX_PATH) * sizeof(TCHAR)
	(4) 전송 완료 메시지
		파일 이름			: (MAX_PATH) * sizeof(TCHAR)
	(3) 파일 송신(이것은 다 보낼때까지 최대 버퍼 사이즈 4096만큼 채워서 보낸다)
		데이터 				: 1~4096 바이트

*/	

#endif