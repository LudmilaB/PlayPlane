#pragma once
#include "TransNode.h"

////////////////////////////////////////////////////////////////////////////


class ModelNode : public TransNode
{
public:
	ModelNode(IDirect3DDevice9* pd3dDevice);
	~ModelNode();

    HRESULT             GetInstance(const LPCWSTR szFilename);

						// Update traversal for physics, AI, etc.
	virtual void		Update(double fTime);

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);

private:
	int					m_iModel;
};

////////////////////////////////////////////////////////////////////////////
