//-----------------------------------------------------------------------------
// File: MeshInstance.cpp
//
// Desc: Defines the mesh instance class. 
//       It holds an instance of a mesh to draw and a transform matrix 
//       that specifies where to draw that mesh. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#include "DXUT.h"
#include "body.h"
#include "gameobject.h"
#include "MeshInstance.h"
#include "ModelNode.h"
#include "TransNode.h"
//L
#define airplane

CMeshInstance::CMeshInstance(GameObject& owner) : 
	CInstance(owner)
{
}

CMeshInstance::~CMeshInstance()
{
}


HRESULT CMeshInstance::GetInstance(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename)
{
	Unload();

	TransNode* ptn = new TransNode(pd3dDevice);
	ModelNode* pmn = new ModelNode(pd3dDevice);
	HRESULT hr = pmn->GetInstance(szFilename);
	if (FAILED(hr))
	{
		return hr;
	}
#if 1
	// airplane 2.x is too big, so scale it down to one fifth of the original size. 
	D3DXMATRIX matScale;
//	D3DXMatrixScaling(&matScale, 0.10f, 0.10f, 0.10f);
	D3DXMatrixScaling(&matScale, m_owner->GetScale(),m_owner->GetScale(), m_owner->GetScale());
	D3DXMATRIX matTrans;
	// Y offset to make it look like it is resting on the ground. 
	D3DXMatrixTranslation(&matTrans, 0.0f, 0.10f, 0.0f);
	D3DXMatrixMultiply(pmn->GetMatrix(), &matScale, &matTrans);
#ifdef airplane
	D3DXMATRIX mx;
    D3DXMatrixRotationY( &mx, -D3DX_PI / 2.0f );
    D3DXMatrixMultiply( pmn->GetMatrix(), pmn->GetMatrix(), &mx );
#endif
#endif
	ptn->AddChild(std::tr1::shared_ptr<Node>(pmn));
	m_pNode = std::tr1::shared_ptr<Node>(ptn);

	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


void CMeshInstance::Unload()
{
	if (m_pNode)
	{
		m_pNode->Unload();
	}
}


//-----------------------------------------------------------------------------
// Name: CMeshInstance::ResetTime()
// Desc: Resets the local time for this CMeshInstance.
//-----------------------------------------------------------------------------
HRESULT CMeshInstance::ResetTime()
{
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CMeshInstance::AdvanceTime()
// Desc: Advances the local animation time by dTimeDelta. 
//-----------------------------------------------------------------------------
HRESULT CMeshInstance::AdvanceTime( double /* dTimeDelta */, const D3DXVECTOR3* /* pvEye */ )
{
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CMeshInstance::Draw()
// Desc: Draws this CMeshInstance.
//-----------------------------------------------------------------------------
HRESULT CMeshInstance::Draw( IDirect3DDevice9* /*pd3dDevice*/, D3DXMATRIX /*matWorld*/ )
{
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CMeshInstance::SetOrientation()
// Desc: Copy the orientation from Body to the Node. 
//-----------------------------------------------------------------------------
void CMeshInstance::SetOrientation()
{
	D3DXMATRIX mxWorld;
	m_owner->GetBody().GetMatrix(&mxWorld);
	TransNode* ptn = reinterpret_cast<TransNode*>(m_pNode.get());
	if (ptn)
	{
		*ptn->GetMatrix() = mxWorld;
	}
}


//-----------------------------------------------------------------------------
// Name: CMeshInstance::RestoreDeviceObjects()
// Desc: Reinitialize necessary objects.
//-----------------------------------------------------------------------------
HRESULT CMeshInstance::RestoreDeviceObjects( LPDIRECT3DDEVICE9 /* pd3dDevice */ )
{
	return S_OK;
}
