#pragma once
#include<Windows.h>
#include <boost/locale.hpp>
#include <string>
#include <stdexcept>

//ͨ�ù���
namespace general
{
	//debug���
	inline void print(LPCSTR str)
	{
		OutputDebugStringA(str);
	}

	//��ȡ��ǰ����
	inline int GetTime()
	{
		return GetTickCount64();
	}

	//תUTF8����
	inline std::string convertToUTF8(const std::string& input, const std::string& fromEncoding) {
		try {
			return boost::locale::conv::to_utf<char>(input, fromEncoding);
		}
		catch (const boost::locale::conv::conversion_error&) {
			throw std::runtime_error("Conversion to UTF-8 failed");
		}
	}

	


}