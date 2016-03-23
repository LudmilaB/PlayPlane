#pragma once

////////////////////////////////////////////////////////////////////////////


class Decal
{
public:
	Decal(IDirect3DDevice9* pd3dDevice,FLOAT Scale = 1, FLOAT X = 0, FLOAT Y = 0, FLOAT Z =0);
	~Decal();

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice);

	D3DXMATRIX	m_matWorld;          //World 3D position  of WorldSquare

private:

	// A structure for our custom vertex type
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z;      // The untransformed, 3D position for the vertex
		DWORD color;        // The vertex color
		FLOAT   tu, tv;    // The texture coordinates
	};
	

	LPDIRECT3DVERTEXBUFFER9 m_pVB; // Buffer to hold Vertices
	LPDIRECT3DINDEXBUFFER9 m_pIB;  // Buffer to hold indices
	IDirect3DStateBlock9*	m_pSaveStateBlock;
	IDirect3DTexture9*		m_pTexture;
};

////////////////////////////////////////////////////////////////////////////
