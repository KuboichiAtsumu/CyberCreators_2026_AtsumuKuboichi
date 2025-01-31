//==============================================================================================================================================
//
// �u���b�N�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BLOCK_H_ //���̃}�N����`������Ȃ�������
#define _BLOCK_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CBlock : public CObjectX
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�^�C�v
	enum class TYPE
	{
		NONE = 0,
		NORMAL,//�m�[�}��
		SLONE,//�⓹
		BOSS,//�{�X
		MAX
	};

	//======================================
	// �\���̒�`
	//======================================

	//�����蔻��p�p�����[�^
	struct CollisionParam
	{
		D3DXVECTOR3 pos;//���W
		D3DXVECTOR3 oldpos;//�X�V�O�̍��W
		D3DXVECTOR3 move;//�ړ���
		D3DXVECTOR3 size;//�T�C�Y
		D3DXVECTOR3 rot;//�p�x
		TYPE type;//���������u���b�N�̃^�C�v
	};

	//�����蔻��t���O
	struct CollisionFlag
	{
		bool X;//X��
		bool Y_UP;//Y����
		bool Y_DOWN;//Y����
		bool Z;//Z��
	};

	//======================================
	// �֐�
	//======================================
	CBlock(int nPriority = static_cast<int>(Category::BLOCK));//�R���X�g���N�^
	~CBlock() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CBlock* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, TYPE type);//��������
	virtual CBlock::CollisionFlag Collision(CollisionParam& aParam);//�����蔻��

	//�O�̃I�u�W�F�N�g���
	void SetPrevBlock(CBlock* pBlock) { m_pPrev = pBlock; }//�ݒ�
	CBlock* GetPrevBlock() { return m_pPrev; }//����

	//���̃I�u�W�F�N�g���
	void SetNextBlock(CBlock* pBlock) { m_pNext = pBlock; }//�ݒ�
	CBlock* GetNextBlock() { return m_pNext; }//�擾

	//�u���b�N�^�C�v
	void SetBlockType(TYPE type) { m_Type = type; }//�ݒ�
	TYPE GetBlockType() { return m_Type; }//�擾

	//�����x�N�g��
	D3DXVECTOR3 GetNormal() { return m_Normal; }//�擾
	D3DXVECTOR3 SEGMENT_XY(float StartX, float StartY, float EndX, float EndY);//XY�Ԃ̐����x�N�g���v�Z����
	D3DXVECTOR3 SEGMENT_XZ(float StartX, float StartZ, float EndX, float EndZ);//XZ�Ԃ̐����x�N�g���v�Z����
	D3DXVECTOR3 SEGMENT_YZ(float StartY, float StartZ, float EndY, float EndZ);//YZ�Ԃ̐����x�N�g���v�Z����

private:
	//======================================
	// �ϐ�
	//======================================
	CBlock* m_pPrev;//�O�̃I�u�W�F�N�g���ւ̃|�C���^
	CBlock* m_pNext;//���̃I�u�W�F�N�g���ւ̃|�C���^
	TYPE m_Type;//���
	D3DXVECTOR3 m_Normal;//�����x�N�g��
};

#endif
