//==============================================================================================================================================
//
// �J�����Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _CAMERA_H_ //���̃}�N����`������Ȃ�������
#define _CAMERA_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "main.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CCamera
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//���
	enum class STATE
	{
		NONE = 0,//��~
		LEFTTURN,//����]
		RIGHTTURN,//�E��]
		LOOP_TURN,//���[�v�^�[��
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CCamera();//�R���X�g���N�^
	~CCamera();//�f�X�g���N�^
	HRESULT Init();//����������
	void Uninit();//�I������
	void Update();//�X�V����
	void RotationStage();//�X�e�[�W�J������]����
	void SetCamera();//�J�����̐ݒ�

	//���_
	void SetPosV(const D3DXVECTOR3& pos) { m_posV = pos; }//�ݒ�
	D3DXVECTOR3& GetPosV() { return m_posV; }//�擾

	//�����_
	void SetPosR(const D3DXVECTOR3& pos) { m_posR = pos; }//�ݒ�
	D3DXVECTOR3& GetPosR() { return m_posR; }//�擾

	//�p�x
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }//�ݒ�
	D3DXVECTOR3& GetRot() { return m_rot; }//�擾

	//���
	void SetState(STATE state);//�ݒ�

	//�J�������[�N���
	void SetCameraWork(const D3DXVECTOR3& TargetPos_V, const D3DXVECTOR3& TargetPos_R, int nMoveFrame);//�ݒ�

	//���݂̈ړ��t���[��
	int GetMoveFrame() { return m_nMoveFrame; }//�擾

	//�h��̏��
	void SetShake(int nFrame, int nShake)
	{
		m_nShakeFrame = nFrame;//�h���t���[����
		m_nMaxNumShake = nShake;//�U��̗�
	}

	//���Z�b�g
	void Reset();

	//�萔�擾
	float GetBossMinDistance() { return BOSS_MIN_DISTANCE; }//�{�X��J���������ŏ��l
	float GetBossMaxDistance() { return BOSS_MAX_DISTANCE; }//�{�X��J���������ő�l

	//======================================
	// �萔
	//======================================
	static inline const D3DXVECTOR3 FirstPosV = { 0.0f, 50.0f, -500.0f };

private:
	//======================================
	// �֐�
	//======================================
	void Turn();//��]����
	void Move();//�ړ�����
	void CameraWork();//�J�������[�N����
	void SetPosRPlayer();//�v���C���[�ɒ����_��ݒ肷�鏈��

	//======================================
	// �萔
	//======================================
	const D3DXVECTOR3 VecU = { 0.0f, 1.0f, 0.0f };
	const int MOVE_FRAME = 90;//�ړ��t���[��
	const float ADJUST_POS_SPEED = 10.0f;//�J�������W�������x
	const float MAX_ROLL = 0.05f;//��]���x
	const float MAX_STAGEROTATION = 0.1f;//�X�e�[�W�J������]���x
	const float TITLE_POSV_Y = 1000.0f;//�^�C�g�����̍���
	const float TITLE_DISTANCE = 3000.0f;//�^�C�g�����̎��_���璍���_�܂ł̋���
	const float TITLE_TURN_SPEED = 0.005f;//�^�C�g����ʉ�]���x
	const float BOSS_MIN_DISTANCE = 650.0f;//�{�X��J���������ŏ��l
	const float BOSS_MAX_DISTANCE = 1000.0f;//�{�X��J���������ő�l
 
	//======================================
	// �ϐ�
	//======================================
	D3DXVECTOR3 m_posV;//���_
	D3DXVECTOR3 m_posR;//�����_
	D3DXVECTOR3 m_rot;//�p�x
	D3DXVECTOR3 m_TargetRot;//�ڕW�̊p�x
	D3DXVECTOR3 m_move_V;//���_�ړ���
	D3DXVECTOR3 m_move_R;//�����_�ړ���
	D3DXMATRIX m_mtxProjection;//�v���W�F�N�V�����s��
	D3DXMATRIX m_mtxView;//�r���[�s��
	STATE m_State;//���
	int m_nShakeFrame;//�h���t���[����
	int m_nMaxNumShake;//�h��̗ʂ̍ő�l
	int m_nMoveFrame;//�ړ��t���[��
	float m_fLength;//����
	float m_fDistanceMag;//���_�ƒ����_�̋����{��
	float m_fAddHeight;//���_�̍������Z��
	float m_fMoveRotY;//��]��
	bool m_bWork;//�J�������[�N�t���O
	bool m_bSetMove_V;//���_�ړ��ʐݒ�t���O
	bool m_bSetMove_R;//�����_�ړ��ʐݒ�t���O
};

#endif
