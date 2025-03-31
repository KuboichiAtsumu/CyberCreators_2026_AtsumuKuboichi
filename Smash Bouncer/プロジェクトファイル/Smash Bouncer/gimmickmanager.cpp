//==============================================================================================================================================
//
// ギミック管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "gimmickmanager.h"//ギミックマネージャー

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CGimmickManager::CGimmickManager(int nPriority) : CObjectManager(nPriority)
{
	//静的メンバ変数初期化
	m_pTop = nullptr;//先頭アドレス
	m_pCur = nullptr;//終端アドレス
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CGimmickManager::~CGimmickManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CGimmickManager::Init()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = { 0.0f,0.0f,0.0f };//位置
	D3DXVECTOR3 rot = { 0.0f,0.0f,0.0f };//向き
	D3DXVECTOR3 scale = { 1.0f,1.0f,1.0f };//拡大率
	int nNumAll = 0;//総数
	int nCategory = 0;//カテゴリー
	int nType = 0;//種類
	const char* StageFile = "";//ステージ情報ファイルパス

	//シーンに応じてファイルを変更
	switch (CManager::GetScene()->GetMode())
	{
		//チュートリアル
	case CScene::MODE_TUTORIAL:
		StageFile = CManager::TUTORIAL_STAGE;
		break;

		//ゲーム
	case CScene::MODE_GAME:
		StageFile = CManager::GAME_STAGE;
		break;

	default:
		break;
	}

	//ファイルを開く
	FILE* pFile = fopen(StageFile, "rb");

	//オブジェクトデータの読み込み
	if (pFile != nullptr)
	{
		//ファイルの読み込み
		fread(&nNumAll, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < nNumAll; nCnt++)
		{
			//データの取得
			fread(&pos, sizeof(D3DXVECTOR3), 1, pFile);//位置
			fread(&rot, sizeof(D3DXVECTOR3), 1, pFile);//向き
			fread(&scale, sizeof(D3DXVECTOR3), 1, pFile);//拡大率
			fread(&nCategory, sizeof(int), 1, pFile);//カテゴリー
			fread(&nType, sizeof(int), 1, pFile);//種類

			//カテゴリーがブロックの場合
			if (nCategory == CXfile::GIMMICK)
			{
				//ブロックを生成
				CGimmick* pGimmick = CGimmick::Create(pos, rot, nType);

				//先頭アドレスが存在しない場合
				if (m_pTop == nullptr)
				{
					//生成したエネミー情報を先頭アドレスに代入
					m_pTop = pGimmick;

					//前のオブジェクト情報にnullptrを代入
					pGimmick->SetPrevGimmick(nullptr);
				}

				//先頭アドレスが存在する場合
				else
				{
					//先頭アドレスを取得
					CGimmick* pAllGimmick = m_pTop;

					//オブジェクト情報がnullptrになるまで繰り返す
					while (pAllGimmick != nullptr)
					{
						//次のオブジェクト情報が存在しない場合
						if (pAllGimmick->GetNextGimmick() == nullptr)
						{
							//次のオブジェクト情報に生成したエネミー情報を保存
							pAllGimmick->SetNextGimmick(pGimmick);

							//生成したギミック情報の前のオブジェクト情報を保存
							pGimmick->SetPrevGimmick(pAllGimmick);

							break;
						}

						//オブジェクト情報を次の情報に変更する
						pAllGimmick = pAllGimmick->GetNextGimmick();
					}
				}

				//自身の次のオブジェクト情報にnullptrを代入
				pGimmick->SetNextGimmick(nullptr);

				//終端アドレスを代入
				m_pCur = pGimmick;
			}
		}

		//ファイルを閉じる
		fclose(pFile);
	}

	//オブジェクトマネージャー初期化処理
	if (FAILED(CObjectManager::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CGimmickManager::Uninit()
{
	//オブジェクトマネージャー終了処理
	CObjectManager::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CGimmickManager::Update()
{
	//先頭アドレスを取得する
	CGimmick* pGimmick = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pGimmick != nullptr)
	{
		//次のオブジェクト情報を取得
		CGimmick* pNext = pGimmick->GetNextGimmick();

		//削除フラグがtrueの場合
		if (pGimmick->GetDeleteFlag())
		{
			//前のオブジェクト情報を取得
			CGimmick* pPrev = pGimmick->GetPrevGimmick();

			//前のオブジェクト情報が存在する場合
			if (pPrev != nullptr)
			{
				//次のオブジェクト情報を繋ぐ
				pPrev->SetNextGimmick(pNext);
			}
			//前のオブジェクト情報が存在しない場合
			else
			{
				//次のオブジェクト情報の前のオブジェクト情報をnullptrに設定
				pNext->SetPrevGimmick(nullptr);

				//先頭アドレスを変更
				m_pTop = pNext;
			}

			//次のオブジェクト情報が存在する場合
			if (pNext != nullptr)
			{
				//前のオブジェクト情報を繋ぐ
				pNext->SetPrevGimmick(pPrev);
			}
			//次のオブジェクト情報が存在しない場合
			else
			{
				//前のオブジェクト情報の次のオブジェクト情報をnullptrに設定
				pPrev->SetNextGimmick(nullptr);

				//終端アドレスを変更
				m_pCur = pPrev;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pGimmick = pNext;
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CGimmickManager::Draw()
{

}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
void CGimmickManager::Collision(D3DXVECTOR3& pos, D3DXVECTOR3& move, D3DXVECTOR3 size, D3DXVECTOR3& rot)
{
	//先頭アドレスを取得する
	CGimmick* pGimmick = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pGimmick != nullptr)
	{
		//次のオブジェクト情報を取得
		CGimmick* pNext = pGimmick->GetNextGimmick();

		//当たり判定
		pGimmick->Collision(pos, move, size, rot);

		//オブジェクト情報を次の情報に変更する
		pGimmick = pNext;
	}
}
