#include "DXUT.h"
//#include "Game.h"
#include "TeapotNode.h"

////////////////////////////////////////////////////////////////////////////


TeapotNode::TeapotNode(IDirect3DDevice9* pd3dDevice) : 
	Node(pd3dDevice)
,	pMesh(0)
{
	Reload(pd3dDevice);
}


TeapotNode::~TeapotNode()
{
	Unload();
}

void TeapotNode::Reload(IDirect3DDevice9* pd3dDevice)
{
	Unload();
	D3DXCreateTeapot(pd3dDevice, &pMesh, NULL);

	// Call base class
	Node::Reload(pd3dDevice);
}

void TeapotNode::Unload()
{
	if (pMesh)
	{
		pMesh->Release();
		pMesh = 0;
	}
}

void
TeapotNode::Update(double /* fTime */)
{
}


void
TeapotNode::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	pMesh->DrawSubset(0);

	// Call base class
	Node::Render(pd3dDevice, matWorld);
}
