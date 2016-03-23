#include "DXUT.h"
#include "DXUT/DXUTcamera.h"
#include "Instance.h"
#include "InstanceNode.h"
#include "ModelFiles.h"

extern CBaseCamera*            g_pCamera;              // The camera

////////////////////////////////////////////////////////////////////////////


InstanceNode::InstanceNode(IDirect3DDevice9* pd3dDevice) : 
	TransNode(pd3dDevice)
,	m_pInstance(0)
,	m_fLastTime(0.f)
,	m_fTime(0.f)
{
}


InstanceNode::~InstanceNode()
{
	Unload();
}


HRESULT InstanceNode::GetInstance(const LPCWSTR szFilename)
{
	assert(0);							// not implemented; do not call! 
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


void InstanceNode::Update(double fTime)
{
	m_fLastTime = m_fTime;
	m_fTime = fTime;
    m_pInstance->AdvanceTime( m_fTime - m_fLastTime, g_pCamera->GetEyePt() );
}


void InstanceNode::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
	// Concatenate out matrix with matWorld
	D3DXMatrixMultiply(&matWorld, &m_mat, &matWorld);

    m_pInstance->Draw(pd3dDevice, matWorld);

	// Call base class
	Node::Render(pd3dDevice, matWorld);
}
