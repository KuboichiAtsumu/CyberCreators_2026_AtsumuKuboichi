//==============================================================================================================================================
//
// Xファイル・テクスチャタグ
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _TAG_H_ //このマクロ定義がされなかったら
#define _TAG_H_ //2重インクルード防止のマクロ定義

class CTag
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//タグ
	enum class TAG
	{
		PLAYER = 0,//プレイヤー
		BLOCK,//ブロック
		GIMMICK,//ギミック
		BULLET,//弾
		ENEMY,//エネミー
		BACK_3D,//3D背景
		BOSS,//ボス
		AREA,//エリア
		BG,//背景
		EFFECT,//エフェクト
		BOSS_ATTACK,//ボス攻撃
		UI,//UI
		SIGN_BOARD,//看板
		EFFECT_2D,//2Dエフェクト
		MAX
	};

	//======================================
	// 関数
	//======================================
	CTag();
	~CTag();
	void SetTag(TAG& tag, std::string str);
	void SetNumType(int nNumType, TAG tag) { m_nNumType[static_cast<int>(tag)] = nNumType; }
	int GetNumType(TAG tag) { return m_nNumType[static_cast<int>(tag)]; }

private:
	//======================================
	// 変数
	//======================================
	int m_nNumType[static_cast<int>(TAG::MAX)];
};

#endif