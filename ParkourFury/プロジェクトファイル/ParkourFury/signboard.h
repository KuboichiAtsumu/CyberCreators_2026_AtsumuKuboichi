//==============================================================================================================================================
//
// 看板Uに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _SIGNBOARD_H_ //このマクロ定義がされなかったら
#define _SIGNBOARD_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CSignBoard : public CObjectX
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//タイプ
	enum class TYPE
	{
		NONE = 0,

		//チュートリアル
		TUTORIAL_JUMP,//ジャンプ説明
		TUTORIAL_DASH,//ダッシュ説明
		TUTORIAL_WALLKICK,//壁キック説明
		TUTORIAL_SHOT,//発射説明
		TUTORIAL_SLIDING,//スライディング説明

		//ゲーム
		GUIDANCE_MINIGAME,//ミニゲームエリア案内
		GUIDANCE_BOSS,//ボスエリア案内

		MAX
	};

	//======================================
	// 関数
	//======================================
	CSignBoard(int nPriority = static_cast<int>(Category::SIGN_BOARD));//コンストラクタ
	~CSignBoard() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CSignBoard* Create(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 scale, D3DXVECTOR3 rot);//生成処理

	//前のオブジェクト情報
	void SetPrev(CSignBoard* pUI) { m_pPrev = pUI; }//設定
	CSignBoard* GetPrev() { return m_pPrev; }//決定

	//次のオブジェクト情報
	void SetNext(CSignBoard* pUI) { m_pNext = pUI; }//設定
	CSignBoard* GetNext() { return m_pNext; }//取得

	//タイプ
	void SetType(TYPE type) { m_Type = type; }
	TYPE GetType() { return m_Type; }//取得

private:
	//======================================
	// 各タイプテクスチャファイルパス
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE

		//チュートリアル
		"data/TEXTURE/SIGN_BOARD/TutorialJump.png",//ジャンプ説明
		"data/TEXTURE/SIGN_BOARD/TutorialDash.png",//ダッシュ説明
		"data/TEXTURE/SIGN_BOARD/TutorialWallKick.png",//壁キック説明
		"data/TEXTURE/SIGN_BOARD/TutorialShot.png",//発射説明
		"data/TEXTURE/SIGN_BOARD/TutorialSliding.png",//スライディング説明

		//ゲーム
		"data/TEXTURE/SIGN_BOARD/GuidanceMiniGame.png",//ミニゲームエリア案内
		"data/TEXTURE/SIGN_BOARD/GuidanceBoss.png",//ボスエリア案内
	};

	//======================================
	// 変数
	//======================================
	CSignBoard* m_pPrev;//前のオブジェクト情報へのポインタ
	CSignBoard* m_pNext;//次のオブジェクト情報へのポインタ
	TYPE m_Type;//タイプ
};

#endif
