//==============================================================================================================================================
//
// �M�~�b�N�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "gimmick.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CGimmick::CGimmick(int nPriority) : CObjectX(nPriority)
{
	//�����o�ϐ�������
	m_pPrev = nullptr;//�O�̃M�~�b�N���
	m_pNext = nullptr;//���̃M�~�b�N���
	m_Type = TYPE::NONE;//�^�C�v
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CGimmick::~CGimmick()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CGimmick::Init()
{
	//���f������
	CXfile* pModel = CXfile::GetInstance();//�C���X�^���X���擾
	BindModel(pModel->GetAddress(CTag::TAG::GIMMICK, static_cast<int>(m_Type)));//�ݒ�
	SetSize();//�T�C�Y�ݒ�

	//X�t�@�C���I�u�W�F�N�g����������
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CGimmick::Uninit()
{
	//X�t�@�C���I�u�W�F�N�g�I������
	CObjectX::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CGimmick::Release()
{
	//X�t�@�C���I�u�W�F�N�g�������
	CObjectX::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CGimmick::Update()
{
	//X�t�@�C���I�u�W�F�N�g�X�V����
	CObjectX::Update();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CGimmick::Draw()
{
	//�G�f�B�^���[�h�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//�`�悹���ɏ����𔲂���
		return;
	}

	//X�t�@�C���I�u�W�F�N�g�`�揈��
	CObjectX::Draw();
}

//===========================================================================================================
// ��������
//===========================================================================================================
#include "gimmickmanager.h"
#include "playerspawner.h"
#include "rotationzone.h"
#include "bossspawner.h"
#include "bossmovepoint.h"
#include "ballspawner.h"
#include "ballmovepoint.h"
CGimmick* CGimmick::Create(D3DXVECTOR3 pos, D3DXVECTOR3 scale, TYPE type)
{
	//�^�C�v�ɉ����ă������𓮓I�m��
	CGimmick* pGimmick = nullptr;

	//�^�C�v�ʂɓ��I�m��
	switch (type)
	{
		//�v���C���[�X�|�i�[
	case TYPE::PLAYER_SPAWNER:
		pGimmick = NEW CPlayerSpawner();
		break;

		//��]�]�[��
	case TYPE::ROTATION_ZONE:
		pGimmick = NEW CRotationZone();
		break;

		//�{�X�X�|�i�[
	case TYPE::BOSS_SPAWNER:
		pGimmick = NEW CBossSpawner();
		break;

		//�{�X�ړ��|�C���g
	case TYPE::BOSS_MOVE:
		pGimmick = NEW CBossMovePoint();
		break;

		//�{�[���X�|�i�[
	case TYPE::BALL_SPAWNER:
		pGimmick = NEW CBallSpawner();
		break;

		//�{�[���ړ��|�C���g
	case TYPE::BALL_MOVE:
		pGimmick = NEW CBallMovePoint();
		break;

	default:
		break;
	}

	//�M�~�b�N��񂪑��݂���ꍇ
	if (pGimmick != nullptr)
	{
		//�p�����[�^�ݒ�
		pGimmick->m_Type = type;//�^�C�v
		pGimmick->SetPos(pos);//���W
		pGimmick->SetScale(scale);//�g�嗦

		//����������
		pGimmick->Init();

		//�}�l�[�W���[�o�^����
		CManager::GetInstance()->GetScene()->GetGimmickManager()->Regist(pGimmick);
	}

	return pGimmick;
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
bool CGimmick::Collision(CollisionParam& aParam)
{
	//�����擾
	bool bCollision = false;//�����蔻��p�t���O
	D3DXVECTOR3 Pos = GetPos();//�ʒu
	D3DXVECTOR3 Size = GetSize();//�T�C�Y

	//�d�Ȃ����ꍇ
	if ((aParam.pos.x >= Pos.x - Size.x * 0.5f
		&& aParam.pos.x <= Pos.x + Size.x * 0.5f)
		&& (aParam.pos.y >= Pos.y - Size.y * 0.5f
			&& aParam.pos.y <= Pos.y + Size.y * 0.5f)
		&& (aParam.pos.z >= Pos.z - Size.z * 0.5f
			&& aParam.pos.z <= Pos.z + Size.z * 0.5f))
	{
		//�����蔻��̃t���O��true�ɂ���
		bCollision = true;
	}

	return bCollision;
}
