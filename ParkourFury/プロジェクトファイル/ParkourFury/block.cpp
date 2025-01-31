//==============================================================================================================================================
//
// �u���b�N�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "block.h"
#include "slone.h"
#include "bossblock.h"
#include "blockmanager.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CBlock::CBlock(int nPriority) : 
	CObjectX(nPriority),
	m_pPrev(nullptr),
	m_pNext(nullptr),
	m_Type(CBlock::TYPE::NONE),
	m_Normal( 0.0f, 0.0f, 0.0f )
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CBlock::~CBlock()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CBlock::Init()
{
	//���f������
	CXfile* pModel = CXfile::GetInstance();//�C���X�^���X���擾
	BindModel(pModel->GetAddress(CTag::TAG::BLOCK, static_cast<int>(m_Type)));//�ݒ�
	SetSize();//�T�C�Y�ݒ�

	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^

	//�V�F�[�_�[�Ǎ�
	ID3DXEffect* pEffect = GetEffect();
	if (FAILED(D3DXCreateEffectFromFile(pDevice, "ToonShader.fx", nullptr, nullptr, D3DXSHADER_DEBUG, nullptr, &pEffect, nullptr)))
	{
		MessageBox(0, "�V�F�[�_�[�̓ǂݍ��݂Ɏ��s���܂����I", "�G���[", MB_OK);
	}
	SetEffect(pEffect);

	//���N���X����������
	if (FAILED(CObjectX::Init())) return E_FAIL;

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CBlock::Uninit()
{
	//���N���X�I������
	CObjectX::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CBlock::Release()
{
	//���N���X�������
	CObjectX::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CBlock::Update()
{
	//���N���X�X�V����
	CObjectX::Update();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CBlock::Draw()
{
	//�G�f�B�^���[�h�̏ꍇ�͕`�悵�Ȃ�
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{
		return;
	}

	//���N���X�`�揈��
	CObjectX::Draw();
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
CBlock::CollisionFlag CBlock::Collision(CollisionParam& aParam)
{
	//�����擾
	CollisionFlag aFlag = {};//�����蔻��t���O�\����
	D3DXVECTOR3 Pos = GetPos();//���W
	D3DXVECTOR3 Size = GetSize();//�T�C�Y

	//=======================================================================
	// Y��
	//=======================================================================

	//�㑤�̔���
	if (aParam.pos.y < Pos.y + Size.y * 0.5f && 
		aParam.oldpos.y >= Pos.y + Size.y * 0.5f)
	{
		//XZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//���W���C��
			aParam.pos.y = Pos.y + Size.y * 0.5f;

			//Y���̈ړ��ʂ����Z�b�g
			aParam.move.y = 0.0f;

			//Y���̏㑤�̓����蔻���true�ɂ���
			aFlag.Y_UP = true;

			//�������g�̃^�C�v��ۑ�
			aParam.type = m_Type;
		}
	}

	//�����̔���
	if (aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
		aParam.oldpos.y + aParam.size.y <= Pos.y - Size.y * 0.5f)
	{
		//XZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//���W���C��
			aParam.pos.y = Pos.y - Size.y * 0.5f - aParam.size.y;

			//Y���̈ړ��ʂ����Z�b�g
			aParam.move.y = 0.0f;

			//Y���̉����̓����蔻���true�ɂ���
			aFlag.Y_DOWN = true;

			//�������g�̃^�C�v��ۑ�
			aParam.type = m_Type;
		}
	}

	//=======================================================================
	// X��
	//=======================================================================

	//�����̔���
	if (aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
		aParam.oldpos.x + aParam.size.x * 0.5f <= Pos.x - Size.x * 0.5f)
	{
		//YZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
			aParam.pos.y < Pos.y + Size.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//���W���C��
 			aParam.pos.x = Pos.x - Size.x * 0.5f - aParam.size.x * 0.5f;

			//X���̓����蔻��
			aFlag.X = true;

			//�������g�̃^�C�v��ۑ�
			aParam.type = m_Type;
		}
	}

	//�E���̔���
	if (aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f && 
		aParam.oldpos.x - aParam.size.x * 0.5f >= Pos.x + Size.x * 0.5f)
	{
		//YZ�Ԃ͈͓̔��ɂ���ꍇ
		if ((aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
			aParam.pos.y < Pos.y + Size.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//���W���C��
			aParam.pos.x = Pos.x + Size.x * 0.5f + aParam.size.x * 0.5f;

			//X���̓����蔻���true�ɂ���
			aFlag.X = true;

			//�������g�̃^�C�v��ۑ�
			aParam.type = m_Type;
		}
	}

	//=======================================================================
	// Z��
	//=======================================================================

	//��O���̔���
	if (aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
		aParam.oldpos.z + aParam.size.z * 0.5f <= Pos.z - Size.z * 0.5f)
	{
		//XY�Ԃ͈͓̔��ɂ���ꍇ
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
				aParam.pos.y < Pos.y + Size.y * 0.5f))
		{
			//���W���C��
			aParam.pos.z = Pos.z - Size.z * 0.5f - aParam.size.z * 0.5f;

			//Z���̓����蔻���true�ɂ���
			aFlag.Z = true;

			//�������g�̃^�C�v��ۑ�
			aParam.type = m_Type;
		}
	}

	//�����̔���
	if (aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f && 
		aParam.oldpos.z - aParam.size.z * 0.5f >= Pos.z + Size.z * 0.5f)
	{
		//XY�Ԃ͈͓̔��ɂ���ꍇ
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
				aParam.pos.y < Pos.y + Size.y * 0.5f))
		{
			//���W���C��
			aParam.pos.z = Pos.z + Size.z * 0.5f + aParam.size.z * 0.5f;

			//Z���̓����蔻���true�ɂ���
			aFlag.Z = true;

			//�������g�̃^�C�v��ۑ�
			aParam.type = m_Type;
		}
	}

	return aFlag;
}

//===========================================================================================================
// XY�Ԃ̐����x�N�g���v�Z����
//===========================================================================================================
D3DXVECTOR3 CBlock::SEGMENT_XY(float StartX, float StartY, float EndX, float EndY)
{
	//�����x�N�g�������߂�
	D3DXVECTOR3 Start(StartX, StartY, 0.0f);
	D3DXVECTOR3 End(EndX, EndY, 0.0f);

	D3DXVECTOR3 r = End - Start;

	m_Normal.x = -r.y;
	m_Normal.y = r.x;

	//�x�N�g���𐳋K��
	D3DXVec3Normalize(&m_Normal, &m_Normal);

	return m_Normal;
}

//===========================================================================================================
// XZ�Ԃ̐����x�N�g���v�Z����
//===========================================================================================================
D3DXVECTOR3 CBlock::SEGMENT_XZ(float StartX, float StartZ, float EndX, float EndZ)
{
	//�����x�N�g�������߂�
	D3DXVECTOR3 Start(StartX, 0.0f, StartZ);
	D3DXVECTOR3 End(EndX, 0.0f, EndZ);

	D3DXVECTOR3 r = End - Start;

	m_Normal.x = -r.z;
	m_Normal.z = r.x;

	//�x�N�g���𐳋K��
	D3DXVec3Normalize(&m_Normal, &m_Normal);

	return m_Normal;
}

//===========================================================================================================
// YZ�Ԃ̐����x�N�g���v�Z����
//===========================================================================================================
D3DXVECTOR3 CBlock::SEGMENT_YZ(float StartY, float StartZ, float EndY, float EndZ)
{
	//�����x�N�g�������߂�
	D3DXVECTOR3 Start(0.0f, StartY, StartZ);
	D3DXVECTOR3 End(0.0f, EndY, EndZ);

	D3DXVECTOR3 r = End - Start;

	m_Normal.z = -r.y;
	m_Normal.y = r.z;

	//�x�N�g���𐳋K��
	D3DXVec3Normalize(&m_Normal, &m_Normal);

	return m_Normal;
}

//===========================================================================================================
// ��������
//===========================================================================================================
CBlock* CBlock::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, TYPE type)
{
	//�^�C�v�ɉ����ă������𓮓I�m��
	CBlock* pBlock = nullptr;

	switch (type)
	{
		//�m�[�}��
	case TYPE::NORMAL:
		pBlock = NEW CBlock(static_cast<int>(Category::BLOCK));
		break;

		//�⓹
	case TYPE::SLONE:
		pBlock = NEW CSlone();
		break;

		//�{�X�u���b�N
	case TYPE::BOSS:
		pBlock = NEW CBossBlock();

	default:
		break;
	}

	//�u���b�N��񂪑��݂���ꍇ
	if (pBlock != nullptr)
	{
		//�p�����[�^�ݒ�
		pBlock->SetPos(pos);//���W
		pBlock->SetRot(rot);//�p�x
		pBlock->SetScale(scale);//�g�嗦
		pBlock->SetBlockType(type);//�u���b�N�^�C�v�ݒ�

		//����������
		pBlock->Init();

		//�}�l�[�W���[�o�^����
		CManager::GetInstance()->GetScene()->GetBlockManager()->Regist(pBlock);
	}

	return pBlock;
}
