//==============================================================================================================================================
//
// X�t�@�C���I�u�W�F�N�g�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CObjectX::CObjectX(int nPriority) : CObject3D(nPriority),
	m_pEffect(nullptr),
	m_vtxMin( 0.0f, 0.0f, 0.0f ),
	m_vtxMax( 0.0f, 0.0f, 0.0f ),
	m_Size( 0.0f, 0.0f, 0.0f ),
	m_Scale( 1.0f, 1.0f, 1.0f ),
	m_OutLineScale( 1.1f, 1.1f, 1.1f, 0.0f ),
	m_OutLineColor( 1.0f, 1.0f, 1.0f, 1.0f ),
	m_fLength(0.0f),
	m_fAngle(0.0f),
	m_StandLine(STAND_LINE::XY)
{
	//���f����񏉊���
	m_aModelInfo.pMesh = nullptr;
	m_aModelInfo.pBuffMat = nullptr;
	m_aModelInfo.dwNumMat = 0;
	m_aModelInfo.Diffuse.clear();
	m_aModelInfo.FirstDiffuse.clear();
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CObjectX::~CObjectX()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CObjectX::Init()
{
	//�I�u�W�F�N�g����������
	if (FAILED(CObject::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CObjectX::Uninit()
{
	//3D�I�u�W�F�N�g�I������
	CObject3D::Uninit();
}

//===========================================================================================================
// �������
//===========================================================================================================
void CObjectX::Release()
{
	//���f����񏉊���
	m_aModelInfo.pMesh = nullptr;
	m_aModelInfo.pBuffMat = nullptr;

	//�G�t�F�N�g�������
	m_pEffect = nullptr;

	//3D�I�u�W�F�N�g�������
	CObject3D::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CObjectX::Update()
{
	//�p�x�ɍ��킹�čő�E�ŏ��̒��_���W���X�V
	m_vtxMin.x = GetPos().x + sinf(GetRot().y - (D3DX_PI - m_fAngle)) * m_fLength;//X���W�̍ŏ��l
	m_vtxMin.z = GetPos().z + cosf(GetRot().y + m_fAngle) * m_fLength;//Z���W�̍ŏ��l
	m_vtxMax.x = GetPos().x + sinf(GetRot().y + m_fAngle) * m_fLength;//X���W�̍ő�l
	m_vtxMax.z = GetPos().z + cosf(GetRot().y - (D3DX_PI - m_fAngle)) * m_fLength;//Z���W�̍ő�l

	//�X�V���ꂽ���_���W�ɉ����ăT�C�Y���X�V
	m_Size =
	{
		abs(m_vtxMax.x - m_vtxMin.x),//x��
		abs(m_vtxMax.y - m_vtxMin.y) * m_Scale.y,//y��
		abs(m_vtxMax.z - m_vtxMin.z)//z��
	};

	//���W�̐���̐ݒ�
	SetStandLine();
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CObjectX::Draw()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	D3DXMATRIX mtxWorld;//���[���h�}�g���b�N�X���
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;//�v�Z�p�}�g���b�N�X
	D3DMATERIAL9 matDef;//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;//�}�e���A���̃f�[�^�ւ̃|�C���^
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//�e�N�X�`�����
	D3DXVECTOR3 Pos = GetPos();//���W���
	D3DXVECTOR3 Rot = GetRot();//�p�x���

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	//�V�F�[�_�[��񂪑��݂���
	if (m_pEffect != nullptr)
	{
		//�V�F�[�_�[�̊g�嗦��ݒ�
		m_pEffect->SetFloat("Scale_X", m_Scale.x);
		m_pEffect->SetFloat("Scale_Y", m_Scale.y);
		m_pEffect->SetFloat("Scale_Z", m_Scale.z);

		//�A�E�g���C���̑�����ݒ�
		m_pEffect->SetFloat("OutlineThickness_X", m_OutLineScale.x);
		m_pEffect->SetFloat("OutlineThickness_Y", m_OutLineScale.y);
		m_pEffect->SetFloat("OutlineThickness_Z", m_OutLineScale.z);

		//�A�E�g���C���̃J���[��ݒ�
		D3DXVECTOR4 Color = static_cast<D3DXVECTOR4>(m_OutLineColor);
		m_pEffect->SetVector("OutLineColor", &Color);
	}
	else
	{
		//�g�嗦�𔽉f
		D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);
	}

	//�p�x�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, Rot.y, Rot.x, Rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//���W�𔽉f
	D3DXMatrixTranslation(&mtxTrans, Pos.x, Pos.y, Pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//�@���̒�����1�ɂ���B
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	//�V�F�[�_�[�p�����[�^�ݒ�
	SetShader(mtxWorld);

	if (m_aModelInfo.pBuffMat != nullptr)
	{
		//�}�e���A���f�[�^�ւ̃|�C���^���擾
		pMat = (D3DXMATERIAL*)m_aModelInfo.pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)m_aModelInfo.dwNumMat; nCntMat++)
		{
			//�F�����ݒ�
			pMat->MatD3D.Diffuse = m_aModelInfo.Diffuse[nCntMat];

			//�}�e���A���̐ݒ�
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, pTexture);

			//�V�F�[�_�[��񂪑��݂���
			if(m_pEffect != nullptr)
			{
				//�V�F�[�_�[�̕`����J�n
				UINT numPasses = 0;
				m_pEffect->Begin(&numPasses, 0);

				for (UINT Cnt = 0; Cnt < numPasses; Cnt++)
				{
					m_pEffect->BeginPass(Cnt);

					//���f���̕`��
					m_aModelInfo.pMesh->DrawSubset(nCntMat);

					m_pEffect->EndPass();
				}

				//�V�F�[�_�[�̕`����I��
				m_pEffect->End();
			}
			//�V�F�[�_�[��񂪑��݂��Ȃ�
			else
			{
				//���f���̕`��
				m_aModelInfo.pMesh->DrawSubset(nCntMat);
			}
		}
	}

	//�ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);
}

//===========================================================================================================
// �V�F�[�_�[�p�����[�^�ݒ�
//===========================================================================================================
void CObjectX::SetShader(D3DXMATRIX mtxWorld)
{
	//�V�F�[�_�[��񂪑��݂��Ȃ�
	if (m_pEffect == nullptr)
	{
		return;
	}

	//���[�J���ϐ��錾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[�C���X�^���X�擾
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	D3DXMATRIX mtxView, mtxProj, mtxWorldViewProj;//�v�Z�p�}�g���b�N�X

	/*pDevice->GetTransform(D3DTS_WORLD, &mtxWorld);*/
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);

	D3DXMatrixMultiply(&mtxWorldViewProj, &mtxWorld, &mtxView);
	D3DXMatrixMultiply(&mtxWorldViewProj, &mtxWorldViewProj, &mtxProj);

	//�V�F�[�_�[�ɍs���ݒ�
	m_pEffect->SetMatrix("WorldViewProj", &mtxWorldViewProj);
	m_pEffect->SetMatrix("World", &mtxWorld);

	//���C�g�̃x�N�g����ݒ�
	D3DXVECTOR3 lightDir(0.8f, 0.5f, -1.0f);
	D3DXVec3Normalize(&lightDir, &lightDir);
	m_pEffect->SetValue("LightDirection", &lightDir, sizeof(D3DXVECTOR3));
}

//===========================================================================================================
// �����Ă������̐ݒ菈��
//===========================================================================================================
void CObjectX::SetStandLine()
{
	//���W�����擾
	D3DXVECTOR3 pos = GetPos();

	//Z���̐���ɂ���ꍇ
	if (pos.x > CManager::VTXMIN_FIELD.x && pos.x < CManager::VTXMAX_FIELD.x)
	{
		//�����Ă�������XY�ɐݒ�
		m_StandLine = STAND_LINE::XY;
	}
	//X���̐���ɂ���ꍇ
	else if (pos.z > CManager::VTXMIN_FIELD.z && pos.z < CManager::VTXMAX_FIELD.z)
	{
		//�����Ă�������YZ�ɐݒ�
		m_StandLine = STAND_LINE::YZ;
	}
}

//===========================================================================================================
// ���f���o�^
//===========================================================================================================
void CObjectX::SetSize()
{
	//���[�J���ϐ��錾
	BYTE* pVtxBuff;//���_�o�b�t�@�ւ̃|�C���^

	//���_���̎擾
	int nNumVtx = m_aModelInfo.pMesh->GetNumVertices();

	//���_�t�H�[�}�b�g�̃T�C�Y���擾
	DWORD sizeFVF = D3DXGetFVFVertexSize(m_aModelInfo.pMesh->GetFVF());

	//���_�o�b�t�@�̃��b�N
	m_aModelInfo.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nIdxVtx = 0; nIdxVtx < nNumVtx; nIdxVtx++)
	{
		//���_���W�̑��
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

		//���f���̍ŏ��l�ƍő�l���擾
		if (vtx.x < m_vtxMin.x)
		{//X���W�̍ŏ��l
			m_vtxMin.x = vtx.x;
		}
		if (vtx.y < m_vtxMin.y)
		{//Y���W�̍ŏ��l
			m_vtxMin.y = vtx.y;
		}
		if (vtx.z < m_vtxMin.z)
		{//Z���W�̍ŏ��l
			m_vtxMin.z = vtx.z;
		}
		if (vtx.x > m_vtxMax.x)
		{//X���W�̍ő�l
			m_vtxMax.x = vtx.x;
		}
		if (vtx.y > m_vtxMax.y)
		{//Y���W�̍ő�l
			m_vtxMax.y = vtx.y;
		}
		if (vtx.z > m_vtxMax.z)
		{//Z���W�̍ő�l
			m_vtxMax.z = vtx.z;
		}

		//���_�t�H�[�}�b�g�̃T�C�Y���|�C���^��i�߂�
		pVtxBuff += sizeFVF;
	}

	//��ނ��Ƃ̃T�C�Y�����߂�
	m_Size =
	{
		(m_vtxMax.x - m_vtxMin.x) * m_Scale.x,//x��
		(m_vtxMax.y - m_vtxMin.y) * m_Scale.y,//y��
		(m_vtxMax.z - m_vtxMin.z) * m_Scale.z//z��
	};

	//�Ίp���̒��������߂�
	m_fLength = sqrtf(powf(m_Size.x, 2.0f) + powf(m_Size.z, 2.0f)) * 0.5f;

	//�Ίp���̊p�x�����߂�
	m_fAngle = atan2f(m_Size.x, m_Size.z);

	//���_�o�b�t�@�̃A�����b�N
	m_aModelInfo.pMesh->UnlockVertexBuffer();
}
