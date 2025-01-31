//==============================================================================================================================================
//
// ブロック管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "blockmanager.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBlockManager::CBlockManager() : CObjectManager()
{
	//メンバ変数初期化
	m_pTop = nullptr;//先頭アドレス
	m_pCur = nullptr;//終端アドレス
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBlockManager::~CBlockManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBlockManager::Init()
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
void CBlockManager::Uninit()
{
	//基底クラス終了処理
	CObjectManager::Uninit();
}

//===========================================================================================================
// 指定されたタイプのみの終了処理
//===========================================================================================================
void CBlockManager::UninitTypeAll(CBlock::TYPE type)
{
	//先頭のポインタをコピー
	CBlock* pBlock = m_pTop;

	//次のブロック情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//タイプが一致した場合
		if (pBlock->GetBlockType() == type)
		{
			//終了処理
			pBlock->Uninit();
			pBlock = nullptr;
		}

		//次のブロック情報のポインタをコピー
		pBlock = pNext;
	}
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBlockManager::Update()
{
	//先頭アドレスを取得する
	CBlock* pBlock = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//ブロック情報が存在する場合
		//削除フラグがtrueの場合
		if (pBlock != nullptr && pBlock->GetDeleteFlag())
		{
			//前のオブジェクト情報を取得
			CBlock* pPrev = pBlock->GetPrevBlock();

			//前のオブジェクト情報が存在する場合
			if (pPrev != nullptr)
			{
				//次のオブジェクト情報を繋ぐ
				pPrev->SetNextBlock(pNext);
			}
			//前のオブジェクト情報が存在しない場合
			else
			{
				//次のオブジェクト情報が存在する場合
				if (pNext != nullptr)
				{
					//次のオブジェクト情報の前のオブジェクト情報をnullptrに設定
					pNext->SetPrevBlock(nullptr);
				}

				//先頭アドレスを変更
				m_pTop = pNext;
			}

			//次のオブジェクト情報が存在する場合
			if (pNext != nullptr)
			{
				//前のオブジェクト情報を繋ぐ
				pNext->SetPrevBlock(pPrev);
			}
			//次のオブジェクト情報が存在しない場合
			else
			{
				//前のオブジェクトが存在する場合
				if (pPrev != nullptr)
				{
					//前のオブジェクト情報の次のオブジェクト情報をnullptrに設定
					pPrev->SetNextBlock(nullptr);
				}

				//終端アドレスを変更
				m_pCur = pPrev;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pBlock = pNext;
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBlockManager::Draw()
{
}

//===========================================================================================================
// 登録処理
//===========================================================================================================
void CBlockManager::Regist(CBlock* pBlock)
{
	//先頭アドレスが存在しない場合
	if (m_pTop == nullptr)
	{
		//生成したブロック情報を先頭アドレスに代入
		m_pTop = pBlock;

		//前のオブジェクト情報にnullptrを代入
		pBlock->SetPrevBlock(nullptr);
	}

	//先頭アドレスが存在する場合
	else
	{
		//先頭アドレスを取得
		CBlock* pAllBlock = m_pTop;

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pAllBlock != nullptr)
		{
			//次のオブジェクト情報が存在しない場合
			if (pAllBlock->GetNextBlock() == nullptr)
			{
				//次のオブジェクト情報に生成したブロック情報を保存
				pAllBlock->SetNextBlock(pBlock);

				//生成したブロック情報の前のオブジェクト情報を保存
				pBlock->SetPrevBlock(pAllBlock);
				break;
			}

			//オブジェクト情報を次の情報に変更する
			pAllBlock = pAllBlock->GetNextBlock();
		}
	}

	//自身の次のオブジェクト情報にnullptrを代入
	pBlock->SetNextBlock(nullptr);

	//終端アドレスを代入
	m_pCur = pBlock;
}

//===========================================================================================================
// 読込処理
//===========================================================================================================
void CBlockManager::Load()
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

			//タグがブロックではない場合
			if (static_cast<CTag::TAG>(nTag) != CTag::TAG::BLOCK)
			{
				//生成せずに読込を続ける
				continue;
			}

			//ブロックを生成
			CBlock::Create(pos, rot, scale, static_cast<CBlock::TYPE>(nType));
		}

		//ファイルを閉じる
		fclose(pFile);
	}
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
CBlock::CollisionFlag CBlockManager::Collision(CBlock::CollisionParam& aParam)
{
	//先頭アドレスを取得する
	CBlock::CollisionFlag aFlag = {};//各座標の判定
	CBlock::CollisionFlag aSaveFlag = {};//判定取得用
	CBlock* pBlock = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//ブロック情報が存在する場合
		//タイプが一致した場合
		if (pBlock != nullptr)
		{
			//当たり判定処理
			aSaveFlag = pBlock->Collision(aParam);

			//横の判定がまだfalseの場合
			if (!aFlag.X && aSaveFlag.X)
			{
				//falseの判定をtrueに更新
				aFlag.X = aSaveFlag.X;
			}
			//上の判定がまだfalseの場合
			if (!aFlag.Y_UP && aSaveFlag.Y_UP)
			{
				//falseの判定をtrueに更新
				aFlag.Y_UP = aSaveFlag.Y_UP;
			}
			//下の判定がまだfalseの場合
			if (!aFlag.Y_DOWN && aSaveFlag.Y_DOWN)
			{
				//falseの判定をtrueに更新
				aFlag.Y_DOWN = aSaveFlag.Y_DOWN;
			}
			//奥の判定がまだfalseの場合
			if (!aFlag.Z && aSaveFlag.Z)
			{
				//falseの判定をtrueに更新
				aFlag.Z = aSaveFlag.Z;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pBlock = pNext;
	}

	return aFlag;
}

//===========================================================================================================
// ブロック検索処理
//===========================================================================================================
CBlock* CBlockManager::FindBlock(CBlock::TYPE type)
{
	//先頭のポインタをコピー
	CBlock* pBlock = m_pTop;

	//次のブロック情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//タイプが一致した場合
		if (pBlock->GetBlockType() == type)
		{
			//ポインタを返す
			return pBlock;
		}

		//次のブロック情報のポインタをコピー
		pBlock = pBlock->GetNextBlock();
	}

	return nullptr;
}
