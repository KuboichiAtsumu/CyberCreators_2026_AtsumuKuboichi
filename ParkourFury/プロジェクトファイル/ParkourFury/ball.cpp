//==============================================================================================================================================
//
// �{�[���Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "ball.h"
#include "game.h"
#include "UImanager.h"
#include "ballmovepoint.h"
#include "blockmanager.h"
#include "gimmickmanager.h"
#include "ballmovepoint.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CBall::CBall() : CEnemy()
{
	//�����o�ϐ�������
	m_TargetMove = { 0.0f, 0.0f, 0.0f };//�ڕW�̈ړ���
	m_nPointIdx = 0;//���݂���|�W�V�����̃C���f�b�N�X
	m_bUpdateMove = true;//�ړ��ʍX�V�t���O
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CBall::~CBall()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CBall::Init()
{
	//���N���X����������
	if (FAILED(CEnemy::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CBall::Uninit()
{
	//�`�F�C�X�G���A�x���̃I�u�W�F�N�g��������
	CUI* pUI = CManager::GetInstance()->GetScene()->GetUIManager()->FindUI(CUI::TYPE::ESCAPE);

	//�������ʂ��s��v
	if (pUI == nullptr || pUI->GetUIType() != CUI::TYPE::ESCAPE)
	{
		return;
	}

	//UI�I������
	pUI->Uninit();

	//���N���X�I������
	CEnemy::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CBall::Release()
{
	//���N���X�������
	CEnemy::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CBall::Update()
{
	//���N���X�X�V����
	CEnemy::Update();

	//��]����
	Turn();

	//�d�͏���
	Gravity();

	//�ړ�����
	Move();

	//�����蔻��
	CollisionObj();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CBall::Draw()
{
	//�G�f�B�^���[�h�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//�`�悹���ɏ����𔲂���
		return;
	}

	//���N���X�`�揈��
	CEnemy::Draw();
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
bool CBall::Collision(CollisionParam& aParam)
{
	//�����蔻��t���O
	bool bCollision = false;

	//�����蔻�肪true
	if (CEnemy::Collision(aParam))
	{
		//�t���O��true�ɂ���
		bCollision = true;
	}

	return bCollision;
}

//===========================================================================================================
// ���Z�b�g����
//===========================================================================================================
void CBall::Reset()
{
	//�p�����[�^�ݒ�
	SetMove({ 0.0f, 0.0f, 0.0f });//�ړ���

	//�����o�ϐ�������
	m_TargetMove = { 0.0f, 0.0f, 0.0f };//�ڕW�̈ړ���
	m_nPointIdx = 0;//���݂���|�W�V�����̃C���f�b�N�X
	m_bUpdateMove = true;//�ړ��ʍX�V�t���O
}

//===========================================================================================================
// ��]����
//===========================================================================================================
void CBall::Turn()
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 move = GetMove();//�ړ��ʏ��
	D3DXVECTOR3 rot = GetRot();//�p�x���
	float fTurn = 0.0f;//��]��

	//��ʍ��ɓ����Ă���
	if (move.z > 0.0f)
	{
		fTurn = TURN_SPEED;
	}
	//��ʉE�ɓ����Ă���
	else
	{
		fTurn = -TURN_SPEED;
	}

	//��]����
	rot.x += fTurn;

	//�p�x�␳
	CorrectionRot(rot.x);

	//�X�V���ꂽ�p�x���ݒ�
	SetRot(rot);
}

//===========================================================================================================
// �d�͏���
//===========================================================================================================
void CBall::Gravity()
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 move = GetMove();//�ړ��ʎ擾

	//�d�͂����Z
	move.y -= GRAVITY_POWER;

	//�d�͂����������Ȃ��悤�ɂ���
	if (move.y < MAX_GRAVITY)
	{
		move.y = MAX_GRAVITY;
	}

	//�ړ��ʂ�ݒ�
	SetMove(move);
}

//===========================================================================================================
// �ړ�����
//===========================================================================================================
void CBall::Move()
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 pos = GetPos();//���W�擾
	D3DXVECTOR3 move = GetMove();//�ړ��ʎ擾

	//�X�V�t���O��true
	if (m_bUpdateMove)
	{
		//���̃|�C���g������
		CBallMovePoint* pPoint = CBallMovePoint::Find(m_nPointIdx);

		//�������ʂ����v���Ȃ������珈���𔲂���
		if (pPoint == nullptr || pPoint->GetIdx() != m_nPointIdx) return;

		//�|�C���g�܂ł̕����x�N�g�������߂�
		D3DXVECTOR3 Direction = pPoint->GetPos() - pos;

		//���̃|�C���g�ւ̊p�x�����߂�
		float fAngle = atan2f(Direction.x, Direction.z);

		//�ړ��ʂ�ݒ�
		m_TargetMove = { sinf(fAngle) * MOVE_SPEED, move.y, cosf(fAngle) * MOVE_SPEED };

		//�t���O��false�ɂ���
		m_bUpdateMove = false;
	}

	//�ړ��ʂ��ڕW�̒l��菬�����ꍇ
	if (move.z < m_TargetMove.z)
	{
		//�ړ��ʂ����Z
		move.z += ADD_MOVE;

		//�ڕW�̒l�𒴂��Ȃ��悤�ɒ���
		if (move.z > m_TargetMove.z)
		{
			move.z = m_TargetMove.z;
		}
	}
	//�ړ��ʂ��ڕW�̒l���傫���ꍇ
	else if (move.z > m_TargetMove.z)
	{
		//�ړ��ʂ����Z
		move.z -= ADD_MOVE;

		//�ڕW�̒l�������Ȃ��悤�ɒ���
		if (move.z < m_TargetMove.z)
		{
			move.z = m_TargetMove.z;
		}
	}
 
	//�ړ��ʂ�ݒ�
	SetMove(move);

	//�ߋ��̍��W��ݒ�
	SetOldPos(pos);

	//�ړ��ʂ����Z���č��W��ݒ�
	SetPos(pos + move);
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
void CBall::CollisionObj()
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 pos = GetPos();//���W
	D3DXVECTOR3 size = GetSize();//�T�C�Y

	//�u���b�N�̐擪���擾
	CBlock* pBlock = CManager::GetInstance()->GetScene()->GetBlockManager()->GetTop();

	//�|�C���^��nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pBlock != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CBlock* pNext = pBlock->GetNextBlock();

		//�u���b�N��񂪑��݂���ꍇ
		if (pBlock != nullptr)
		{
			//�p�����[�^�ݒ�
			D3DXVECTOR3 BlockPos = pBlock->GetPos();//���W
			D3DXVECTOR3 BlockSize = pBlock->GetSize();//�T�C�Y

			//�u���b�N�Ƃ̓����蔻��
			CollisionBlock(BlockPos, BlockSize);
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pBlock = pNext;
	}

	//�M�~�b�N�̐擪���擾
	CGimmick* pGimmick = CManager::GetInstance()->GetScene()->GetGimmickManager()->GetTop();

	//�|�C���^��nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pGimmick != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CGimmick* pNext = pGimmick->GetNext();

		//�{�[���ړ��|�C���g�̏����擾�����ꍇ
		if (pGimmick != nullptr && pGimmick->GetType() == CGimmick::TYPE::BALL_MOVE)
		{
			//�p�����[�^�ݒ�
			D3DXVECTOR3 PointPos = pGimmick->GetPos();//���W
			D3DXVECTOR3 PointSize = pGimmick->GetSize();//�T�C�Y

			//�|�C���g�Ƃ̓����蔻�肪true
			if (CollisionPoint(PointPos, PointSize))
			{
				//���̖ڕW�|�C���g�̃C���f�b�N�X���擾
				CBallMovePoint* pBallMovePoint = dynamic_cast<CBallMovePoint*>(pGimmick);
				m_nPointIdx = pBallMovePoint->GetIdx() + 1;

				//�t���O��true�ɂ���
				m_bUpdateMove = true;

				//�ŏI�n�_�ɓ��B�����ꍇ
				if (m_nPointIdx == pBallMovePoint->GetAll())
				{
					//�I������
					Uninit();
				}

				break;
			}
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pGimmick = pNext;
	}
}

//===========================================================================================================
// �u���b�N�Ƃ̓����蔻��
//===========================================================================================================
void CBall::CollisionBlock(D3DXVECTOR3 BlockPos, D3DXVECTOR3 BlockSize)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 pos = GetPos();//�������g�̍��W
	D3DXVECTOR3 oldpos = GetOldPos();//�������g�̉ߋ��̍��W
	D3DXVECTOR3 size = GetSize();//�������g�̃T�C�Y
	D3DXVECTOR3 move = GetMove();//�������g�̈ړ���

	//=======================================================================
	// Y��
	//=======================================================================

	//�㑤�̔���
	if (pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f
		&& oldpos.y - size.y * 0.5f >= BlockPos.y + BlockSize.y * 0.5f)
	{
		//XZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((pos.x >= BlockPos.x - BlockSize.x * 0.5f
			&& pos.x <= BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.z >= BlockPos.z - BlockSize.z * 0.5f
				&& pos.z <= BlockPos.z + BlockSize.z * 0.5f))
		{
			//���W���C��
			pos.y = BlockPos.y + BlockSize.y * 0.5f + size.y * 0.5f;

			//Y���̈ړ��ʂ����Z�b�g
			move.y *= BOUND_POWER;
		}
	}

	//�����̔���
	if (pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
		&& oldpos.y + size.y * 0.5f <= BlockPos.y - BlockSize.y * 0.5f)
	{
		//XZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((pos.x > BlockPos.x - BlockSize.x * 0.5f
			&& pos.x < BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.z > BlockPos.z - BlockSize.z * 0.5f
				&& pos.z < BlockPos.z + BlockSize.z * 0.5f))
		{
			//���W���C��
			pos.y = BlockPos.y - BlockSize.y * 0.5f - size.y * 0.5f;

			//Y���̈ړ��ʂ����Z�b�g
			move.y = 0.0f;
		}
	}

	//=======================================================================
	// X��
	//=======================================================================

	//�����̔���
	if (pos.x + size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
		&& oldpos.x + size.x * 0.5f <= BlockPos.x - BlockSize.x * 0.5f)
	{
		//YZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
			&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f)
			&& (pos.z + size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f
				&& pos.z - size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//���W���C��
			pos.x = BlockPos.x - BlockSize.x * 0.5f - size.x * 0.5f;
			move.x *= -1.0f;
		}
	}

	//�E���̔���
	if (pos.x - size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f
		&& oldpos.x - size.x * 0.5f >= BlockPos.x + BlockSize.x * 0.5f)
	{
		//YZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
			&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f)
			&& (pos.z + size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f
				&& pos.z - size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//���W���C��
			pos.x = BlockPos.x + BlockSize.x * 0.5f + size.x * 0.5f;
			move.x *= -1.0f;
		}
	}

	//=======================================================================
	// Z��
	//=======================================================================

	//��O���̔���
	if (pos.z + size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f
		&& oldpos.z + size.z * 0.5f <= BlockPos.z - BlockSize.z * 0.5f)
	{
		//XY�Ԃ͈͓̔��ɂ���ꍇ
		if ((pos.x + size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
			&& pos.x - size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
				&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f))
		{
			//���W���C��
			pos.z = BlockPos.z - BlockSize.z * 0.5f - size.z * 0.5f;
			move.z *= -1.0f;
		}
	}

	//�����̔���
	if (pos.z - size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f
		&& oldpos.z - size.z * 0.5f >= BlockPos.z + BlockSize.z * 0.5f)
	{
		//XY�Ԃ͈͓̔��ɂ���ꍇ
		if ((pos.x + size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
			&& pos.x - size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
				&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f))
		{
			//���W���C��
			pos.z = BlockPos.z + BlockSize.z * 0.5f + size.z * 0.5f;
			move.z *= -1.0f;
		}
	}

	//�p�����[�^�ݒ�
	SetPos(pos);//���W
	SetMove(move);//�ړ���
}

//===========================================================================================================
// �|�C���g�Ƃ̓����蔻��
//===========================================================================================================
bool CBall::CollisionPoint(D3DXVECTOR3 PointPos, D3DXVECTOR3 PointSize)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 pos = GetPos();//�������g�̍��W
	D3DXVECTOR3 size = GetSize();//�������g�̃T�C�Y
	bool bCollision = false;//�����蔻��t���O

	//�͈͓��ɂ���ꍇ
	if ((pos.x >= PointPos.x - PointSize.x * 0.5f && 
			pos.x <= PointPos.x + PointSize.x * 0.5f) && 
		(pos.y - size.y * 0.5f >= PointPos.y - PointSize.y * 0.5f &&
			pos.y - size.y * 0.5f <= PointPos.y + PointSize.y * 0.5f) &&
		(pos.z >= PointPos.z - PointSize.z * 0.5f && 
			pos.z <= PointPos.z + PointSize.z * 0.5f))
	{
		//�t���O��true�ɂ���
		bCollision = true;
	}

	return bCollision;
}
