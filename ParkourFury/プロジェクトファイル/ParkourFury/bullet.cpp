//==============================================================================================================================================
//
// �o���b�g�̏���
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "bullet.h"
#include "boss.h"
#include "particle.h"
#include "player.h"
#include "blockmanager.h"
#include "enemymanager.h"
#include "bulletmanager.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CBullet::CBullet(int nPriority) : CObjectX(nPriority),
	m_pPrev(nullptr),
	m_pNext(nullptr),
	m_Type(TYPE::NONE),
	m_nLife(0)
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CBullet::~CBullet()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CBullet::Init()
{
	//���f������
	CXfile* pModel = CXfile::GetInstance();
	BindModel(pModel->GetAddress(CTag::TAG::BULLET, static_cast<int>(m_Type)));//�ݒ�
	SetSize();//�T�C�Y�ݒ�

	//���N���X����������
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CBullet::Uninit()
{
	//���N���X�I������
	CObjectX::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CBullet::Update()
{
	//���N���X�̍X�V����
	CObjectX::Update();

	//������0�̏ꍇ
	if (m_nLife <= 0)
	{
		//�I������
		Uninit();
	}
	//�������c���Ă���ꍇ
	else
	{
		//���������炷
		m_nLife--;
	}

	//���W���X�V
	SetOldPos(GetPos());
	SetPos(GetPos() + GetMove());

	//�G�l�~�[�Ƃ̓����蔻��
	CollisionEnemy();

	//�{�X�Ƃ̓����蔻��
	CollisionBoss();

	//�u���b�N�Ƃ̓����蔻��
	CollisionBlock();
}

//===========================================================================================================
// �G�l�~�[�Ƃ̓����蔻��
//===========================================================================================================
void CBullet::CollisionEnemy()
{
	//�����蔻��p�̍\���̏���ݒ�
	CEnemy::CollisionParam aParam = {};
	aParam.pos = GetPos();//���W
	aParam.size = GetSize();//�T�C�Y
	aParam.pEnemy = nullptr;//�G�ꂽ�G�l�~�[�̏��

	//�G�l�~�[�Ƃ̓����蔻�肪false�̏ꍇ
	if (!CManager::GetInstance()->GetScene()->GetEnemyManager()->Collision(aParam)) return;
	
	//�I������
	Uninit();

	//�G�l�~�[�^�C�v���{�[���ł͂Ȃ��ꍇ
	if (aParam.pEnemy->GetEnemyType() == CEnemy::TYPE::BALL) return;

	//���j���ʉ��Đ�
	CManager::GetInstance()->GetSound()->Play(CSound::SOUND::HIT);

	//�G�l�~�[�̏I������
	aParam.pEnemy->Uninit();

	//�p�[�e�B�N������
	for (int nCnt = 0; nCnt < 30; nCnt++)
	{
		//�����_���ɃJ���[��ݒ�
		float fR = static_cast<float>(rand() % 10 + 1) * 0.1f;
		float fG = static_cast<float>(rand() % 10 + 1) * 0.1f;
		float fB = static_cast<float>(rand() % 10 + 1) * 0.1f;

		CParticle::Create(CParticle::TYPE::NORMAL, aParam.pEnemy->GetPos(), { 25.0f, 25.0f, 0.0f }, 20, { fR, fG, fB, 1.0f });
	}

	//�v���C���[�J�e�S���[�̃I�u�W�F�N�g�����擾
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);

	//�I�u�W�F�N�g��񂪑��݂���ꍇ
	if (pFindObj == nullptr) return;

	//�v���C���[�N���X�Ƀ_�E���L���X�g
	CPlayer* pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//�L�������Z
	pPlayer->IncreaseKill();
}

//===========================================================================================================
// �{�X�Ƃ̓����蔻��
//===========================================================================================================
void CBullet::CollisionBoss()
{
	//�{�X�J�e�S���[�̃I�u�W�F�N�g�����擾
	CObject* pFindObj = CObject::FindObject(CObject::Category::BOSS);
	CBoss* pBoss = nullptr;

	//�I�u�W�F�N�g��񂪑��݂���ꍇ
	if (pFindObj != nullptr)
	{
		//�{�X�N���X�Ƀ_�E���L���X�g
		pBoss = CObject::DownCast<CBoss, CObject>(pFindObj);
	}

	//�����蔻��p�̍\���̏���ݒ�
	CBoss::CollisionParam aParam = {};
	aParam.pos = GetPos();//���W
	aParam.size = GetSize();//�T�C�Y

	//�{�X�Ƃ̓����蔻�肪true�̏ꍇ
	if (pBoss != nullptr && pBoss->Collision(aParam))
	{
		//�I������
		Uninit();

		//�{�X�̃��C�t��������
		pBoss->DecreaseLife();
	}
}

//===========================================================================================================
// �u���b�N�Ƃ̓����蔻��
//===========================================================================================================
void CBullet::CollisionBlock()
{
	//�����蔻��p�p�����[�^�ݒ�
	CBlock::CollisionParam aParam;
	aParam.pos = GetPos();//���W
	aParam.oldpos = GetOldPos();//�ߋ��̍��W
	aParam.size = GetSize();//�T�C�Y
	aParam.move = GetMove();//�ړ���
	aParam.rot = GetRot();//�p�x
	aParam.type = CBlock::TYPE::NONE;//���������u���b�N�̃^�C�v

	//�㑤�̔�����擾
	CBlock::CollisionFlag aFlag = CManager::GetInstance()->GetScene()->GetBlockManager()->Collision(aParam);
	
	//�����ꂩ�̓����蔻�肪true
	if (aFlag.X || aFlag.Y_UP || aFlag.Y_DOWN || aFlag.Z)
	{
		//�e���폜
		Uninit();

		//�p�[�e�B�N������
		for (int nCnt = 0; nCnt < 10; nCnt++)
		{
			CParticle::Create(CParticle::TYPE::NORMAL, aParam.pos, { 15.0f, 15.0f, 0.0f }, 10, { 1.0f, 1.0f, 0.0f, 1.0f });
		}
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CBullet::Draw()
{
	//���N���X�`�揈��
	CObjectX::Draw();
}

//===========================================================================================================
// ��������
//===========================================================================================================
CBullet* CBullet::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, const D3DXVECTOR3& rot, TYPE m_type)
{
	//�^�C�v�ɉ����ă������𓮓I�m��
	CBullet* pBullet = NEW CBullet(static_cast<int>(CObject::Category::BULLET));
	switch (m_type)
	{
		//�v���C���[
	case TYPE::PLAYER:
		break;

	default:
		break;
	}

	//�o���b�g��񂪑��݂���ꍇ
	if (pBullet != nullptr)
	{
		//�p�����[�^�ݒ�
		pBullet->SetPos(pos);//���W
		pBullet->SetMove(move);//�ړ���
		pBullet->SetRot(rot);//�p�x
		pBullet->SetBulletType(m_type);//�^�C�v
		pBullet->SetLife(30);

		//����������
		pBullet->Init();

		//�}�l�[�W���[�o�^
		CManager::GetInstance()->GetScene()->GetBulletManager()->Regist(pBullet);
	}

	return pBullet;
}
