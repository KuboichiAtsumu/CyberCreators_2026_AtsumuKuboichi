//==============================================================================================================================================
//
// �t�F�[�h�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "fade.h"
#include "manager.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CFade::CFade()
{
	//�����o�ϐ�������
	m_state = CFade::STATE::FADE_NONE;//���݂̏��
	m_NextMode = CScene::MODE::MODE_TITLE;//���̃��[�h
	m_size = { SCREEN_WIDTH, SCREEN_HEIGHT };//�T�C�Y
	m_pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };//���W
	m_rot = { 0.0f, 0.0f, 0.0f };//�p�x
	m_col = { 1.0f, 1.0f, 1.0f, 0.0f };//�J���[
	m_fLength = sqrtf(powf(m_size.x, 2.0f) + powf(m_size.y, 2.0f)) * 0.5f;//�Ίp���̒���
	m_fAngle = atan2f(m_size.x, m_size.y);//�Ίp���̊p�x
	m_pVtxBuff = nullptr;//���_�o�b�t�@
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CFade::~CFade()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CFade::Init()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	VERTEX_2D* pVtx; //���_���ւ̃|�C���^
	
	//���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���_���W�̐ݒ�
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;

	//�e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		//���_���W��Z����0.0f�ŌŒ�
		pVtx[nCntVtx].pos.z = 0.0f;

		//rhw�̐ݒ�
		pVtx[nCntVtx].rhw = 1.0f;

		//���_�J���[�̐ݒ�
		pVtx[nCntVtx].col = m_col;
	}

	//���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CFade::Uninit()
{
	//���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CFade::Update()
{
	//���[�J���ϐ��錾
	VERTEX_2D* pVtx;//���_���ւ̃|�C���^

	//�t�F�[�h�C��
	if (m_state == CFade::STATE::FADE_IN)
	{
		//�|���S���𓧖��ɂ��Ă���
		m_col.a -= FADE_SPEED;

		//���l��0.0f�ȉ��̏ꍇ
		if (m_col.a <= 0.0f)
		{
			//0.0f�ȉ��ɂȂ�Ȃ��悤�ɒ���
			m_col.a = 0.0f;

			//�t�F�[�h���Ȃ���Ԃɂ���
			m_state = CFade::STATE::FADE_NONE;
		}
	}

	//�t�F�[�h�A�E�g
	else if (m_state == CFade::STATE::FADE_OUT)
	{
		//�|���S����s�����ɂ��Ă���
		m_col.a += FADE_SPEED;

		//���l��1.0f�ȏ�̏ꍇ
		if (m_col.a >= 1.0f)
		{
			//1.0f�ȏ�ɂȂ�Ȃ��悤�ɒ���
			m_col.a = 1.0f;

			//�t�F�[�h�C����Ԃɂ���
			m_state = CFade::STATE::FADE_IN;

			//���̉�ʂֈڍs
			CManager::GetInstance()->SetMode(m_NextMode);
		}
	}

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���_�J���[�̐ݒ�
	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		pVtx[nCntVtx].col = m_col;
	}

	//���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CFade::Draw()
{
	//�t�F�[�h���Ă��Ȃ��ꍇ
	if (m_state == STATE::FADE_NONE)
	{
		//�`�悹���ɏ����𔲂���
		return;
	}

	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�ւ̃|�C���^

	//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, nullptr);

	//�|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//===========================================================================================================
// ��������
//===========================================================================================================
CFade* CFade::Create()
{
	//�������𓮓I�m��
	CFade* pFade = NEW CFade();

	//����������
	pFade->Init();

	return pFade;
}

//===========================================================================================================
// ��Ԑݒ菈��
//===========================================================================================================
void CFade::SetFade(CScene::MODE mode)
{
	//���̃V�[����ݒ�
	m_NextMode = mode;

	//�t�F�[�h�A�E�g��Ԃɂ���
	m_state = CFade::STATE::FADE_OUT;
}
