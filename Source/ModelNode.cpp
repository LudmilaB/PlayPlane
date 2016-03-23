#include "DXUT.h"
//#include "Game.h"
#include "ModelNode.h"
#include "ModelFiles.h"

extern CModelFiles				g_ModelFiles;			// Holds all the models 

////////////////////////////////////////////////////////////////////////////


ModelNode::ModelNode(IDirect3DDevice9* pd3dDevice) : 
	TransNode(pd3dDevice)
,	m_iModel(-1)
{
}


ModelNode::~ModelNode()
{
	Unload();
}

// Get instance of specific file. 

// We now let ModelFiles do the work and keep a copy of the model. 

HRESULT ModelNode::GetInstance(const LPCWSTR szFilename)
{
	m_iModel = g_ModelFiles.GetInstance(szFilename);
	if (m_iModel < 0)
	{
		return DXUTERR_MEDIANOTFOUND;
	}

	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


void ModelNode::Update(double /* fTime */)
{
}


void ModelNode::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
	// Concatenate out matrix with matWorld
	D3DXMatrixMultiply(&matWorld, &m_mat, &matWorld);

	if (m_iModel >=0)
	{
		IModelFile* modelFile = g_ModelFiles.PIModelFileFromIModelFile(m_iModel);
		if (modelFile)
		{
			modelFile->Render(pd3dDevice, matWorld);
		}
	}

	// Call base class
	Node::Render(pd3dDevice, matWorld);
}
