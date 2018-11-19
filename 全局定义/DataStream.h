#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
//#include "GameLog.h"


#define Stream_VALUE(Name)	\
	if(bSend)			\
{                           \
	kData.pushValue(Name);\
}		\
else\
{\
	kData.popValue(Name);\
}\

#define Stream_VALUE_SYSTEMTIME(Name)	\
	if(bSend)			\
{                           \
	kData.pushValue(&Name,sizeof(SYSTEMTIME));\
}	\
	else\
{\
	kData.popValue(&Name,sizeof(SYSTEMTIME));\
}\

#define StructVecotrMember(TypeMem,Member)\
{\
	if (!bSend)\
{\
	int iCout = 0;\
	kData.popValue(iCout);\
	for (int i = 0;i<iCout;i++)\
{\
	TypeMem kTempValue;\
	kTempValue.StreamValue(kData,bSend);\
	this->Member.push_back(kTempValue);\
}\
}\
	else\
{\
	int iCout = this->Member.size();\
	kData.pushValue(iCout);\
	for (int i = 0;i<iCout;i++)\
{\
	this->Member[i].StreamValue(kData,bSend);\
}\
}\
}

#define StructVecotr(TypeMem,Member)\
{\
	if (!bSend)\
			{\
		kData.popValue<TypeMem>(Member);\
			}\
				else\
				{\
		kData.pushValue<TypeMem>(Member);\
				}\
}

class DataStream :public std::vector<char>
{
public:
	DataStream(){}
	DataStream(void* pData,int dSize)
	{
		pushValue((char*)pData,dSize);
	}
	char* data()
	{
		return &((*this)[0]);
	}
	DataStream& pushValue(unsigned short value)
	{
		push(value);
		return *this;
	}  
	DataStream& popValue(unsigned short& value)
	{
		return pop(value);
	} 
	DataStream& pushValue(DataStream& value)
	{
		push(int(value.size()));
		if (!value.size())
		{
			return *this;
		}
		memcpy(inc_size(value.size()), (void*)&value[0], value.size());
		return *this;
	}  
	DataStream& popValue(DataStream& value)
	{
		if (size() < sizeof(int))
		{
			return *this;
		}

		int nSize = 0;
		pop(nSize);
		if (nSize == 0)
		{
			return *this;
		}
		if (nSize > (int)size())
		{
			return *this;
		}
		std::vector<char>::iterator first=begin(), last=first+nSize;
		value.assign(first, last);
		erase(first, last);
		return *this;
	} 
	DataStream& pushValue(unsigned int value)
	{
		push(value);
		return *this;
	}  
	DataStream& popValue(unsigned int& value)
	{
		return pop(value);
	}  

	DataStream& pushValue(int value)
	{
		push(value);
		return *this;
	}  
	DataStream& popValue(int& value)
	{
		return pop(value);
	} 

	DataStream& pushValue(const std::string& data)
	{
		push(int(data.size()));
		if (!data.size())
		{
			return *this;
		}
		memcpy(inc_size(data.size()), (void*)data.c_str(), data.size());
		return *this;
	}    
	DataStream& popValue(std::string& data)
	{
		if (size() < sizeof(int))
		{
			return *this;
		}

		int nSize = 0;
		pop(nSize);
		if (nSize == 0)
		{
			return *this;
		}
		std::vector<char>::iterator first=begin(), last=first+nSize;
		data.assign(first, last);
		erase(first, last);
		return *this;
	}
	DataStream& pushValue(long long value)
	{
		push(value);
		return *this;
	}  
	DataStream& popValue(long long& value)
	{
		return pop(value);
	} 

	DataStream& pushValue(unsigned long long value)
	{
		push(value);
		return *this;
	}  
	DataStream& popValue(unsigned long long& value)
	{
		return pop(value);
	} 
	DataStream& pushValue(float value)
	{
		return push(value);
	}  
	DataStream& popValue(float& value)
	{
		return pop(value);
	} 
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	DataStream& pushValue(unsigned long value)
	{
		push((unsigned int)value);
		return *this;
	}  
	DataStream& popValue(unsigned long& value)
	{
		if (size() <= sizeof(unsigned int))
		{
			return *this;
		}

		unsigned int kTempValue = 0; 
		pop(kTempValue);
		value = kTempValue;
		return *this;
	} 
#endif
	DataStream& pushValue(unsigned char value)
	{
		return push(value);
	}  
	DataStream& popValue(unsigned char& value)
	{
		return pop(value);
	}
	DataStream& pushValue(void* value,int iSize)
	{
		if (iSize == 0)
		{
			return *this;
		}
		memcpy(inc_size(iSize), value, iSize);
		return *this;
	}  
	DataStream& popValue(void* value,unsigned int iSize)
	{
		if (size() < iSize)
		{
			return *this;
		}
		memcpy(value, &operator[](0), iSize);
		erase(begin(), begin()+iSize);
		return *this;
	}
	DataStream& pushValue(bool value)
	{
		return push(value);
	}  
	DataStream& popValue(bool& value)
	{
		return pop(value);
	}

	//std::vector´¦Àí
	template<typename C>
	DataStream& pushValue(const std::vector<C>& data)
	{
		unsigned int nSize=data.size();
		pushValue(nSize);
		for (size_t i=0; i<nSize; ++i)
		{
			pushValue(data[i]);
		}
		return *this;
	}
	template<typename C>
	DataStream& popValue(std::vector<C>& data)
	{
		unsigned int nSize = 0;
		popValue(nSize);
		for (size_t i=0; i<nSize; ++i)
		{
			C tmp;
			popValue(tmp);
			data.push_back(tmp);
		}            
		return *this;
	}
private:

	char* inc_size(size_t delta_size)
	{
		size_t last_size=size();
		resize(last_size+delta_size);
		return &operator[](last_size);
	}
	DataStream& popSize(int iSize)
	{
		if (iSize<=0)
		{
			return *this;
		}
		if (iSize>(int)size())
		{
			return *this;
		}
		erase(begin(), begin()+iSize);
		return *this;
	}
	template<typename C>
	DataStream& push(C data)
	{
		memcpy(inc_size(sizeof(data)), &data, sizeof(data));
		return *this;
	}

	template<typename C>
	DataStream& pop(C& data)
	{
		if (size() < sizeof(data))
		{
			return *this;
		}
		memcpy(&data, &operator[](0), sizeof(data));
		erase(begin(), begin()+sizeof(data));
		return *this;
	}
};