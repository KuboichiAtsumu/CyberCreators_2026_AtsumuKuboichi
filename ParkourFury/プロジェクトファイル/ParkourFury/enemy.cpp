//==============================================================================================================================================
//
// �u���b�N�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "enemy.h"
#include "enemymanager.h"
#include "ball.h"
#include "normalenemy.h"
#include "moveenemy.h"
#include "bulletenemy.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
int CEnemy::m_nAll = 0;

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CEnemy::CEnemy(int nPriority) : CObjectX(nPriority),
	m_pPrev{ nullptr },
	m_pNext{ nullptr },
	m_Type{ CEnemy::TYPE::NONE }
{
	//�������Z
	m_nAll++;
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CEnemy::~CEnemy()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CEnemy::Init()
{
	//�^�C�v����int�^�ŕۑ�
	int nType = static_cast<int>(m_Type);

	//���f������
	CXfile* pModel = CXfile::GetInstance();
	int nModelIdx = pModel->Regist(CTag::TAG::ENEMY, nType, MODEL_FILE[nType]);
	BindModel(pModel->GetAddress(nModelIdx));//�ݒ�
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
void CEnemy::Uninit()
{
	//���N���X�I������
	CObjectX::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CEnemy::Release()
{
	//���N���X�������
	CObjectX::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CEnemy::Update()
{
	//���N���X�X�V����
	CObjectX::Update();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CEnemy::Draw()
{
	//�G�f�B�^���[�h�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//�`�悹���ɏ����𔲂���
		return;
	}

	//X�t�@�C���̃I�u�W�F�N�g�`�揈��
	CObjectX::Draw();
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
bool CEnemy::Collision(CollisionParam& aParam)
{
	//�񋓌^��`
	enum class Vec3
	{
		X = 0,
		Y,
		Z,
		MAX
	};

	//���[�J���ϐ��錾
	bool bCollision[static_cast<int>(Vec3::MAX)] = {};//�e���̓����蔻��t���O

	//�e���̔ԍ���int�^�ŕۑ�
	int X = static_cast<int>(Vec3::X);
	int Y = static_cast<int>(Vec3::Y);
	int Z = static_cast<int>(Vec3::Z);

	//���ݗ����Ă����������擾
	STAND_LINE Line = GetStandLine();

	//XY����ɂ���ꍇ
	if (Line == STAND_LINE::XY)
	{
		//X�����画��
		bCollision[X] = Collision_X(aParam);
		bCollision[Z] = Collision_Z(aParam);
	}
	//YZ����ɂ���ꍇ
	else if (Line == STAND_LINE::YZ)
	{
		//Z�����画��
		bCollision[Z] = Collision_Z(aParam);
		bCollision[X] = Collision_X(aParam);
	}

	//Y���̔���
	bCollision[Y] = Collision_Y(aParam);

	//�S�Ă̎��̔��肪true�̏ꍇ
	if (bCollision[X] && bCollision[Y] && bCollision[Z])
	{
		//�������g�̏���ݒ�
		aParam.pEnemy = this;
		return true;
	}
	//�����ꂩ�̎��̔��肪false�̏ꍇ
	else
	{
		return false;
	}
}

//===========================================================================================================
// X���̓����蔻��
//===========================================================================================================
bool CEnemy::Collision_X(CollisionParam aParam)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 Pos = GetPos();//���W���
	D3DXVECTOR3 Size = GetSize();//�T�C�Y���

	//X�����d�Ȃ��Ă���ꍇ
	if (aParam.pos.x + aParam.size.x * 0.5f >= Pos.x - Size.x * 0.5f
		&& aParam.pos.x - aParam.size.x * 0.5f <= Pos.x + Size.x * 0.5f)
	{
		return true;
	}
	//X�����d�Ȃ��Ă��Ȃ��ꍇ
	else
	{
		return false;
	}
}

//===========================================================================================================
// Y���̓����蔻��
//===========================================================================================================
bool CEnemy::Collision_Y(CollisionParam aParam)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 Pos = GetPos();//���W���
	D3DXVECTOR3 Size = GetSize();//�T�C�Y���

	//Y�����d�Ȃ��Ă���ꍇ
	if (aParam.pos.y + aParam.size.y * 0.5f >= Pos.y - Size.y * 0.5f
		&& aParam.pos.y - aParam.size.y * 0.5f <= Pos.y + Size.y * 0.5f)
	{
		return true;
	}
	//Y�����d�Ȃ��Ă��Ȃ��ꍇ
	else
	{
		return false;
	}
}

//===========================================================================================================
// Z���̓����蔻��
//===========================================================================================================
bool CEnemy::Collision_Z(CollisionParam aParam)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 Pos = GetPos();//���W���
	D3DXVECTOR3 Size = GetSize();//�T�C�Y���

	//Z�����d�Ȃ��Ă���ꍇ
	if (aParam.pos.z + aParam.size.z * 0.5f >= Pos.z - Size.z * 0.5f
		&& aParam.pos.z - aParam.size.z * 0.5f <= Pos.z + Size.z * 0.5f)
	{
		return true;
	}
	//Z�����d�Ȃ��Ă��Ȃ��ꍇ
	else
	{
		return false;
	}
}

//===========================================================================================================
// ��������
//===========================================================================================================
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type)
{
	//�^�C�v�ɉ����ă������𓮓I�m��
	CEnemy* pEnemy = nullptr;
	switch (type)
	{
		//�{�[��
	case TYPE::BALL:
		pEnemy = NEW CBall();
		break;

		//�m�[�}��
	case TYPE::NORMAL:
		pEnemy = NEW CNormalEnemy();
		break;

		//���[�u
	case TYPE::MOVE:
		pEnemy = NEW CMoveEnemy();
		break;

		//�o���b�g
	case TYPE::BULLET:
		pEnemy = NEW CBulletEnemy();
		break;

	default:
		break;
	}

	//�|�C���^�ɏ�񂪑��݂���ꍇ
	if (pEnemy != nullptr)
	{
		//�p�����[�^�ݒ�
		pEnemy->SetPos(pos);//���W
		pEnemy->SetRot(rot);//�p�x
		pEnemy->SetScale(scale);//�g�嗦
		pEnemy->SetEnemyType(type);//�u���b�N�^�C�v�ݒ�

		//����������
		pEnemy->Init();

		//�}�l�[�W���[�o�^����
		CManager::GetInstance()->GetScene()->GetEnemyManager()->Regist(pEnemy);
	}

	return pEnemy;
}

