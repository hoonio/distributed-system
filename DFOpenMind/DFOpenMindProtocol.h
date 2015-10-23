#ifndef __DFOPENMINDPROTOCOL_H__
#define __DFOPENMINDPROTOCOL_H__

#define USER_ID_LENGTH		11

/*---------------------------------------------------------------------------------
 �˻� ���� �� �켱����.

  1. ����� ID 
  2. File �̸� �Ϻ�.
  3. ����(���� Ȯ���� �Է�����)
  4. ��¥�� �ð�(����/����/���� �� �ϳ�, �������� ��������)
  5. ���� ������(�ִ�/�ּ�).

	��ҹ��� ���о���, ���� ���丮 �˻� ����.
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
#define	SEARCH_USER_ID(a)			((a.base_methods) & FIND_USER_ID)			// ���� ���̵�� �˻�
#define SEARCH_FILE_NAME(a)			((a.base_methods) & FIND_NAME)				// ���� �̸����� �˻�
#define SEARCH_FILE_EXTENSION(a)	((a.base_methods) & FIND_EXTENSION)		// ���� Ȯ���ڷ� �˻�
#define SEARCH_FILE_TIME(a)			((a.base_methods) & FIND_TIME)				// ���� �ð����� �˻�
#define	SEARCH_SIZE(a)				((a.base_methods) & FIND_SIZE)				// ���� ũ��� �˻�.

#define SEARCH_CREATION_TIME(b)		((b.time_methods) & FIND_CREATION_TIME)	// ���� �����ð����� �˻�
#define SEARCH_ACCESS_TIME(b)		((b.time_methods) & FIND_ACCESS_TIME)		// ���� ���ٽð����� �˻�
#define SEARCH_MODIFY_TIME(b)		((b.time_methods) & FIND_MODIFY_TIME)		// ���� �����ð����� �˻�

#define VALID_MIN_SIZE(c)			((c.valid_check) & FIND_MIN_SIZE)			// MinSize�� Validate.
#define VALID_MAX_SIZE(c)			((c.valid_check) & FIND_MAX_SIZE)			// MaxSize�� "
#define VALID_MIN_TIME(c)			((c.valid_check) & FIND_MIN_TIME)			// MinTime�� "
#define VALID_MAX_TIME(c)			((c.valid_check) & FIND_MAX_TIME)			// MaxTime�� "

// bit field for search methods.
typedef struct _Search_Methods
{
	unsigned base_methods	: 5;
	unsigned time_methods	: 3;
	unsigned valid_check	: 4;
} Search_Methods;


// ��Ʈ��ũ�� ���� Ŭ���̾�Ʈ�� ������ �ִ� �޽��� ����ü�̴�.
// Ŭ���̾�Ʈ�� �� ������ �����Ѵ�.
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

// ���񽺰� ������ ������ ���� ���ϵ��� ������ ���� ����ü.
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
#define DFOM_CS_LOGIN			1		// ���� �α����� �˸�.
#define DFOM_CS_LOGOUT			2		// �α� �ƿ��� �˸�
#define DFOM_CS_NEWUSER			3		// ���ο� ���� ������ �˸�
#define DFOM_CS_SENDINFOS		4		// ���� �ڽ��� �������� �˸�
#define DFOM_CS_SEARCH			5		// Ž�� ��û
#define DFOM_CS_ALIVE			6		// ��� ������ �뺸

#define DFOM_SC_LOGIN			101		// �α� ���� �޽���
#define DFOM_SC_LOGINUSERS		102		// ���� �α�� �����/���� �α��� ����ڸ� �˷���
#define DFOM_SC_LOGOUT			103		// �α� �ƿ����� �ٸ� ����ڿ��� �˸�.
#define DFOM_SC_NEWUSER			104		// ���ο� ���� ���� ���� �޽���.
#define DFOM_SC_SEARCH_RESULTS	105		// Ž�� ��� ������ �۽�(���е� ����)
#define DFOM_SC_ALIVE			106		// ����ִ��� �޽��� �۽��� �䱸.

#define DFOM_CC_REQUEST			200		// ���� �۽��� ��û
#define DFOM_CC_RESPONSE		201		// ���� �۽� ��û�� ���� ����.
#define DFOM_CC_XFER_START		202		// ���� ���� ������ ��û
#define DFOM_CC_XFER_FIN		203		// ���� ���� ���Ḧ �˸�
//#define DFOM_CC_FILE_TRANSFER	204		// ���� ������ �۽�..

// error code constants
#define FAIL_SUCCESS			200		// ����~
#define FAIL_USER_ALREADY_LOGIN	201		// �α��� ����
#define FAIL_ID_NOT_EXIST		202		//		"
#define FAIL_PASSWORD_MISMATCH	203		//		"
#define FAIL_MAX_USERS			204		// ���̵� ���� ����
#define FAIL_ID_ALREADY_EXIST	205		//		"


/*
 Message Protocol Definition

  ��� �޽����� ù��° 1����Ʈ�� �޽��� Ÿ������ �Ѵ�(0 ~ 255).

  1. Ŭ���̾�Ʈ -> ����
  	(1)�α��� �޽��� 
		���� ID				: 13 * sizeof(TCHAR)
		�н�����			:  9 * sizeof(TCHAR)
	(2) �α� �ƿ� �޽���
		���� ID				: 13 * sizeof(TCHAR)
	(3) ���ο� ���� ���� �޽���.
		���ο� ���� ID		: 13 * sizeof(TCHAR)
		�н�����			:  9 * sizeof(TCHAR)
		�ּ�				: 61 * sizeof(TCHAR)
		��ȭ				: 17 * sizeof(TCHAR)
		E-MAIL				: 41 * sizeof(TCHAR)
    (4) ���� ���� ���� �޽���(�۽ŵ� ������ ������ ���� ��� �߰��ȴ�)
		FILE_INFO ����ü	: sizeof(FILE_INFO)
  	(5) �˻� ��û �޽���
		�ִ� ��� ��		: 2����Ʈ
		FILE_FIND_INFO����ü: sizeof(FILE_FIND_INFO)
	(6) ��������� �뺸(���ٸ� �޽��� ������ ���� 1����Ʈ �޽��� Ÿ�Ը� �۽�)

  2. ���� -> Ŭ���̾�Ʈ
	(1) �α��� Ȯ�� �޽���
		����/���� ����		: 1����Ʈ(true/false)
		���� ����			: 1����Ʈ(...)
		���� ID				: 13 * sizeof(TCHAR)
	(2) �α��ε� ����� �޽���
		����� ID ��		: 1����Ʈ	(�ִ� 50���� �Ѵ�)
		�űԷα� ����		: 1����Ʈ (1�̸� 1���� ������ �ű� �α����� �˸���)
		����� ip			: 4����Ʈ!!!
		�α��ε� ����� ID	: 13 * sizeof(TCHAR)
	(3) �α׾ƿ� �޽���
		����� ID			: 13 * sizeof(TCHAR)
	(4) ���� ���� ��� �޽���
		����/����			: 1����Ʈ(true/false)
		���п���			: 1����Ʈ(...)
		���� ID				: 13 * sizeof(TCHAR)
	(5) �˻� ��� �޽���
		�˻��� ���������� ��: 2����Ʈ(���н� 0)
		���° ��������		: 2����Ʈ(���н� 0, ������ 1���� ���۵�)
		���� ���� ����ü	: sizeof(FILE_INFO); 604 ����Ʈ
	(6) ����ִ��� ����(���ٸ� �������)

  3. Ŭ���̾�Ʈ -> Ŭ���̾�Ʈ(�޽��� ���̸� ǥ���ϴ� 2����Ʈ�� ���� �ʴ´�. ��� ���̸� �ƴϱ�)
	(1) ���� �۽� ��û
		���� �̸�			: (MAX_PATH) * sizeof(TCHAR)
		���� �̸�			: 13 * sizeof(TCHAR)
	(2) ���� �۽� ��û ����
		���� ��û ���		: 1 ����Ʈ (���� �� 0, ���� �� 1)
		���� ũ��			: UINT 4 ����Ʈ
	(3) ���� ���� ��û �޽��� 
		���� �̸�			: (MAX_PATH) * sizeof(TCHAR)
	(4) ���� �Ϸ� �޽���
		���� �̸�			: (MAX_PATH) * sizeof(TCHAR)
	(3) ���� �۽�(�̰��� �� ���������� �ִ� ���� ������ 4096��ŭ ä���� ������)
		������ 				: 1~4096 ����Ʈ

*/	

#endif