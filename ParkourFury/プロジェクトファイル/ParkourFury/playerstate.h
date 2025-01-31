//==============================================================================================================================================
//
// �v���C���[�̏�ԂɊւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _PLAYERSTATE_H_ //���̃}�N����`������Ȃ�������
#define _PLAYERSTATE_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �N���X��`
//===========================================================================================================

//���N���X
class CPlayerState
{
public:
	CPlayerState() {};
	~CPlayerState() {};
};

//�j���[�g����
class CNeutral : public CPlayerState
{
public:
	CNeutral() {};
	~CNeutral() {};
};

//�n��ړ�
class CLandingMove : public CPlayerState
{
public:
	CLandingMove() {};
	~CLandingMove() {};
};

//��i�W�����v
class CJump : public CPlayerState
{
public:
	CJump() {};
	~CJump() {};
};

//��i�W�����v
class CDoubleJump : public CPlayerState
{
public:
	CDoubleJump() {};
	~CDoubleJump() {};
};

//����
class CFall : public CPlayerState
{
public:
	CFall() {};
	~CFall() {};
};

//�ǂ���
class CWallSlide : public CPlayerState
{
public:
	CWallSlide() {};
	~CWallSlide() {};
};

//�ǃL�b�N
class CWallKick : public CPlayerState
{
public:
	CWallKick() {};
	~CWallKick() {};
};

//�_�b�V��
class CDash : public CPlayerState
{
public:
	CDash() {};
	~CDash() {};
};

//�R�͂܂�
class CCliffGrab : public CPlayerState
{
public:
	CCliffGrab() {};
	~CCliffGrab() {};
};

#endif