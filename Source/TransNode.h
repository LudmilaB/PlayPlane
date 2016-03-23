#pragma once
#include "Node.h"

////////////////////////////////////////////////////////////////////////////


class TransNode : public Node
{
public:
						TransNode(IDirect3DDevice9* pd3dDevice);
	virtual				~TransNode();

						// Update traversal for physics, AI, etc.
	virtual void		Update(double fTime);

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);

	D3DXMATRIX*			GetMatrix()			{ return &m_mat; }

protected:
	D3DXMATRIX			m_mat;
};


////////////////////////////////////////////////////////////////////////////
