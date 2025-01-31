//==============================================================================================================================================
//
// 3D�I�u�W�F�N�g�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECT3D_H_ //���̃}�N����`������Ȃ�������
#define _OBJECT3D_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "object.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CObject3D : public CObject
{
public:
	//======================================
	// �֐�
	//======================================
	CObject3D(int nPriority = 3);//�R���X�g���N�^
	~CObject3D() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void UpdateAnim();//�e�N�X�`���A�j���[�V��������
	void Draw() override;//�`�揈��

	//���W
	void SetPos(const D3DXVECTOR3& pos) { m_pos = pos; }//�ݒ�
	D3DXVECTOR3& GetPos() { return m_pos; }//�擾

	//�X�V�O�̍��W
	void SetOldPos(const D3DXVECTOR3& oldpos) { m_oldpos = oldpos; }//�ݒ�
	D3DXVECTOR3& GetOldPos() { return m_oldpos; }//�擾

	//�ړ���
	void SetMove(const D3DXVECTOR3& move) { m_move = move; }//�ݒ�
	D3DXVECTOR3& GetMove() { return m_move; }//�擾

	//�p�x
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }//�ݒ�
	D3DXVECTOR3& GetRot() { return m_rot; }//�擾

	//�T�C�Y
	void SetSize(const D3DXVECTOR3& size) { m_size = size; }//�ݒ�
	D3DXVECTOR3& GetSize() { return m_size; }//�擾

	//�J���[
	void SetColor(const D3DXCOLOR& col) { m_col = col; }//�ݒ�
	D3DXCOLOR& GetColor() { return m_col; }//�擾

	//�e�N�X�`��������
	void SetDivisionTex(int nWidth, int nHeight)//�ݒ�
	{
		m_nDivisionWidth = nWidth;//���̕�����
		m_nDivisionHeight = nHeight;//�c�̕�����
	}
	int GetDivisionWidth() { return m_nDivisionWidth; }//���̎擾
	int GetDivisionHeight() { return m_nDivisionHeight; }//�c�̎擾

	//�A�j���[�V�����p�^�[��
	void SetDelayAnim(int nDelayAnim) { m_nDelayAnim = nDelayAnim; }//�A�j���[�V�����x���ݒ�
	int GetPatternAnimWidth() { return m_nPatternAnimWidth; }//���̃p�^�[���̎擾
	int GetPatternAnimHeight() { return m_nPatternAnimHeight; }//�c�̃p�^�[���̎擾

private:
	//======================================
	// �ϐ�
	//======================================
	D3DXVECTOR3 m_pos;//���W
	D3DXVECTOR3 m_oldpos;//�ߋ��̍��W
	D3DXVECTOR3 m_move;//�ړ���
	D3DXVECTOR3 m_rot;//�p�x
	D3DXVECTOR3 m_size;//�T�C�Y
	D3DXCOLOR m_col;//�J���[
	int m_nDivisionWidth;//���̃e�N�X�`��������
	int m_nDivisionHeight;//���̃e�N�X�`��������
	int m_nDelayAnim;//�A�j���[�V�����x��
	int m_nPatternAnimWidth;//���̃A�j���[�V�����p�^�[��
	int m_nPatternAnimHeight;//�c�̃A�j���[�V�����p�^�[��
	int m_nCounterAnim;//�A�j���[�V�����J�E���^
};

#endif