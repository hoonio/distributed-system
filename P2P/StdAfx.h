// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__EC9445A6_5FA0_46CC_A926_B7B9D3CE9E42__INCLUDED_)
#define AFX_STDAFX_H__EC9445A6_5FA0_46CC_A926_B7B9D3CE9E42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



// #include <afxsock.h>		// MFC socket extensions




// User Code
#define FILENAME_SERVERLIBRARY "ServerLibrary.dat"
#define FILENAME_DOWNLOADSLIST "DownloadsList.dat"
#define FILENAME_UPLOADSLIST "UploadsList.dat"
#define FILENAME_PREFERENCES "Preferences.ini"

#define FOLDERNAME_TEMP "Temp"
#define FOLDERNAME_INCOMING "Incoming"

// MSN �˾��޽�������
#define POPUP_MESSEGE_SERVER_CONNECT	1	// ������ �߰��Ǿ� ���ӵ�
#define POPUP_MESSEGE_CONNECT			2	// ���ӵ�
#define POPUP_MESSEGE_DISCONNECT		3	// ��������
#define POPUP_MESSEGE_DOWNLOADCOMPLETE	4	// �ٿ�Ϸ�
#define POPUP_MESSEGE_SERVER_DISCONNECT	5	// ������ ��������


#define UM_SETPANETEXT WM_USER+10	// ������������ statusbar ����
#define UM_POPUPMESSAGE WM_USER+11	// �˾��޽���
#define UM_ADDITEMSEARCHFILE WM_USER+12	// �˾��޽���
#define UM_DOWNLOAD_COMPLETE WM_USER+13	// �ٿ�ε�Ϸ�
#define UM_UPLOAD_COMPLETE WM_USER+14	// ���ε�Ϸ�
#define UM_ADD_UPLOAD_DATA WM_USER+15	// ���ε嵥��Ÿ�߰�
#define UM_ADD_DOWNLOAD_DATA WM_USER+16	// �ٿ�ε嵥��Ÿ�߰�




#ifdef _DEBUG
	#define MYDEBUG	// �Ϲ� �����
#endif



//#define MYSOCKETDEBUG	// ���ϰ��õ����






//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__EC9445A6_5FA0_46CC_A926_B7B9D3CE9E42__INCLUDED_)

