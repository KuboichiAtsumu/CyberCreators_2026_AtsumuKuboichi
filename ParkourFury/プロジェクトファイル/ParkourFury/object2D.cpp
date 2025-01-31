//==============================================================================================================================================
//
// 2D�I�u�W�F�N�g�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "object2D.h"//2D�I�u�W�F�N�g

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	//�����o�ϐ�������
	m_pos = { 0.0f, 0.0f };//���W
	m_size = { 0.0f, 0.0f };//�T�C�Y
	m_move = { 0.0f, 0.0f };//�ړ���
	m_rot = { 0.0f, 0.0f, 0.0f };//�p�x
	m_col = { 1.0f, 1.0f, 1.0f, 1.0f };//�F

	//�e�N�X�`���A�j���[�V����
	m_nDivisionWidth = 1;//���̃e�N�X�`��������
	m_nDivisionHeight = 1;//�c�̃e�N�X�`��������
	m_nDelayAnim = 1;//�A�j���[�V�����x��
	m_nPatternAnimWidth = 1;//���̃A�j���[�V�����p�^�[��
	m_nPatternAnimHeight = 1;//�c�̃A�j���[�V�����p�^�[��
	m_nCounterAnim = 0;//�A�j���[�V�����J�E���^

	m_fAngle = 0.0f;//�Ίp���̊p�x
	m_fLength = 0.0f;//�Ίp���̒���
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CObject2D::~CObject2D()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CObject2D::Init()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	VERTEX_2D* pVtx; //���_���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^

	//�����o�ϐ�������
	m_fLength = sqrtf(powf(m_size.x, 2.0f) + powf(m_size.y, 2.0f)) * 0.5f;//�Ίp���̒���
	m_fAngle = atan2f(m_size.x, m_size.y);//�Ίp���̊p�x

	//���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&pVtxBuff,
		nullptr);

	//���_�o�b�t�@�̐ݒ�
	SetVtxBuff(pVtxBuff);

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���_���W�̐ݒ�
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;

	//�������ۑ�
	float fDivisionWidth = static_cast<float>(m_nDivisionWidth);
	float fDivisionHeight = static_cast<float>(m_nDivisionHeight);

	//�e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / fDivisionWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / fDivisionHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f / fDivisionWidth, 1.0f / fDivisionHeight);

	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		//rhw�̐ݒ�
		pVtx[nCntVtx].rhw = 1.0f;

		//���_�J���[�̐ݒ�
		pVtx[nCntVtx].col = m_col;
	}

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();

	//���N���X����������
	if (FAILED(CObject::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CObject2D::Uninit()
{
	//�폜�t���O��true�ɐݒ�
	SetDeleteFlag(true);
}

//===========================================================================================================
// �������
//===========================================================================================================
void CObject2D::Release()
{
	//���N���X�������
	CObject::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CObject2D::Update()
{
	//���[�J���ϐ��錾
	VERTEX_2D* pVtx;//���_���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^

	//�Ίp��
	m_fLength = sqrtf(powf(m_size.x, 2.0f) + powf(m_size.y, 2.0f)) * 0.5f;//����
	m_fAngle = atan2f(m_size.x, m_size.y);//�p�x

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���_���W�̐ݒ�
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;

	//���[�J���ϐ��ɏ���ۑ�
	float fDivisionWidth = static_cast<float>(m_nDivisionWidth);//���̃e�N�X�`��������
	float fDivisionHeight = static_cast<float>(m_nDivisionHeight);//�c�̃e�N�X�`��������
	float fPatternAnimWidth = static_cast<float>(m_nPatternAnimWidth);//���̃A�j���[�V�����p�^�[��
	float fPatternAnimHeight = static_cast<float>(m_nPatternAnimHeight);//�c�̃A�j���[�V�����p�^�[��

	//�e�N�X�`�����W�̐ݒ�
	pVtx[0].tex.x = 1.0f / fDivisionWidth * fPatternAnimWidth;
	pVtx[1].tex.x = 1.0f / fDivisionWidth * (fPatternAnimWidth + 1.0f);
	pVtx[2].tex.x = 1.0f / fDivisionWidth * fPatternAnimWidth;
	pVtx[3].tex.x = 1.0f / fDivisionWidth * (fPatternAnimWidth + 1.0f);

	pVtx[0].tex.y = 1.0f / fDivisionHeight * (fPatternAnimHeight - 1.0f);
	pVtx[1].tex.y = 1.0f / fDivisionHeight * (fPatternAnimHeight - 1.0f);
	pVtx[2].tex.y = 1.0f / fDivisionHeight * fPatternAnimHeight;
	pVtx[3].tex.y = 1.0f / fDivisionHeight * fPatternAnimHeight;

	//���_�J���[�̐ݒ�
	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		pVtx[nCntVtx].col = m_col;
	}

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();
}

//===========================================================================================================
// �A�j���[�V��������
//===========================================================================================================
void CObject2D::UpdateAnim()
{
	//���[�J���ϐ��錾
	VERTEX_2D* pVtx;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//�A�j���[�V�����J�E���^�����Z
	m_nCounterAnim++;

	//�e�N�X�`���X�V����
	if (m_nCounterAnim % m_nDelayAnim == 0)
	{
		//���̃A�j���[�V�����p�^�[�������Z
		m_nPatternAnimWidth++;

		//���̃e�N�X�`���p�^�[�����ő�܂ŒB�����ꍇ
		if (m_nPatternAnimWidth > m_nDivisionWidth)
		{
			//���̃A�j���[�V�����p�^�[�������Z�b�g
			m_nPatternAnimWidth = 1;

			//�c�̃A�j���[�V�����p�^�[�������Z
			m_nPatternAnimHeight++;

			//�c�̃A�j���[�V����
			if (m_nPatternAnimHeight > m_nDivisionHeight)
			{
				//�c�̃A�j���[�V�����p�^�[�������Z�b�g
				m_nPatternAnimHeight = 1;
			}
		}

		//�A�j���[�V�����J�E���^�����Z�b�g
		m_nCounterAnim = 0;
	}

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CObject2D::Draw()
{
	//�G�f�B�^���[�h�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//�`�悹���ɏ����𔲂���
		return;
	}

	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//�e�N�X�`�����ւ̃|�C���^

	//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_2D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture);

	//�|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
