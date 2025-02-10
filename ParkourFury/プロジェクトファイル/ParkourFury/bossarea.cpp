//==============================================================================================================================================
//
// �{�X�G���A����
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "bossarea.h"
#include "game.h"
#include "player.h"
#include "boss.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CBossArea::CBossArea() : CArea()
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CBossArea::~CBossArea()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CBossArea::Init()
{
	//���N���X����������
	if (FAILED(CArea::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CBossArea::Uninit()
{
	//���N���X�I������
	CArea::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CBossArea::Release()
{
	//���N���X�������
	CArea::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CBossArea::Update()
{
	//���N���X�X�V����
	CArea::Update();
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
bool CBossArea::Collision(const D3DXVECTOR3& pos)
{
	//�����蔻��t���O
	bool bCollision = false;

	//�����蔻�肪true�̏ꍇ
	if (CArea::Collision(pos))
	{
		//���[�J���ϐ��錾
		CCamera* pCamera = CManager::GetInstance()->GetCamera();//�J�����̏��
		D3DXVECTOR3 BossAreaPos = GetPos();//���݂̍��W
		CPlayer* pPlayer = nullptr;//�v���C���[�ւ̃|�C���^
		CBoss* pBoss = nullptr;//�{�X�ւ̃|�C���^
		D3DXVECTOR3 PlayerPos = {};//�v���C���[���W
		D3DXVECTOR3 BossPos = {};//�{�X���W

		//�v���C���[�J�e�S���[�̃I�u�W�F�N�g�����擾
		CObject* pFindPlayer = CObject::FindObject(CObject::Category::PLAYER);

		//�I�u�W�F�N�g��񂪑��݂���ꍇ
		if (pFindPlayer != nullptr)
		{
			//�v���C���[�N���X�Ƀ_�E���L���X�g
			pPlayer = CObject::DownCast<CPlayer, CObject>(pFindPlayer);

			//�_�E���L���X�g����
			if (pPlayer != nullptr)
			{
				//�v���C���[���W�擾
				PlayerPos = pPlayer->GetPos();
			}
			//�_�E���L���X�g���s
			else
			{
				//�����𔲂���
				return false;
			}
		}
		//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
		else
		{
			//�����𔲂���
			return false;
		}

		//�{�X�J�e�S���[�̃I�u�W�F�N�g�����擾
		CObject* pFindBoss = CObject::FindObject(CObject::Category::BOSS);

		//�I�u�W�F�N�g��񂪑��݂���ꍇ
		if (pFindBoss != nullptr)
		{
			//�v���C���[�N���X�Ƀ_�E���L���X�g
			pBoss = CObject::DownCast<CBoss, CObject>(pFindBoss);

			//�_�E���L���X�g����
			if (pBoss != nullptr)
			{
				//�v���C���[���W�擾
				BossPos = pBoss->GetPos();
			}
			//�_�E���L���X�g���s
			else
			{
				//�����𔲂���
				return false;
			}
		}
		//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
		else
		{
			//�����𔲂���
			return false;
		}

		//Y���W�𒆐S���ɒ���
		PlayerPos.y += pPlayer->GetSize().y * 0.5f;
		BossPos.y += pBoss->GetSize().y * 0.5f;

		//�J�����̒����_���v���C���[�ƃ{�X�̊Ԃɐݒ�
		D3DXVECTOR3 CameraPosR = (PlayerPos + BossPos) * 0.5f;

		//�v���C���[�ƃ{�X�̋����ɍ��킹�ăJ�����̋������v�Z
		float fDistance = sqrtf(powf((PlayerPos.y - BossPos.y), 2.0f) + powf((PlayerPos.z - BossPos.z), 2.0f)) * 0.5f + pCamera->GetBossMinDistance();

		//�ő�l�𒴂��Ȃ��悤�ɒ���
		if (fDistance > pCamera->GetBossMaxDistance()) fDistance = pCamera->GetBossMaxDistance();

		//�J�����̈ړ��t���[����0
		if (pCamera->GetMoveFrame() <= 0)
		{
			//�����_�ݒ�
			pCamera->SetCameraWork({ CameraPosR.x - fDistance , CameraPosR.y, CameraPosR.z }, CameraPosR, 10);
		}

		//�����蔻���true�ɂ���
		bCollision = true;
	}

	return bCollision;
}
