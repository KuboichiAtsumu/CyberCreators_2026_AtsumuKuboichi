//==============================================================================================================================================
//
// ���[�V�����Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _MOTION_H_ //���̃}�N����`������Ȃ�������
#define _MOTION_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"//X�t�@�C���I�u�W�F�N�g

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CMotionCharacter : public CObjectX
{
public:
	//=============================================================
	// �񋓌^��`
	//=============================================================
	
	//���[�V�����^�C�v
	enum class TYPE
	{
		PLAYER,//�v���C���[
		BOSS,//�{�X
		MAX_TYPE
	};

	//�p�[�c���g
	enum class HALF_BODY
	{
		UP_BODY,//�㔼�g
		DOWN_BODY,//�����g
	};

	//=============================================================
	// �\����
	//=============================================================
	
	//�L�[���
	typedef struct
	{
		D3DXVECTOR3 pos;//���W
		D3DXVECTOR3 rot;//�p�x
	}Key;

	//�L�[�Z�b�g���
	typedef struct
	{
		std::vector<Key> aKey;//�L�[�̏��̍\����
		int nMaxFrame;//�ő�t���[����
	}KeySet;

	//���[�V�������
	typedef struct
	{
		std::vector<KeySet> aKeyset;//�p�[�c�̏��
		TYPE type;//���[�V�����̑ΏۃI�u�W�F�N�g�^�C�v
		int nCntKeySet;//�L�[���J�E���^
		int nCntFrame;//�t���[�����J�E���^
		int nMaxKeySet;//�e���[�V�����̍ő�L�[�Z�b�g��
		int nLoop;//�e���[�V���������[�v���邩�ǂ���
	}MotionSet;

	//�p�[�c���
	typedef struct
	{
		CXfile::ModelInfo m_aModelInfo;//�e���f�����
		D3DXMATRIX mtxWorld;//���[���h�}�g���b�N�X
		D3DXVECTOR3 pos;//���W
		D3DXVECTOR3 FirstPos;//�����ʒu
		D3DXVECTOR3 WorldPos;//���[���h���W
		D3DXVECTOR3 rot;//�p�x
		HALF_BODY Half;//���g�̔���
		int nIndex;//�ԍ�
		int nParent;//�e�̔ԍ�
		std::string aPartsFile;//�p�[�c�̃t�@�C����
	}ModelParts;

	//=============================================================
	// �֐�
	//=============================================================
	CMotionCharacter(int nPriority);//�R���X�g���N�^
	~CMotionCharacter() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Release() override;//�������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	void DrawOutLine(int nParts, D3DXMATRIX mtxWorld);//�A�E�g���C���`�揈��
	virtual void SetMotionState(int nState);//���[�V������Ԑݒ菈��
	virtual void SetPartsState();//�p�[�c���Ƃ̏�Ԑݒ菈��
	void Motion(int nState, int nParts);//���[�V��������
	void LoadParts();//�p�[�c�t�@�C���Ǎ�����
	void LoadMotion();//���[�V�������Ǎ�����

	//���[�V�������
	void SetMotionSet(int nIdx, MotionSet motionset) { m_aMotion.at(nIdx) = motionset; }//�ݒ�
	MotionSet GetMotionSet(int nIdx) { return m_aMotion.at(nIdx); }//�擾

	//���[�V�����t���O
	void SetMotionFlag(bool bMotion) { m_bMotion = bMotion; }//�ݒ�
	bool GetMotionFlag() { return m_bMotion; }//�擾

	//�p�[�c���
	void SetLocalPos(int nIdx, D3DXVECTOR3 pos) { m_aParts.at(nIdx).pos = pos; }//���[�J�����W�ݒ�
	void SetWorldPos(int nIdx, D3DXVECTOR3 WorldPos) { m_aParts.at(nIdx).WorldPos = WorldPos; };//���[���h���W
	void SetPartsInfo(int nIdx, ModelParts aParts) { m_aParts.at(nIdx) = aParts; }//�ݒ�
	void SetPartsFlash(float a);//�J���[���l�ݒ�
	void SetAllPartsSize();//�S�p�[�c�����킹���T�C�Y
	ModelParts GetPartsInfo(int nIdx) { return m_aParts.at(nIdx); }//�擾
	int GetPartsSize() { return m_aParts.size(); }//�p�[�c�̐�

	//�^�C�v
	void SetCharacterType(TYPE type) { m_Type = type; }//�ݒ�

	//���S���W
	D3DXVECTOR3& GetCenterPos() { return m_CenterPos; }//�擾

	//���
	int GetMotionState() { return m_nState; }//�擾

	//���x
	float GetSpeed() { return m_fSpeed; }//�擾

	//�W�����v��
	float GetJump() { return m_fJump; }//�擾

	//���n����
	void SetLandingFlag(bool bLanding) { m_bLanding = bLanding; }//�ݒ�
	bool GetLandingFlag() { return m_bLanding; }//�擾

private:
	//==========================
	// �^�C�v���Ƃ̏��t�@�C��
	//==========================
	static constexpr const char* PARTS_FILE[static_cast<int>(TYPE::MAX_TYPE)] =
	{
		"data/MODEL/PLAYER/SetPlayer.txt",
		"data/MODEL/BOSS/SetBoss.txt",
	};

	//=============================================================
	// �֐�
	//=============================================================
	void SetPartsInfo(CXfile::ModelInfo aModelInfo, int nParts);//�p�[�c���f�����ݒ菈��
	void CorrectionPos();//���W�̕␳

	//=============================================================
	// �萔
	//=============================================================
	const float MAX_JUDGE_MOVE = 0.01f;//�ړ���Ԕ���̈ړ��ʔ͈�
	const float OUTLINE_SCALE = 1.1f;//�A�E�g���C���̊g�嗦(����)

	//=============================================================
	// �ϐ�
	//=============================================================
	std::vector<MotionSet> m_aMotion;//���[�V�������
	std::vector<ModelParts> m_aParts;//�e�p�[�c���
	D3DXVECTOR3 m_CenterPos;//���S���W
	TYPE m_Type;//�^�C�v
	int m_nState;//���݂̃��[�V����
	float m_fSpeed;//���x
	float m_fJump;//�W�����v��
	bool m_bLanding;//���n����
	bool m_bMotion;//���[�V�������邩�ǂ����̔���
};

#endif
