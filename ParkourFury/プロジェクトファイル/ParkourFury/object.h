//==============================================================================================================================================
//
// �I�u�W�F�N�g�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECT_H_ //���̃}�N����`������Ȃ�������
#define _OBJECT_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �t�@�C���C���N���[�h
//===========================================================================================================
#include "manager.h"
#include "texture.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CObject
{
public:
	//======================================
	// �񋓌^��`
	//======================================
	
	//�J�e�S���[
	enum class Category
	{
		NONE = 0,
		BG,//�w�i
		GIMMICK,//�M�~�b�N
		BULLET,//�e
		FIELD,//�t�B�[���h
		EDIT,//�G�f�B�^�I�u�W�F�N�g
		BOSS,//�{�X
		BLOCK,//�u���b�N
		PLAYER,//�v���C���[
		ENEMY,//�G�l�~�[
		AREA,//�G���A
		BOSS_ATTACK,//�{�X�U��
		BACK_3D,//3D�w�i
		SIGN_BOARD,//�Ŕ�
		EVENT,//�C�x���g
		EFFECT,//�G�t�F�N�g
		UI,//UI
		EFFECT_2D,//2D�G�t�F�N�g
		MANAGER,//�}�l�[�W���[
		MAX_CATEGORY
	};

	//======================================
	// �֐�
	//======================================
	CObject(int nPriority = 3);//�R���X�g���N�^
	virtual ~CObject();//�f�X�g���N�^
	virtual HRESULT Init();//����������
	virtual void Uninit() = 0;//�I������
	virtual void Release();//�������
	virtual void Update() = 0;//�X�V����
	virtual void Draw() = 0;//�`�揈��
	static void UninitAll();//�S�I�u�W�F�N�g�I������
	static void ReleaseAll();//�S�I�u�W�F�N�g�������
	static void UpdateAll();//�S�I�u�W�F�N�g�X�V����
	static void DrawAll();//�S�I�u�W�F�N�g�`�揈��
	static CObject* FindObject(Category category);//�I�u�W�F�N�g��������
	static void CorrectionRot(float& fRot);//�p�x�␳����
	static bool MovingJudgeXZ(const D3DXVECTOR3& move);//�����Ă��锻�菈��
	template <typename T1, typename T2> static T1* DownCast(T2* pBase);//�_�E���L���X�g����

	//���_�o�b�t�@
	void SetVtxBuff(LPDIRECT3DVERTEXBUFFER9 pVtxBuff) { m_pVtxBuff = pVtxBuff; }//�ݒ�
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff() { return m_pVtxBuff; }//�擾
	
	//�e�N�X�`�����
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }//�ݒ�
	LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }//�擾

	//�폜�t���O
	void SetDeleteFlag(bool bDelete) { m_bDelete = bDelete; }//�ݒ�
	bool GetDeleteFlag() { return m_bDelete; }//�擾
	
	//�擪�I�u�W�F�N�g
	static CObject* GetTop(int nPriority) { return m_pTop[nPriority]; }//�擾

	//���̃I�u�W�F�N�g���
	CObject* GetNext() { return m_pNext; }//�擾

	//�J�e�S���[
	Category GetCategory() { return m_category; }//�擾

private:
	//======================================
	// �ϐ�
	//======================================
	static CObject* m_pTop[static_cast<int>(Category::MAX_CATEGORY)];//�擪�I�u�W�F�N�g�̃|�C���^
	static CObject* m_pCur[static_cast<int>(Category::MAX_CATEGORY)];//�I�[�I�u�W�F�N�g�̃|�C���^
	CObject* m_pPrev;//�O�̃I�u�W�F�N�g�̃|�C���^
	CObject* m_pNext;//���̃I�u�W�F�N�g�̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;//���_�o�b�t�@
	LPDIRECT3DTEXTURE9 m_pTexture;//�e�N�X�`�����ւ̃|�C���^
	Category m_category;//���
	int m_nPriority;//�`��D��x��
	bool m_bDelete;//�폜�t���O
};

//===========================================================================================================
// �_�E���L���X�g����
//===========================================================================================================
template<typename T1, typename T2> inline T1* CObject::DownCast(T2* pBase)
{
	//�����̃I�u�W�F�N�g���w�肳�ꂽ�N���X�Ƀ_�E���L���X�g
	T1* pObj = dynamic_cast<T1*>(pBase) ;

	//�_�E���L���X�g���s
	if (!pObj)
	{
		assert(false);
	}

	return pObj;
}

#endif
