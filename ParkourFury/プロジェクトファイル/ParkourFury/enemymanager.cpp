//==============================================================================================================================================
//
// エネミー管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "enemymanager.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CEnemyManager::m_nFirstAll = 0;//生成時の総数
int CEnemyManager::m_nNowAll = 0;//総数

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEnemyManager::CEnemyManager() : CObjectManager(),
	m_pTop(nullptr),
	m_pCur(nullptr)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CEnemyManager::~CEnemyManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CEnemyManager::Init()
{
	//基底クラス初期化処理
	if (FAILED(CObjectManager::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CEnemyManager::Uninit()
{
	//基底クラス終了処理
	CObjectManager::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CEnemyManager::Update()
{
	//先頭アドレスを取得する
	CEnemy* pEnemy = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pEnemy != nullptr)
	{
		//次のオブジェクト情報を取得
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//エネミー情報が存在する場合
		//削除フラグがtrueの場合
		if (pEnemy != nullptr && pEnemy->GetDeleteFlag())
		{
			//前のオブジェクト情報を取得
			CEnemy* pPrev = pEnemy->GetPrevEnemy();

			//前のオブジェクト情報が存在する場合
			if (pPrev != nullptr)
			{
				//次のオブジェクト情報を繋ぐ
				pPrev->SetNextEnemy(pNext);
			}
			//前のオブジェクト情報が存在しない場合
			else
			{
				//次の情報が存在する場合
				if (pNext != nullptr)
				{
					//次ののオブジェクト情報の前のオブジェクト情報をnullptrに設定
					pNext->SetPrevEnemy(nullptr);
				}

				//先頭アドレスを変更
				m_pTop = pNext;
			}

			//次のオブジェクト情報が存在する場合
			if (pNext != nullptr)
			{
				//前のオブジェクト情報を繋ぐ
				pNext->SetPrevEnemy(pPrev);
			}
			//次のオブジェクト情報が存在しない場合
			else
			{
				//前の情報が存在する場合
				if (pPrev != nullptr)
				{
					//前のオブジェクト情報の次のオブジェクト情報をnullptrに設定
					pPrev->SetNextEnemy(nullptr);
				}

				//終端アドレスを変更
				m_pCur = pPrev;
			}

			//総数を減算
			m_nNowAll++;
		}

		//オブジェクト情報を次の情報に変更する
		pEnemy = pNext;
	}
}

//===========================================================================================================
// 登録処理
//===========================================================================================================
void CEnemyManager::Regist(CEnemy* pEnemy)
{
	//先頭アドレスが存在しない場合
	if (m_pTop == nullptr)
	{
		//生成したエネミー情報を先頭アドレスに代入
		m_pTop = pEnemy;

		//前のオブジェクト情報にnullptrを代入
		pEnemy->SetPrevEnemy(nullptr);
	}

	//先頭アドレスが存在する場合
	else
	{
		//先頭アドレスを取得
		CEnemy* pAllEnemy = m_pTop;

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pAllEnemy != nullptr)
		{
			//次のオブジェクト情報が存在しない場合
			if (pAllEnemy->GetNextEnemy() == nullptr)
			{
				//次のオブジェクト情報に生成したエネミー情報を保存
				pAllEnemy->SetNextEnemy(pEnemy);

				//生成したエネミー情報の前のオブジェクト情報を保存
				pEnemy->SetPrevEnemy(pAllEnemy);
				break;
			}

			//オブジェクト情報を次の情報に変更する
			pAllEnemy = pAllEnemy->GetNextEnemy();
		}
	}

	//自身の次のオブジェクト情報にnullptrを代入
	pEnemy->SetNextEnemy(nullptr);

	//終端アドレスを代入
	m_pCur = pEnemy;

	//総数を加算
	m_nNowAll++;

	//生成時の総数を更新
	m_nFirstAll = m_nNowAll;
}

//===========================================================================================================
// 読込処理
//===========================================================================================================
void CEnemyManager::Load()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = { 0.0f, 0.0f, 0.0f };//座標情報保存用
	D3DXVECTOR3 rot = { 0.0f, 0.0f, 0.0f };//角度情報保存用
	D3DXVECTOR3 scale = { 0.0f, 0.0f, 0.0f };//拡大率情報保存用
	int nTag = 1;//タグ情報保存用
	int nType = 0;//タイプ情報保存用
	int nNumAll = 0;//総数

	//ファイルを開く
	FILE* pFile = fopen(CManager::GetInstance()->StageFile[static_cast<int>(CManager::GetInstance()->GetScene()->GetMode())], "rb");

	//オブジェクトデータの読み込み
	if (pFile != nullptr)
	{
		//ファイルの読み込み
		fread(&nNumAll, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < nNumAll; nCnt++)
		{
			//データの取得
			fread(&pos, sizeof(D3DXVECTOR3), 1, pFile);//座標
			fread(&rot, sizeof(D3DXVECTOR3), 1, pFile);//角度
			fread(&scale, sizeof(D3DXVECTOR3), 1, pFile);//拡大率
			fread(&nTag, sizeof(int), 1, pFile);//タグ
			fread(&nType, sizeof(int), 1, pFile);//種類

			//タグがエネミーの場合
			if (static_cast<CTag::TAG>(nTag) == CTag::TAG::ENEMY)
			{
				//生成
				CEnemy::Create(pos, rot, scale, static_cast<CEnemy::TYPE>(nType));
			}
		}

		//ファイルを閉じる
		fclose(pFile);
	}
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CEnemyManager::Collision(CEnemy::CollisionParam& aParam)
{
	//先頭の情報を取得
	CEnemy* pEnemy = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pEnemy != nullptr)
	{
		//次のオブジェクト情報を取得
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//当たり判定がtrueの場合
		if (pEnemy != nullptr && pEnemy->Collision(aParam))
		{
			return true;
		}

		//オブジェクト情報を次の情報に変更する
		pEnemy = pNext;
	}

	return false;
}

//===========================================================================================================
// 検索処理
//===========================================================================================================
CEnemy* CEnemyManager::FindEnemy(CEnemy::TYPE type)
{
	//先頭の情報を取得
	CEnemy* pEnemy = m_pTop;

	//次のエネミー情報がnullptrになるまで繰り返す
	while (pEnemy != nullptr)
	{
		//次のオブジェクト情報を取得
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//タイプが一致した場合
		if (pEnemy->GetEnemyType() == type)
		{
			//ポインタを返す
			return pEnemy;
		}

		//次のエネミー情報のポインタをコピー
		pEnemy = pNext;
	}

	return nullptr;
}
