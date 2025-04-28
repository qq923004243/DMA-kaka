#pragma once
#include<Windows.h>
#include <boost/locale.hpp>
#include <string>
#include <stdexcept>

//通用功能
namespace general
{
	//debug输出
	inline void print(LPCSTR str)
	{
		OutputDebugStringA(str);
	}

	//获取当前毫秒
	inline int GetTime()
	{
		return GetTickCount64();
	}

	//转UTF8编码
	inline std::string convertToUTF8(const std::string& input, const std::string& fromEncoding) {
		try {
			return boost::locale::conv::to_utf<char>(input, fromEncoding);
		}
		catch (const boost::locale::conv::conversion_error&) {
			throw std::runtime_error("Conversion to UTF-8 failed");
		}
	}

	


}