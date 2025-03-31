//==============================================================================================================================================
//
// サウンド処理に関するヘッダーファイル
// Author : Yuudai Takaya
//
//==============================================================================================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "main.h"//メインヘッダー

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CSound
{
public:
	//======================================
	// サウンド一覧
	//======================================
	typedef enum
	{
		BGM_TITLE = 0,//タイトルBGM
		BGM_TUTORIAL,//チュートリアルBGM
		BGM_GAME,//ゲームBGM
		BGM_RESULT,//リザルトBGM
		SE_MODE,//画面遷移効果音
		SE_SMASH,//反射効果音
		SE_WARP,//ワープ効果音
		SE_SWITCH,//スイッチ効果音
		SE_BONUS,//ボーナス効果音
		SE_HIT,//ヒット効果音
		SE_BOUND,//反射効果音
		SOUND_LABEL_MAX,
	} SOUND_LABEL;

	//======================================
	// サウンド情報の構造体定義
	//======================================
	typedef struct
	{
		const char* pFilename;// ファイル名
		int nCntLoop;// ループカウント
		float Volume;// 音量
	}SOUNDINFO;

	//======================================
	// サウンドの情報
	//======================================
	CSound::SOUNDINFO m_aSoundInfo[CSound::SOUND_LABEL_MAX] =
	{
		"data/SOUND/BGM/Title000.wav", 1, 1.0f,//タイトルBGM
		"data/SOUND/BGM/Tutorial000.wav", 1, 1.0f,//チュートリアルBGM
		"data/SOUND/BGM/Game000.wav", 1, 1.0f,//ゲームBGM
		"data/SOUND/BGM/Result000.wav", 1, 1.0f,//リザルトBGM
		"data/SOUND/SE/ChangeMode000.wav", 0, 1.0f,//画面遷移効果音
		"data/SOUND/SE/Smash000.wav", 0, 2.0f,//反射効果音
		"data/SOUND/SE/Warp000.wav", 0, 1.0f,//ワープ効果音
		"data/SOUND/SE/Switch000.wav", 0, 1.0f,//スイッチ効果音
		"data/SOUND/SE/Bonus000.wav", 0, 1.0f,//ボーナス獲得効果音
		"data/SOUND/SE/Hit000.wav", 0, 1.0f,//ヒット効果音
		"data/SOUND/SE/Bound000.wav", 0, 0.7f,//反射効果音
	};

	//======================================
	// 関数
	//======================================
	HRESULT Init(HWND hWnd);//初期化処理
	void Uninit();//終了処理
	HRESULT Play(SOUND_LABEL label);//再生処理
	void Stop(SOUND_LABEL label);//停止処理(選択)
	void Stop();//全停止処理

private:
	//======================================
	// 関数
	//======================================
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	//======================================
	// 変数
	//======================================
	IXAudio2* m_pXAudio2 = nullptr;// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* m_pMasteringVoice = nullptr;// マスターボイス
	IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX];// ソースボイス
	BYTE* m_apDataAudio[SOUND_LABEL_MAX];// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX];// オーディオデータサイズ
};

#endif
