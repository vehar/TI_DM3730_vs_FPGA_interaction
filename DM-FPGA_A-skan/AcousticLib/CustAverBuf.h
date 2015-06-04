#pragma once
#include "windows.h"

template <class T>
class CustAverBuf
{
	T **pAverBuf;
	UCHAR averSize, maxAverSize;
	USHORT bufSize, maxBufSize;
	UCHAR sumBufIdx, insBufIdx, resBufIdx, extrBufIdx;

	void resetBuf(T *buf, int size);
	void resetData();
	UCHAR getLastChangedIdx();
	bool filled;
public:
	CustAverBuf(UCHAR _averSize, UCHAR _averMaxSize, USHORT _bufSize, USHORT _maxBufSize);
	~CustAverBuf();
	void useInsBuf();
	T *getInsBuf() {return pAverBuf[insBufIdx];}
	T *getResBuf();
	USHORT getBufSize() {return bufSize;}
	void setBufSize(USHORT _bufSize);
	void setAverSize(UCHAR _averSize);
	bool isFilled() {return filled;}
};

#include "CustAverBuf.cpp" //to avoid lnk errors, using templates

