#pragma once
#include "windows.h"

template <class T>
class CustAccumType
{
	T sum;
	UINT count;
	CRITICAL_SECTION cs;

public:
	CustAccumType() 
	{
		InitializeCriticalSection(&cs);
		reset();
	}

	~CustAccumType()
	{
		DeleteCriticalSection(&cs);	
	}

	void reset() 
	{ 
		sum=(T)0; 
		count = 0;
	} 
/*
	void operator+=(T val) 
	{                           
		return *this; 
	}
*/
	void operator= (T& val)
	{
		EnterCriticalSection(&cs);
		sum += val;
		count++;
		LeaveCriticalSection(&cs);
	}

	void getVal() 
	{
		if(count == 0) return 0;
		return sum/count;
	}
    
	UINT getCount()
	{
		return count;
	}

	operator T() 
	{
		T result;
		EnterCriticalSection(&cs);
		if(count == 0) result = 0;
        else	
		{
			result = sum/(count); 
			reset();
		}
		LeaveCriticalSection(&cs);
		return result;
	}
};



