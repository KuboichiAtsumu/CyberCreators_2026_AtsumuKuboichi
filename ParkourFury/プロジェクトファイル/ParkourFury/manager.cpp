//==============================================================================================================================================
//
// �Q�[���}�l�[�W���[
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "manager.h"
#include "xfile.h"
#include "texture.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
CManager* CManager::m_pManager = nullptr;

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CManager::CManager() : 
	m_pRenderer{ nullptr },
	m_pKeyboard{ nullptr },
	m_pMouse{ nullptr },
	m_pJoypad{ nullptr },
	m_pSound{ nullptr },
	m_pCamera{ nullptr },
	m_pLight{ nullptr },
	m_pScene{ nullptr },
	m_pFade{ nullptr },
	m_pCollision{ nullptr },
	m_pTag{ nullptr }
{
	srand((unsigned)time(nullptr));
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CManager::~CManager()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	//�����_���[�̐���
	m_pRenderer = NEW CRenderer();
	m_pRenderer->Init(hWnd, bWindow);

	//�L�[�{�[�h�̏�����
	m_pKeyboard = NEW CInputKeyboard();
	m_pKeyboard->Init(hInstance, hWnd);

	//�}�E�X�̏�����
	m_pMouse = NEW CInputMouse();
	m_pMouse->Init(hInstance, hWnd);

	//�W���C�p�b�h�̏�����
	m_pJoypad = NEW CInputJoypad();
	m_pJoypad->Init();

	//�T�E���h�̏�����
	m_pSound = NEW CSound();
	m_pSound->Init(hWnd);

	//�J�����̏�����
	m_pCamera = NEW CCamera();
	m_pCamera->Init();

	//���C�g�̏�����
	m_pLight = NEW CLight();
	m_pLight->Init();

	//�^�O�̃C���X�^���X�𐶐�
	m_pTag = NEW CTag();

	//�����蔻��̃C���X�^���X�𐶐�
	m_pCollision = NEW CCollision();

	//�v�Z�����̃C���X�^���X�𐶐�
	m_pCalculate = NEW CCalculate();
	
	//�t�F�[�h��ʐ���
	m_pFade = CFade::Create();

	//���[�h�̏�����
	SetMode(CScene::MODE::MODE_TITLE);

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CManager::Uninit()
{
	//�T�E���h�S��~
	GetSound()->Stop();

	//�^�O
	if (m_pTag != nullptr)
	{
		delete m_pTag;
		m_pTag = nullptr;
	}

	//�t�F�[�h
	if (m_pFade != nullptr)
	{
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = nullptr;
	}

	//�V�[��
	if (m_pScene != nullptr)
	{
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = nullptr;
	}

	//�����_���[
	if (m_pRenderer != nullptr)
	{
		m_pRenderer->Uninit();
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}

	//�L�[�{�[�h
	if (m_pKeyboard != nullptr)
	{
		m_pKeyboard->Uninit();
		delete m_pKeyboard;
		m_pKeyboard = nullptr;
	}

	//�}�E�X
	if (m_pMouse != nullptr)
	{
		m_pMouse->Uninit();
		delete m_pMouse;
		m_pMouse = nullptr;
	}

	//Pad
	if (m_pJoypad != nullptr)
	{
		m_pJoypad->Uninit();
		delete m_pJoypad;
		m_pJoypad = nullptr;
	}

	//�T�E���h
	if (m_pSound != nullptr)
	{
		m_pSound->Uninit();
		delete m_pSound;
		m_pSound = nullptr;
	}

	//�J����
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}
	
	//���C�g
	if (m_pLight != nullptr)
	{
		m_pLight->Uninit();
		delete m_pLight;
		m_pLight = nullptr;
	}

	//�����蔻��
	if (m_pCollision != nullptr)
	{
		delete m_pCollision;
		m_pCollision = nullptr;
	}

	//�v�Z����
	if (m_pCalculate != nullptr)
	{
		delete m_pCalculate;
		m_pCalculate = nullptr;
	}

	//�e�N�X�`���ꊇ�Ǘ�
	CTexture::Release();

	//X�t�@�C���ꊇ�Ǘ�
	CXfile::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CManager::Update()
{
	//�W���C�p�b�h�X�V����
	m_pJoypad->Update();

	//�L�[�{�[�h�X�V����
	m_pKeyboard->Update();

	//�}�E�X�X�V����
	m_pMouse->Update();

	//�t�F�[�h�X�V����
	m_pFade->Update();

	//�V�[���X�V����
	m_pScene->Update();

	//�����_���[�X�V����
	m_pRenderer->Update();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CManager::Draw()
{
	//�����_���[�`�揈��
	m_pRenderer->Draw();
}

//===========================================================================================================
// ���[�h�̐ݒ�
//===========================================================================================================
void CManager::SetMode(CScene::MODE mode)
{
	//�V�[����񂪑��݂���ꍇ
	if (m_pScene != nullptr)
	{
		//�I������
		m_pScene->Uninit();

		delete m_pScene;
		m_pScene = nullptr;
	}

	//�V���ȃV�[���𐶐�
	m_pScene = CScene::Create(mode);

	//�V�[������������
	m_pScene->Init();
}

//===========================================================================================================
// �C���X�^���X�̎擾
//===========================================================================================================
CManager* CManager::GetInstance()
{
	//�C���X�^���X�����݂��Ȃ��ꍇ
	if (m_pManager == nullptr)
	{
		//�C���X�^���X�𐶐�
		m_pManager = NEW CManager();
	}

	return m_pManager;
}

//===========================================================================================================
// �C���X�^���X�̔j��
//===========================================================================================================
void CManager::Release()
{
	//�C���X�^���X�����݂���ꍇ
	if (m_pManager != nullptr)
	{
		//�C���X�^���X��j��
		m_pManager->Uninit();
		delete m_pManager;
		m_pManager = nullptr;
	}
}
