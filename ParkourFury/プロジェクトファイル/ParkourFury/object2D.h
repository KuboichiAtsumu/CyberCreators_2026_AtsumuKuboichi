#pragma once
//==============================================================================================================================================
//
// 2D�I�u�W�F�N�g�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECT2D_H_ //���̃}�N����`������Ȃ�������
#define _OBJECT2D_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "object.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CObject2D : public CObject
{
public:
	//======================================
	// �֐�
	//======================================
	CObject2D(int nPriority = 3);//�R���X�g���N�^
	~CObject2D() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void UpdateAnim();//�A�j���[�V��������
	void Draw() override;//�`�揈��

	//���W
	void SetPos(const D3DXVECTOR2& pos) { m_pos = pos; }//�ݒ�
	D3DXVECTOR2& GetPos() { return m_pos; }//�擾

	//�T�C�Y
	void SetSize(const D3DXVECTOR2& size) { m_size = size; }//�ݒ�
	D3DXVECTOR2& GetSize() { return m_size; }//�擾

	//�ړ���
	void SetMove(const D3DXVECTOR2& move) { m_move = move; }//�ݒ�
	D3DXVECTOR2& GetMove() { return m_move; }//�擾

	//�p�x
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }//�ݒ�
	D3DXVECTOR3& GetRot() { return m_rot; }//�擾

	//�J���[
	void SetColor(const D3DXCOLOR& col) { m_col = col; }//�ݒ�
	D3DXCOLOR& GetColor() { return m_col; }//�擾

	//���̃e�N�X�`��������
	void SetDivisionTex_X(int nDivisionTex) { m_nDivisionWidth = nDivisionTex; }//�ݒ�

	//���̃A�j���[�V�����p�^�[��
	void SetPatternAnim_X(int nPatternAnim) { m_nPatternAnimWidth = nPatternAnim; }//�ݒ�

private:
	//======================================
	// �ϐ�
	//======================================
	D3DXVECTOR2 m_pos;//���W
	D3DXVECTOR2 m_size;//�T�C�Y
	D3DXVECTOR2 m_move;//�ړ���
	D3DXVECTOR3 m_rot;//�p�x
	D3DXCOLOR m_col;//�J���[
	int m_nDivisionWidth;//���̃e�N�X�`��������
	int m_nDivisionHeight;//���̃e�N�X�`��������
	int m_nDelayAnim;//�A�j���[�V�����x��
	int m_nPatternAnimWidth;//���̃A�j���[�V�����p�^�[��
	int m_nPatternAnimHeight;//�c�̃A�j���[�V�����p�^�[��
	int m_nCounterAnim;//�A�j���[�V�����J�E���^
	float m_fAngle;//�Ίp���̊p�x
	float m_fLength;//�Ίp���̒���
};

#endif