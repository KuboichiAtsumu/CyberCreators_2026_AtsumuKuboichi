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
	enum class SOUND
	{
		//BGM
		BGM_TITLE,//タイトルBGM
		BGM_TUTORIAL,//チュートリアルBGM
		BGM_GAME,//ゲームBGM
		BGM_RESULT,//リザルトBGM

		//SE
		SHOT,//発射
		NICE,//NICE
		GREAT,//GREAT
		EXCELLENT,//EXCELLENT
		HIT,//撃破
		JUMP,//ジャンプ
		DOUBLE_JUMP,//ダブルジャンプ
		SLIDING,//スライディング
		SELECT,//決定音
		MINIGAME_CLEAR,//ミニゲームクリア

		SOUND_LABEL_MAX,
	};

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
	CSound::SOUNDINFO m_aSoundInfo[static_cast<int>(SOUND::SOUND_LABEL_MAX)] =
	{
		//BGM
		"data/SOUND/BGM/Title.wav", 1, 1.0f,//タイトル
		"data/SOUND/BGM/Tutorial.wav", 1, 1.0f,//チュートリアル
		"data/SOUND/BGM/Game.wav", 1, 1.0f,//ゲーム
		"data/SOUND/BGM/Result.wav", 1, 1.0f,//リザルト

		//SE
		"data/SOUND/SE/Shot.wav", 0, 1.0f,//発射
		"data/SOUND/SE/Nice.wav", 0, 0.7f,//NICE
		"data/SOUND/SE/Great.wav", 0, 0.7f,//GREAT
		"data/SOUND/SE/Excellent.wav", 0, 0.7f,//EXCELLENT
		"data/SOUND/SE/Hit.wav", 0, 1.0f,//撃破
		"data/SOUND/SE/Jump.wav", 0, 2.0f,//ジャンプ
		"data/SOUND/SE/DoubleJump.wav", 0, 2.0f,//ダブルジャンプ
		"data/SOUND/SE/Sliding.wav", 0, 3.0f,//スライディング
		"data/SOUND/SE/Select.wav", 0, 1.0f,//決定音
		"data/SOUND/SE/MiniGameClear.wav", 0, 0.7f,//ミニゲームクリア
	};

	//======================================
	// 関数
	//======================================
	HRESULT Init(HWND hWnd);//初期化処理
	void Uninit();//終了処理
	HRESULT Play(SOUND label);//再生処理
	void Stop(SOUND label);//停止処理(選択)
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
	IXAudio2SourceVoice* m_apSourceVoice[static_cast<int>(SOUND::SOUND_LABEL_MAX)];// ソースボイス
	BYTE* m_apDataAudio[static_cast<int>(SOUND::SOUND_LABEL_MAX)];// オーディオデータ
	DWORD m_aSizeAudio[static_cast<int>(SOUND::SOUND_LABEL_MAX)];// オーディオデータサイズ
};

#endif
