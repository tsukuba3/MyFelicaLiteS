#pragma once

#include <Windows.h>
#include "felica_nfc_library.h"
#include <jni.h>

#pragma comment(lib ,"felica_nfc_library.lib")
#pragma warning(disable : 4996)

#define IDM_SIZE        (8)         // IDm のサイズ
#define BLOCK_SIZE      (16)        // ブロック（読み書きの最小単位）のサイズ

#define SYSTEM_CODE     (0x88b4)    // 0x88b4 = FeliCa Lite-S のシステムコード
#define SERVICE_CODE_RW (0x0009)    // 0x0009 = Random Read/Write のサービスコード
#define SERVICE_CODE_RO (0x000B)    // 0x000B = Random Read Only のサービスコード

void convJString(JNIEnv *env, const char *pBuff, int num);
bool MyPolling(unsigned short usSystemCode, unsigned char pIDm[IDM_SIZE]);
bool MyWriteWithoutEncryption(unsigned char *pIDm, unsigned short usServiceCode, unsigned char ucBlockOffset, unsigned char *pDataW);
bool MyReadWithoutEncryption(unsigned char *pIDm, unsigned short usServiceCode, unsigned char ucBlockOffset, unsigned char *pDataR);

extern "C"
{
	JNIEXPORT void JNICALL Java_Koneko_set(JNIEnv *env, jclass hoge, jobjectArray retObj, int num);
	int JNICALL Java_Koneko_felica(JNIEnv *env, jobject hoge, int rw, jstring wData, int block);
}