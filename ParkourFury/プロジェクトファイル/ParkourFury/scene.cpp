//==============================================================================================================================================
//
// �V�[���Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "scene.h"
#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "result.h"
#include "ranking.h"
#include "edit3D.h"
#include "edit2D.h"
#include "editmanager.h"
#include "blockmanager.h"
#include "gimmickmanager.h"
#include "bulletmanager.h"
#include "enemymanager.h"
#include "areamanager.h"
#include "UImanager.h"
//#include <EffekseerForDXLib_170e_324b/VC/DxLib.h>
//#include <EffekseerForDXLib_170e_324b/VC/EffekseerForDXLib.h>

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
CScene::ResultScore CScene::m_aResultScore = {};

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CScene::CScene() : 
	m_pEdit2D{ nullptr },
	m_pEditManager{ nullptr },
	m_pBlockManager{ nullptr },
	m_pGimmickManager{ nullptr },
	m_pBulletManager{ nullptr },
	m_pEnemyManager{ nullptr },
	m_pAreaManager{ nullptr },
	m_pUIManager{ nullptr },
	m_mode{ MODE::MODE_NONE },
	m_Scene{ SCENE::GAME }
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CScene::~CScene()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CScene::Init()
{
#ifdef _DEBUG
	//2D�G�f�B�^����������
	m_pEdit2D = NEW CEdit2D();
	if (FAILED(m_pEdit2D->Init()))
	{
		return E_FAIL;
	}
#endif // _DEBUG

	//UI�}�l�[�W���[����������
	m_pUIManager = NEW CUIManager();
	if (FAILED(m_pUIManager->Init()))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CScene::Uninit()
{
#ifdef _DEBUG
	//�G�f�B�^�I�u�W�F�N�g�}�l�[�W���[
	if (m_pEditManager != nullptr)
	{
		m_pEditManager->Uninit();
		m_pEditManager = nullptr;
	}

	//2D�I�u�W�F�N�g�}�l�[�W���[
	if (m_pEdit2D != nullptr)
	{
		m_pEdit2D->Uninit();
		delete m_pEdit2D;
		m_pEdit2D = nullptr;
	}
#endif // _DEBUG

	//�u���b�N�}�l�[�W���[
	if (m_pBlockManager != nullptr)
	{
		m_pBlockManager->Uninit();
		m_pBlockManager = nullptr;
	}

	//�M�~�b�N�}�l�[�W���[
	if (m_pGimmickManager != nullptr)
	{
		m_pGimmickManager->Uninit();
		m_pGimmickManager = nullptr;
	}

	//�o���b�g�}�l�[�W���[
	if (m_pBulletManager != nullptr)
	{
		m_pBulletManager->Uninit();
		m_pBulletManager = nullptr;
	}

	//�G�l�~�[�}�l�[�W���[
	if (m_pEnemyManager != nullptr)
	{
		m_pEnemyManager->Uninit();
		m_pEnemyManager = nullptr;
	}

	//�G���A�}�l�[�W���[
	if (m_pAreaManager != nullptr)
	{
		m_pAreaManager->Uninit();
		m_pAreaManager = nullptr;
	}

	//UI�}�l�[�W���[
	if (m_pUIManager != nullptr)
	{
		m_pUIManager->Uninit();
		m_pUIManager = nullptr;
	}

	//�G�l�~�[�������Z�b�g
	CEnemy::ResetAll();

	//�S�I�u�W�F�N�g�I������
	CObject::UninitAll();

	//�J�����̍��W�������Z�b�g
	CManager::GetInstance()->GetCamera()->Reset();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CScene::Update()
{
	//�f�o�b�O�̂�
#ifdef _DEBUG
	//�}�l�[�W���[�̃C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();

	//�L�[�{�[�h�̃C���X�^���X�擾
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();

	switch (m_Scene)
	{
		//�Q�[���V�[��
	case SCENE::GAME:

		//F5�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetTrigger(DIK_F5))
		{
			//2D�G�f�B�^�V�[���ɂ���
			m_Scene = SCENE::EDIT_2D;
		}
		break;

		//2D�G�f�B�^�V�[��
	case SCENE::EDIT_2D:

		//2D�G�f�B�^�X�V����
		m_pEdit2D->Update();

		//F5�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetTrigger(DIK_F5))
		{
			//�Q�[���V�[���ɂ���
			m_Scene = SCENE::GAME;
		}
		break;

		//3D�G�f�B�^�V�[��
	case SCENE::EDIT_3D:
		//3D�G�f�B�^�X�V����
		CEdit3D::EditUpdate();
		break;

	default:
		break;

	}
#endif // DEBUG
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CScene::Draw()
{
}

//===========================================================================================================
// ��������
//===========================================================================================================
CScene* CScene::Create(MODE mode)
{
	//���[�J���ϐ��錾
	CScene* pScene = nullptr;//�V�[�����̃|�C���^
	
	//���[�h�ʂɓ��I�m��
	switch (mode)
	{
		//�^�C�g��
	case CScene::MODE::MODE_TITLE:
		pScene = CTitle::Create();
		break;
	
		//�`���[�g���A��
	case CScene::MODE::MODE_TUTORIAL:
		pScene = CTutorial::Create();
		break;

		//�Q�[��
	case CScene::MODE::MODE_GAME:
		pScene = CGame::Create();
		break;

		//���U���g
	case CScene::MODE::MODE_RESULT:
		pScene = CResult::Create();
		break;

		//�����L���O
	case CScene::MODE::MODE_RANKING:
		pScene = CRanking::Create();
		break;

	default:
		break;
	}

	//���[�h�̐ݒ�
	pScene->SetMode(mode);

	return pScene;
}
