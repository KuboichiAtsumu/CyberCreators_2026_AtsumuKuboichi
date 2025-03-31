//==============================================================================================================================================
//
// ブロックに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BLOCK_H_ //このマクロ定義がされなかったら
#define _BLOCK_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"//Xファイルオブジェクトヘッダー

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CBlock : public CObjectX
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//種類
	typedef enum
	{
		WALL,//壁
		BARRIER,//バリア
		BOUNDARY,//ステージ境界
		BATTERY,//砲台
		KEY,//鍵付きブロック
		COLLAPSE,//崩れるブロック
		MAX
	}BLOCKTYPE;

	//======================================
	// 関数
	//======================================
	CBlock(int nPriority = 3);//コンストラクタ
	~CBlock() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CBlock* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nType);//生成処理
	virtual void Collision(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& size);//当たり判定
	virtual bool Reflection(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& move, D3DXVECTOR3& rot, D3DXVECTOR3& size);//反射処理
	D3DXVECTOR3 GetNormal() { return m_Normal; }//垂直ベクトル
	CBlock* GetPrevBlock() { return m_pPrev; }//前のオブジェクト情報
	CBlock* GetNextBlock() { return m_pNext; }//次のオブジェクト情報
	BLOCKTYPE GetBlockType() { return m_BlockType; }//ブロックタイプ
	void SetPrevBlock(CBlock* pBLOCK) { m_pPrev = pBLOCK; }//前のオブジェクト情報
	void SetNextBlock(CBlock* pBLOCK) { m_pNext = pBLOCK; }//次のオブジェクト情報

private:
	//======================================
	// 関数
	//======================================
	void SetBlockType(BLOCKTYPE type) { m_BlockType = type; }//タイプ設定
	void SEGMENT(float StartX, float StartZ, float EndX, float EndZ);//垂直ベクトル計算処理

	//======================================
	// 変数
	//======================================
	D3DXVECTOR3 m_Normal;//垂直ベクトル
	BLOCKTYPE m_BlockType;//種類
	CBlock* m_pPrev;//前のオブジェクトのポインタ
	CBlock* m_pNext;//次のオブジェクトのポインタ
};

#endif
