// FelicaRW.cpp : 実装ファイル
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FelicaRW.h"

#pragma warning(disable : 4996)

unsigned char account[16];
unsigned char domain[16];
unsigned char user_id[16];
unsigned char password[16];
unsigned char name[16];

void JNICALL Java_Koneko_set(JNIEnv *env, jclass hoge, jobjectArray retObj, int num)
{
	jstring jstr = NULL;
	int len = 0;
	int uniLen = 0;
	WCHAR *retBuff = NULL;

	const char *pBuffA = (const char*)account;
	len = strlen(pBuffA);

	// Unicode文字列の長さを取得
	uniLen = MultiByteToWideChar(CP_ACP, 0, pBuffA, len, NULL, 0);

	retBuff = new WCHAR[uniLen];

	// Unicode文字列に変換
	MultiByteToWideChar(CP_ACP, 0, pBuffA, len, retBuff, uniLen);

	// Javaの文字列生成
	jstr = env->NewString((jchar *)retBuff, uniLen);

	delete retBuff;

	// 配列の指定インデックスに文字列をセット
	env->SetObjectArrayElement(retObj, 0, jstr);

	const char *pBuffD = (const char*)domain;
	len = strlen(pBuffD);

	// Unicode文字列の長さを取得
	uniLen = MultiByteToWideChar(CP_ACP, 0, pBuffD, len, NULL, 0);

	retBuff = new WCHAR[uniLen];

	// Unicode文字列に変換
	MultiByteToWideChar(CP_ACP, 0, pBuffD, len, retBuff, uniLen);

	// Javaの文字列生成
	jstr = env->NewString((jchar *)retBuff, uniLen);

	delete retBuff;

	// 配列の指定インデックスに文字列をセット
	env->SetObjectArrayElement(retObj, 1, jstr);

	const char *pBuffU = (const char*)user_id;
	len = strlen(pBuffU);

	// Unicode文字列の長さを取得
	uniLen = MultiByteToWideChar(CP_ACP, 0, pBuffU, len, NULL, 0);

	retBuff = new WCHAR[uniLen];

	// Unicode文字列に変換
	MultiByteToWideChar(CP_ACP, 0, pBuffU, len, retBuff, uniLen);

	// Javaの文字列生成
	jstr = env->NewString((jchar *)retBuff, uniLen);

	delete retBuff;

	// 配列の指定インデックスに文字列をセット
	env->SetObjectArrayElement(retObj, 2, jstr);

	const char *pBuffP = (const char*)password;
	len = strlen(pBuffP);

	// Unicode文字列の長さを取得
	uniLen = MultiByteToWideChar(CP_ACP, 0, pBuffP, len, NULL, 0);

	retBuff = new WCHAR[uniLen];

	// Unicode文字列に変換
	MultiByteToWideChar(CP_ACP, 0, pBuffP, len, retBuff, uniLen);

	// Javaの文字列生成
	jstr = env->NewString((jchar *)retBuff, uniLen);

	delete retBuff;

	// 配列の指定インデックスに文字列をセット
	env->SetObjectArrayElement(retObj, 3, jstr);

	const char *pBuffN = (const char*)name;
	
	jstr = env->NewStringUTF(pBuffN);

	// 配列の指定インデックスに文字列をセット
	env->SetObjectArrayElement(retObj, 4, jstr);
}

bool PrintNfcLastErr(char* pFuncName)
{
	unsigned long pErrInfo[2];
	bool bSuccess;

	bSuccess = felicalib_nfc_get_last_error(pErrInfo);
	if (bSuccess)
	{
		printf("%s でエラー発生 [%x, %x].\n",
			pFuncName,
			pErrInfo[0],    // NFCアクセスライブラリエラーコード
			pErrInfo[1]);   // ドライバエラーコード
	}
	else
	{
		printf("%s でエラー発生. エラー情報取得失敗.\n",
			pFuncName);
	}

	return true;
}

bool MyPrintHex(unsigned char *pBuf, int iLen) {

	char buf[16];
	for (int i = 0; i < iLen; i++)
	{
		sprintf(buf, "%02x ", pBuf[i]);
		//strcat(po, buf);
	}
	sprintf(buf, "%s", "\r\n");
	//strcat(po, buf);

	return true;
}

bool MyPolling(unsigned short usSystemCode,
	unsigned char pIDm[IDM_SIZE]) {
	bool bSuccess;

	// Polling コマンドの組み立て
	unsigned char pCom[] = {
		0x00,           // データ長
		0x00,           // コマンドコード
		0x00, 0x00,     // システムコード
		0x00,           // リクエストコード
		0x00            // タイムスロット
	};
	unsigned char pRes[512];

	unsigned short usComLen = sizeof(pCom);
	unsigned short usResLen = sizeof(pRes);

	pCom[0] = sizeof(pCom);

	pCom[2] = usSystemCode >> 8;
	pCom[3] = usSystemCode & 0xFF;

	// コマンドの発行
	bSuccess = felicalib_nfc_thru(pCom, usComLen, pRes, &usResLen);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_thru(polling)");
		return false;
	}

	// レスポンスのチェック
	if (usResLen != 1 + 1 + 8 + 8)
	{
		printf("polling のレスポンス長 %d が不正 #1\n", usResLen);
		return false;
	}
	if (pRes[0] != 1 + 1 + 8 + 8)
	{
		printf("polling のレスポンス長 %d が不正 #2\n", pRes[0]);
		return false;
	}
	if (pRes[1] != 0x01)
	{
		printf("polling のレスポンスコード %d が不正\n", pRes[1]);
		return false;
	}

	// IDm のコピー
	memcpy_s(pIDm, IDM_SIZE, &pRes[2], IDM_SIZE);

	return true;
}

/*
bSuccess = MyWriteWithoutEncryption(
pIDm,                       // Polling コマンドで取得した IDm IN
SERVICE_CODE_RW,            // サービスコード IN
pBlockNum[iBlockCount],     // ブロック番号 IN
pDataR);                    // 読み出したデータの格納場所 OUT
*/

bool MyWriteWithoutEncryption(unsigned char *pIDm,
	unsigned short usServiceCode,
	unsigned char ucBlockOffset,
	unsigned char *pDataW) {
	bool bSuccess;

	// Write Without Encryption コマンドの組み立て
	unsigned char pCom[] = {
		0x00,                                           // データ長
		0x08,                                           // コマンドコード
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // IDm
		0x01,                                           // サービス数
		0x00, 0x00,                                     // サービスコード
		0x01,                                           // ブロック数
		0x00, 0x00, 0x00,                               // ブロック
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 任意のデータ
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 任意のデータ
	};

	unsigned char pRes[512];

	unsigned short usComLen = sizeof(pCom);
	unsigned short usResLen = sizeof(pRes);

	pCom[0] = sizeof(pCom);

	memcpy_s(&pCom[2], sizeof(pCom) - 2, pIDm, IDM_SIZE);

	pCom[11] = usServiceCode & 0xFF;
	pCom[12] = usServiceCode >> 8;

	pCom[15] = ucBlockOffset;

	memcpy_s(&pCom[17], sizeof(pCom) - 17, pDataW, BLOCK_SIZE);

	// コマンドの発行
	bSuccess = felicalib_nfc_thru(pCom, usComLen, pRes, &usResLen);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_thru(write without encryption)");
		return false;
	}

	// レスポンスのチェック
	if (usResLen != 1 + 1 + 8 + 1 + 1)
	{
		printf("write w/o enc のレスポンス長 %d が不正 #1\n", usResLen);
		return false;
	}
	if (pRes[0] != 1 + 1 + 8 + 1 + 1)
	{
		printf("write w/o enc のレスポンス長 %d が不正 #2\n", pRes[0]);
		return false;
	}
	if (pRes[1] != 0x09)
	{
		printf("write w/o enc のレスポンスコード %d が不正\n", pRes[1]);
		return false;
	}
	if (pRes[10] != 0x00)
	{
		printf("write w/o enc のステータスフラグが [%02x, %02x] のエラー\n", pRes[10], pRes[11]);
		return false;
	}

	return true;
}

bool MyReadWithoutEncryption(unsigned char *pIDm,
	unsigned short usServiceCode,
	unsigned char ucBlockOffset,
	unsigned char *pDataR) {
	bool bSuccess;

	// Read Without Encryption コマンドの組み立て
	unsigned char pCom[] = {
		0x00,                                           // データ長
		0x06,                                           // コマンドコード
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // IDm
		0x01,                                           // サービス数
		0x00, 0x00,                                     // サービスコード
		0x01,                                           // ブロック数
		0x00, 0x00, 0x00                                // ブロック
	};

	unsigned char pRes[512];

	unsigned short usComLen = sizeof(pCom);
	unsigned short usResLen = sizeof(pRes);

	pCom[0] = sizeof(pCom);

	memcpy_s(&pCom[2], sizeof(pCom) - 2, pIDm, IDM_SIZE);

	pCom[11] = usServiceCode & 0xFF;
	pCom[12] = usServiceCode >> 8;

	pCom[15] = ucBlockOffset;

	// コマンドの発行
	bSuccess = felicalib_nfc_thru(pCom, usComLen, pRes, &usResLen);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_thru(read without encryption)");
		return false;
	}

	// レスポンスのチェック
	if (usResLen < 1 + 1 + 8 + 1 + 1)
	{
		printf("read w/o enc のレスポンス長 %d が不正 #1\n", usResLen);
		return false;
	}
	if (pRes[0] < 1 + 1 + 8 + 1 + 1)
	{
		printf("read w/o enc のレスポンス長 %d が不正 #2\n", pRes[0]);
		return false;
	}
	if (pRes[1] != 0x07)
	{
		printf("read w/o enc のレスポンスコード %d が不正\n", pRes[1]);
		return false;
	}
	if (pRes[10] != 0x00)
	{
		printf("read w/o enc のステータスフラグが [%02x, %02x] のエラー\n", pRes[10], pRes[11]);
		return false;
	}

	// 読み出したデータのコピー
	memcpy_s(pDataR, BLOCK_SIZE, &pRes[13], BLOCK_SIZE);

	return true;
}

int JNICALL Java_Koneko_felica(JNIEnv *env, jobject hoge, int rw, jstring wData, int block)
{
	//strcpy(po, "IDm = ");

	bool bSuccess;
	unsigned char pIDm[IDM_SIZE];

	// ライブラリの初期化
	bSuccess = felicalib_nfc_initialize();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_initialize");
		goto L_End;
	}

	// ポートのオープン
	char szPort[16] = "";
	bSuccess = felicalib_nfc_open(szPort);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_open");
		goto L_Uninitialize;
	}

	// デバイスの補足とアクセス権の取得
	DEVICE_INFO *pInfo;
	bSuccess = felicalib_nfc_poll_and_start_dev_access(
		DEVICE_TYPE_NFC_18092_212K      // FeliCa 212kbps
		| DEVICE_TYPE_NFC_18092_424K,   // FeliCa 424kbps
		3000,                           // タイムアウト ms
		&pInfo);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_poll_and_start_dev_access");
		goto L_Close;
	}

	// IDm の表示
	DEVICE_DATA_NFC_18092_212_424K *pf;
	pf = (DEVICE_DATA_NFC_18092_212_424K *)pInfo->dev_info;

	MyPrintHex(pf->NFCID2, IDM_SIZE);

	// Polling コマンド発行
	bSuccess = MyPolling(SYSTEM_CODE, pIDm);
	if (!bSuccess)
	{
		goto L_StopDevAccess;
	}

	int pBlockNum[] =
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
		0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88,
	};
	char *pBlockName[] =
	{
		"S_PAD0", "S_PAD1", "S_PAD2", "S_PAD3", "S_PAD4", "S_PAD5", "S_PAD6", "S_PAD7",
		"S_PAD8", "S_PAD9", "S_PAD10", "S_PAD11", "S_PAD12", "S_PAD13", "REG",
		"ID", "D_ID", "SER_C", "SYS_C", "CKV", "CK",
		"MC",
	};

	if (1 == rw)
	{
		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling コマンドで取得した IDm
			SERVICE_CODE_RO,            // サービスコード
			pBlockNum[0],     // ブロック番号
			account);                    // 読み出したデータの格納場所

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling コマンドで取得した IDm
			SERVICE_CODE_RO,            // サービスコード
			pBlockNum[1],     // ブロック番号
			domain);                    // 読み出したデータの格納場所

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling コマンドで取得した IDm
			SERVICE_CODE_RO,            // サービスコード
			pBlockNum[2],     // ブロック番号
			user_id);                    // 読み出したデータの格納場所

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling コマンドで取得した IDm
			SERVICE_CODE_RO,            // サービスコード
			pBlockNum[3],     // ブロック番号
			password);                    // 読み出したデータの格納場所

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling コマンドで取得した IDm
			SERVICE_CODE_RO,            // サービスコード
			pBlockNum[4],     // ブロック番号
			name);                    // 読み出したデータの格納場所
	}
	else
	{
		unsigned char pDataW[16] =
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

		char* koneko = "koneko";

		MyWriteWithoutEncryption(
			pIDm,
			SERVICE_CODE_RW,		    	//usServiceCode,
			block,							//ucBlockOffset,
			(unsigned char*)koneko);
	}

L_StopDevAccess:
	// デバイス使用権の解放
	bSuccess = felicalib_nfc_stop_dev_access(
		RE_NOTIFICATION_SAME_DEVICE);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_stop_dev_access");
	}

	// デバイス捕捉処理の停止
	bSuccess = felicalib_nfc_stop_poll_mode();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_stop_poll_mode");
	}

L_Close:
	// ポートのクローズ
	bSuccess = felicalib_nfc_close();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_close");
	}

L_Uninitialize:
	// ライブラリの終了処理
	bSuccess = felicalib_nfc_uninitialize();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_uninitialize");
	}

L_End:

	return 0;
}
