#pragma once
#include "Node.h"

////////////////////////////////////////////////////////////////////////////


class TeapotNode : public Node
{
public:
	TeapotNode(IDirect3DDevice9* pd3dDevice);
	~TeapotNode();

    void                Reload(IDirect3DDevice9* pd3dDevice);
    void                Unload();

						// Update traversal for physics, AI, etc.
	virtual void		Update(double fTime);

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);

private:
	ID3DXMesh*			pMesh;
};

////////////////////////////////////////////////////////////////////////////
