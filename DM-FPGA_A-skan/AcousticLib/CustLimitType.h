#pragma once
#include "windows.h"

template <class T>
class CustLimitType
{
	T minVal, maxVal, val;
public:
	CustLimitType(){}
	CustLimitType(T _minVal, T _maxVal)
	{
		init(_minVal, _maxVal);
	}
	CustLimitType(T _minVal, T _maxVal, T _val)
	{
		init(_minVal, _maxVal, _val);
	}
	T getMinVal() {return minVal;}
	T getMaxVal() {return maxVal;}
	//T get() {return val;}

	void setVal(T val)
	{
		*this = val;
	}
	void operator=(T& _val)
	{
		if(_val > maxVal)  
			_val = val = maxVal;
	   else
		   if(_val<minVal) 
			   _val = val = minVal;
		   else
			val = _val;
	}


	void init(T _minVal, T _maxVal)
	{
		minVal = _minVal;
		maxVal = _maxVal;
	}

	void init(T _minVal, T _maxVal, T _val)
	{
		init(_minVal, _maxVal);
		*this = _val;
	}

	operator T() const
	{
		return val;
	}
};

//#include "CustAverBuf.cpp" //to avoid lnk errors, using templates

