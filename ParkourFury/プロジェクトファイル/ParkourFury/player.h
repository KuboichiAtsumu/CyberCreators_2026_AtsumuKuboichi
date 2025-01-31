//==============================================================================================================================================
//
// �v���C���[�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _PLAYER_H_ //���̃}�N����`������Ȃ�������
#define _PLAYER_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "motion.h"
#include "block.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CPlayer : public CMotionCharacter
{
public:
	//======================================
	// �֐�
	//======================================
	CPlayer(int nPriority = 3);//�R���X�g���N�^
	~CPlayer() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void SetMotionState(int nState) override;//���[�V������Ԑݒ菈��
	void SetPartsState() override;//�p�[�c���Ƃ̏�Ԑݒ菈��
	void Draw() override;//�`�揈��
	static CPlayer* Create(const D3DXVECTOR3& pos);//��������
	float GetHeat() { return m_fHeat; }//�q�[�g�ʎ擾

	//�L����
	void IncreaseKill() { m_nKill++; }//���Z
	int GetKill() { return m_nKill; }//�擾

	//======================================
	// �萔
	//======================================
	static constexpr float MAX_HEAT = 100.0f;//�ő�q�[�g��

private:
	//======================================
	// �񋓌^��`
	//======================================

	//�p�[�c
	enum class PARTS
	{
		CENTER = 0,//�Z���^�[
		STOMACH,//��
		BODY,//��
		FACE,//��
		ArmUR,//�E�r��
		ArmDR,//�E�r��
		HandR,//�E��
		ArmUL,//���r��
		ArmDL,//�����r
		HandL,//����
		LegUR,//�E�r��
		LegDR,//�E�r��
		FeetR,//�E��
		LegUL,//���r��
		LegDL,//���r��
		FeetL,//����
		MAX
	};

	//���
	enum class STATE
	{
		NEUTRAL = 0,//�ʏ�
		LANDING_MOVE,//�n��ړ�
		JUMP,//��i�ڃW�����v
		DOUBLE_JUMP,//��i�ڃW�����v
		FALL,//����
		WALL_SLIDE,//�ǂ���
		WALL_KICK,//�ǃL�b�N
		DASH,//�_�b�V��
		CLIFF_GRAB,//�R�͂܂�
		SHOT,//����
		SLIDING,//�X���C�f�B���O
		DAMAGE,//�_���[�W
		MAX
	};

	//�ǂ̔���
	enum class WallState
	{
		NONE = 0,
		SLIDE,//�ǂ���
		KICK,//�ǃL�b�N
		CLIFF,//�R�͂܂�
		MAX
	};

	//�G�ꂽ�ǂ̐���
	enum class WallLine
	{
		NONE = 0,
		XY,//XY����
		YZ,//YZ����
	};

	//======================================
	// �֐�
	//======================================

	//����֌W
	void Controll();//���쏈��
	void Jump(D3DXVECTOR3& move, float fDiaJump);//�W�����v
	void Move(D3DXVECTOR3& move, float fDiaSpeed, float fTargetRotY);//�ړ�����
	void Dash(float& fDiaSpeed, float& fFirstDiaSpeed);//�_�b�V������
	void Shot();//���ˏ���
	void Sliding(D3DXVECTOR3& move);//�X���C�f�B���O����
	void Landing(CBlock::CollisionParam& aParam);//���n������
	void Falling(CBlock::CollisionParam& aParam, WallState wallstate);//����������
	WallState WallKick(CBlock* pBlock, CBlock::CollisionParam& aParam);//�ǃL�b�N����
	void ReflectAngleCalculate(CBlock* pBlock, CBlock::CollisionParam& aParam, WallLine Line, WallState& wallstate);//���˃x�N�g���v�Z����

	//�p�����[�^
	void SetWallKickTurn();//�ǃL�b�N��]���x�ݒ菈��
	void IncreaseHeat(float fAdd);//�q�[�g�ʑ�������
	void DecreaseHeat();//�q�[�g�ʌ�������
	void AddCombo();//�R���{�����Z����
	void ResetCombo();//�R���{�����Z�b�g����

	//�G�t�F�N�g
	void OccurHeatEffect();//�q�[�g�G�t�F�N�g
	void OccurSmoke();//��
	void DisplayEffect2D();//2D�G�t�F�N�g�\������

	//���f�����
	void Flash();//�_�ŏ���

	//�����蔻��
	void Collision();
	void CollisionBlock();//�u���b�N�Ƃ̓����蔻��
	WallState CollisionWallKick(CBlock::CollisionParam& aParam);//�ǃL�b�N���菈��
	void CollisionEnemy();//�G�l�~�[�Ƃ̓����蔻��
	void CollisionBoss();//�{�X�Ƃ̓����蔻��
	void CollisionBossAttack();//�{�X�̍U���Ƃ̓����蔻��
	void CollisionMiniGameArea();//�~�j�Q�[���G���A
	void CollisionChaseArea();//�`�F�C�X�G���A
	void CollisionBossArea();//�{�X�G���A
	void CollisionArea();//�S�G���A

	//���X�|�[��
	void RespawnChaseArea();//�`�F�C�X�G���A���X�|�[������

	//�f�o�b�O�p����
#ifdef _DEBUG
	void Warp();//���[�v����
#endif // _DEBUG

	//======================================
	// �萔
	//======================================
	const D3DXVECTOR2 DISPLAY_EFFECT2D = { 330.0f, 550.0f };//2D�G�t�F�N�g�\�����W
	const int JUMP_TIMES = 2;//�W�����v��
	const int DASH_FRAME = 5;//�_�b�V���̃t���[��
	const int DOUBLE_PUSH = 13;//2����͎�t�t���[��
	const int RIGOR_CLIFF = 120;//�R�ɒ͂܂�鎞��
	const int POSSIBLE_FALL = 30;//�R�݂͂��瑀��\�ɂȂ�܂ł̎���
	const int SHOT_SPEED = 10;//�e�̔��ˊԊu
	const int DEC_HEAT_CT = 8;//�q�[�g�ʌ����N�[���^�C��
	const int DAMAGE_TIME = 30;//�_���[�W��Ԃ̎���
	const int COMBO_TIME = 30;//�R���{�p���t���[��
	const int NUM_CREATE_PARTICLE = 50;//�p�[�e�B�N��������
	const int MAX_COMBO = 3;//�ő�R���{��
	const int DECREASE_COMBO_FRAME = 90;//�R���{�����Z�b�g�܂ł̃t���[����
	const float DASH_SPEED = 8.0f;//�_�b�V���̑��x�{��
	const float MAX_TURN = 1.0f;//��]���x
	const float BASE_INERTIA = 0.4f;//�������x��l
	const float JUMP_INERTIA = 1.0f;//�W�����v������
	const float MAX_GRAVITY = -10.0f;//�d�͏��
	const float BULLET_SPEED = 15.0f;//�e�̑��x
	const float ACCEL_SLIDING = 10.0f;//�X���C�f�B���O�̉����x
	const float MAX_KICKPOWER_WIDTH = 8.5f;//�ǃL�b�N��X���̋����ő�l
	const float MAX_KICKPOWER_HEIGHT = 75.0f;//�ǃL�b�N��Y���̋����ő�l
	const float SLIDE_POWER = 2.0f;//�ǂ���̏d�͂̋���
	const float RANGE_EFFECT = 10.0f;//�q�[�g�G�t�F�N�g�����͈�
	const float RANGE_EFFECT2D_ROT = 0.3f;//2D�G�t�F�N�g�X���͈�

	//======================================
	// �ϐ�
	//======================================
	STATE m_State;//���
	int m_nCntState;//��ԃJ�E���^
	int m_nCntDash;//�_�b�V�����̃t���[���J�E���^
	int m_nCntDoublePush;//2����̓t���[���J�E���^
	int m_nCntJump;//�W�����v�J�E���^
	int m_nDecTenCT;//�q�[�g�ʌ����N�[���^�C��
	int m_nComboTime;//�R���{�p���t���[��
	int m_nKill;//�L����
	int m_nCombo;//�R���{��
	int m_nDecComboFrame;//�R���{�������J�E���g�t���[��
	float m_fHeat;//�q�[�g��
	float m_fInertia;//�������x
	float m_fWallKickTurn;//�ǃL�b�N�̉�]���x
	float m_fTargetRot_Y;//�ڕW�̊p�x
	bool m_bShooting;//�ˌ��t���O
	bool m_bBossCreate;//�{�X�����ς݃t���O
};

#endif