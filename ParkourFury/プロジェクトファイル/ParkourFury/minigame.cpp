//==============================================================================================================================================
//
// �~�j�Q�[���Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "minigame.h"
#include "game.h"
#include "areamanager.h"
#include "enemymanager.h"
#include "player.h"
#include "UI.h"
#include "minigameclear.h"
#include "minigamefailed.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
int CMiniGame::m_nTimer = 0;

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CMiniGame::CMiniGame(int nPriority) : CObject(nPriority),
	m_nCntFrame(0)
{
	m_nTimer = TIMER;//�^�C�}�[
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CMiniGame::~CMiniGame()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CMiniGame::Init()
{
	//���N���X����������
	if (FAILED(CObject::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CMiniGame::Uninit()
{
	//�폜�t���O��true�ɐݒ�
	SetDeleteFlag(true);
}

//===========================================================================================================
// �������
//===========================================================================================================
void CMiniGame::Release()
{
	//���N���X�������
	CObject::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CMiniGame::Update()
{
	//�Q�[���V�[���̃C���X�^���X�擾
	CGame* pGame = CGame::GetInstance();

	//�~�j�Q�[�����ł͂Ȃ��ꍇ
	if (pGame != nullptr && pGame->GetArea() != CGame::GAME_AREA::MINI_GAME)
	{
		//�I������
		Uninit();

		//�����𔲂���
		return;
	}

	//�^�C�}�[�J�E���g����
	TimerCount();
}

//===========================================================================================================
// �^�C�}�[�J�E���g����
//===========================================================================================================
void CMiniGame::TimerCount()
{
	//�t���[���J�E���^�����Z
	m_nCntFrame++;

	//�t���[���J�E���^��60�t���[��
	if (m_nCntFrame == MAX_FRAME)
	{
		//�J�E���^�����Z�b�g
		m_nCntFrame = 0;
	}
	//�t���[���J�E���^��60�t���[���ł͂Ȃ�
	else
	{
		//�����𔲂���
		return;
	}

	//�V�[���̃C���X�^���X���擾
	CScene* pScene = CManager::GetInstance()->GetScene();

	//�~�j�Q�[���G���A������
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::MINI_GAME);

	//�G���A���nullptr�ł͂Ȃ��ꍇ
	if (pArea == nullptr)
	{
		assert(false);
	}

	//�G�l�~�[�J�E���^
	int nCntEnemy = 0;
	std::vector<CEnemy*> aEnemy;

	//�G�l�~�[�̐擪�I�u�W�F�N�g���擾
	CEnemy* pEnemy = pScene->GetEnemyManager()->GetTop();

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pEnemy != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//�����蔻�肪true�̏ꍇ
		if (pArea->Collision(pEnemy->GetPos()))
		{
			//�G�l�~�[�J�E���^���Z
			nCntEnemy++;

			//�Ώۂ̃G�l�~�[����ۑ�
			aEnemy.push_back(pEnemy);
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pEnemy = pNext;
	}

	//�v���C���[�J�e�S���[�̃I�u�W�F�N�g�����擾
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//�I�u�W�F�N�g��񂪑��݂���ꍇ
	if (pFindObj != nullptr)
	{
		//�v���C���[�N���X�Ƀ_�E���L���X�g
		pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);
	}
	//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
	else
	{
		assert(false);
	}

	//�G�t�F�N�g�������W��ݒ�
	D3DXVECTOR3 CreatePos = pPlayer->GetPos();
	CreatePos.y += CREATE_EFFECT_HEIGHT;

	//�G���A���̃G�l�~�[��0
	if (nCntEnemy == 0)
	{
		//�v���C���[�̏�ɃN���A�G�t�F�N�g��\��
		CMiniGameClear::Create(CreatePos, pPlayer);

		//�G���A���폜
		pArea->Uninit();

		//���݂̃G���A��ʏ�G���A�ɐݒ�
		CGame::GetInstance()->SetArea(CGame::GAME_AREA::NORMAL);

		//�I������
		Uninit();
	}

	//�^�C�}�[��0�ł͂Ȃ��ꍇ
	if (m_nTimer > 0)
	{
		//�^�C�}�[�����炷
		m_nTimer--;
	}

	//�G�l�~�[����ۑ�
	int nNumEnemy = aEnemy.size();

	//�^�C�}�[��0�̎��ɃG�l�~�[���c���Ă���ꍇ
	if (m_nTimer == 0 && nCntEnemy > 0)
	{
		//�G���A���̃G�l�~�[���폜
		for (int nCnt = 0; nCnt < nNumEnemy; nCnt++)
		{
			aEnemy.at(nCnt)->Uninit();
		}

		//�v���C���[�̏�Ɏ��s�G�t�F�N�g��\��
		CMiniGameFailed::Create(CreatePos, pPlayer);

		//�G���A���폜
		pArea->Uninit();

		//���݂̃G���A��ʏ�G���A�ɐݒ�
		CGame::GetInstance()->SetArea(CGame::GAME_AREA::NORMAL);

		//�I������
		Uninit();
	}

	//�G���A���̃G�l�~�[���폜
	for (int nCnt = 0; nCnt < nNumEnemy; nCnt++)
	{
		aEnemy.at(nCnt) = nullptr;
	}

	//�z������
	aEnemy.clear();
}

//===========================================================================================================
// ��������
//===========================================================================================================
CMiniGame* CMiniGame::Create()
{
	//�������𓮓I�m��
	CMiniGame* pMiniGame = NEW CMiniGame(static_cast<int>(Category::EVENT));

	//�|�C���^��nullptr�ł͂Ȃ��ꍇ
	if (pMiniGame != nullptr)
	{
		//����������
		pMiniGame->Init();
	}

	//���[�J���ϐ��錾
	int nNum = m_nTimer;//���݂̃^�C�}�[��ۑ�
	int nDigit = 0;//����

	//���������߂�
	while (nNum != 0)
	{//nNum��0�ɂȂ�܂�
		nNum /= 10;//�^�C�}�[��10�Ŋ���
		nDigit++;//���������Z
	}

	//�~�j�Q�[���p�^�C�}�[����
	CUI::Create(CUI::TYPE::MINIGAME_HEAD, { 950.0f, 570.0f }, { 400.0f, 60.0f }, { 0.0f, 0.0f, 0.0f });//�~�j�Q�[�����o��
	CUI::Create(CUI::TYPE::MINIGAME_BACK, { 950.0f, 640.0f }, { 500.0f, 50.0f }, { 0.0f, 0.0f, 0.0f });//�~�j�Q�[���^�C�}�[�Q�[�W�w�i
	CUI::Create(CUI::TYPE::MINIGAME_GAGE, { 950.0f - 490.0f * 0.5f, 640.0f }, { 490.0f, 40.0f }, { 0.0f, 0.0f, 0.0f });//�~�j�Q�[���^�C�}�[�Q�[�W
	CUI::Create(CUI::TYPE::MINIGAME_FRAME, { 950.0f, 659.0f }, { 500.0f, 88.0f }, { 0.0f, 0.0f, 0.0f });//�~�j�Q�[���^�C�}�[�Q�[�W�t���[��

	return pMiniGame;
}
