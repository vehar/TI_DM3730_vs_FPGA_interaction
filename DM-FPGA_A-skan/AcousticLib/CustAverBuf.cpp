#include "CustAverBuf.h"

template <class T>
CustAverBuf<T>::CustAverBuf(UCHAR _averSize, UCHAR _averMaxSize, USHORT _bufSize, USHORT _maxBufSize)
{
	if(_averSize == 0) _averSize = 1;
	if(_averMaxSize < _averSize) _averMaxSize = _averSize;
	if(_bufSize == 0) _bufSize = 1;
	if(_maxBufSize < _bufSize) _bufSize = _maxBufSize;
	
	averSize	= _averSize;
	bufSize		= _bufSize;
	maxBufSize	= _maxBufSize;
	maxAverSize = _averMaxSize;
	sumBufIdx	= maxAverSize;
	insBufIdx	= maxAverSize + 1;
	resBufIdx	= maxAverSize + 2;
	pAverBuf	= new T*[maxAverSize+3];

	for(int i=0; i<maxAverSize+3; i++)
	{
		pAverBuf[i] = new T[_maxBufSize];
		resetBuf(pAverBuf[i], _maxBufSize);
	}
	extrBufIdx = 0;//-(averSize);
	filled = false;
}

template <class T>
CustAverBuf<T>::~CustAverBuf()
{
	for(int i=0; i<maxAverSize+3; i++)
		delete[] pAverBuf[i];
	delete [] pAverBuf;
};

template <class T>
void CustAverBuf<T>::useInsBuf()
{
	if( averSize != 1)
	{
		for(int i=0; i<bufSize; i++)
		{
			pAverBuf[sumBufIdx][i] -= pAverBuf[extrBufIdx][i];
			pAverBuf[sumBufIdx][i] += pAverBuf[insBufIdx][i];
			pAverBuf[resBufIdx][i] =  pAverBuf[sumBufIdx][i] / averSize;
		}

		if(extrBufIdx >= 0)
		{
			T *tempBuf = pAverBuf[extrBufIdx];
			pAverBuf[extrBufIdx] = pAverBuf[insBufIdx];
			pAverBuf[insBufIdx] = tempBuf;
		}

		extrBufIdx++;
		if(extrBufIdx == averSize)
		{
			extrBufIdx = 0;
			filled = true;
		}
	}
	else
	{
		filled = true;
	}
};


template <class T>
void CustAverBuf<T>::setBufSize(USHORT _bufSize)
{
	if(_bufSize> maxBufSize) _bufSize = maxBufSize;
	if(_bufSize != bufSize)
	{
		bufSize = _bufSize;
		resetData();
	}
};

template <class T>
void CustAverBuf<T>::setAverSize(UCHAR _averSize)
{
	if(_averSize > maxAverSize) _averSize = maxAverSize;
	if(_averSize != averSize)
	{
		averSize = _averSize;
		resetData();
	}
};

template <class T>
void CustAverBuf<T>::resetData()
{
	extrBufIdx = 0;
	filled = false;
	for(int i=0;i<averSize;i++)
		resetBuf(pAverBuf[i], bufSize);
	resetBuf(pAverBuf[resBufIdx], bufSize);
	resetBuf(pAverBuf[sumBufIdx], bufSize);
}

template <class T>
void CustAverBuf<T>::resetBuf(T *buf, int size)
{
	memset(buf,0, size*sizeof(T));
};

template <class T>
UCHAR CustAverBuf<T>::getLastChangedIdx()
{
	if(extrBufIdx == 0) return averSize-1;
	else
		return extrBufIdx-1;
};

template <class T>
T* CustAverBuf<T>::getResBuf() 
{
	if(averSize == 1)
		return pAverBuf[insBufIdx];
	else
		if(filled)
			return pAverBuf[resBufIdx];
		else
			return pAverBuf[getLastChangedIdx()];
};

