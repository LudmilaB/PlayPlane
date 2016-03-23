#pragma once
#include "Node.h"

#define D3DFVF_MINIMAPVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_MINIMAPVER (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

class MinimapSquare : public Node 
{
public:
	MinimapSquare():Node(0){};
	MinimapSquare(IDirect3DDevice9* pd3dDevice, DWORD Color, FLOAT X = 0, FLOAT Y = 0, FLOAT Side =5,LPDIRECT3DTEXTURE9 pTexture = NULL);
	~MinimapSquare();

// Our custom FVF, which describes our custom vertex structure
    virtual void Render(IDirect3DDevice9* pd3dDevice,  D3DXMATRIX matWorld);

	private:

	// A structure for our custom vertex type
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z, rhw;      // The transformed, 3D position for the vertex
		DWORD color;        // The vertex color
		FLOAT   tu, tv;    // The texture coordinates
	};

	LPDIRECT3DVERTEXBUFFER9 m_pVB; // Buffer to hold Vertices

	LPDIRECT3DTEXTURE9 m_pTexture;

};

