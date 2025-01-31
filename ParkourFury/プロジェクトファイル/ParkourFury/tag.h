//==============================================================================================================================================
//
// X�t�@�C���E�e�N�X�`���^�O
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _TAG_H_ //���̃}�N����`������Ȃ�������
#define _TAG_H_ //2�d�C���N���[�h�h�~�̃}�N����`

class CTag
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�^�O
	enum class TAG
	{
		PLAYER = 0,//�v���C���[
		BLOCK,//�u���b�N
		GIMMICK,//�M�~�b�N
		BULLET,//�e
		ENEMY,//�G�l�~�[
		BACK_3D,//3D�w�i
		BOSS,//�{�X
		AREA,//�G���A
		BG,//�w�i
		EFFECT,//�G�t�F�N�g
		BOSS_ATTACK,//�{�X�U��
		UI,//UI
		SIGN_BOARD,//�Ŕ�
		EFFECT_2D,//2D�G�t�F�N�g
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CTag();
	~CTag();
	void SetTag(TAG& tag, std::string str);
	void SetNumType(int nNumType, TAG tag) { m_nNumType[static_cast<int>(tag)] = nNumType; }
	int GetNumType(TAG tag) { return m_nNumType[static_cast<int>(tag)]; }

private:
	//======================================
	// �ϐ�
	//======================================
	int m_nNumType[static_cast<int>(TAG::MAX)];
};

#endif