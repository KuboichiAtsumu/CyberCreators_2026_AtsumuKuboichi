//==============================================================================================================================================
//
// �{�X�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BOSS_H_ //���̃}�N����`������Ȃ�������
#define _BOSS_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "motion.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CBoss : public CMotionCharacter
{
public:
	//======================================
	// �\���̒�`
	//======================================

	//�����蔻��p�p�����[�^
	struct CollisionParam
	{
		D3DXVECTOR3 pos;//���W
		D3DXVECTOR3 size;//�T�C�Y
	};

	//======================================
	// �֐�
	//======================================
	CBoss(int nPriority = static_cast<int>(Category::BOSS));//�R���X�g���N�^
	~CBoss() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CBoss* Create(D3DXVECTOR3 pos);//��������
	bool Collision(CollisionParam& aParam);//�����蔻��
	void DecreaseLife();//���C�t��������

	//HP
	int GetHP() { return m_nLife; }//���݂�HP
	int GetMaxHP() { return MAX_LIFE; }//�ő�HP

private:
	//======================================
	// �񋓌^��`
	//======================================

	//���
	enum class STATE
	{
		NEUTRAL = 0,//�ʏ�
		ATTACK,//�U��
		MAX
	};

	//�U���^�C�v
	enum class ATTACK_TYPE
	{
		BULLET_DEPTH = 0,//�e(��)
		BULLET_WIDTH,//�e(��)
		BLOCK,//�u���b�N����
		MAX
	};

	//�U���p�^�[��
	enum class ATTACK_PATTERN
	{
		BULLET_DEPTH = 0,//�e(��)
		BULLET_WIDTH,//�e(��)
		WALL_BLOCK,//�ǐ���
		FORTH_BLOCK,//�u���b�N����
		BULLET_AREA,//�e(����̈ʒu)
		MAX
	};

	//======================================
	// �\���̒�`
	//======================================

	//�U�����
	struct AttackInfo
	{
		ATTACK_TYPE Type;//�^�C�v
		int nCreate;//������
		std::vector<D3DXVECTOR3> pos;//���W
		std::vector<D3DXVECTOR3> rot;//�p�x
		std::vector<D3DXVECTOR3> scale;//�g�嗦
	};

	//======================================
	// �֐�
	//======================================
	void LoadAttackPattern();//�U���p�^�[���Ǎ�����
	bool Collision_X(CollisionParam aParam);//X���̓����蔻��
	bool Collision_Y(CollisionParam aParam);//Y���̓����蔻��
	bool Collision_Z(CollisionParam aParam);//Z���̓����蔻��
	void SetPosition();//�����ʒu�̐ݒ菈��
	void Gravity();//�d�͏���
	void Move();//�ړ�����
	void Attack();//�U������
	void SetAttackPattern();//�U���p�^�[���ݒ菈��
	void BulletDepth();//�e(��)�̍U������
	void BulletWidth();//�e(��)�̍U������
	void BlockCreate();//�u���b�N�����U������
	void CollisionBlock();//�u���b�N�Ƃ̓����蔻��
	void AngleModifying();//�p�x�C������
	void SetCameraWorkInfo();//�J�������[�N���ݒ菈��

	//======================================
	// �萔
	//======================================
	const int SETPOS_TIME = 240;//�����ʒu�ύX�̊Ԋu
	const int ATTACK_TIME = 90;//�U���Ԋu
	const int MAX_LIFE = 20;//�ő僉�C�t
	const int NUM_ATTACK = 5;//�U���̐�
	const float GRAVITY_POWER = 0.5f;//�d�͂̋���
	const float MAX_GRAVITY = -10.0f;//�d�͂̍ő勭�x
	const float BULLET_RANGE = 200.0f;//�e�̍U�������͈̔�(�v���C���[�����S)
	const float BLOCK_RANGE_Y = 200.0f;//�u���b�N����Y���͈�
	const float BLOCK_RANGE_Z = 400.0f;//�u���b�N����Z���͈�
	const float MAX_BLOCK_SCALE = 2.0f;//�����u���b�N�̍ő�g�嗦

	//======================================
	// �ϐ�
	//======================================

	//�U���֌W
	std::vector<AttackInfo> m_AttackInfo;//�U���̏��
	int m_nAttackPattern;//�U���p�^�[��

	//�p�����[�^
	STATE m_State;//���
	int m_nLife;//���C�t
	int m_nSetCoolTime;//�����ʒu�ύX�܂ł̃N�[���^�C��
	int m_nAttackCT;//�U���܂ł̃N�[���^�C��
	int m_nIdxPosition;//���݂���|�C���g�̃C���f�b�N�X
	int m_nOldPosition;//�O�̃|�C���g�̃C���f�b�N�X
	float m_fInertia;//�ړ��ʌ�����
	bool m_bAttack;//�U���t���O
};

#endif
