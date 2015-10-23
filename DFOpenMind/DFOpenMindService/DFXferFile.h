#ifndef __DFXFERFILE_H__
#define __DFXFERFILE_H__

#pragma once

#include "DFCmn.h"



class DFFileXferEngine
{
public:
	DFFileXferEngine();
	~DFFileXferEngine();

private:
	

};


#endif

// file sender thread entry function.
DWORD WINAPI FileSenderThread(LPVOID p)
{
	DFFileSender* pFileSender = (DFFileSender*)p;
	return pFileSender->SendFile();
}

// file recver thread entry function.
DWORD WINAPI FileRecverThread(LPVOID p)
{
	DFFileRecver* pFileRecver = (DFFileRecver*)p;
	return pFileRecver->RecvFile();
}