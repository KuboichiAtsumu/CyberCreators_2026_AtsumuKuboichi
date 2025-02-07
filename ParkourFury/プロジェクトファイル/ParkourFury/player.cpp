//==============================================================================================================================================
//
// �v���C���[�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "player.h"
#include "bullet.h"
#include "particle.h"
#include "boss.h"
#include "bossattack.h"
#include "game.h"
#include "minigame.h"
#include "ball.h"
#include "ballspawner.h"
#include "smoke.h"
#include "UI.h"
#include "effect2D.h"
#include "gimmickmanager.h"
#include "areamanager.h"
#include "blockmanager.h"
#include "enemymanager.h"
#include "signboardmanager.h"
#include "UImanager.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CPlayer::CPlayer(int nPriority) : CMotionCharacter(nPriority),
	m_State(STATE::NEUTRAL),
	m_nCntState(0),
	m_nCntDash(0),
	m_nCntDoublePush(0),
	m_nCntJump(JUMP_TIMES),
	m_nDecTenCT(DEC_HEAT_CT),
	m_nKill(0),
	m_nComboTime(0),
	m_nCombo(0),
	m_nDecComboFrame(0),
	m_fInertia(BASE_INERTIA),
	m_fHeat(0.0f),
	m_fWallKickTurn(0.0f),
	m_fTargetRot_Y(0.0f),
	m_bShooting(false),
	m_bBossCreate(false),
	m_bCollisionBlock(false)
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CPlayer::~CPlayer()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CPlayer::Init()
{
	//���N���X����������
	if (FAILED(CMotionCharacter::Init()))
	{
		return E_FAIL;
	}

	//�p�����[�^�ݒ�
	SetRot({ 0.0f, -D3DX_PI * 0.5f, 0.0f });//�p�x

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CPlayer::Uninit()
{
	//���N���X�I������
	CMotionCharacter::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CPlayer::Release()
{
	//���N���X�������
	CMotionCharacter::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CPlayer::Update()
{
	//�G�f�B�^���[�h�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::GAME)
	{//�X�V�����ɏ����𔲂���
		return;
	}

	//���N���X�X�V����
	CMotionCharacter::Update();

	//��ԃJ�E���^�����炷
	if (m_nCntState > 0)
	{
		m_nCntState--;
	}

	//�e���V�����ʌ�������
	DecreaseHeat();

	//�R���{�����Z�b�g����
	ResetCombo();

	//���[�V��������
	SetMotionState(static_cast<int>(m_State));

	//���쏈��
	Controll();

	//�����蔻�菈��
	Collision();

	//�q�[�g�G�t�F�N�g��������
	OccurHeatEffect();

	//�f�o�b�O�p����
#ifdef _DEBUG
	//���[�v����
	Warp();
#endif
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CPlayer::Draw()
{
	//���[�V�����L�����N�^�[�`�揈��
	CMotionCharacter::Draw();
}

//===========================================================================================================
// ��������
//===========================================================================================================
CPlayer* CPlayer::Create(const D3DXVECTOR3& pos)
{
	//�������𓮓I�m��
	CPlayer* pPlayer = NEW CPlayer(static_cast<int>(Category::PLAYER));

	//�v���C���[�̏�񂪑��݂���ꍇ
	if (pPlayer != nullptr)
	{
		//�p�����[�^�ݒ�
		pPlayer->SetPos(pos);//�������W�ݒ�
		pPlayer->SetOldPos(pos);//�ߋ��̍��W
		pPlayer->SetCharacterType(TYPE::PLAYER);//�L�����N�^�[�^�C�v

		//����������
		pPlayer->Init();
	}

	return pPlayer;
}

//===========================================================================================================
// ���[�V������Ԑݒ菈��
//===========================================================================================================
void CPlayer::SetMotionState(int nState)
{
	//�ǃL�b�N���[�V�������̏ꍇ
	if (nState == static_cast<int>(STATE::WALL_KICK) &&
		GetMotionFlag())
	{
		//�p�x�𔽓]������
		D3DXVECTOR3 rot = GetRot();
		rot.y += m_fWallKickTurn;

		//�p�x�␳
		CorrectionRot(rot.y);

		//�p�x�ݒ�
		SetRot(rot);
	}

	//���N���X����
	CMotionCharacter::SetMotionState(nState);

	//�ړ���Ԃł͂Ȃ��ꍇ
	if (nState != static_cast<int>(STATE::LANDING_MOVE))
	{
		//��Ԃ̃C���f�b�N�X��ۑ�
		int nIdxState = static_cast<int>(STATE::LANDING_MOVE);

		//�ړ����[�V�����̏����擾
		MotionSet aMotion = GetMotionSet(nIdxState);

		//�ړ���Ԃ̃t���[�������J�E���g
		aMotion.nCntFrame++;

		//�J�E���g���w��̃t���[�����ɂȂ����ꍇ
		if (aMotion.nCntFrame > aMotion.aKeyset.at(aMotion.nCntKeySet).nMaxFrame)
		{
			//�t���[�����J�E���g��������
			aMotion.nCntFrame = 0;

			//�L�[�Z�b�g�������Z
			aMotion.nCntKeySet++;

			//�L�[�����ő吔�ɂȂ����ꍇ
			if (aMotion.nCntKeySet == aMotion.nMaxKeySet + 1 &&
				static_cast<bool>(aMotion.nLoop))
			{//���[�v����ꍇ

				//�L�[�Z�b�g����������
				aMotion.nCntKeySet = 0;
			}
		}

		//���[�V��������ݒ�
		SetMotionSet(nIdxState, aMotion);
	}

	//�p�[�c���Ƃ̏�Ԑݒ菈��
	SetPartsState();
}

//===========================================================================================================
// �p�[�c���Ƃ̏�Ԑݒ菈��
//===========================================================================================================
void CPlayer::SetPartsState()
{
	//���[�J���ϐ��錾
	int nState = 0;//��ԕۑ��p�ϐ�
	int nNumParts = GetPartsSize();//�p�[�c���

	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		//��Ԃ�int�^�ɕۑ�
		nState = static_cast<int>(m_State);

		//�p�[�c�����擾
		ModelParts aParts = GetPartsInfo(nCntParts);

		//�����g�p�[�c�̏ꍇ
		if (aParts.Half == HALF_BODY::DOWN_BODY &&
			CObject::MovingJudgeXZ(GetMove()) &&
			GetLandingFlag() &&
			m_State != STATE::DASH &&
			m_State != STATE::DAMAGE &&
			m_State != STATE::SLIDING)
		{
			//�ړ���Ԃɐݒ�
			nState = static_cast<int>(STATE::LANDING_MOVE);
		}
		//�㔼�g�p�[�c�Ŏˌ���
		else if (aParts.Half == HALF_BODY::UP_BODY && 
			m_State != STATE::WALL_SLIDE &&
			m_bShooting)
		{
			//�ˌ���Ԃɐݒ�
			nState = static_cast<int>(STATE::SHOT);
		}

		//���[�V��������
		Motion(nState, nCntParts);
	}
}

//===========================================================================================================
// ���쏈��
//===========================================================================================================
void CPlayer::Controll()
{
	//���[�J���ϐ��錾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[�̃C���X�^���X
	CInputKeyboard* pKeyBoard = pManager->GetKeyboard();// �L�[�{�[�h���̃C���X�^���X
	CInputJoypad* pJoyPad = pManager->GetJoypad();// �W���C�p�b�h���̃C���X�^���X
	D3DXVECTOR3 CameraRot = pManager->GetCamera()->GetRot();//�J�����̊p�x���
	D3DXVECTOR3 pos = GetPos();//���W
	D3DXVECTOR3 move = GetMove();//�ړ���
	float fDiaSpeed = GetSpeed();//���x�{��
	float fFirstDiaSpeed = fDiaSpeed;//���x�{�������l��ۑ�
	float fDiaJump = GetJump();//�W�����v��
	float fGravity = fDiaJump * 0.04f;//�d��

	//�_���[�W��Ԃł͂Ȃ��ꍇ
	if (m_State != STATE::DAMAGE)
	{
		//�_�b�V������
		Dash(fDiaSpeed, fFirstDiaSpeed);

		//�X���C�f�B���O����
		Sliding(move);

		//�W�����v����
		Jump(move, fDiaJump);

		//���ˏ���
		Shot();

		//�ǃL�b�N��Ԃ̏ꍇ
		if (m_State == STATE::WALL_KICK)
		{
			//���x�{����ቺ������
			fDiaSpeed *= 0.001f;
		}

		//�R�͂ݍd�����ԊO�̏ꍇ
		if (m_nCntState <= RIGOR_CLIFF - POSSIBLE_FALL)
		{
			//���ړ�
			if (pKeyBoard->GetPress(DIK_A) || 
				pJoyPad->GetPress(CInputJoypad::JOYKEY::JOYKEY_LEFT))
			{
				//�ړ�����
				Move(move, fDiaSpeed, CameraRot.y + D3DX_PI * 0.5f);
			}
			//�E�ړ�
			else if (pKeyBoard->GetPress(DIK_D) || 
				pJoyPad->GetPress(CInputJoypad::JOYKEY::JOYKEY_RIGHT))
			{
				//�ړ�����
				Move(move, fDiaSpeed, CameraRot.y - D3DX_PI * 0.5f);
			}
		}
	}

	//�R�͂܂��Ԃł͂Ȃ��ꍇ
	if (m_State != STATE::CLIFF_GRAB &&
		m_State != STATE::WALL_SLIDE)
	{
		//�d�͂����Z
		move.y -= fGravity;

		//�d�͂����������Ȃ��悤�ɂ���
		if (move.y < MAX_GRAVITY)
		{
			move.y = MAX_GRAVITY;
		}
	}
	//�R�͂܂��Ԃ̏ꍇ
	else
	{
		//�J�E���g��0�ɂȂ����ꍇ
		if (m_nCntState == 0)
		{
			//�����I�ɗ�����Ԃɂ���
			m_State = STATE::FALL;
		}
	}

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
// �W�����v����
//===========================================================================================================
void CPlayer::Jump(D3DXVECTOR3& move, float fDiaJump)
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CSound* pSound = pManager->GetSound();//�T�E���h
	CCalculate* pCalculate = pManager->GetCalculate();//�v�Z����

	//�W�����v
	if ((CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_SPACE) || 
		CManager::GetInstance()->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_A)) && 
		m_State != STATE::WALL_SLIDE && 
		m_nCntJump > 0)
	{
		//�W�����v�͂����Z
		move.y = fDiaJump - (JUMP_INERTIA * (JUMP_TIMES - m_nCntJump));

		//�W�����v�J�E���^�����炷
		m_nCntJump--;

		//�c��̃W�����v�񐔂�1�ȏ�
		if (m_nCntJump == JUMP_TIMES - 1)
		{
			//�W�����v��Ԃɂ���
			m_State = STATE::JUMP;

			//SE�Đ�
			pSound->Play(CSound::SOUND::JUMP);
		}
		//�c��̃W�����v�񐔂�0
		else if (m_nCntJump == 0)
		{
			//��i�W�����v��Ԃɂ���
			m_State = STATE::DOUBLE_JUMP;

			//�e���V�����ʑ���
			IncreaseHeat(10.0f);

			//�R���{�����Z����
			AddCombo();

			//2D�G�t�F�N�g�\��
			DisplayEffect2D();

			//�w�肵�������J��Ԃ�
			for (int nCnt = 0; nCnt < NUM_CREATE_PARTICLE; nCnt++)
			{
				//�����_���ɃJ���[��ݒ�
				float fR = pCalculate->RandFloat(0.1f, 1.0f);
				float fG = pCalculate->RandFloat(0.1f, 1.0f);
				float fB = pCalculate->RandFloat(0.1f, 1.0f);

				//�p�[�e�B�N������
				CParticle::Create(CParticle::TYPE::NORMAL, GetCenterPos(), { 25.0f, 25.0f, 0.0f }, 40, { fR, fG, fB, 1.0f });
			}

			//SE�Đ�
			pSound->Play(CSound::SOUND::DOUBLE_JUMP);
		}

		//�������x��߂�
		m_fInertia = BASE_INERTIA;
	}
}

//===========================================================================================================
// �ړ�����
//===========================================================================================================
void CPlayer::Move(D3DXVECTOR3& move, float fDiaSpeed, float fTargetRotY)
{
	//�p�x�����擾
	D3DXVECTOR3 rot = GetRot();

	//�X���C�f�B���O��Ԃ̏ꍇ
	//�ǃL�b�N��Ԃ̏ꍇ
	if (m_State == STATE::SLIDING || 
		m_State == STATE::WALL_KICK)
	{
		//�����𔲂���
		return;
	}

	//�R�͂܂��Ԃ̏ꍇ
	else if (m_State == STATE::CLIFF_GRAB)
	{
		//�R���������Ă��Ȃ��ꍇ
		if (rot.y != fTargetRotY)
		{
			//������Ԃɂ���
			m_State = STATE::FALL;
		}
		//�R���������Ă���ꍇ
		else
		{
			//�����𔲂���
			return;
		}
	}

	//�q�[�g�ʂɍ��킹�Ĕ{�����グ��
	float fHeat = 1.0f + (m_fHeat / MAX_HEAT) * 0.5f;

	//�ړ��ʂ����Z
	move.x += -sinf(fTargetRotY) * fDiaSpeed * fHeat;//X��
	move.z += -cosf(fTargetRotY) * fDiaSpeed * fHeat;//Z��

	//�ڕW�̊p�x��␳
	CorrectionRot(fTargetRotY);
	rot.y = fTargetRotY;

	//�p�x��ݒ�
	SetRot(rot);
}

//===========================================================================================================
// �_�b�V������
//===========================================================================================================
void CPlayer::Dash(float& fDiaSpeed, float& fFirstDiaSpeed)
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CInputKeyboard* pKeyBoard = pManager->GetKeyboard();//�L�[�{�[�h
	CInputJoypad* pJoyPad = pManager->GetJoypad();//�W���C�p�b�h
	CCalculate* pCalculate = pManager->GetCalculate();//�v�Z����

	//2����͎�t���e�t���[���J�E���^��1�ȏ�̏ꍇ
	if (m_nCntDoublePush > 0)
	{
		//�J�E���^�����炷
		m_nCntDoublePush--;
	}

	//�_�b�V���̃t���[���J�E���^��1�ȏ�̏ꍇ
	if (m_nCntDash > 0)
	{
		//�_�b�V����Ԃɂ���
		m_State = STATE::DASH;

		//�J�E���^�����炷
		m_nCntDash--;

		//���x�{�����グ��
		fDiaSpeed = fFirstDiaSpeed * DASH_SPEED;

		//�������x���Z�b�g
		m_fInertia = BASE_INERTIA;

		//�w�肵�������J��Ԃ�
		for (int nCnt = 0; nCnt < 10; nCnt++)
		{
			//�����_���ɃJ���[��ݒ�
			float fR = pCalculate->RandFloat(0.1f, 1.0f);
			float fG = pCalculate->RandFloat(0.1f, 1.0f);
			float fB = pCalculate->RandFloat(0.1f, 1.0f);

			//�p�[�e�B�N������
			CParticle::Create(CParticle::TYPE::NORMAL, GetCenterPos(), { 40.0f, 40.0f, 0.0f }, 30, { fR, fG, fB, 1.0f });
		}
	}

	//�R�͂܂��Ԃ��ǃL�b�N��ԂȂ珈���𔲂���
	if (m_State == STATE::CLIFF_GRAB || m_State == STATE::WALL_KICK) return;

	//�ړ��L�[�����͂��ꂽ�ꍇ
	if (pKeyBoard->GetTrigger(DIK_A) || 
		pKeyBoard->GetTrigger(DIK_D) ||
		pJoyPad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_LEFT) || 
		pJoyPad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_RIGHT))
	{
		//2����͎�t�t���[���J�E���^��0�̏ꍇ
		if (m_nCntDoublePush <= 0)
		{
			//2����͎�t�t���[���J�E���^��ݒ�
			m_nCntDoublePush = DOUBLE_PUSH;
		}
		//2����͎�t�t���[���J�E���^��1�ȏ�̏ꍇ
		else
		{
			//�_�b�V���̃t���[���J�E���^��ݒ�
			m_nCntDash = DASH_FRAME;



			//�e���V�����ʑ���
			IncreaseHeat(10.0f);

			//�R���{�����Z����
			AddCombo();

			//2D�G�t�F�N�g�\��
			DisplayEffect2D();
		}
	}
}

//===========================================================================================================
// ���ˏ���
//===========================================================================================================
void CPlayer::Shot()
{
	//�ǂ��蒆
	if (m_State == STATE::WALL_SLIDE)
	{
		//���������Ȃ�
		return;
	}
	//�ǃL�b�N��
	if (m_State == STATE::WALL_KICK && GetMotionFlag())
	{
		//���������Ȃ�
		return;
	}

	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();//�L�[�{�[�h
	CInputJoypad* pJoypad = pManager->GetJoypad();//�W���C�p�b�h

	//���˃{�^���̓��͔��肪true
	if (pKeyboard->GetPress(DIK_F) || pJoypad->GetPress(CInputJoypad::JOYKEY::JOYKEY_B))
	{
		//���[�J���ϐ��錾
		D3DXVECTOR3 BulletPos(0.0f, 0.0f, 0.0f);//���˒n�_
		ModelParts aParts = {};//�p�[�c���ۑ��p
		int nParts = 0;//�p�[�c�C���f�b�N�X�ۑ��p

		//���˒n�_��X��,Z��������ɐݒ�
		nParts = static_cast<int>(PARTS::HandL);
		aParts = GetPartsInfo(nParts);
		BulletPos.x = aParts.WorldPos.x;
		BulletPos.z = aParts.WorldPos.z;

		//���˒n�_��Y������r�����ɐݒ�
		nParts = static_cast<int>(PARTS::ArmUL);
		aParts = GetPartsInfo(nParts);
		BulletPos.y = aParts.WorldPos.y;

		//�v���C���[�̏����擾
		D3DXVECTOR3 rot = GetRot();//�p�x

		//�e�̈ړ��ʐݒ�
		D3DXVECTOR3 BulletMove(-sinf(rot.y) * BULLET_SPEED, 0.0f, -cosf(rot.y) * BULLET_SPEED);

		//���Ԋu�Œe�𔭎�
		if ((pKeyboard->GetRepeat(DIK_F, SHOT_SPEED) || pJoypad->GetRepeat(CInputJoypad::JOYKEY::JOYKEY_B, SHOT_SPEED)) &&
			!ShotPointCollisionBlock(BulletPos))//���˒n�_�ƃu���b�N�̓����蔻�肪false
		{
			//�e�𐶐�
			CBullet::Create(BulletPos, BulletMove, rot, CBullet::TYPE::PLAYER);

			//���ˉ��Đ�
			pManager->GetSound()->Play(CSound::SOUND::SHOT);

			//�ˌ��t���O��true�ɂ���
			m_bShooting = true;

			//�X���C�f�B���O��Ԃł͂Ȃ��ꍇ
			if (m_State != STATE::SLIDING)
			{
				//���ˏ�Ԃɂ���
				m_State = STATE::SHOT;

				//�������x���Z�b�g
				m_fInertia = BASE_INERTIA;
			}
		}
	}
	//���͂���Ă��Ȃ��ꍇ
	else
	{
		//�ˌ��t���O��false�ɂ���
		m_bShooting = false;

		//���ˏ�Ԃ̏ꍇ
		if (m_State == STATE::SHOT)
		{
			//�j���[�g������Ԃɂ���
			m_State = STATE::NEUTRAL;
		}
	}
}

//===========================================================================================================
// �X���C�f�B���O����
//===========================================================================================================
void CPlayer::Sliding(D3DXVECTOR3& move)
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CCalculate* pCalculate = pManager->GetCalculate();//�v�Z����

	//�Z���^�[�p�[�c�̃C���f�b�N�X��ۑ�
	int nParts = static_cast<int>(PARTS::CENTER);

	//�X���C�f�B���O���̏ꍇ
	if (m_State == STATE::SLIDING)
	{
		//�e���V�����ʑ���
		IncreaseHeat(0.25f);

		//�Z���^�[�p�[�c�̒����p�̍��W��ݒ�
		D3DXVECTOR3 pos(0.0f, 5.0f, 0.0f);

		//�Z���^�[�p�[�c�̃��[�J�����W�𒲐�
		SetLocalPos(nParts, pos);

		//���[�V�������I�����Ă���ꍇ
		if (!GetMotionFlag())
		{
			//�j���[�g������Ԃɂ���
			m_State = STATE::NEUTRAL;
		}

		//����������
		OccurSmoke();

		//�����_���ɃJ���[��ݒ�
		float fR = pCalculate->RandFloat(0.1f, 1.0f);
		float fG = pCalculate->RandFloat(0.1f, 1.0f);
		float fB = pCalculate->RandFloat(0.1f, 1.0f);

		//�p�[�e�B�N������
		CParticle::Create(CParticle::TYPE::NORMAL, GetCenterPos(), { 35.0f, 35.0f, 0.0f }, 20, { fR, fG, fB, 1.0f });
	}
	//�X���C�f�B���O���ł͂Ȃ��ꍇ
	else
	{
		//�Z���^�[�p�[�c�����擾
		ModelParts aParts = GetPartsInfo(nParts);

		//���[�J�����W��߂�
		SetLocalPos(nParts, aParts.FirstPos);
	}

	//�n��ŉ����͂���Ă���ꍇ
	if ((pManager->GetKeyboard()->GetTrigger(DIK_S) ||
		pManager->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_DOWN)) &&
		GetLandingFlag() && 
		m_State != STATE::SLIDING)
	{
		//�X���C�f�B���O��Ԃɂ���
		m_State = STATE::SLIDING;

		//�p�����[�^�����擾
		D3DXVECTOR3 rot = GetRot();//�p�x
	
		//�ړ��ʂ����Z
		move.x -= sinf(rot.y) * ACCEL_SLIDING;//X��
		move.z -= cosf(rot.y) * ACCEL_SLIDING;//Z��

		//SE�Đ�
		CManager::GetInstance()->GetSound()->Play(CSound::SOUND::SLIDING);
	}
}

//===========================================================================================================
// ���n������
//===========================================================================================================
void CPlayer::Landing(CBlock::CollisionParam& aParam)
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[

	//�W�����v�񐔃��Z�b�g
	m_nCntJump = JUMP_TIMES;

	//�������x���Z�b�g
	m_fInertia = BASE_INERTIA;

	//�_���[�W��Ԃ̏ꍇ�����𔲂���
	if (m_State == STATE::DAMAGE) return;

	//�����Ă��Ȃ��ꍇ
	if (!CObject::MovingJudgeXZ(aParam.move) && 
		m_State != STATE::SHOT && 
		m_State != STATE::SLIDING)
	{
		//�j���[�g������Ԃɐݒ�
		m_State = STATE::NEUTRAL;
	}

	//�����Ă���ꍇ
	else if (CObject::MovingJudgeXZ(aParam.move) && 
		m_State != STATE::SHOT && 
		m_State != STATE::SLIDING && 
		m_nCntDash == 0)
	{
		//�j���[�g������Ԃɐݒ�
		m_State = STATE::LANDING_MOVE;
	}

	//�X���C�f�B���O���̏ꍇ
	else if (m_State == STATE::SLIDING)
	{
		//���[�V���������擾
		MotionSet aMotionSet = GetMotionSet(static_cast<int>(m_State));

		//�⓹�̏ꍇ
		if (aParam.type == CBlock::TYPE::SLONE && 
			(pManager->GetKeyboard()->GetPress(DIK_S) ||
				pManager->GetJoypad()->GetPress(CInputJoypad::JOYKEY::JOYKEY_DOWN)))
		{
			//�ړ��ʂ��������Ȃ�
			m_fInertia = -0.02f;

			//���[�V���������[�v������
			aMotionSet.nLoop = 1;
		}
		//�㏸��or���s
		if (aParam.pos.y - aParam.oldpos.y >= 0.0f)
		{
			//�������x��������
			m_fInertia = BASE_INERTIA * 0.2f;

			//���[�V���������[�v�����Ȃ�
			aMotionSet.nLoop = 0;
		}

		//���[�V�������̐ݒ�
		SetMotionSet(static_cast<int>(m_State), aMotionSet);
	}
}

//===========================================================================================================
// ����������
//===========================================================================================================
void CPlayer::Falling(CBlock::CollisionParam& aParam, WallState wallstate)
{
	//�W�����v�̉񐔂��c���Ă���ꍇ���Z����
	if (m_nCntJump == JUMP_TIMES) m_nCntJump--;

	//�ǂ����Ԃł͂Ȃ��ꍇ
	//���ˏ�Ԃł͂Ȃ��ꍇ
	if (m_State != STATE::WALL_SLIDE && 
		m_State != STATE::SHOT && 
		m_State != STATE::DAMAGE)
	{
		//������Ԃɂ���
		m_State = STATE::FALL;
	}
	//�ǂ����Ԃ̏ꍇ
	else if (m_State == STATE::WALL_SLIDE)
	{
		//�p�[�c�C���f�b�N�X��ۑ�
		int nParts = static_cast<int>(PARTS::FACE);

		//��̃p�[�c�����擾
		ModelParts aParts = GetPartsInfo(nParts);

		//�J�����̊p�x�����擾
		float fCameraRot_Y = CManager::GetInstance()->GetCamera()->GetRot().y;

		//��𐳖ʂɌ�����
		aParts.rot.y = -aParam.rot.y + fCameraRot_Y;
		CorrectionRot(aParts.rot.y);

		//�X�V���ꂽ�p�[�c����ݒ�
		SetPartsInfo(nParts, aParts);

		//�ǂ̔���͕ǂ���ł͂Ȃ��ꍇ
		if (wallstate != WallState::SLIDE)
		{
			//������Ԃɂ���
			m_State = STATE::FALL;
		}
	}

	//�������x���Z�b�g
	m_fInertia = BASE_INERTIA;
}

//===========================================================================================================
// �ǃL�b�N����
//===========================================================================================================
CPlayer::WallState CPlayer::WallKick(CBlock* pBlock, CBlock::CollisionParam& aParam)
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();//�L�[�{�[�h
	CInputJoypad* pJoypad = pManager->GetJoypad();//�W���C�p�b�h

	//���[�J���ϐ��錾
	WallState wallstate = WallState::NONE;//�ǂ̐ڐG���
	WallLine Line = WallLine::NONE;//�G�ꂽ�ǂ̐���
	D3DXVECTOR3 BlockPos = pBlock->GetPos();//�u���b�N���W
	D3DXVECTOR3 BlockSize = pBlock->GetSize();//�u���b�N�T�C�Y
	bool bCollision = false;//�ǂƂ̐ڐG����t���O
	bool bCliff = false;//�R�͂܂蔻��

	//�ړ��L�[��������Ă���ꍇ
	if (pKeyboard->GetPress(DIK_D) ||
		pJoypad->GetPress(CInputJoypad::JOYKEY::JOYKEY_RIGHT) ||
		pKeyboard->GetPress(DIK_A) ||
		pJoypad->GetPress(CInputJoypad::JOYKEY::JOYKEY_LEFT))
	{
		//�����̔���
		if ((aParam.pos.x + aParam.size.x * 0.5f >= BlockPos.x - BlockSize.x * 0.5f && 
				aParam.oldpos.x + aParam.size.x * 0.5f <= BlockPos.x - BlockSize.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//�u���b�N�̐����x�N�g�������߂�
			pBlock->SEGMENT_XY(BlockPos.x, BlockPos.y - BlockSize.y * 0.5f, BlockPos.x, BlockPos.y + BlockSize.y * 0.5f);

			//�ڐG�����true�ɂ���
			bCollision = true;

			//�G�ꂽ�����XY�ɐݒ�
			Line = WallLine::XY;

			//�v���C���[�̒��_���u���b�N���������ꍇ
			if (aParam.pos.y + aParam.size.y >= BlockPos.y + BlockSize.y * 0.5f - 10.0f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f - 10.0f)
			{
				//�R�͂܂蔻���true�ɂ���
				bCliff = true;

				//�J�����̊p�x���擾
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//�p�x�̉��Z��
				float fAddRot_Y = 0.0f;

				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				if (fCamera_Rot_Y >= 0.0f)
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = -D3DX_PI * 0.5f;
				}
				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				else
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = D3DX_PI * 0.5f;
				}

				//�p�x���E�����ɒ���
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}

		//�E���̔���
		else if ((aParam.pos.x - aParam.size.x * 0.5f <= BlockPos.x + BlockSize.x * 0.5f && 
					aParam.oldpos.x - aParam.size.x * 0.5f >= BlockPos.x + BlockSize.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//�u���b�N�̐����x�N�g�������߂�
			pBlock->SEGMENT_XY(BlockPos.x, BlockPos.y + BlockSize.y * 0.5f, BlockPos.x, BlockPos.y - BlockSize.y * 0.5f);

			//�ڐG�����true�ɂ���
			bCollision = true;

			//�G�ꂽ�����XY�ɐݒ�
			Line = WallLine::XY;

			//�v���C���[�̒��_���u���b�N���������ꍇ
			if (aParam.pos.y + aParam.size.y >= BlockPos.y + BlockSize.y * 0.5f - 10.0f
				&& aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f - 10.0f)
			{
				//�R�͂܂蔻���true�ɂ���
				bCliff = true;

				//�J�����̊p�x���擾
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//�p�x�̉��Z��
				float fAddRot_Y = 0.0f;

				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				if (fCamera_Rot_Y >= 0.0f)
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = D3DX_PI * 0.5f;
				}
				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				else
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = -D3DX_PI * 0.5f;
				}

				//�p�x���E�����ɒ���
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}

		//��O���̔���
		else if ((aParam.pos.z + aParam.size.z * 0.5f >= BlockPos.z - BlockSize.z * 0.5f && 
					aParam.oldpos.z + aParam.size.z * 0.5f <= BlockPos.z - BlockSize.z * 0.5f) && 
			(aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f) && 
			(aParam.pos.x + aParam.size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f && 
				aParam.pos.x - aParam.size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f))
		{
			//�u���b�N�̐����x�N�g�������߂�
			pBlock->SEGMENT_YZ(BlockPos.y - BlockSize.y * 0.5f, BlockPos.z, BlockPos.y + BlockSize.y * 0.5f, BlockPos.z);

			//�ڐG�����true�ɂ���
			bCollision = true;

			//�G�ꂽ�����YZ�ɐݒ�
			Line = WallLine::YZ;

			//�v���C���[�̒��_���u���b�N���������ꍇ
			if (aParam.pos.y + aParam.size.y >= BlockPos.y + BlockSize.y * 0.5f - 10.0f
				&& aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f - 10.0f)
			{
				//�R�͂܂蔻���true�ɂ���
				bCliff = true;

				//�J�����̊p�x���擾
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//�p�x�̉��Z��
				float fAddRot_Y = 0.0f;

				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				if (fCamera_Rot_Y >= 0.0f)
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = D3DX_PI * 0.5f;
				}
				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				else
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = -D3DX_PI * 0.5f;
				}

				//�p�x���E�����ɒ���
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}

		//�����̔���
		else if ((aParam.pos.z - aParam.size.z * 0.5f <= BlockPos.z + BlockSize.z * 0.5f
			&& aParam.oldpos.z - aParam.size.z * 0.5f >= BlockPos.z + BlockSize.z * 0.5f)
			&& (aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f
				&& aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f)
			&& (aParam.pos.x + aParam.size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
				&& aParam.pos.x - aParam.size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f))
		{
			//�u���b�N�̐����x�N�g�������߂�
			pBlock->SEGMENT_YZ(BlockPos.y + BlockSize.y * 0.5f, BlockPos.z, BlockPos.y - BlockSize.y * 0.5f, BlockPos.z);

			//�ڐG�����true�ɂ���
			bCollision = true;

			//�G�ꂽ�����YZ�ɐݒ�
			Line = WallLine::YZ;

			//�v���C���[�̒��_���u���b�N���������ꍇ
			if (abs((aParam.pos.y + aParam.size.y) - (BlockPos.y + BlockSize.y * 0.5f)) <= 10.0f)
			{
				//�R�͂܂蔻���true�ɂ���
				bCliff = true;

				//�J�����̊p�x���擾
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//�p�x�̉��Z��
				float fAddRot_Y = 0.0f;

				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				if (fCamera_Rot_Y >= 0.0f)
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = -D3DX_PI * 0.5f;
				}
				//�J�����̊p�x�̒l�����̒l�̏ꍇ
				else
				{
					//���Z�ʂ�ݒ�
					fAddRot_Y = D3DX_PI * 0.5f;
				}

				//�p�x���E�����ɒ���
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}
	}

	//�ڐG���肪true�̏ꍇ
	if (bCollision)
	{
		//���˃x�N�g���v�Z����
		ReflectAngleCalculate(pBlock, aParam, Line, wallstate);
	}

	return wallstate;
}

//===========================================================================================================
// ���˃x�N�g���v�Z��������
//===========================================================================================================
void CPlayer::ReflectAngleCalculate(CBlock* pBlock, CBlock::CollisionParam& aParam, WallLine Line, WallState& wallstate)
{
	//���[�J���ϐ��錾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[�C���X�^���X
	CCalculate* pCalculate = pManager->GetCalculate();//�v�Z�����C���X�^���X
	D3DXVECTOR3 Normal = pBlock->GetNormal();//�����x�N�g��
	float fKickPowerWidth = 0.0f;//�ǃL�b�N�̉��̋���
	float fKickPowerHeight = 0.0f;//�ǃL�b�N�̏c�̋���
	float fReflectAngle = 0.0f;//���˃x�N�g���̊p�x

	//�����x�N�g�������߂�
	D3DXVECTOR3 Direction = aParam.pos - GetOldPos();

	//�i�s�x�N�g���𐳋K��
	D3DXVec3Normalize(&Direction, &Direction);

	//�W�������߂�
	D3DXVECTOR3 Factor(-Direction.x * Normal.x, -Direction.y * Normal.y, -Direction.z * Normal.z);

	//�W���𐳋K��
	D3DXVec3Normalize(&Factor, &Factor);

	//���s�x�N�g�������߂�
	D3DXVECTOR3 Parallel(Direction.x + Factor.x * Normal.x, Direction.y + Factor.y * Normal.y, Direction.z + Factor.z * Normal.z);

	//���s�x�N�g���𐳋K��
	D3DXVec3Normalize(&Parallel, &Parallel);

	//���˃x�N�g�������߂�
	D3DXVECTOR3 Reflect(Direction.x + 2.0f * Factor.x * Normal.x, Direction.y + 2.0f * Factor.y * Normal.y, Direction.z + 2.0f * Factor.z * Normal.z);

	//���˃x�N�g���𐳋K��
	D3DXVec3Normalize(&Reflect, &Reflect);

	//�W�����v�{�^���������ꂽ�ꍇ
	if (pManager->GetKeyboard()->GetTrigger(DIK_SPACE) || 
		pManager->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_A))
	{
		//���˂̃p���[��ݒ�
		fKickPowerWidth = MAX_KICKPOWER_WIDTH;
		fKickPowerHeight = MAX_KICKPOWER_HEIGHT;

		//�ǃL�b�N��Ԃɐݒ�
		wallstate = WallState::KICK;

		//�R���{�����Z����
		AddCombo();

		//2D�G�t�F�N�g�\��
		DisplayEffect2D();

		//�w�肵�������J��Ԃ�
		for (int nCnt = 0; nCnt < NUM_CREATE_PARTICLE; nCnt++)
		{
			//�����_���ɃJ���[��ݒ�
			float fR = pCalculate->RandFloat(0.1f, 1.0f);
			float fG = pCalculate->RandFloat(0.1f, 1.0f);
			float fB = pCalculate->RandFloat(0.1f, 1.0f);

			//�������W
			D3DXVECTOR3 CreatePos(aParam.pos.x, aParam.pos.y + aParam.size.y * 0.5f, aParam.pos.z);

			//�p�[�e�B�N������
			CParticle::Create(CParticle::TYPE::NORMAL, CreatePos, { 25.0f, 25.0f, 0.0f }, 40, { fR, fG, fB, 1.0f });
		}

		//�ڕW�̊p�x��ݒ�
		m_fTargetRot_Y = GetRot().y + D3DX_PI;

		//�p�x�␳
		CObject::CorrectionRot(m_fTargetRot_Y);

		//�ǃL�b�N��]���x�ݒ菈��
		SetWallKickTurn();

		//�G�ꂽ���オXY�̏ꍇ
		if (Line == WallLine::XY)
		{
			//���˃x�N�g����X�����ɑ΂��āA�p�x��ύX
			if (Reflect.x < 0.0f)
			{
				fReflectAngle = 1.78f;
			}
			else
			{
				fReflectAngle = -1.78f;
			}
		}
		//�G�ꂽ���オYZ�̏ꍇ
		else if (Line == WallLine::YZ)
		{
			//���˃x�N�g����X�����ɑ΂��āA�p�x��ύX
			if (Reflect.z < 0.0f)
			{
				fReflectAngle = 1.78f;
			}
			else
			{
				fReflectAngle = -1.78f;
			}
		}
	}
	else
	{
		//���_�����u���b�N���������ʒu�̏ꍇ
		if (aParam.pos.y + GetSize().y > pBlock->GetPos().y + pBlock->GetSize().y * 0.5f) return;

		//���˂̃p���[��ݒ�
		fKickPowerHeight = SLIDE_POWER;

		//�ǂ����Ԃɐݒ�
		wallstate = WallState::SLIDE;
	}

	//����Ɗp�x�ɍ��킹�Ĉړ��ʂ�ύX
	if (Line == WallLine::XY)
	{
		aParam.move = { sinf(fReflectAngle + D3DX_PI) * fKickPowerWidth, cosf(fReflectAngle + D3DX_PI) * fKickPowerHeight, 0.0f };
	}
	else if (Line == WallLine::YZ)
	{
		aParam.move = { 0.0f, cosf(fReflectAngle + D3DX_PI) * fKickPowerHeight, sinf(fReflectAngle + D3DX_PI) * fKickPowerWidth };
	}
}

//===========================================================================================================
// �e���V�����ʑ�������
//===========================================================================================================
void CPlayer::IncreaseHeat(float fAdd)
{
	//�e���V�����ʂ𑝉�
	m_fHeat += fAdd;

	//�e���V�����ʂ��ő�l�𒴂����ꍇ
	if (m_fHeat > MAX_HEAT)
	{
		//�ő�l�ɒ���
		m_fHeat = MAX_HEAT;
	}

	//�R���{�p���t���[����ݒ�
	m_nComboTime = COMBO_TIME;
}

//===========================================================================================================
// �e���V�����ʌ�������
//===========================================================================================================
void CPlayer::DecreaseHeat()
{
	//�R���{�^�C���p����
	if (m_nComboTime > 0)
	{
		m_nComboTime--;
		return;
	}

	// �N�[���^�C����1�ȏ�̏ꍇ
	if (m_nDecTenCT > 0)
	{
		// �N�[���^�C������
		m_nDecTenCT--;
	}

	// �N�[���^�C����0�̏ꍇ
	if (m_nDecTenCT == 0)
	{
		// �N�[���^�C�����Z�b�g
		m_nDecTenCT = DEC_HEAT_CT;

		// �e���V�����ʂ�1�ȏ�̏ꍇ
		if (m_fHeat > 0.0f)
		{
			// �e���V�����ʌ���
			m_fHeat -= 1.0f;
		}

		// �e���V�����ʂ�0.0f�������Ȃ��悤�ɒ���
		if (m_fHeat < 0.0f)
		{
			m_fHeat = 0.0f;
		}
	}
}

//===========================================================================================================
// �R���{�����Z����
//===========================================================================================================
void CPlayer::AddCombo()
{
	//�R���{�����ő�ł͂Ȃ�
	if (m_nCombo < MAX_COMBO)
	{
		//�R���{�������Z
		m_nCombo++;

		//�J�E���^���Z�b�g
		m_nDecComboFrame = 0;
	}
}

//===========================================================================================================
// �R���{�����Z�b�g����
//===========================================================================================================
void CPlayer::ResetCombo()
{
	//�R���{����0�̏ꍇ�͏������Ȃ�
	if (m_nCombo <= 0) return;

	//�J�E���^�����Z
	m_nDecComboFrame++;

	//�J�E���^���ő�
	if (m_nDecComboFrame >= DECREASE_COMBO_FRAME)
	{
		//�R���{�����Z�b�g
		m_nCombo = 0;

		//�J�E���^���Z�b�g
		m_nDecComboFrame = 0;

		//2D�G�t�F�N�g�̃|�C���^�擾
		CEffect2D* pEffect2D = CEffect2D::GetEffect2D();

		//2D�G�t�F�N�g���폜
		if (pEffect2D != nullptr)
		{
			//2D�G�t�F�N�g�I������
			pEffect2D->Uninit();
		}
	}
}

//===========================================================================================================
// �q�[�g�G�t�F�N�g��������
//===========================================================================================================
void CPlayer::OccurHeatEffect()
{
	//�v�Z�����C���X�^���X�擾
	CCalculate* pCalculate = CManager::GetInstance()->GetCalculate();

	//���W���擾
	D3DXVECTOR3 PlayerPos = GetPos();

	//����Y���W�ݒ�
	float CreatePos_Y = PlayerPos.y;

	//����X���W�ݒ�
	float CreatePos_X = pCalculate->RandFloat(PlayerPos.x - RANGE_EFFECT, PlayerPos.x + RANGE_EFFECT);

	//����Z���W�ݒ�
	float CreatePos_Z = pCalculate->RandFloat(PlayerPos.z - RANGE_EFFECT, PlayerPos.z + RANGE_EFFECT);

	//�������W�ݒ�
	D3DXVECTOR3 CreatePos(CreatePos_X, CreatePos_Y, CreatePos_Z);

	//���݂̃q�[�g�ʂ̊��������߂�
	float fRatioHeat = m_fHeat / MAX_HEAT * 10.0f;

	//�����ɍ��킹�ăp�[�e�B�N���𔭐�
	for (int nCnt = 0; nCnt < static_cast<int>(fRatioHeat); nCnt++)
	{
		CParticle::Create(CParticle::TYPE::ASCEND, CreatePos, { 15.0f, 15.0f, 0.0f }, 20, { 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

//===========================================================================================================
// ����������
//===========================================================================================================
void CPlayer::OccurSmoke()
{
	//�v�Z�����C���X�^���X�擾
	CCalculate* pCalculate = CManager::GetInstance()->GetCalculate();

	//���W���擾
	D3DXVECTOR3 PlayerPos = GetPos();

	//����Y���W�ݒ�
	float CreatePos_Y = PlayerPos.y;

	//����X���W�ݒ�
	float CreatePos_X = pCalculate->RandFloat(PlayerPos.x - RANGE_EFFECT, PlayerPos.x + RANGE_EFFECT);

	//����Z���W�ݒ�
	float CreatePos_Z = pCalculate->RandFloat(PlayerPos.z - RANGE_EFFECT, PlayerPos.z + RANGE_EFFECT);

	//�������W�ݒ�
	D3DXVECTOR3 CreatePos(CreatePos_X, CreatePos_Y, CreatePos_Z);

	//���G�t�F�N�g�𔭐�
	CSmoke::Create(CreatePos, { 20.0f, 20.0f, 0.0f }, 30);
}

//===========================================================================================================
// 2D�G�t�F�N�g�\������
//===========================================================================================================
void CPlayer::DisplayEffect2D()
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CUIManager* pUIManager = pManager->GetScene()->GetUIManager();//UI�}�l�[�W���[
	CCalculate* pCalculate = pManager->GetCalculate();//�v�Z����

	//�C���X�^���X��nullptr
	if (pUIManager == nullptr)
	{
		return;
	}

	//�q�[�g�Q�[�W�t���[��������
	CUI* pUI = pUIManager->FindUI(CUI::TYPE::HEAT_GAGE_FRAME);

	//�q�[�g�Q�[�W�t���[���̃p�����[�^�擾
	D3DXVECTOR2 UIpos = pUI->GetPos();//���W
	D3DXVECTOR2 UIsize = pUI->GetSize();//�T�C�Y

	//�������W�ݒ�
	D3DXVECTOR2 CreatePos(UIpos.x, UIpos.y - UIsize.y * 0.5f);

	//�����������_���ɐݒ�
	float fRot_Z = pCalculate->RandFloat(-RANGE_EFFECT2D_ROT, RANGE_EFFECT2D_ROT);

	//�G�t�F�N�g�\��
	CEffect2D::Create(static_cast<CEffect2D::TYPE>(m_nCombo), CreatePos, { 0.0f, 0.0f, fRot_Z });
}

//===========================================================================================================
// �_�ŏ���
//===========================================================================================================
void CPlayer::Flash()
{
	//���[�J���ϐ��錾
	static bool bLightUp = false;

	//���C�g�A�b�v���̏ꍇ
	if (bLightUp)
	{
		//���f���𖾂邭�ݒ�
		SetPartsFlash(1.0f);
	}
	else
	{
		//���f�����Â��ݒ�
		SetPartsFlash(0.1f);
	}

	//���C�g�A�b�v�t���O��؂�ւ���
	bLightUp = bLightUp ? false : true;

	//��ԃJ�E���^��0�̏ꍇ
	if (m_nCntState == 0)
	{
		//�ʏ��Ԃɂ���
		m_State = STATE::NEUTRAL;
	}
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
void CPlayer::Collision()
{
	//�u���b�N�Ƃ̓����蔻��
	CollisionBlock();

	//�G���A�Ƃ̓����蔻��
	CollisionArea();

	//�G�l�~�[�Ƃ̓����蔻��
	CollisionEnemy();

	//�_���[�W��Ԃ̏ꍇ
	if (m_State == STATE::DAMAGE)
	{
		//�_�ŏ���
		Flash();
	}
	//�_���[�W��Ԃł͂Ȃ��ꍇ
	else
	{
		//���f���̖��邳��߂�
		SetPartsFlash(1.0f);

		//�{�X�Ƃ̓����蔻��
		CollisionBoss();

		//�{�X�̍U���Ƃ̓����蔻��
		CollisionBossAttack();
	}
}

//===========================================================================================================
// �u���b�N�Ƃ̓����蔻��
//===========================================================================================================
void CPlayer::CollisionBlock()
{
	//�����蔻��p�p�����[�^�ݒ�
	CBlock::CollisionParam aParam;
	aParam.pos = GetPos();//���W
	aParam.oldpos = GetOldPos();//�ߋ��̍��W
	aParam.size = GetSize();//�T�C�Y
	aParam.move = GetMove();//�ړ���
	aParam.rot = GetRot();//�p�x
	aParam.type = CBlock::TYPE::NONE;//���������u���b�N�̃^�C�v

	//�ǃL�b�N���菈��
	WallState wallstate = CollisionWallKick(aParam);

	//�㑤�̔�����擾
	CBlock::CollisionFlag aFlag = CManager::GetInstance()->GetScene()->GetBlockManager()->Collision(aParam);
	SetLandingFlag(aFlag.Y_UP);

	//X����Z���̔��肪true
	if (aFlag.X || aFlag.Z)
	{
		//�_�b�V����Ԃ�����
		m_nCntDash = 0;
	}

	//Y���̔��肪true
	if (aFlag.Y_UP)
	{
		//���n������
		Landing(aParam);
	}
	//�����Ă���ꍇ
	else if(!aFlag.Y_UP && aParam.move.y < 0.00f)
	{
		//����������
		Falling(aParam, wallstate);
	}

	//�X�V���ꂽ�p�����[�^��ݒ�
	SetPos(aParam.pos);//���W
	SetMove(aParam.move);//�ړ���
	SetRot(aParam.rot);//�p�x
}

//===========================================================================================================
// �ǃL�b�N���菈��
//===========================================================================================================
CPlayer::WallState CPlayer::CollisionWallKick(CBlock::CollisionParam& aParam)
{
	//���[�J���ϐ��錾
	WallState wallstate = WallState::NONE;

	//�u���b�N�̐擪�A�h���X�擾
	CBlock* pBlock = CManager::GetInstance()->GetScene()->GetBlockManager()->GetTop();

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pBlock != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CBlock* pNext = pBlock->GetNextBlock();

		//�u���b�N�^�C�v���⓹�ł͂Ȃ��ꍇ
		if ((pBlock != nullptr && pBlock->GetBlockType() != CBlock::TYPE::SLONE) && !GetLandingFlag())
		{
			//�ǔ��菈��
			wallstate = WallKick(pBlock, aParam);

			//�ǂ̏�Ԃ�NONE�ł͂Ȃ��ꍇ���[�v���I��
			if (wallstate != WallState::NONE) break;
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pBlock = pNext;
	}

	//�ǂ̔��育�Ƃɏ�Ԃ�ω�
	switch (wallstate)
	{
		//�ǂ���
	case WallState::SLIDE:
		//�v���C���[��ǂ����Ԃɐݒ�
		m_State = STATE::WALL_SLIDE;
		break;

		//�ǃL�b�N
	case WallState::KICK:
		//�v���C���[��ǃL�b�N��Ԃɐݒ�
		m_State = STATE::WALL_KICK;

		//�������x����߂�
		m_fInertia = BASE_INERTIA * 0.05f;

		//�e���V�����ʑ���
		IncreaseHeat(10.0f);
		break;

	default:
		break;
	}

	return wallstate;
}

//===========================================================================================================
// �G�l�~�[�Ƃ̓����蔻��
//===========================================================================================================
void CPlayer::CollisionEnemy()
{
	//�����蔻��p�p�����[�^�ݒ�
	CEnemy::CollisionParam aParam;
	aParam.pos = GetPos();//���W
	aParam.size = GetSize();//�T�C�Y
	aParam.pEnemy = nullptr;//���������G�l�~�[�̏��

	//�G�l�~�[�Ƃ̓����蔻�肪true�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetEnemyManager()->Collision(aParam) && 
		aParam.pEnemy != nullptr)
	{
		//�_���[�W��Ԓ��ɒʏ�̓G�ɓ��������ꍇ
		if (aParam.pEnemy->GetEnemyType() != CEnemy::TYPE::BALL && 
			m_State == STATE::DAMAGE)
		{
			//���������G�l�~�[�̏��̃|�C���^��nullptr�ɂ���
			aParam.pEnemy = nullptr;

			//�����𔲂���
			return;
		}
		//�{�[���ƏՓ˂����ꍇ
		else if (aParam.pEnemy->GetEnemyType() == CEnemy::TYPE::BALL)
		{
			//�`�F�C�X�G���A���X�|�[������
			RespawnChaseArea();

			//�����𔲂���
			return;
		}

		//�_���[�W��Ԃɂ���
		m_State = STATE::DAMAGE;

		//��ԃJ�E���^��ݒ�
		m_nCntState = 30;

		//�G�l�~�[�̈ʒu�����擾
		D3DXVECTOR3 EnemyPos = aParam.pEnemy->GetPos();

		//�v���C���[�̈ړ��ʏ��擾
		D3DXVECTOR3 PlayerMove = GetMove();

		//�����x�N�g�������߂�
		D3DXVECTOR3 Direction = GetPos() - EnemyPos;
		D3DXVec3Normalize(&Direction, &Direction);

		//�ړ��ʏ��X�V
		PlayerMove.x = Direction.x * 50.0f;
		PlayerMove.y = Direction.y;
		PlayerMove.z = Direction.z * 50.0f;

		//�X�V���ꂽ�ړ��ʏ���ݒ�
		SetMove(PlayerMove);
	}

	//���������G�l�~�[�̏��̃|�C���^��nullptr�ɂ���
	aParam.pEnemy = nullptr;
}

//===========================================================================================================
// �{�X�Ƃ̓����蔻��
//===========================================================================================================
void CPlayer::CollisionBoss()
{
	//�{�X�̃I�u�W�F�N�g��������
	CObject* pFindObj = CObject::FindObject(CObject::Category::BOSS);
	CBoss* pBoss = nullptr;

	//�������ʂ����v�����ꍇ�_�E���L���X�g
	if (pFindObj != nullptr) pBoss = CObject::DownCast<CBoss, CObject>(pFindObj);

	//�����蔻��p�p�����[�^�ݒ�
	CBoss::CollisionParam aParam = {};
	aParam.pos = GetPos();//���W
	aParam.size = GetSize();//�T�C�Y

	//�{�X�Ƃ̓����蔻�肪true�̏ꍇ
	if (pBoss != nullptr && pBoss->Collision(aParam))
	{
		//�_���[�W��Ԃɂ���
		m_State = STATE::DAMAGE;

		//��ԃJ�E���^��ݒ�
		m_nCntState = DAMAGE_TIME;

		//�G�l�~�[�̈ʒu�����擾
		D3DXVECTOR3 BossPos = pBoss->GetPos();

		//�v���C���[�̈ړ��ʏ��擾
		D3DXVECTOR3 PlayerMove = GetMove();

		//�����x�N�g�������߂�
		D3DXVECTOR3 Direction = aParam.pos - BossPos;
		D3DXVec3Normalize(&Direction, &Direction);

		//�ړ��ʏ��X�V
		PlayerMove.x = Direction.x * 50.0f;
		PlayerMove.y = Direction.y;
		PlayerMove.z = Direction.z * 50.0f;

		//�X�V���ꂽ�ړ��ʏ���ݒ�
		SetMove(PlayerMove);
	}
}

//===========================================================================================================
// �{�X�̍U���Ƃ̓����蔻��
//===========================================================================================================
void CPlayer::CollisionBossAttack()
{
	//�{�X�̍U���̃I�u�W�F�N�g��������
	CObject* pFindObj = CObject::FindObject(CObject::Category::BOSS_ATTACK);
	CBossAttack* pBossAttack = nullptr;

	//�������ʂ����v�����ꍇ�_�E���L���X�g
	if (pFindObj != nullptr) pBossAttack = CObject::DownCast<CBossAttack, CObject>(pFindObj);

	//�����蔻��p�p�����[�^�ݒ�
	CBossAttack::CollisionParam aParam = {};
	aParam.pos = GetPos();//���W
	aParam.size = GetSize();//�T�C�Y

	//�{�X�Ƃ̓����蔻�肪true�̏ꍇ
	if (pBossAttack != nullptr && pBossAttack->Collision(aParam))
	{
		//�_���[�W��Ԃɂ���
		m_State = STATE::DAMAGE;

		//��ԃJ�E���^��ݒ�
		m_nCntState = DAMAGE_TIME;

		//�G�l�~�[�̈ʒu�����擾
		D3DXVECTOR3 BossAttackPos = pBossAttack->GetPos();

		//�v���C���[�̈ړ��ʏ��擾
		D3DXVECTOR3 PlayerMove = GetMove();

		//�����x�N�g�������߂�
		D3DXVECTOR3 Direction = GetPos() - BossAttackPos;
		D3DXVec3Normalize(&Direction, &Direction);

		//�ړ��ʏ��X�V
		PlayerMove.x = Direction.x * 50.0f;
		PlayerMove.y = Direction.y;
		PlayerMove.z = Direction.z * 50.0f;

		//�X�V���ꂽ�ړ��ʏ���ݒ�
		SetMove(PlayerMove);
	}
}

//===========================================================================================================
// ���˒n�_�̃u���b�N�����蔻��
//===========================================================================================================
bool CPlayer::ShotPointCollisionBlock(const D3DXVECTOR3& pos)
{
	//�C���X�^���X�擾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[
	CCollision* pCollision = pManager->GetCollision();//�����蔻�菈��

	//�u���b�N�̐擪�����擾
	CBlock* pBlock = pManager->GetScene()->GetBlockManager()->GetTop();

	//�����܂ŌJ��Ԃ�
	while (pBlock != nullptr)
	{
		//���̏���ۑ�
		CBlock* pNext = pBlock->GetNextBlock();

		//�����蔻�肪true
		if (pBlock->GetBlockType() == CBlock::TYPE::NORMAL && //�u���b�N�^�C�v���m�[�}��
			pCollision->PointBox3D(pos, pBlock->GetPos(), pBlock->GetSize()))
		{
			//true��Ԃ�
			return true;
		}

		//���̏����擾
		pBlock = pNext;
	}

	//�����܂œ����蔻�肪false
	return false;
}

//===========================================================================================================
// �G���A�Ƃ̓����蔻��
//===========================================================================================================
void CPlayer::CollisionArea()
{
	CollisionMiniGameArea();//�~�j�Q�[���G���A
	CollisionChaseArea();//�`�F�C�X�G���A
	CollisionBossArea();//�{�X�G���A
}

//�~�j�Q�[���G���A
void CPlayer::CollisionMiniGameArea()
{
	//�Q�[���V�[���̃C���X�^���X�擾
	CGame* pGame = CGame::GetInstance();

	//�C���X�^���X�擾���s
	if (pGame == nullptr)
	{
		return;
	}

	//�v���C���[���W�擾
	D3DXVECTOR3 PlayerPos = GetPos();

	//�~�j�Q�[���G���A������
	CArea* pArea = CManager::GetInstance()->GetScene()->GetAreaManager()->FindArea(CArea::TYPE::MINI_GAME);

	//�G���A�Ƃ̓����蔻�肪true
	if (pArea != nullptr &&
		pArea->GetAreaType() == CArea::TYPE::MINI_GAME &&
		pArea->Collision(PlayerPos))
	{
		//�~�j�Q�[�����ł͂Ȃ��Ȃ�C�x���g�Ǘ��𐶐�
		if (pGame->GetArea() != CGame::GAME_AREA::MINI_GAME)
		{
			CMiniGame::Create();
		}

		//���݂̃G���A���~�j�Q�[���G���A�ɐݒ�
		pGame->SetArea(CGame::GAME_AREA::MINI_GAME);
	}

	//�~�j�Q�[�����ɃG���A�O�ɏo���ꍇ�ߋ��̈ʒu�ɖ߂�
	if (pGame->GetArea() == CGame::GAME_AREA::MINI_GAME && !pArea->Collision(PlayerPos))
	{
		SetPos(GetOldPos());
	}
}

//�`�F�C�X�G���A
void CPlayer::CollisionChaseArea()
{
	//�V�[���̃C���X�^���X�擾
	CScene* pScene = CManager::GetInstance()->GetScene();

	//�v���C���[���W�擾
	D3DXVECTOR3 PlayerPos = GetPos();

	//�`�F�C�X�G���A�����݂��Ȃ�
	if (pScene->GetAreaManager() == nullptr)
	{
		return;
	}

	//�`�F�C�X�G���A������
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::CHASE);

	//�G���A�Ƃ̓����蔻�肪true
	if (pArea != nullptr &&
		pArea->GetAreaType() == CArea::TYPE::CHASE &&
		pArea->Collision(PlayerPos))
	{
		//�{�[���X�|�i�[������
		CGimmick* pGimmick = pScene->GetGimmickManager()->Find(CGimmick::TYPE::BALL_SPAWNER);

		//�������ʂ����v
		if (pGimmick != nullptr && pGimmick->GetType() == CGimmick::TYPE::BALL_SPAWNER)
		{
			//�_�E���L���X�g
			CBallSpawner* pBallSpawner = CObject::DownCast<CBallSpawner, CGimmick>(pGimmick);

			//�_�E���L���X�g���������ꍇ�{�[�����X�|�[��
			if (pBallSpawner != nullptr)
			{
				pBallSpawner->Spawn();
			}
		}
	}
}

//�{�X�G���A
void CPlayer::CollisionBossArea()
{
	//�Q�[���V�[���̃C���X�^���X�擾
	CGame* pGame = CGame::GetInstance();

	//�C���X�^���X�擾���s
	if (pGame == nullptr)
	{
		return;
	}

	//�v���C���[���W�擾
	D3DXVECTOR3 PlayerPos = GetPos();

	//�{�X�G���A������
	CArea* pArea = CManager::GetInstance()->GetScene()->GetAreaManager()->FindArea(CArea::TYPE::BOSS);

	//�G���A�Ƃ̓����蔻�肪true�̏ꍇ�A���݂̃G���A���~�j�Q�[���G���A�ɐݒ�
	if (pArea != nullptr &&
		pArea->GetAreaType() == CArea::TYPE::BOSS &&
		pArea->Collision(PlayerPos) &&
		!m_bBossCreate)
	{
		//�{�X�I�u�W�F�N�g����
		CObject* pObj = CObject::FindObject(Category::BOSS);

		//�������ʂ��s��v
		if (pObj == nullptr || pObj->GetCategory() != Category::BOSS)
		{
			return;
		}

		//�{�X�N���X�Ƀ_�E���L���X�g
		CBoss* pBoss = DownCast<CBoss, CObject>(pObj);

		//UI�����p�p�����[�^
		D3DXVECTOR2 FirstPos(1210.0f, 667.0f);//���W
		D3DXVECTOR2 size(30.0f, 20.0f);//�T�C�Y
		D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);//�p�x

		//�̗͂̃t���[���𐶐�
		CUI::Create(CUI::TYPE::BOSS_HP_FRAME, { FirstPos.x, 445.0f }, { size.x + 12.0f, 480.0f }, { 0.0f, 0.0f, 0.0f });

		//�̗͂�UI�𐶐�
		for (int nCnt = 0; nCnt < pBoss->GetMaxHP(); nCnt++)
		{
			//���ɉ����č�����ύX
			D3DXVECTOR2 pos(FirstPos.x, FirstPos.y - (size.y + 2.0f) * nCnt);

			//�̗�UI����
			CUI::Create(CUI::TYPE::BOSS_HP, pos, size, rot);
		}

		//�{�X�G���A�ɐݒ�
		pGame->SetArea(CGame::GAME_AREA::BOSS);

		//�����ς݃t���O��true�ɂ���
		m_bBossCreate = true;
	}

	//�~�j�Q�[�����ɃG���A�O�ɏo���ꍇ�ߋ��̈ʒu�ɖ߂�
	if (pGame->GetArea() == CGame::GAME_AREA::BOSS && !pArea->Collision(PlayerPos)) SetPos(GetOldPos());
}

//===========================================================================================================
// �`�F�C�X�G���A���X�|�[������
//===========================================================================================================
void CPlayer::RespawnChaseArea()
{
	//�V�[���̃C���X�^���X�擾
	CScene* pScene = CManager::GetInstance()->GetScene();

	//===========================================================================
	// �`�F�C�X�G���A������
	//===========================================================================
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::CHASE);

	//�������ʂ���v
	if (pArea != nullptr && pArea->GetAreaType() == CArea::TYPE::CHASE)
	{
		//�`�F�C�X�G���A�Ƀ��X�|�[��
		SetPos(pArea->GetPos());

		//�ړ��ʂ����Z�b�g
		SetMove({ 0.0f, 0.0f, 0.0f });
	}

	//===========================================================================
	// �{�[��������
	//===========================================================================
	CEnemy* pEnemy = pScene->GetEnemyManager()->FindEnemy(CEnemy::TYPE::BALL);

	//�������ʂ��s��v
	if (pEnemy == nullptr || pEnemy->GetEnemyType() != CEnemy::TYPE::BALL)
	{
		//�����𔲂���
		return;
	}

	//�v���C���[�N���X�Ƀ_�E���L���X�g
	CBall* pBall = CObject::DownCast<CBall, CEnemy>(pEnemy);

	//�_�E���L���X�g���s
	if (pBall == nullptr)
	{
		//�����𔲂���
		return;
	}

	//===========================================================================
	// �`�F�C�X�X�|�i�[������
	//===========================================================================
	CGimmick* pGimmick = pScene->GetGimmickManager()->Find(CGimmick::TYPE::BALL_SPAWNER);

	//�������ʂ��s��v
	if (pGimmick == nullptr || pGimmick->GetType() != CGimmick::TYPE::BALL_SPAWNER)
	{
		//�����𔲂���
		return;
	}

	//�X�|�i�[�̈ʒu���擾
	D3DXVECTOR3 SpawnerPos = pGimmick->GetPos();

	//�{�[�����X�|�i�[�Ƀ��X�|�[��
	pBall->SetPos(SpawnerPos);

	//�{�[���̏������Z�b�g
	pBall->Reset();
}

//===========================================================================================================
// �ǃL�b�N��]���x�ݒ菈��
//===========================================================================================================
void CPlayer::SetWallKickTurn()
{
	//���[�J���ϐ��錾
	float fAllFrame = 0.0f;

	//�ǃL�b�N���[�V�������擾
	MotionSet aMotionSet = GetMotionSet(static_cast<int>(STATE::WALL_KICK));

	//�L�[�Z�b�g�̃T�C�Y���擾
	int nKeysetSize = aMotionSet.aKeyset.size();

	//�e�L�[�Z�b�g�̃t���[�����̍��v�����߂�
	for (int nCnt = 0; nCnt < nKeysetSize - 1; nCnt++)
	{
		fAllFrame += static_cast<float>(aMotionSet.aKeyset.at(nCnt).nMaxFrame + 1);
	}

	//�ǃL�b�N�̉�]���x��ݒ�
	m_fWallKickTurn = (m_fTargetRot_Y - GetRot().y) / fAllFrame;
}

#ifdef _DEBUG
//===========================================================================================================
// �f�o�b�O�p����
//===========================================================================================================

//���[�v����
void CPlayer::Warp()
{
	//�C���X�^���X�̎擾
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();//�L�[�{�[�h
	CScene* pScene = CManager::GetInstance()->GetScene();//�V�[��
	CCamera* pCamera = CManager::GetInstance()->GetCamera();//�J����

	//Ctrl + Shift + B
	if (pKeyboard->GetPress(DIK_LCONTROL) && 
		pKeyboard->GetPress(DIK_LSHIFT) && 
		pKeyboard->GetPress(DIK_B) && 
		pScene->GetMode() == CScene::MODE::MODE_GAME)
	{
		//�{�X�Ŕ̏����擾
		CSignBoard* pBossSignBoard = pScene->GetSignBoardManager()->Find(CSignBoard::TYPE::GUIDANCE_BOSS);

		//�|�C���^��nullptr�Ȃ珈���𔲂���
		if (pBossSignBoard == nullptr) return;
		
		//�{�X�ŔɃ��[�v
		D3DXVECTOR3 PlayerPos(-1600.0f, pBossSignBoard->GetPos().y, pBossSignBoard->GetPos().z);
		SetPos(PlayerPos);

		//�J�����̌����𒲐�
		pCamera->SetRot({ 0.0f, D3DX_PI * 0.5f, 0.0f });
	}

	//Ctrl + Shift + M
	else if (pKeyboard->GetPress(DIK_LCONTROL) && 
		pKeyboard->GetPress(DIK_LSHIFT) && 
		pKeyboard->GetPress(DIK_M) && 
		pScene->GetMode() == CScene::MODE::MODE_GAME)
	{
		//�~�j�Q�[���G���A�̏����擾
		CArea* pMiniGameArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::MINI_GAME);

		//�|�C���^��nullptr�Ȃ珈���𔲂���
		if (pMiniGameArea == nullptr) return;

		//�~�j�Q�[���G���A�Ƀ��[�v
		SetPos(pMiniGameArea->GetPos());

		//�J�����̌����𒲐�
		pCamera->SetRot({ 0.0f, D3DX_PI, 0.0f });
	}

	//Ctrl + Shift + C
	if (pKeyboard->GetPress(DIK_LCONTROL) &&
		pKeyboard->GetPress(DIK_LSHIFT) &&
		pKeyboard->GetPress(DIK_C) &&
		pScene->GetMode() == CScene::MODE::MODE_GAME)
	{
		//�{�[���X�|�i�[������
		CGimmick* pGimmick = pScene->GetGimmickManager()->Find(CGimmick::TYPE::BALL_SPAWNER);

		//�������ʂ����v
		if (pGimmick != nullptr && pGimmick->GetType() == CGimmick::TYPE::BALL_SPAWNER)
		{
			//�_�E���L���X�g
			CBallSpawner* pBallSpawner = CObject::DownCast<CBallSpawner, CGimmick>(pGimmick);

			//�_�E���L���X�g���������ꍇ�X�|�i�[�Ƀ��[�v
			if (pBallSpawner != nullptr) SetPos(pBallSpawner->GetPos());

			//�J�����̌����𒲐�
			pCamera->SetRot({ 0.0f, D3DX_PI * 0.5f, 0.0f });
		}
	}
}

#endif //_DEBUG
