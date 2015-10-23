#include "stdafx.h"
#include "DFMapPtrToPtr.h"

DFMapPtrToPtr::DFPlex* PASCAL 
DFMapPtrToPtr::DFPlex::Create(DFMapPtrToPtr::DFPlex*& pHead, UINT nMax, UINT cbElement)
{
	DFMapPtrToPtr::DFPlex* p = 
		(DFMapPtrToPtr::DFPlex*) new BYTE[sizeof(DFMapPtrToPtr::DFPlex) + nMax * cbElement];
//	DFASSERT(p);
			// may throw exception
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

void DFMapPtrToPtr::DFPlex::FreeDataChain()     // free this one and links
{
	DFMapPtrToPtr::DFPlex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		DFMapPtrToPtr::DFPlex* pNext = p->pNext;
		delete[] bytes;
		p = pNext;
	}
}

////////////////////////////////////////////////////////////////////////////////
DFMapPtrToPtr::DFMapPtrToPtr(int nBlockSize)
{
	m_pHashTable		= NULL;
	m_nHashTableSize	= 17;
	m_nCount			= 0;
	m_pFreeList			= NULL;
	m_pBlocks			= NULL;
	m_nBlockSize		= nBlockSize;

//	DFVERIFY(InitializeCriticalSectionAndSpinCount(&m_CS, 4000));
}

DFMapPtrToPtr::~DFMapPtrToPtr()
{
	RemoveAll();
//	DFASSERT(m_nCount == 0);
//	DeleteCriticalSection(&m_CS);
}

void DFMapPtrToPtr::SetAt(void* key, void* newValue)
{ 
	(*this)[key] = newValue; 
}

inline UINT DFMapPtrToPtr::GetHashTableSize() const
{ 
	return m_nHashTableSize; 
}

inline UINT DFMapPtrToPtr::HashKey(void* Key) const
{
	return ((UINT)(void*)(DWORD)Key >> 4);
}

int DFMapPtrToPtr::GetCount() const
{
	return m_nCount;
}

inline BOOL DFMapPtrToPtr::IsEmpty() const
{
	return(m_nCount == 0);
}

POSITION DFMapPtrToPtr::GetStartPosition() const
{
	return (m_nCount == 0) ? NULL : BEFORE_START_POSITION;
}

void DFMapPtrToPtr::InitHashTable(UINT nHashSize, BOOL bAllocNow)
{
	if(m_pHashTable != NULL)
	{
		delete [] m_pHashTable;
		m_pHashTable = NULL;
	}

	if(bAllocNow)
	{
		m_pHashTable = new DFAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(DFAssoc*) * nHashSize);
	}

	m_nHashTableSize = nHashSize;
}

void DFMapPtrToPtr::RemoveAll()
{
//	DFASSERT(this);

	if(m_pHashTable != NULL)
	{
		delete [] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

DFMapPtrToPtr::DFAssoc* DFMapPtrToPtr::NewAssoc()
{
	if(m_pFreeList == NULL)
	{
		DFPlex* newBlock = DFPlex::Create(m_pBlocks, m_nBlockSize, sizeof(DFMapPtrToPtr::DFAssoc));
		DFMapPtrToPtr::DFAssoc* pAssoc = (DFMapPtrToPtr::DFAssoc*) newBlock->data();
		pAssoc += m_nBlockSize - 1;
		for(int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
//	DFASSERT(m_pFreeList != NULL);

	DFMapPtrToPtr::DFAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
//	DFASSERT(m_nCount > 0);

	pAssoc->Key = 0;
	pAssoc->Value = 0;

	return pAssoc;
}

void DFMapPtrToPtr::FreeAssoc(DFMapPtrToPtr::DFAssoc* pAssoc)
{
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
//	DFASSERT(m_nCount >= 0);

	if(m_nCount == 0)
		RemoveAll();
}

DFMapPtrToPtr::DFAssoc* DFMapPtrToPtr::GetAssocAt(void* Key, UINT& nHash) const
{
	nHash = HashKey(Key) % m_nHashTableSize;

	if(m_pHashTable == NULL)
		return NULL;

	DFAssoc* pAssoc;
	for(pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if(pAssoc->Key == Key)
			return pAssoc;
	}

	return NULL;
}

void* DFMapPtrToPtr::GetValueAt(void* Key) const
{
	if(m_pHashTable == NULL)
		return NULL;

	UINT nHash = HashKey(Key) % m_nHashTableSize;

	DFAssoc* pAssoc;
	for(pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if(pAssoc->Key == Key)
			return pAssoc->Value;
	}

	return NULL;
}

BOOL DFMapPtrToPtr::Lookup(void* Key, void*& rValue) const
{
//	DFASSERT(this);

	UINT nHash;
	DFAssoc* pAssoc = GetAssocAt(Key, nHash);
	if(pAssoc == NULL)
		return FALSE;

	rValue = pAssoc->Value;

	return TRUE;
}

void*& DFMapPtrToPtr::operator[](void* Key)
{
//	DFASSERT(this);

	UINT nHash;
	DFAssoc* pAssoc;
	if((pAssoc = GetAssocAt(Key, nHash)) == NULL)
	{
		if(m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);
		pAssoc = NewAssoc();

		pAssoc->Key = Key;

		pAssoc->pNext = m_pHashTable[nHash];

		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->Value;
}

BOOL DFMapPtrToPtr::RemoveKey(void* Key)
{
//	DFASSERT(this);

	if(m_pHashTable == NULL)
		return FALSE;
	
	DFAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey(Key) % m_nHashTableSize];

	DFAssoc* pAssoc;
	for(pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if(pAssoc->Key == Key)
		{
			*ppAssocPrev = pAssoc->pNext;
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;
}

void DFMapPtrToPtr::GetNextAssoc(POSITION& rNextPosition, void*& rKey, void*& rValue) const
{
//	DFASSERT(this);
//	DFASSERT(m_pHashTable != NULL);

	DFAssoc* pAssocRet = (DFAssoc*) rNextPosition;
//	DFASSERT(pAssocRet != NULL);

	if(pAssocRet == (DFAssoc*) BEFORE_START_POSITION)
	{
		for(UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		{
			if((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		}
//		DFASSERT(pAssocRet != NULL);
	}

//	DFASSERT(!IsBadReadPtr(pAssocRet, sizeof(DFAssoc)));

	DFAssoc* pAssocNext;
	if((pAssocNext = pAssocRet->pNext) == NULL)
	{
		for(UINT nBucket = (HashKey(pAssocRet->Key) % m_nHashTableSize) + 1;
			nBucket < m_nHashTableSize; nBucket++)
		{
			if((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
		}
	}

	rNextPosition = (POSITION) pAssocNext;

	rKey = pAssocRet->Key;
	rValue = pAssocRet->Value;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
DFMapStringToPtr::DFPlex* PASCAL 
DFMapStringToPtr::DFPlex::Create(DFMapStringToPtr::DFPlex*& pHead, UINT nMax, UINT cbElement)
{
	DFMapStringToPtr::DFPlex* p = 
		(DFMapStringToPtr::DFPlex*) new BYTE[sizeof(DFMapStringToPtr::DFPlex) + nMax * cbElement];
//	DFASSERT(p);
			// may throw exception
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

void DFMapStringToPtr::DFPlex::FreeDataChain()     // free this one and links
{
	DFMapStringToPtr::DFPlex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		DFMapStringToPtr::DFPlex* pNext = p->pNext;
		delete[] bytes;
		p = pNext;
	}
}


DFMapStringToPtr::DFMapStringToPtr(int nBlockSize)
{
//	DFASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

inline UINT DFMapStringToPtr::HashKey(LPCTSTR key) const
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

void DFMapStringToPtr::InitHashTable(UINT nHashSize, BOOL bAllocNow)
{
//	DFASSERT(this);
//	DFASSERT(m_nCount == 0);
//	DFASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new DFAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(DFAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

void DFMapStringToPtr::RemoveAll()
{
//	DFASSERT(this);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete [] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

DFMapStringToPtr::~DFMapStringToPtr()
{
	RemoveAll();
//	DFASSERT(m_nCount == 0);
}

int DFMapStringToPtr::GetCount() const
{
	return m_nCount;
}

inline BOOL DFMapStringToPtr::IsEmpty() const
{
	return(m_nCount == 0);
}

void DFMapStringToPtr::SetAt(LPCTSTR key, void* newValue)
{ 
	(*this)[key] = newValue; 
}

inline UINT DFMapStringToPtr::GetHashTableSize() const
{ 
	return m_nHashTableSize; 
}

POSITION DFMapStringToPtr::GetStartPosition() const
{
	return (m_nCount == 0) ? NULL : BEFORE_START_POSITION;
}

DFMapStringToPtr::DFAssoc* DFMapStringToPtr::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		DFPlex* newBlock = DFPlex::Create(m_pBlocks, m_nBlockSize,
							sizeof(DFMapStringToPtr::DFAssoc));
		// chain them into free list
		DFMapStringToPtr::DFAssoc* pAssoc =
				(DFMapStringToPtr::DFAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
//	DFASSERT(m_pFreeList != NULL);  // we must have something

	DFMapStringToPtr::DFAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
//	DFASSERT(m_nCount > 0);  // make sure we don't overflow
	memset(&pAssoc->key, 0, MAX_KEY_LEN+1);

	pAssoc->value = 0;

	return pAssoc;
}

void DFMapStringToPtr::FreeAssoc(DFMapStringToPtr::DFAssoc* pAssoc)
{
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
//	DFASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

DFMapStringToPtr::DFAssoc* DFMapStringToPtr::GetAssocAt(LPCTSTR key, UINT& nHash) const
{
	nHash = HashKey(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	DFAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (lstrcmp(pAssoc->key, key) == 0)
			return pAssoc;
	}
	return NULL;
}

BOOL DFMapStringToPtr::Lookup(LPCTSTR key, void*& rValue) const
{
//	DFASSERT(this);

	UINT nHash;
	DFAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

BOOL DFMapStringToPtr::LookupKey(LPCTSTR key, LPCTSTR& rKey) const
{
//	DFASSERT(this);

	UINT nHash;
	DFAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rKey = pAssoc->key;
	return TRUE;
}

void*& DFMapStringToPtr::operator[](LPCTSTR key)
{
//	DFASSERT(this);

	UINT nHash;
	DFAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		memcpy(pAssoc->key, key, MAX_KEY_LEN+1);
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}


BOOL DFMapStringToPtr::RemoveKey(LPCTSTR key)
{
//	DFASSERT(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	DFAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

	DFAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (lstrcmp(pAssoc->key, key) == 0 )
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

void DFMapStringToPtr::GetNextAssoc(POSITION& rNextPosition,
	LPCTSTR& rKey, void*& rValue) const
{
//	DFASSERT(this);
//	DFASSERT(m_pHashTable != NULL);  // never call on empty map

	DFAssoc* pAssocRet = (DFAssoc*)rNextPosition;
//	DFASSERT(pAssocRet != NULL);

	if (pAssocRet == (DFAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
//		DFASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
//	DFASSERT(!IsBadReadPtr(pAssocRet, sizeof(DFAssoc)));
	DFAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	memcpy(&rKey, pAssocRet->key, MAX_KEY_LEN+1);
	rValue = pAssocRet->value;
}


	