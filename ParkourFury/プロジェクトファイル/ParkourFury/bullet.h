//==============================================================================================================================================
//
// バレットに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BULLET_H_
#define _BULLET_H_

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CBullet : public CObjectX
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//弾のタイプ
	enum class TYPE
	{
		NONE = 0,
		PLAYER,//プレイヤー
		MAX
	};

	//======================================
	// 関数
	//======================================
	CBullet(int nPriority = 3);//コンストラクタ
	~CBullet() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CBullet* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, const D3DXVECTOR3& rot, TYPE m_type);//生成処理
	
	//前のバレット情報
	void SetPrevBullet(CBullet* pBullet) { m_pPrev = pBullet; }//設定
	CBullet* GetPrevBullet() { return m_pPrev; }//取得

	//次のバレット情報
	void SetNextBullet(CBullet* pBullet) { m_pNext = pBullet; }//設定
	CBullet* GetNextBullet() { return m_pNext; }//取得

	//タイプ
	void SetBulletType(TYPE Type) { m_Type = Type; }//設定
	TYPE GetBulletType() { return m_Type; }//取得

	//寿命
	void SetLife(int nLife) { m_nLife = nLife; }//設定

private:
	//======================================
	// 関数
	//======================================
	void CollisionEnemy();//エネミーとの当たり判定
	void CollisionBoss();//ボスとの当たり判定
	void CollisionBlock();//ブロックとの当たり判定

	//======================================
	// 変数
	//======================================
	CBullet* m_pPrev;//前の情報のポインタ
	CBullet* m_pNext;//次の情報のポインタ
	TYPE m_Type;//タイプ
	int m_nLife;//寿命
};

#endif
