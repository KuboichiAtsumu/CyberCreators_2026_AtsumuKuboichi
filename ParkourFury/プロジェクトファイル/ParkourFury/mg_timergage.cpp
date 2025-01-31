//==============================================================================================================================================
//
// �~�j�Q�[���^�C�}�[�Q�[�W�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "mg_timergage.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CMG_TimerGage::CMG_TimerGage() : CUI()
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CMG_TimerGage::~CMG_TimerGage()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CMG_TimerGage::Init()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	VERTEX_2D* pVtx; //���_���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//���_�o�b�t�@�ւ̃|�C���^

	//�e�N�X�`������
	CTexture* pTex = CTexture::GetInstance();//�擾
	BindTexture(pTex->GetAddress(CTag::TAG::UI, static_cast<int>(GetUIType())));//�ݒ�

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

	//���W�ݒ�
	D3DXVECTOR2 pos(GetPos());

	//�T�C�Y�ݒ�
	D3DXVECTOR2 size(GetSize());
	m_MaxSize = size;

	//���_���W�̐ݒ�
	pVtx[0].pos.x = pos.x;
	pVtx[0].pos.y = pos.y - size.y * 0.5f;
	pVtx[1].pos.x = pos.x + size.x;
	pVtx[1].pos.y = pos.y - size.y * 0.5f;
	pVtx[2].pos.x = pos.x;
	pVtx[2].pos.y = pos.y + size.y * 0.5f;
	pVtx[3].pos.x = pos.x + size.x;
	pVtx[3].pos.y = pos.y + size.y * 0.5f;

	//�e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//�J���[�擾
	D3DXCOLOR col = GetColor();

	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		//rhw�̐ݒ�
		pVtx[nCntVtx].rhw = 1.0f;

		//���_�J���[��ԐF�ɐݒ�
		pVtx[nCntVtx].col = col;
	}

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();

	//�G�f�B�b�g�\�t���O
	SetEditFlag(false);

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
void CMG_TimerGage::Uninit()
{
	//���N���X�I������
	CUI::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
#include "game.h"
#include "minigame.h"
void CMG_TimerGage::Update()
{
	//�Q�[���V�[���̃C���X�^���X�擾
	CGame* pGame = CGame::GetInstance();

	//�~�j�Q�[�����ł͂Ȃ��ꍇ
	if (pGame != nullptr &&
		pGame->GetArea() != CGame::GAME_AREA::MINI_GAME)
	{
		//�I������
		Uninit();

		//�����𔲂���
		return;
	}

	//���N���X�X�V����
	CUI::Update();

	//�v���C���[�I�u�W�F�N�g������
	CObject* pFindObj = CObject::FindObject(Category::EVENT);
	CMiniGame* pMiniGame = nullptr;

	//�������ʂ���v
	if (pFindObj != nullptr)
	{
		//�v���C���[�N���X�Ƀ_�E���L���X�g
		pMiniGame = CObject::DownCast<CMiniGame, CObject>(pFindObj);
	}
	else
	{
		return;
	}

	//�T�C�Y���擾
	D3DXVECTOR2 size = GetSize();

	//�T�C�Y�����Z����
	size.x -= m_MaxSize.x / static_cast<float>(pMiniGame->TIMER) / MAX_FRAME;

	//�T�C�Y��ݒ�
	SetSize(size);

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	VERTEX_2D* pVtx;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���W�����擾
	D3DXVECTOR2 pos = GetPos();

	//���_���W�̐ݒ�
	pVtx[0].pos.x = pos.x;
	pVtx[0].pos.y = pos.y - size.y * 0.5f;
	pVtx[1].pos.x = pos.x + size.x;
	pVtx[1].pos.y = pos.y - size.y * 0.5f;
	pVtx[2].pos.x = pos.x;
	pVtx[2].pos.y = pos.y + size.y * 0.5f;
	pVtx[3].pos.x = pos.x + size.x;
	pVtx[3].pos.y = pos.y + size.y * 0.5f;

	//�e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = { 0.0f, 0.0f };
	pVtx[1].tex = { size.x / m_MaxSize.x, 0.0f };
	pVtx[2].tex = { 0.0f, 1.0f };
	pVtx[3].tex = { size.x / m_MaxSize.x, 1.0f };

	//���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CMG_TimerGage::Draw()
{
	//���N���X�`�揈��
	CUI::Draw();
}
