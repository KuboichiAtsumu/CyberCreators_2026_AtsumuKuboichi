//==============================================================================================================================================
//
// ステージ別ゾーンに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _STAGEZONE_H_ //このマクロ定義がされなかったら
#define _STAGEZONE_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object3D.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CZone : public CObject3D
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//種類
	typedef enum
	{
		STAGE,//ステージ区分
		REFLECT,//反射ゾーン
		MAX_ZONE_TYPE
	}ZONETYPE;

	//======================================
	// 関数
	//======================================
	CZone(int nPriority = 3);//コンストラクタ
	~CZone() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CZone* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, ZONETYPE type);//生成処理
	static void LoadZoneTexFile();//各ゾーンタイプのテクスチャファイル読込処理
	bool Judge(D3DXVECTOR3& pos);//対象がいるステージの判定
	void Collision(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& size);//当たり判定

	//情報の取得
	int GetID() { return m_nStageID; }//ステージID
	ZONETYPE GetZoneType() { return m_ZoneType; }//ゾーンタイプ
	
	//情報の設定
	void SetCollisionFlag(bool bCollision) { m_bCollision = bCollision; }//当たり判定の有効化フラグ

	//======================================
	// 変数
	//======================================
	static constexpr float STAGEZONE_WIDTH = 400.0f;//ステージゾーンの横幅
	static constexpr float STAGEZONE_HEIGHT = 260.0f;//ステージゾーンの縦幅

	static constexpr float REFLECTZONE_WIDTH = 60.0f;//反射ゾーンの横幅
	static constexpr float REFLECTZONE_HEIGHT = 260.0f;//反射ゾーンの縦幅

private:
	//======================================
	// 関数
	//======================================
	void SetZoneType(ZONETYPE type) { m_ZoneType = type; }//ゾーンタイプ設定

	//======================================
	// 変数
	//======================================
	static constexpr const char* TEXTURE_FILE = "data\\ZoneTexture.txt";//テクスチャパス保存ファイル
	static char m_aTextureFile[MAX_ZONE_TYPE][CProcess::MAX_TEXT];//各タイプのテクスチャファイル
	ZONETYPE m_ZoneType;//ゾーンタイプ
	static int m_nAllNumZone;//ゾーンの数
	static int m_nNumZone[MAX_ZONE_TYPE];//各ゾーンの数
	int m_nStageID;//各ゾーンのステージID
	bool m_bCollision;//ゾーンの当たり判定の有効化フラグ
	bool m_bUse;//使用フラグ
};

#endif

