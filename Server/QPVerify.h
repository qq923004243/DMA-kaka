#pragma once

#include <iostream>
#include <Windows.h>
using namespace std;

#define __out__
#define USER_NORMAL 0
#define USER_LOCK 1
//#define QPEXPORT extern "C" __declspec(dllexport)
typedef void(_stdcall* PFN_GetMachineCode)(char pMem[1024]);
bool QPLibInit(BOOL(*pfnLoadDllFromRes)(PVOID* pDllData, DWORD* dwDllSize));
bool QPInit(const char* ip, unsigned short port, const char* soft = "Õ®”√", const char* version = "1", int commKey = 0);
bool QPGetBulletin(char** ppbuf);
void QPFree(char* p);
void QPSetMachineFeature(char* pFeature, int len);
char* QPGetLastErrorString();
int QPGetLastErrorCode();
bool QPKeyLogin(char* pKey);
bool QPGetVersion(
	__out__  char** ppVersion,
	__out__  char** ppUpdateAddress,
	__out__  char** ppUpdateContent,
	__out__  bool* pbForceUpdating,
	__out__  bool* pbSoftStop
);
char* QPGetLoginData(int code);
bool QPGetHearBeatConfig(bool* pbStart, bool* pbAutoExit, int* pHearMs);
void QPLocalExpVerify();
bool QPSendHearBeat();
int QPToInt(char* p);
bool QPUnBind(
	char* userorkey,
	bool unbindMode,
	__out__ int* unbindSubTime,
	__out__ int* surplusUnbindNumber,
	__out__ char** oldExpTime,
	__out__ char** newExpTime,
	__out__ char** oldSurplusTime,
	__out__ char** newSurplusTime
);
bool QPQueryKeyInfo(
	const char* key,
	__out__ int* statuscode,
	__out__ char** exptime,
	__out__ char** surplusTime,
	__out__ int* keyTime,
	__out__ int* timeType
);
char* QPGetServerTime();
bool QPUserLogin(const char* user, const char* pass);
bool QPUserRegister(
	const char* user,
	const char* pass,
	const char* key,
	const char* em,
	const char* superPass,
	const char* bindInfo,
	__out__ char** surplusTime);
bool QPUserSetPass(const char* user, const char* superPass, const char* newPass);
bool QPUserPay(const char* user, const char* key);
bool QPUserQueryInfo(const char* user, __out__ int* statusCode, __out__ char** expTime, __out__ char** surplusTime);
bool QPGetVars(const char* name, char** val);
bool QPGetPointNumber(int* surplusPoints);
int QPDeductPoints(int deductPoints, int* surplusPoints);
bool QPLeaveLogin();
char* QPCallRemoteJS(const char* jsCmd);
bool QPCloudCalc(
	const char* cmd_string,
	const char* cmd_bytes,
	int cmd_bytes_len,
	int sign,
	int cloudMode,
	__out__ char** result_string,
	__out__ char** result_bytes,
	__out__ int* result_bytes_len
);
bool QPCalcVMProtectSerial(
	char* product,
	char* machineId,
	__out__ char** serial
);
bool QPRefRemoteFile();
bool QPDownRemoteFile(const char* fname, char** fdata, int* len);
bool QPGetVar(char** val);
const char* QPGetStatusC(int nIdx);
const char* QPGetTimeC(int nIdx);

char* SecToDay(double s);
bool CmpTime(char* t1, char* t2);
char* SecToDayForTimeInterForSec(char* t1, char* t2);
void QPSetGetMachineCallBack(PFN_GetMachineCode pfnGetMachineCode);
char* QPGetMachineCode();

