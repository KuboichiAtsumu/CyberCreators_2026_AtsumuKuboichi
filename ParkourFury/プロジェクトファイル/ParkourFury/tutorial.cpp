//==============================================================================================================================================
//
// �`���[�g���A����ʂɊւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "tutorial.h"
#include "edit3D.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CTutorial::CTutorial() : CScene()
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CTutorial::~CTutorial()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
#include "back_3D.h"
#include "editmanager.h"
#include "blockmanager.h"
#include "enemymanager.h"
#include "bulletmanager.h"
#include "gimmickmanager.h"
#include "signboardmanager.h"
HRESULT CTutorial::Init()
{
	//BGM�Đ�
	CManager::GetInstance()->GetSound()->Play(CSound::SOUND::BGM_TUTORIAL);

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

	//�Ŕ}�l�[�W���[�̃C���X�^���X�擾
	CSignBoardManager* pSignBoardManager = NEW CSignBoardManager();
	SetSignBoardManager(pSignBoardManager);//�C���X�^���X�ݒ�
	if (FAILED(pSignBoardManager->Init())) return E_FAIL;//����������

	//�V�[������������
	if (FAILED(CScene::Init())) return E_FAIL;

	//�X�e�[�W�w�i����
	CBack3D::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, CBack3D::TYPE::SKY);

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CTutorial::Uninit()
{
	//BGM�Đ��I��
	CManager::GetInstance()->GetSound()->Stop(CSound::SOUND::BGM_TUTORIAL);

	//���N���X�I������
	CScene::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CTutorial::Update()
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CScene* pScene = pManager->GetScene();//�V�[��
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();//�L�[�{�[�h
	CInputJoypad* pJoypad = pManager->GetJoypad();//�W���C�p�b�h

	//Enter�L�[�������ꂽ�ꍇ
	//START�{�^���������ꂽ�ꍇ
	if ((pKeyboard->GetTrigger(DIK_RETURN) ||
		pJoypad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_START)) &&
		pScene->GetSceneState() == SCENE::GAME)
	{
		//SE�Đ�
		pManager->GetSound()->Play(CSound::SOUND::SELECT);

		//�Q�[����ʂֈڍs
		pManager->GetFade()->SetFade(MODE::MODE_GAME);
	}

	//�J�����̍X�V����
	pManager->GetCamera()->Update();

	//���N���X�X�V
	CScene::Update();

	//�f�o�b�O�̂�
#ifdef _DEBUG

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
			pJoypad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_START))
		{
			//�Q�[����ʂֈڍs
			pManager->GetFade()->SetFade(MODE::MODE_GAME);
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
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CTutorial::Draw()
{
	//�J�����̐ݒ�
	CManager::GetInstance()->GetCamera()->SetCamera();
}

//===========================================================================================================
// ��������
//===========================================================================================================
CTutorial* CTutorial::Create()
{
	//�������𓮓I�m��
	CTutorial* pTutorial = NEW CTutorial();

	return pTutorial;
}
