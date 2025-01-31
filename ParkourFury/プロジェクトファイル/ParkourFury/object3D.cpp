//==============================================================================================================================================
//
// 3D�I�u�W�F�N�g�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "object3D.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority),
	m_pos( 0.0f, 0.0f, 0.0f ),
	m_move( 0.0f, 0.0f, 0.0f ),
	m_rot( 0.0f, 0.0f, 0.0f ),
	m_size( 0.0f, 0.0f, 0.0f ),
	m_col( 1.0f, 1.0f, 1.0f, 1.0f ),
	m_nDivisionWidth(1),
	m_nDivisionHeight(1),
	m_nDelayAnim(1),
	m_nPatternAnimWidth(1),
	m_nPatternAnimHeight(1),
	m_nCounterAnim(0)
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CObject3D::~CObject3D()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CObject3D::Init()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	VERTEX_3D* pVtx;//���_���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^

	//���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&pVtxBuff,
		nullptr);

	//���_�o�b�t�@�̐ݒ�
	SetVtxBuff(pVtxBuff);

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���_���W�̐ݒ�
	pVtx[0].pos = { -m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f };
	pVtx[1].pos = { m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f };
	pVtx[2].pos = { -m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f };
	pVtx[3].pos = { m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f };

	//�e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = { 0.0f, 0.0f };
	pVtx[1].tex = { 1.0f / (float)m_nDivisionWidth, 0.0f };
	pVtx[2].tex = { 0.0f, 1.0f / (float)m_nDivisionHeight };
	pVtx[3].tex = { 1.0f / (float)m_nDivisionWidth, 1.0f / (float)m_nDivisionHeight };

	for (int nCntVer = 0; nCntVer < MAX_VER; nCntVer++)
	{
		//�@���x�N�g���̐ݒ�
		pVtx[nCntVer].nor = { 0.0f, 1.0f, 0.0f };

		//���_�J���[�̐ݒ�
		pVtx[nCntVer].col = m_col;
	}

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();

	//���N���X����������
	if (FAILED(CObject::Init())) return E_FAIL;
	
	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CObject3D::Uninit()
{
	//�폜�t���O��true�ɐݒ�
	SetDeleteFlag(true);
}

//===========================================================================================================
// �������
//===========================================================================================================
void CObject3D::Release()
{
	//���N���X�������
	CObject::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CObject3D::Update()
{
	//���[�J���ϐ��錾
	VERTEX_3D* pVtx; //���_���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);

	for (int nCntVer = 0; nCntVer < MAX_VER; nCntVer++)
	{
		//���_�J���[�̐ݒ�
		pVtx[nCntVer].col = m_col;
	}

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();
}

//===========================================================================================================
// �A�j���[�V��������
//===========================================================================================================
void CObject3D::UpdateAnim()
{
	//���[�J���ϐ��錾
	VERTEX_3D* pVtx;
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

		//�e�N�X�`�����W���ړ�
		pVtx[0].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)(m_nPatternAnimWidth - 1);
		pVtx[1].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)m_nPatternAnimWidth;
		pVtx[2].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)(m_nPatternAnimWidth - 1);
		pVtx[3].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)m_nPatternAnimWidth;

		pVtx[0].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)(m_nPatternAnimHeight - 1);
		pVtx[1].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)(m_nPatternAnimHeight - 1);
		pVtx[2].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)m_nPatternAnimHeight;
		pVtx[3].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)m_nPatternAnimHeight;

		//�A�j���[�V�����J�E���^�����Z�b�g
		m_nCounterAnim = 0;
	}

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CObject3D::Draw()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//�e�N�X�`�����ւ̃|�C���^
	D3DXMATRIX mtxRot, mtxTrans;//�v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxWorld;//���[���h�}�g���b�N�X

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	//�p�x�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//���W�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture);

	//�|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
