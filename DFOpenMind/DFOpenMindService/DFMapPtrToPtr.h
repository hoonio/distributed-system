#ifndef __DFMAPPTRTOPTR_H__
#define __DFMAPPTRTOPTR_H__

#pragma once

// MFC �� 2���� Map Ŭ������ ������ �ҽ���... �Ф�
// hashing�� �ϱⰡ �����Ƽ�.. �Ф�


struct __POSITION { };
typedef __POSITION* POSITION;
#define BEFORE_START_POSITION ((POSITION)-1L)

///////////////////////////////////////////////////////////////////////////////
class DFMapPtrToPtr
{
public:
	DFMapPtrToPtr(int nBlockSize = 10);
	~DFMapPtrToPtr();
	///////////////////////////////////////////////////////////////////////////////
	struct DFPlex     // warning variable length structure
	{
		DFPlex* pNext;

		DWORD dwReserved[1];    // align on 8 byte boundary
		
		void* data() { return this+1; }

		static DFPlex* PASCAL Create(DFPlex*& head, UINT nMax, UINT cbElement);
				// like 'calloc' but no zero fill
				// may throw memory exceptions

		void FreeDataChain();       // free this one and links
	};
protected:
	struct DFAssoc
	{
		DFAssoc*	pNext;
		void*		Key;
		void*		Value;
	};

	DFAssoc**		m_pHashTable;
	UINT			m_nHashTableSize;
	int				m_nCount;
	DFAssoc*		m_pFreeList;
	struct	DFPlex* m_pBlocks;
	int				m_nBlockSize;

	DFAssoc*		NewAssoc();
	void			FreeAssoc(DFAssoc* pAssoc);
	DFAssoc*		GetAssocAt(void* , UINT& ) const;

public:
	int				GetCount() const;
	BOOL			IsEmpty() const;

	BOOL			Lookup(void* Key, void*& rValue) const;

	void*&			operator[] (void* Key);
	void			SetAt(void* Key, void* NewValue);

	BOOL			RemoveKey(void* Key);
	void			RemoveAll();

	POSITION		GetStartPosition() const;
	void			GetNextAssoc(POSITION& rNextPosition, void*& rKey, void*& rValue) const;

	UINT			GetHashTableSize() const;
	void			InitHashTable(UINT HashSize, BOOL bAllocNow = TRUE);

	UINT			HashKey(void* Key) const;

	void*			GetValueAt(void* Key) const;


};

// Ű�� �Ǵ� ���ڿ��� �ִ� ���̸� �����Ѵ�.
#define MAX_KEY_LEN		31//MAX_PATH

/////////////////////////////////////////////////////////////////////////////


// �ִ� MAX_KEY_LEN ��ŭ ���ڼ��� ���� ���ڿ��� Ű�� ���´�.
class DFMapStringToPtr
{
public:
	DFMapStringToPtr(int nBlockSize = 10);
	~DFMapStringToPtr();

	///////////////////////////////////////////////////////////////////////////////
	struct DFPlex     // warning variable length structure
	{
		DFPlex* pNext;

		DWORD dwReserved[1];    // align on 8 byte boundary
		
		void* data() { return this+1; }

		static DFPlex* PASCAL Create(DFPlex*& head, UINT nMax, UINT cbElement);
				// like 'calloc' but no zero fill
				// may throw memory exceptions

		void FreeDataChain();       // free this one and links
	};

protected:
	// Association
	struct DFAssoc
	{
		DFAssoc*	pNext;
		UINT		nHashValue;  // needed for efficient iteration
		TCHAR		key[MAX_KEY_LEN+1];
		void*		value;
	};

public:
// Attributes
	// number of elements
	int				GetCount() const;
	BOOL			IsEmpty() const;

	// Lookup
	BOOL			Lookup(LPCTSTR key, void*& rValue) const;
	BOOL			LookupKey(LPCTSTR key, LPCTSTR& rKey) const;

// Operations
	// Lookup and add if not there
	void*&			operator[](LPCTSTR key);

	// add a new (key, value) pair
	void			SetAt(LPCTSTR key, void* newValue);

	// removing existing (key, ?) pair
	BOOL			RemoveKey(LPCTSTR key);
	void			RemoveAll();

	// iterating all (key, value) pairs
	POSITION		GetStartPosition() const;
	void			GetNextAssoc(POSITION& rNextPosition, LPCTSTR& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT			GetHashTableSize() const;
	void			InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	// Routine used to user-provided hash keys
	UINT			HashKey(LPCTSTR key) const;

protected:
	DFAssoc**		m_pHashTable;
	UINT			m_nHashTableSize;
	int				m_nCount;
	DFAssoc*		m_pFreeList;
	struct DFPlex*	m_pBlocks;
	int				m_nBlockSize;

	DFAssoc*		NewAssoc();
	void			FreeAssoc(DFAssoc*);
	DFAssoc*		GetAssocAt(LPCTSTR, UINT&) const;
};

#endif
