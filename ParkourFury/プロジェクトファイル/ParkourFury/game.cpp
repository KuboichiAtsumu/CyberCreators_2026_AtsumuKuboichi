//==============================================================================================================================================
//
// �Q�[����ʂɊւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "game.h"
#include "timer.h"
#include "back_3D.h"
#include "UI.h"
#include "editmanager.h"
#include "blockmanager.h"
#include "gimmickmanager.h"
#include "bulletmanager.h"
#include "enemymanager.h"
#include "areamanager.h"
#include "signboardmanager.h"
#include "player.h"
#include "edit3D.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
CGame* CGame::m_pGame = nullptr;

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CGame::CGame() : CScene(),
	m_pTimer{ nullptr },
	m_Area{ GAME_AREA::NORMAL }
{
	//�����o�ϐ�������
	m_SecondlyHeat.clear();
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CGame::~CGame()
{
}

//===========================================================================================================
// �C���X�^���X�擾
//===========================================================================================================
CGame* CGame::GetInstance()
{
	//�Q�[�����ł͂Ȃ��ꍇ
	if (CManager::GetInstance()->GetScene()->GetMode() != MODE::MODE_GAME)
	{
		//�C���X�^���X���擾���Ȃ�
		return nullptr;
	}

	//�C���X�^���X�����݂��Ȃ��ꍇ
	if (m_pGame == nullptr)
	{
		//�C���X�^���X�𐶐�
		m_pGame = NEW CGame();

		//�^�C�}�[����
		m_pGame->m_pTimer = NEW CTimer();//�������𓮓I�m��
		m_pGame->m_pTimer->Init();//����������
	}

	return m_pGame;
}

//===========================================================================================================
// �C���X�^���X�j��
//===========================================================================================================
void CGame::Release()
{
	//�^�C�}�[
	if (m_pGame->m_pTimer != nullptr)
	{
		m_pGame->m_pTimer->Uninit();
		delete m_pGame->m_pTimer;
		m_pGame->m_pTimer = nullptr;
	}

	//�C���X�^���X�����݂���ꍇ
	if (m_pGame != nullptr)
	{
		delete m_pGame;
		m_pGame = nullptr;
	}
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CGame::Init()
{
	//BGM�Đ�
	CManager::GetInstance()->GetSound()->Play(CSound::SOUND::BGM_GAME);

#ifdef _DEBUG
	//�G�f�B�^�}�l�[�W���[����������
	CEdit3DManager* pEditManager = NEW CEdit3DManager();
	SetEditManager(pEditManager);//�C���X�^���X�ݒ�
	if (FAILED(pEditManager->Init())) return E_FAIL;//����������
#endif // _DEBUG

	//�u���b�N�}�l�[�W���[�̃C���X�^���X�擾
	CBlockManager* pBlockManager = NEW CBlockManager();
	SetBlockManager(pBlockManager);//�C���X�^���X�ݒ�
	if (FAILED(pBlockManager->Init())) return E_FAIL;//����������

	//�G�l�~�[�}�l�[�W���[�̃C���X�^���X�擾
	CEnemyManager* pEnemyManager = NEW CEnemyManager();
	SetEnemyManager(pEnemyManager);//�C���X�^���X�ݒ�
	if (FAILED(pEnemyManager->Init())) return E_FAIL;//����������

	//�o���b�g�}�l�[�W���[�̃C���X�^���X�擾
	CBulletManager* pBulletManager = NEW CBulletManager();
	SetBulletManager(pBulletManager);//�C���X�^���X�ݒ�
	if (FAILED(pBulletManager->Init())) return E_FAIL;//����������

	//�M�~�b�N�}�l�[�W���[�̃C���X�^���X�擾
	CGimmickManager* pGimmickManager = NEW CGimmickManager();
	SetGimmickManager(pGimmickManager);//�C���X�^���X�ݒ�
	if (FAILED(pGimmickManager->Init())) return E_FAIL;//����������

	//�G���A�}�l�[�W���[�̃C���X�^���X�擾
	CAreaManager* pAreaManager = NEW CAreaManager();
	SetAreaManager(pAreaManager);//�C���X�^���X�ݒ�
	if (FAILED(pAreaManager->Init())) return E_FAIL;//����������

	//�Ŕ}�l�[�W���[�̃C���X�^���X�擾
	CSignBoardManager* pSignBoardManager = NEW CSignBoardManager();
	SetSignBoardManager(pSignBoardManager);//�C���X�^���X�ݒ�
	if (FAILED(pSignBoardManager->Init())) return E_FAIL;//����������

	//�V�[������������
	if (FAILED(CScene::Init()))
	{
		return E_FAIL;
	}

	//�C���X�^���X����
	GetInstance();

	//�X�e�[�W�w�i����
	CBack3D::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, CBack3D::TYPE::SKY);

	//�G���A�w�i����
	CBack3D::Load();

	//UI����
	CUI::Create(CUI::TYPE::TIMER_HEADING, { 1190.0f, 25.0f }, { 80.0f, 30.0f }, { 0.0f, 0.0f, 0.0f });
	CUI::Create(CUI::TYPE::TIME_NUMBER, { 1220.0f, 90.0f }, { 60.0f, 90.0f }, { 0.0f, 0.0f, 0.0f });

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CGame::Uninit()
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();

	//BGM�Đ��I��
	pManager->GetSound()->Stop(CSound::SOUND::BGM_GAME);

	//���U���g�p�X�R�A�̍\���̏��擾
	ResultScore aResultScore = GetResultScore();

	//�S�[���^�C���擾
	aResultScore.nGoalTime = m_pGame->m_pTimer->GetTime();

	//�v���C���[�J�e�S���[�̃I�u�W�F�N�g�����擾
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);

	//�I�u�W�F�N�g��񂪑��݂���ꍇ
	if (pFindObj == nullptr) return;

	//�v���C���[�N���X�Ƀ_�E���L���X�g
	CPlayer* pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//�L�����擾
	aResultScore.fNumKillenemy = static_cast<float>(pPlayer->GetKill()) / static_cast<float>(CEnemy::GetAll()) * 100.0f;

	//���U���g�p�X�R�A�ݒ�
	SetResultScore(aResultScore);

	//�G���A�����ʏ�G���A�ɂ���
	m_pGame->m_Area = GAME_AREA::NORMAL;

	//�t�H�O�𖳌���
	pManager->GetRenderer()->OffFog();

	//�^�C�}�[���Z�b�g
	m_pGame->m_pTimer->Reset();

	//�C���X�^���X�̔j��
	Release();

	//���N���X�I������
	CScene::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CGame::Update()
{	
	//�}�l�[�W���[�̃C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();

	//�f�o�b�O�̂�
#ifdef _DEBUG

	//�L�[�{�[�h�̃C���X�^���X�擾
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();
	
	//�V�[���̃C���X�^���X�擾
	CScene* pScene = pManager->GetScene();

	switch (pScene->GetSceneState())
	{
		//�Q�[���V�[��
	case SCENE::GAME:

		//F4�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetTrigger(DIK_F4))
		{
			//3D�G�f�B�^�V�[���ɂ���
			pScene->SetScene(SCENE::EDIT_3D);
		}
		
		//Enter�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetTrigger(DIK_RETURN) ||
			pManager->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_START))
		{
			//���U���g��ʂֈڍs
			pManager->GetFade()->SetFade(MODE::MODE_RESULT);
		}

		break;

		//3D�G�f�B�^�V�[��
	case SCENE::EDIT_3D:

		//F4�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetTrigger(DIK_F4))
		{
			//�Q�[���V�[���ɂ���
			pScene->SetScene(SCENE::GAME);
		}

		break;

	default:
		break;
	}
#endif // DEBUG

	//�^�C�}�[
	m_pGame->m_pTimer->Update();

	//�J�����̍X�V����
	pManager->GetCamera()->Update();

	//�t�H�O�ݒ菈��
	SetFog();

	//���N���X�X�V
	CScene::Update();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CGame::Draw()
{
	//�J�����̐ݒ�
	CManager::GetInstance()->GetCamera()->SetCamera();
}

//===========================================================================================================
// ��������
//===========================================================================================================
CGame* CGame::Create()
{
	//�������𓮓I�m��
	CGame* pGame = NEW CGame();

	return pGame;
}

//===========================================================================================================
// ���σq�[�g�ʌv�Z����
//===========================================================================================================
void CGame::AverageHeat()
{
	//�v���C���[�J�e�S���[�̃I�u�W�F�N�g������
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);

	//�������ʂ��s��v
	if (pFindObj == nullptr)
	{
		//�����𔲂���
		return;
	}

	//�v���C���[�N���X�Ƀ_�E���L���X�g
	CPlayer* pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//�_�E���L���X�g���s
	if (pPlayer == nullptr)
	{
		//�����𔲂���
		return;
	}

	//���݂̃q�[�g�ʂ�ۑ�
	m_SecondlyHeat.push_back(pPlayer->GetHeat());

	//���݂̃f�[�^�����擾
	int nSize = m_SecondlyHeat.size();

	//�ۑ�����Ă���q�[�g�ʂ̍��v�l���v�Z
	float fAllHeat = 0.0f;
	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		fAllHeat += m_SecondlyHeat.at(nCnt);
	}

	//���U���g�p�X�R�A�̍\���̏��擾
	ResultScore aResultScore = GetResultScore();

	//���σq�[�g�ʂ��X�V���Đݒ�
	aResultScore.fAverageHeat = fAllHeat / static_cast<float>(nSize);
	SetResultScore(aResultScore);
}

//===========================================================================================================
// �t�H�O�ݒ菈��
//===========================================================================================================
void CGame::SetFog()
{
	//�����_���[�C���X�^���X�擾
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();

	//�ʏ�G���A
	if (m_pGame->m_Area == GAME_AREA::NORMAL)
	{
		//�t�H�O�𖳌���
		pRenderer->OffFog();
	}
	//�C�x���g�G���A
	else
	{
		//�t�H�O��L����
		pRenderer->OnFog();
	}
}