//==============================================================================================================================================
//
// マネージャーに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _MANAGER_H_ //このマクロ定義がされなかったら
#define _MANAGER_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "main.h"//メイン
#include "texture.h"//テクスチャ一括管理
#include "xfile.h"//モデル一括管理
#include "keyboard.h"//キーボード
#include "joypad.h"//ジョイパッド
#include "mouse.h"//マウス
#include "scene.h"//シーン
#include "debugproc.h"//デバッグ表示
#include "renderer.h"//レンダラー
#include "sound.h"//サウンド

//===========================================================================================================
// 前方宣言
//===========================================================================================================
class CCamera;//カメラクラス
class CLight;//ライトクラス
class CFade;//フェードクラス

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CManager
{
public:
	//======================================
	// 関数
	//======================================
	CManager();//コンストラクタ
	~CManager();//デストラクタ
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);//初期化処理
	void Uninit();//終了処理
	void Update();//更新処理
	void Draw();//描画処理
	static void SetMode(CScene::MODE mode);//モードの設定

	//情報の取得
	static CRenderer* GetRenderer() { return m_pRenderer; }//レンダラー
	static CInputKeyboard* GetKeyboard() { return m_pKeyboard; }//キーボード
	static CInputJoypad* GetJoypad() { return m_pJoypad; }//ジョイパッド
	static CInputMouse* GetMouse() { return m_pMouse; }//マウス
	static CSound* GetSound() { return m_pSound; }//サウンド
	static CCamera* GetCamera() { return m_pCamera; }//カメラ
	static CLight* GetLight() { return m_pLight; }//ライト
	static CFade* GetFade() { return m_pFade; }//フェード
	static CScene* GetScene() { return m_pScene; }//シーン
	static CTexture* GetTexture() { return m_pTexture; }//テクスチャ一括情報
	static CXfile* GetModel() { return m_pModel; }//モデル一括情報

	//======================================
	// 変数
	//======================================
	static constexpr const char* GAME_STAGE = "data\\STAGE\\Stage.bin";//ゲームステージ情報
	static constexpr const char* TUTORIAL_STAGE = "data\\STAGE\\TutorialStage.bin";//チュートリアルステージ情報

private:
	//======================================
	// 変数
	//======================================
	static CRenderer* m_pRenderer;//レンダラー情報
	static CInputKeyboard* m_pKeyboard;//キーボード情報
	static CInputJoypad* m_pJoypad;//Pad情報
	static CInputMouse* m_pMouse;//Pad情報
	static CSound* m_pSound;//サウンド
	static CCamera* m_pCamera;//カメラ情報
	static CLight* m_pLight;//ライト情報
	static CScene* m_pScene;//現在の画面
	static CFade* m_pFade;//フェード
	static CTexture* m_pTexture;//テクスチャ一括情報
	static CXfile* m_pModel;//モデル一括情報
};

#endif