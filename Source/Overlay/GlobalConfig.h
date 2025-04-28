#pragma once

enum class Language1
{
	Chinese,
	English
};

Language1 currentLanguage1 = Language1::Chinese;

const char* languages[] = { U8("中文"), "EN" };

static const char* Translate(const char* chinese, const char* english)
{
	return currentLanguage1 == Language1::Chinese ? chinese : english;
}
int currentLanguageIndex = 0;  // 定义全局变量