//-----------------------------------------------------------------------------
// File: WorldModelFile.h
//
// Desc: Header file for the CWorldModelFile class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#ifndef WORLDMODELFILE_H
#define WORLDMODELFILE_H

#include "ModelFile.h"
#include "WorldFile.h"

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z;      // The untransformed, 3D position for the vertex
	DWORD color;        // The vertex color
	FLOAT u, v;         // The texture coordinates
};

// A quad that is made up of two clockwise winding order triangles, 
// each of which is composed of three vertices. 
struct CUSTOMQUAD
{
    CUSTOMVERTEX vertex[6];
};

class CWorldModelFile : public IModelFile
{
public:
    CWorldModelFile();
    ~CWorldModelFile();

    HRESULT     Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename);
    void        Unload();

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

				// Render traversal for drawing objects
	void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);

	WorldFile&	GetWorldFile() { return *m_pWorldFile; }

private:

	WorldFile*				m_pWorldFile;

    // Two instances of everything: one for EMPTY_CELL, the other for OCCUPIED_CELL

    static CUSTOMQUAD*      m_pCellQuads[WorldFile::CELL_MAX];
    static UINT             m_cCellQuads[WorldFile::CELL_MAX];

    LPDIRECT3DVERTEXBUFFER9 m_pVB[WorldFile::CELL_MAX];         // Buffer to hold Vertices
    CUSTOMVERTEX*           m_pVertices[WorldFile::CELL_MAX];   // Triangle vertices in the world 
    UINT                    m_cVertices[WorldFile::CELL_MAX];   // Number of vertices in m_pVB 

    IDirect3DTexture9*      m_pTexture[WorldFile::CELL_MAX];    // textures

	HRESULT CWorldModelFile::LoadTextures(IDirect3DDevice9* pd3dDevice);
	void CWorldModelFile::CalculateBufferSizes();
	HRESULT CWorldModelFile::CreateAndLockBuffers(IDirect3DDevice9* pd3dDevice);
	void CWorldModelFile::FillAndUnlockBuffers(IDirect3DDevice9* pd3dDevice);

    CUSTOMVERTEX* CopyCellQuads(CUSTOMVERTEX* pVertexDest, 
                                CUSTOMQUAD* pQuadSource, UINT cQuadSource, 
                                int y, int x);

	// Default copy constructors won't work because of the m_pVB field; 
	// disable them by making them private. 
    CWorldModelFile(const CWorldModelFile&);			// no implementation 
	CWorldModelFile& operator=(const CWorldModelFile&);	// no implementation 
};

#endif // #ifndef WORLDMODELFILE_H
