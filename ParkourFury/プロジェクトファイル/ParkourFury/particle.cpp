//==============================================================================================================================================
//
// �p�[�e�B�N���Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "particle.h"
#include "normalparticle.h"
#include "ascendparticle.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CParticle::CParticle(int nPriority) : CBillboardEffect{ nPriority },
	m_MaxSize( 0.0f, 0.0f, 0.0f ),
	m_type(TYPE::NONE),
	m_nLife(10),
	m_nMaxLife(m_nLife)
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CParticle::~CParticle()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CParticle::Init()
{
	//���N���X����������
	if (FAILED(CBillboardEffect::Init())) return E_FAIL;
	
	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CParticle::Uninit()
{
	//���N���X�I������
	CBillboardEffect::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CParticle::Release()
{
	//���N���X�������
	CBillboardEffect::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CParticle::Update()
{
	//���W�X�V
	SetPos(GetPos() + GetMove());

	//��������
	m_nLife--;

	//������0
	if (m_nLife <= 0)
	{
		//�I������
		Uninit();
	}

	//�����ɍ��킹�Ĕ�������
	D3DXVECTOR3 size = GetSize();
	size.x = m_MaxSize.x * (static_cast<float>(m_nLife) / static_cast<float>(m_nMaxLife));
	size.y = m_MaxSize.y * (static_cast<float>(m_nLife) / static_cast<float>(m_nMaxLife));
	SetSize(size);

	//���N���X�X�V����
	CBillboardEffect::Update();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CParticle::Draw()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	D3DXMATRIX mtxTrans;//�v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxView;//�r���[�}�g���b�N�X�擾�p
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//�e�N�X�`�����ւ̃|�C���^
	D3DXMATRIX mtxWorld;//���[���h�}�g���b�N�X���
	D3DXVECTOR3 pos = GetPos();//���W���

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	//�r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//�|���S�����J�����ɑ΂��Đ��ʂɌ�����
	D3DXMatrixInverse(&mtxWorld, nullptr, &mtxView);//�t�s������߂�
	mtxWorld._41 = 0.0f;
	mtxWorld._42 = 0.0f;
	mtxWorld._43 = 0.0f;

	//���W�𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//���C�g�𖳌���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//Z�o�b�t�@�������ݖ�����
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//�A���t�@�e�X�g�L����
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	//�A���t�@�u�����f�B���O�����Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture);

	//�|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//�A���t�@�u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//���C�g��L����
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//Z�o�b�t�@�������ݗL����
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//�A���t�@�e�X�g������
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

//===========================================================================================================
// ��������
//===========================================================================================================
CParticle* CParticle::Create(TYPE type, const D3DXVECTOR3& pos, const D3DXVECTOR3& size, int nLife, const D3DXCOLOR& col)
{
	//���[�J���ϐ��錾
	CParticle* pParticle = nullptr;//�p�[�e�B�N�����̃|�C���^

	switch (type)
	{
		//�m�[�}��
	case TYPE::NORMAL:
		pParticle = NEW CNormalParticle();
		break;

		//�㏸
	case TYPE::ASCEND:
		pParticle = NEW CAscendParticle();
		break;

	default:
		break;
	}

	//�^�[�Q�b�g��񂪑��݂���ꍇ
	if (pParticle != nullptr)
	{
		//�p�����[�^�ݒ�
		pParticle->SetType(CBillboardEffect::TYPE::PARTICLE);//�r���{�[�h�^�C�v
		pParticle->SetPos(pos);//���W
		pParticle->SetColor(col);//�J���[
		pParticle->SetSize(size);//�T�C�Y
		pParticle->m_MaxSize = size;//�ő�T�C�Y
		pParticle->m_nLife = nLife;//���C�t
		pParticle->m_nMaxLife = nLife;//�ő僉�C�t
		pParticle->m_type = type;//�p�[�e�B�N���^�C�v

		//�ړ��ʐݒ�
		pParticle->SetMoveValue();

		//����������
		pParticle->Init();
	}

	return pParticle;
}

