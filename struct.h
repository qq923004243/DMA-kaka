#pragma once

struct v4
{
	float x;
	float y;
	float z;
	float w;
};

struct v3
{
	float x;
	float y;
	float z;
};

struct v2
{
	float x;
	float y;
};

struct player
{
	std::string name;
	int kill;
	std::string id;
	UINT64 obj;
	UINT64 mesh;
};

struct car
{
	std::string name;
	int color;
	UINT64 obj;
};

struct item
{
	std::string name;
	int color;
	UINT64 obj;
};



// 封包数据结构
//封包结构中的dwValue  
//
struct CustomData {
	DWORD dwValue;
	float x, y, z;
	std::string name;
	float view;

	std::vector<std::string> positions;
};

