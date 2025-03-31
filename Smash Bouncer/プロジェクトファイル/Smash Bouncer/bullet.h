#pragma once
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
#include "objectX.h"//Xファイルオブジェクト

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
	typedef enum
	{
		NONE = 0,
		PLAYER,//プレイヤー
		CLONE,//クローン
		BOUND_ENEMY,//エネミー(反射)
		LAUNCH_ENEMY,//エネミー(打ち上げ)
		BOUND_INVINCIBLE,//無敵(反射)
		LAUNCH_INVINCIBLE,//無敵(打ち上げ)
		MAX
	}BULLETTYPE;

	//======================================
	// 関数
	//======================================
	CBullet(int nPriority = 3);//コンストラクタ
	~CBullet() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CBullet* Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, BULLETTYPE m_type);//生成処理
	virtual bool Collision(D3DXVECTOR3 pos);//当たり判定

	//情報設定処理
	void SetBulletType(BULLETTYPE BulletType);//バレットタイプ
	void SetPrevBullet(CBullet* pBullet) { m_pPrev = pBullet; }//前のオブジェクト情報
	void SetNextBullet(CBullet* pBullet) { m_pNext = pBullet; }//次のオブジェクト情報

	//情報取得処理
	CBullet* GetPrevBullet() { return m_pPrev; }//前のオブジェクト情報
	CBullet* GetNextBullet() { return m_pNext; }//次のオブジェクト情報
	BULLETTYPE GetBulletType() { return m_BulletType; }//バレットタイプ
	D3DXVECTOR3 GetDistance() { return m_Distance; }//対象オブジェクトとの各座標距離
	float GetLength() { return m_fLength; }//対象オブジェクトとの対角距離

private:
	//======================================
	// 変数
	//======================================
	CBullet* m_pPrev;//前のオブジェクトのポインタ
	CBullet* m_pNext;//次のオブジェクトのポインタ
	BULLETTYPE m_BulletType;//タイプ
	D3DXVECTOR3 m_Distance;//対象オブジェクトとの各座標距離
	float m_fLength;//対象オブジェクトとの対角距離
};

#endif
