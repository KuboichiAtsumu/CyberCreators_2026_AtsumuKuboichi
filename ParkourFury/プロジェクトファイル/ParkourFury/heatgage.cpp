//==============================================================================================================================================
//
// �q�[�g�Q�[�W�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "heatgage.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CHeatGage::CHeatGage() : CUI()
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CHeatGage::~CHeatGage()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CHeatGage::Init()
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

	//�Q�[����
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::GAME) SetSize({ 0.0f, size.y });

	//���_���W�̐ݒ�
	pVtx[0].pos.x = pos.x;
	pVtx[0].pos.y = pos.y - size.y * 0.5f;
	pVtx[1].pos.x = pos.x;
	pVtx[1].pos.y = pos.y - size.y * 0.5f;
	pVtx[2].pos.x = pos.x;
	pVtx[2].pos.y = pos.y + size.y * 0.5f;
	pVtx[3].pos.x = pos.x;
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

	//���N���X����������
	if (FAILED(CObject::Init())) return E_FAIL;

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CHeatGage::Uninit()
{
	//���N���X�I������
	CUI::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
#include "player.h"
void CHeatGage::Update()
{
	//���N���X�X�V����
	CUI::Update();

	//�v���C���[�I�u�W�F�N�g������
	CObject* pFindObj = CObject::FindObject(Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//�I�u�W�F�N�g��񂪑��݂���ꍇ
	if (pFindObj != nullptr)
	{
		//�v���C���[�N���X�Ƀ_�E���L���X�g
		pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

		//�g��E�k������
		Scaling(pPlayer->GetHeat());
	}
}

//===========================================================================================================
// �g��E�k������
//===========================================================================================================
void CHeatGage::Scaling(float fHeat)
{
	//�G�f�B�^����p�t���O
	static bool bEdit;

	//�T�C�Y���擾
	D3DXVECTOR2 size = GetSize();

	//���W�����擾
	D3DXVECTOR2 pos = GetPos();

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	VERTEX_2D* pVtx;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//�G�f�B�^���̓T�C�Y�̍ő�l���X�V
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::GAME)
	{
		//�G�f�B�^���[�h�ɂȂ������ɃT�C�Y�ő�l�ɖ߂�
		if (!bEdit)
		{
			size = m_MaxSize;
			SetSize(size);
			bEdit = true;
		}

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
		pVtx[1].tex = { 1.0f, 0.0f };
		pVtx[2].tex = { 0.0f, 1.0f };
		pVtx[3].tex = { 1.0f, 1.0f };

		return;
	}

	//�G�f�B�^����
	bEdit = false;

	//�ڕW�̃T�C�Y
	D3DXVECTOR2 TargetSize(0.0f, size.y);

	//���݂̃q�[�g�ʂ̊��������߂�
	float fRatioHeat = fHeat / CPlayer::MAX_HEAT;

	//������1.0�𒴂��Ȃ��悤�ɒ���
	if (fRatioHeat > 1.0f)
	{
		fRatioHeat = 1.0f;
	}

	//�q�[�g�ʂɍ��킹�ĖڕW�̃T�C�Y��ݒ�
	TargetSize.x = m_MaxSize.x * fRatioHeat;

	//�ڕW�̃T�C�Y�����݂̃T�C�Y���傫���ꍇ
	if (TargetSize.x - size.x > 0.0f)
	{
		//�T�C�Y�����Z����
		size.x += GAGE_MOVE_SPEED;

		//�ڕW�T�C�Y�𒴂����ꍇ
		if (TargetSize.x - size.x < 0.0f)
		{
			//�ڕW�̃T�C�Y�ɒ���
			size.x = TargetSize.x;
		}
	}
	//�ڕW�̃T�C�Y���݂̃T�C�Y��菬�����ꍇ
	else if (TargetSize.x - size.x < 0.0f)
	{
		//�T�C�Y�����Z����
		size.x -= 0.7f;

		//�ڕW�T�C�Y�𒴂����ꍇ
		if (TargetSize.x - size.x > 0.0f)
		{
			//�ڕW�̃T�C�Y�ɒ���
			size.x = TargetSize.x;
		}
	}

	//�T�C�Y��ݒ�
	SetSize(size);

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
void CHeatGage::Draw()
{
	//���N���X�`�揈��
	CUI::Draw();
}
