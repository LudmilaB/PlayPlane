//-----------------------------------------------------------------------------
// File: MeshFile.h
//
// Desc: Header file for the CMeshFile class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#ifndef MESHFILE_H
#define MESHFILE_H

#include "ModelFile.h"

class CMeshFile : public IModelFile
{
public:
	CMeshFile();
	~CMeshFile();

    HRESULT             Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename);
    void                Unload();

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);

private:
	ID3DXMesh*			m_pMesh;
	D3DMATERIAL9*       m_pMeshMaterials;		// Materials for our mesh
	LPDIRECT3DTEXTURE9* m_pMeshTextures;		// Textures for our mesh
	DWORD               m_dwNumMaterials;		// Number of mesh materials
};

#endif // #ifndef MESHFILE_H
