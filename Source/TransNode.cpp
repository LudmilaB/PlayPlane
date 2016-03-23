#include "DXUT.h"
#include "TransNode.h"

////////////////////////////////////////////////////////////////////////////


TransNode::TransNode(IDirect3DDevice9* pd3dDevice) :
	Node(pd3dDevice)
{
	D3DXMatrixIdentity(&m_mat);
}


TransNode::~TransNode()
{
}


void TransNode::Update(double fTime)
{
	// Call base class
	Node::Update(fTime);
}


void TransNode::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
	// Concatenate out matrix with matWorld
	D3DXMatrixMultiply(&matWorld, &m_mat, &matWorld);

	// Call base class
	Node::Render(pd3dDevice, matWorld);
}

////////////////////////////////////////////////////////////////////////////
