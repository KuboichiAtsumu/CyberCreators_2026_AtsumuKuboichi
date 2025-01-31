//==============================================================================================================================================
//
// �{�X�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "boss.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CBoss::CBoss(int nPriority) : 
	CMotionCharacter(nPriority),
	m_State{ STATE::NEUTRAL },
	m_nLife{ MAX_LIFE },
	m_nSetCoolTime{ 120 },
	m_nAttackCT{ ATTACK_TIME },
	m_nIdxPosition{ -1 },
	m_nOldPosition { m_nIdxPosition },
	m_nAttackPattern{ -1 },
	m_bAttack{ false }
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CBoss::~CBoss()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
#include "UI.h"
HRESULT CBoss::Init()
{
	//���N���X����������
	if (FAILED(CMotionCharacter::Init()))
	{
		return E_FAIL;
	}

	//�U���p�^�[���ǂݍ��ݏ���
	LoadAttackPattern();

	// �p�����[�^�ݒ�
	SetRot({ 0.0f, D3DX_PI, 0.0f });// �p�x

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CBoss::Uninit()
{
	//���N���X�I������
	CMotionCharacter::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CBoss::Release()
{
	//���N���X�������
	CMotionCharacter::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
#include "game.h"
void CBoss::Update()
{
	//�G�f�B�^���[�h�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//�X�V�����ɏ����𔲂���
		return;
	}

	//���[�V������Ԑݒ菈��
	SetMotionState(static_cast<int>(m_State));

	//�p�[�c���Ƃ̏�Ԑݒ菈��
	SetPartsState();

	//�Q�[���V�[���̃C���X�^���X�擾
	CGame* pGame = CGame::GetInstance();

	//���݂̃G���A���{�X�G���A�ł͂Ȃ��ꍇ
	if (pGame != nullptr && 
		pGame->GetArea() != CGame::GAME_AREA::BOSS)
	{//�X�V�����ɏ����𔲂���
		return;
	}

	//���N���X�X�V����
	CMotionCharacter::Update();

	//�����ʒu�̐ݒ菈��
	SetPosition();

	//�d�͏���
	Gravity();

	//�ړ�����
	Move();

	//�U������
	Attack();

	//�u���b�N�Ƃ̓����蔻��
	CollisionBlock();

	//�p�x�C������
	AngleModifying();

	//�U����ԂŃ��[�V�������I����Ă���ꍇ
	if (m_State == STATE::ATTACK && !GetMotionFlag())
	{
		//�j���[�g������Ԃɂ���
		m_State = STATE::NEUTRAL;
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CBoss::Draw()
{
	//���N���X�`�揈��
	CMotionCharacter::Draw();
}

//===========================================================================================================
// ��������
//===========================================================================================================
#include "enemymanager.h"
CBoss* CBoss::Create(D3DXVECTOR3 pos)
{
	//�^�C�v�ɉ����ă������𓮓I�m��
	CBoss* pBoss = NEW CBoss(static_cast<int>(Category::BOSS));
	
	//�u���b�N��񂪑��݂���ꍇ
	if (pBoss != nullptr)
	{
		//�p�����[�^�ݒ�
		pBoss->SetPos(pos);//���W
		pBoss->SetCharacterType(CMotionCharacter::TYPE::BOSS);//�L�����N�^�[�^�C�v
	
		//����������
		pBoss->Init();
	}

	return pBoss;
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
bool CBoss::Collision(CollisionParam& aParam)
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
		return true;
	}
	//�����ꂩ�̎��̔��肪false�̏ꍇ
	else
	{
		return false;
	}
}

//===========================================================================================================
// ���C�t��������
//===========================================================================================================
void CBoss::DecreaseLife()
{
	//���C�t����
	m_nLife--;

	//���C�t��0�ɂȂ����ꍇ
	if (m_nLife == 0)
	{
		//�I������
		Uninit();

		//���U���g��ʂֈڍs
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE::MODE_RESULT);
	}
}

//===========================================================================================================
// �U���p�^�[���Ǎ�����
//===========================================================================================================
void CBoss::LoadAttackPattern()
{
	//���[�J���ϐ��錾
	std::string str;//�t�@�C�������p
	std::stringstream ss;//������ۑ��p
	AttackInfo aAttackInfo = {};//���[�V�������
	
	//�t�@�C�����J��
	std::ifstream file("data/ATTACK/BossAttack.txt");

	//�t�@�C�����J���Ȃ��ꍇ
	if (!file)
	{
		assert(false);
		return;
	}

	//EOF��������܂Ō������J��Ԃ�
	while (1)
	{
		//����
		std::getline(file, str);

		//EOF�����������ꍇ
		if (str.find("END") != std::string::npos)
		{
			//�t�@�C�������
			file.close();
			break;
		}
		//AttackSet�����������ꍇ
		else if (str.find("AttackSet") != std::string::npos)
		{
			//�\���̏�񏉊���
			aAttackInfo.nCreate = 0;

			//�e���[�V�����̃f�[�^������
			while (1)
			{
				//����
				std::getline(file, str);

				//EndSet�����������ꍇ
				if (str.find("EndSet") != std::string::npos)
				{
					//�\���̏����i�[
					m_AttackInfo.push_back(aAttackInfo);

					//�ǂݍ��ݏI��
					break;
				}

				//Type�����������ꍇ
				if (str.find("Type") != std::string::npos)
				{
					//�^�C�v�ۑ��p
					int nType = 0;

					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//�U���̎�ނ�ǂݍ���
					ss >> str >> nType;
					aAttackInfo.Type = static_cast<ATTACK_TYPE>(nType);
				}

				//AttackInfo�����������ꍇ
				if (str.find("AttackInfo") != std::string::npos)
				{
					//�C���f�b�N�X��ۑ�
					int nIdx = aAttackInfo.nCreate;

					//�e�L�[�Z�b�g�̃f�[�^������
					while (1)
					{
						//����
						std::getline(file, str);

						//EndInfo�����������ꍇ
						if (str.find("EndInfo") != std::string::npos)
						{
							//�����������Z
							aAttackInfo.nCreate++;

							//�e�L�[�Z�b�g�̃f�[�^�̌������I������
							break;
						}
						//POS�����������ꍇ
						else if (str.find("POS") != std::string::npos)
						{
							//�ǂݍ��񂾕������ۑ�
							ss = static_cast<std::stringstream>(str);

							//���W��ǂݍ���
							aAttackInfo.pos.push_back({ 0.0f, 0.0f, 0.0f });
							ss >> str >> aAttackInfo.pos.at(nIdx).x >> aAttackInfo.pos.at(nIdx).y >> aAttackInfo.pos.at(nIdx).z;
						}
						//ROT�����������ꍇ
						else if (str.find("ROT") != std::string::npos)
						{
							//�ǂݍ��񂾕������ۑ�
							ss = static_cast<std::stringstream>(str);

							//�p�x��ǂݍ���
							aAttackInfo.rot.push_back({ 0.0f, 0.0f, 0.0f });
							ss >> str >> aAttackInfo.rot.at(nIdx).x >> aAttackInfo.rot.at(nIdx).y >> aAttackInfo.rot.at(nIdx).z;
						}
						//SCALE�����������ꍇ
						else if (str.find("SCALE") != std::string::npos)
						{
							//�ǂݍ��񂾕������ۑ�
							ss = static_cast<std::stringstream>(str);

							//�g�嗦��ǂݍ���
							aAttackInfo.scale.push_back({ 0.0f, 0.0f, 0.0f });
							ss >> str >> aAttackInfo.scale.at(nIdx).x >> aAttackInfo.scale.at(nIdx).y >> aAttackInfo.scale.at(nIdx).z;
						}
						//��L�ȊO�����������ꍇ
						else
						{
							//�X�L�b�v���ēǂݍ��݂𑱂���
							continue;
						}
					}
				}
				//��L�ȊO�����������ꍇ
				else
				{
					//�X�L�b�v���ēǂݍ��݂𑱂���
					continue;
				}
			}
		}
		//��L�ȊO�����������ꍇ
		else
		{
			//�����𑱂���
			continue;
		}
	}
}

//===========================================================================================================
// X���̓����蔻��
//===========================================================================================================
bool CBoss::Collision_X(CollisionParam aParam)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 Pos = GetPos();//���W���
	D3DXVECTOR3 Size = GetSize();//�T�C�Y���

	//X�����d�Ȃ��Ă���ꍇ
	if (aParam.pos.x + aParam.size.x * 0.5f >= Pos.x - Size.x * 0.5f && 
		aParam.pos.x - aParam.size.x * 0.5f <= Pos.x + Size.x * 0.5f)
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
bool CBoss::Collision_Y(CollisionParam aParam)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 Pos = GetPos();//���W���
	D3DXVECTOR3 Size = GetSize();//�T�C�Y���

	//Y�����d�Ȃ��Ă���ꍇ
	if (aParam.pos.y + aParam.size.y * 0.5f >= Pos.y &&
		aParam.pos.y - aParam.size.y * 0.5f <= Pos.y + Size.y)
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
bool CBoss::Collision_Z(CollisionParam aParam)
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 Pos = GetPos();//���W���
	D3DXVECTOR3 Size = GetSize();//�T�C�Y���

	//Z�����d�Ȃ��Ă���ꍇ
	if (aParam.pos.z + aParam.size.z * 0.5f >= Pos.z - Size.z * 0.5f && 
		aParam.pos.z - aParam.size.z * 0.5f <= Pos.z + Size.z * 0.5f)
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
// �����ʒu�̐ݒ菈��
//===========================================================================================================
#include "bossmovepoint.h"
#include "blockmanager.h"
void CBoss::SetPosition()
{
	//�U�����̏ꍇ
	if (m_State == STATE::ATTACK)
	{
		//�����𔲂���
		return;
	}

	//�N�[���^�C�������炷
	if (m_nSetCoolTime > 0)
	{
		m_nSetCoolTime--;
	}

	//�N�[���^�C����0�ł͂Ȃ��ꍇ
	if (m_nSetCoolTime != 0)
	{
		//�����𔲂���
		return;
	}

	//�ߋ��̈ʒu��ۑ�
	m_nOldPosition = m_nIdxPosition;

	//���̃|�C���g�Ɉړ�
	m_nIdxPosition++;

	//���������ŏ��̃|�C���g�Ƀ��[�v
	if (m_nIdxPosition == static_cast<int>(CBossMovePoint::Point::MAX))
	{
		m_nIdxPosition = 0;
	}

	//���̗����ʒu���擾
	CBossMovePoint* pPoint = CBossMovePoint::Find(m_nIdxPosition);

	//���݂���|�C���g�̃C���f�b�N�X��ύX
	m_nIdxPosition = pPoint->GetIdx();

	//���W����ݒ�
	SetPos(pPoint->GetPos());

	//�N�[���^�C�������Z�b�g
	m_nSetCoolTime = SETPOS_TIME;

	//�U���t���O��true�ɂ���
	m_bAttack = true;

	//�J�������[�N���ݒ菈��
	SetCameraWorkInfo();
}

//===========================================================================================================
// �J�������[�N���ݒ菈��
//===========================================================================================================
#include "player.h"
void CBoss::SetCameraWorkInfo()
{
	//���[�J���ϐ��錾
	CCamera* pCamera = CManager::GetInstance()->GetCamera();//�J�����̏��
	CPlayer* pPlayer = nullptr;//�v���C���[�ւ̃|�C���^
	D3DXVECTOR3 PlayerPos = {};//�v���C���[���W
	D3DXVECTOR3 BossPos = GetPos();//�{�X���W

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
			return;
		}
	}
	//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
	else
	{
		//�����𔲂���
		return;
	}

	//Y���W�𒆐S���ɒ���
	PlayerPos.y += pPlayer->GetSize().y * 0.5f;
	BossPos.y += GetSize().y * 0.5f;

	//�J�����̒����_���v���C���[�ƃ{�X�̊Ԃɐݒ�
	D3DXVECTOR3 CameraPosR = (PlayerPos + BossPos) * 0.5f;

	//�v���C���[�ƃ{�X�̋����ɍ��킹�ăJ�����̋������v�Z
	float fDistance = sqrtf(powf((PlayerPos.y - BossPos.y), 2.0f) + powf((PlayerPos.z - BossPos.z), 2.0f)) * 0.5f + pCamera->GetBossMinDistance();

	//�ő�l�𒴂��Ȃ��悤�ɒ���
	if (fDistance > pCamera->GetBossMaxDistance()) fDistance = pCamera->GetBossMaxDistance();

	//�J�������[�N���ݒ�
	pCamera->SetCameraWork({ CameraPosR.x - fDistance , CameraPosR.y, CameraPosR.z }, CameraPosR, 30);
}

//===========================================================================================================
// �d�͏���
//===========================================================================================================
void CBoss::Gravity()
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
void CBoss::Move()
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 pos = GetPos();//���W���擾
	D3DXVECTOR3 move = GetMove();//�ړ��ʏ��擾

	//�ړ��ʂ�����
	move.x += -move.x * m_fInertia;
	move.z += -move.z * m_fInertia;

	//�ړ��ʂ�ݒ�
	SetMove(move);

	//�ߋ��̍��W��ݒ�
	SetOldPos(pos);

	//�ړ��ʂ����Z���č��W��ݒ�
	SetPos(pos + move);
}

//===========================================================================================================
// �U������
//===========================================================================================================
#include "bossarea.h"
#include "player.h"
#include "bossattack.h"
#include "target.h"
void CBoss::Attack()
{
	//�U���t���O��false�̏ꍇ
	if (!m_bAttack)
	{
		//�����𔲂���
		return;
	}

	//�U���܂ł̃N�[���^�C����1�ȏ�̏ꍇ
	if (m_nAttackCT > 0)
	{
		//�U���܂ł̃N�[���^�C�������炷
		m_nAttackCT--;
	}

	//���Z��A�N�[���^�C����0�ł͂Ȃ��ꍇ
	if (m_nAttackCT != 0)
	{
		//�����𔲂���
		return;
	}

	//�U���p�^�[���ݒ�
	SetAttackPattern();

	//�U���^�C�v�ɉ����čU��
	switch (m_AttackInfo.at(m_nAttackPattern).Type)
	{
		//�e(��)
	case ATTACK_TYPE::BULLET_DEPTH:
		BulletDepth();
		break;

		//�e(��)
	case ATTACK_TYPE::BULLET_WIDTH:
		BulletWidth();
		break;

		//�u���b�N����
	case ATTACK_TYPE::BLOCK:
		BlockCreate();
		break;
	}

	//�U����Ԃɂ���
	m_State = STATE::ATTACK;

	//�U���t���O��false�ɂ���
	m_bAttack = false;

	//�U���܂ł̃N�[���^�C����ݒ�
	m_nAttackCT = ATTACK_TIME;
}

//===========================================================================================================
// �U���p�^�[���ݒ菈��
//===========================================================================================================
#include "areamanager.h"
void CBoss::SetAttackPattern()
{
	//�����ʒu�ɉ����Ď��̍s����ω�������
	switch (m_nIdxPosition)
	{
		//����
	case static_cast<int>(CBossMovePoint::Point::LEFT_DOWN):
		//�ŏ��̍U���̏ꍇ
		if (m_nOldPosition == -1)
		{
			//�u���b�N����
			m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::FORTH_BLOCK);
		}
		//��T�ڈȍ~�̏ꍇ
		else
		{
			//�e(��)
			m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::BULLET_DEPTH);
		}
		break;

		//�E��
	case static_cast<int>(CBossMovePoint::Point::RIGHT_DOWN):
		//�u���b�N����
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::FORTH_BLOCK);
		break;

		//����
	case static_cast<int>(CBossMovePoint::Point::LEFT_UP):
		//�ǐ���
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::WALL_BLOCK);
		break;

		//�E��
	case static_cast<int>(CBossMovePoint::Point::RIGHT_UP):
		//�e(��)
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::BULLET_DEPTH);
		break;

	default:
		break;
	}
}

//===========================================================================================================
// �e(��)�̍U������
//===========================================================================================================
void CBoss::BulletDepth()
{
	//�v���C���[�J�e�S���[�̃I�u�W�F�N�g�����擾
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
	if (pFindObj == nullptr)
	{
		//�����𔲂���
		return;
	}

	//�v���C���[�N���X�Ƀ_�E���L���X�g
	pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//�v���C���[��񂪑��݂��Ȃ��ꍇ
	if (pPlayer == nullptr)
	{
		//�����𔲂���
		return;
	}

	//�v���C���[�̈ʒu���擾
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	//�����_���Ȉʒu�ɍU��
	for (int nCnt = 0; nCnt < m_AttackInfo.at(m_nAttackPattern).nCreate; nCnt++)
	{
		//�����ʒu��ۑ�
		D3DXVECTOR3 AttackPos = m_AttackInfo.at(m_nAttackPattern).pos.at(nCnt);
		D3DXVECTOR3 AttackRot = m_AttackInfo.at(m_nAttackPattern).rot.at(nCnt);

		//�����p�p�����[�^�ݒ�
		D3DXVECTOR3 CreatePos(PlayerPos.x + 400.0f, PlayerPos.y + AttackPos.y, PlayerPos.z + AttackPos.z);//�������W
		D3DXVECTOR3 TargetPos(PlayerPos.x, PlayerPos.y + AttackPos.y, PlayerPos.z + AttackPos.z);//�ڕW�̍��W
		D3DXVECTOR3 rot(0.0f, CManager::GetInstance()->GetCamera()->GetRot().y + AttackRot.y, 0.0f);//�p�x
		CorrectionRot(rot.y);//�p�x�␳
		D3DXVECTOR3 move(sinf(rot.y) * 4.0f, 0.0f, cosf(rot.y) * 4.0f);//�ړ���

		//�U���𐶐�
		CObject3D* pObj = CBossAttack::Create(CreatePos, TargetPos, rot, move);

		//�^�[�Q�b�g�𐶐�
		CTarget::Create(TargetPos, pObj);
	}
}

//===========================================================================================================
// �e(��)�̍U������
//===========================================================================================================
void CBoss::BulletWidth()
{
	//�{�X�G���A�̏����擾
	CArea* pArea = CManager::GetInstance()->GetScene()->GetAreaManager()->FindArea(CArea::TYPE::BOSS);

	//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
	if (pArea == nullptr)
	{
		//�����𔲂���
		return;
	}

	//�G���A�̏����擾
	D3DXVECTOR3 AreaPos = pArea->GetPos();//���W
	
	//�����_���Ȉʒu�ɍU��
	for (int nCnt = 0; nCnt < m_AttackInfo.at(m_nAttackPattern).nCreate; nCnt++)
	{
		//�����ʒu��ۑ�
		D3DXVECTOR3 AttackPos = m_AttackInfo.at(m_nAttackPattern).pos.at(nCnt);
		D3DXVECTOR3 AttackRot = m_AttackInfo.at(m_nAttackPattern).rot.at(nCnt);

		//�����p�p�����[�^�ݒ�
		D3DXVECTOR3 CreatePos(AreaPos.x, AreaPos.y + AttackPos.y, AreaPos.z + AttackPos.z);//�������W
		D3DXVECTOR3 TargetPos(AreaPos.x, AreaPos.y + AttackPos.y, AreaPos.z);//�ڕW�̍��W
		D3DXVECTOR3 rot(0.0f, CManager::GetInstance()->GetCamera()->GetRot().y + AttackRot.y, 0.0f);//�p�x
		CorrectionRot(rot.y);//�p�x�␳
		D3DXVECTOR3 move(-sinf(rot.y) * 4.0f, 0.0f, -cosf(rot.y) * 4.0f);//�ړ���

		//�U���𐶐�
		CObject3D* pObj = CBossAttack::Create(CreatePos, TargetPos, rot, move);

		//�^�[�Q�b�g�𐶐�
		CTarget::Create(TargetPos, pObj);
	}
}

//===========================================================================================================
// �u���b�N�����U������
//===========================================================================================================
void CBoss::BlockCreate()
{
	//�V�[�����̃C���X�^���X���擾
	CScene* pScene = CManager::GetInstance()->GetScene();

	//�X�e�[�W���ɂ���{�X�u���b�N��S�č폜
	pScene->GetBlockManager()->UninitTypeAll(CBlock::TYPE::BOSS);

	//�{�X�G���A�̏����擾
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::BOSS);

	//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
	if (pArea == nullptr)
	{
		//�����𔲂���
		return;
	}

	//�G���A�̏����擾
	D3DXVECTOR3 AreaPos = pArea->GetPos();//���W

	//�����_���Ȉʒu�ɍU��
	for (int nCnt = 0; nCnt < m_AttackInfo.at(m_nAttackPattern).nCreate; nCnt++)
	{
		//�����ʒu��ۑ�
		D3DXVECTOR3 AttackPos = m_AttackInfo.at(m_nAttackPattern).pos.at(nCnt);

		//�����p�p�����[�^�ݒ�
		D3DXVECTOR3 CreatePos(AreaPos.x, AreaPos.y + AttackPos.y, AreaPos.z + AttackPos.z);

		//�����p�p�����[�^�ݒ�
		D3DXVECTOR3 TargetScale = m_AttackInfo.at(m_nAttackPattern).scale.at(nCnt);

		//�����p�x
		D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);

		//�u���b�N��������
		CBlock::Create(CreatePos, rot, TargetScale, CBlock::TYPE::BOSS);
	}

	//�����p�^�[���ɉ����Ēe�̍U�����o��
	switch (m_nAttackPattern)
	{
		//�u���b�N����
	case static_cast<int>(ATTACK_PATTERN::FORTH_BLOCK):
		//�e(��)
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::BULLET_WIDTH);
		BulletWidth();
		break;

	default:
		break;
	}
}

//===========================================================================================================
// �u���b�N�Ƃ̓����蔻��
//===========================================================================================================
#include "blockmanager.h"
void CBoss::CollisionBlock()
{
	//�����蔻��p�p�����[�^�ݒ�
	CBlock::CollisionParam aParam;
	aParam.pos = GetPos();//���W
	aParam.oldpos = GetOldPos();//�ߋ��̍��W
	aParam.size = GetSize();//�T�C�Y
	aParam.move = GetMove();//�ړ���
	aParam.rot = GetRot();//�p�x
	
	//�u���b�N�̓����蔻��
	CManager::GetInstance()->GetScene()->GetBlockManager()->Collision(aParam);
	
	//�X�V���ꂽ�p�����[�^��ݒ�
	SetPos(aParam.pos);//���W
	SetMove(aParam.move);//�ړ���
	SetRot(aParam.rot);//�p�x
}

//===========================================================================================================
// �p�x�C������
//===========================================================================================================
#include "player.h"
void CBoss::AngleModifying()
{
	//�v���C���[�J�e�S���[�̃I�u�W�F�N�g�����擾
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//�I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
	if (pFindObj == nullptr)
	{
		//�����𔲂���
		return;
	}

	//�v���C���[�N���X�Ƀ_�E���L���X�g
	pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//�v���C���[��񂪑��݂��Ȃ��ꍇ
	if (pPlayer == nullptr)
	{
		//�����������ɔ�����
		return;
	}

	//�v���C���[�܂ł̕����x�N�g�������߂�
	D3DXVECTOR3 Direction = pPlayer->GetPos() - GetPos();
	D3DXVec3Normalize(&Direction, &Direction);//���K��

	//�p�x�����擾
	D3DXVECTOR3 rot = GetRot();

	//�����x�N�g����Z���W�����̏ꍇ
	if (Direction.z < 0.0f)
	{
		//�E�Ɍ�����
		rot.y = 0.0f;
	}
	//�����x�N�g����Z���W�����̏ꍇ
	else
	{
		//���Ɍ�����
		rot.y = D3DX_PI;
	}

	//�p�x����ݒ�
	SetRot(rot);
}
