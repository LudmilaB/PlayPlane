//-----------------------------------------------------------------------------
// File: MeshFile.cpp
//
// Desc: CMeshFile holds all the data needed to draw a mesh. The data is 
//       loaded from disk. The CModelFiles class holds a vector of 
//       CMeshFile classes. A ModelNode then holds an index to that vector in 
//       CModelFiles. There can be several ModelNode classes refering 
//       to a single CMeshFile class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#include "DXUT.h"
#include "DXUT/SDKmisc.h"
#include <stdlib.h>
#include "GameError.h"
#include "MeshFile.h"

CMeshFile::CMeshFile() : 
	m_pMesh(0)
,	m_pMeshMaterials(0)
,	m_pMeshTextures(0)
,	m_dwNumMaterials(0L)
,	IModelFile()
{
}


CMeshFile::~CMeshFile()
{
	Unload();
}


/*
Load the .x mesh in szFilename into this CMeshFile. 

I had some problems with finding the textures for some of the models I was 
trying to load, so this function now uses DXUTFindDXSDKMediaFileCch() to 
search for textures. Because it does so, it seems to also make sense to 
search for the .x mesh as well. 

In short, for szFilename, you should pass in just the name of the mesh, e.g., 
"airplane 2.x", instead of a fully-qualified path, e.g., "media\\airplane 2.x". 
*/
HRESULT CMeshFile::Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename)
{
    // Clear out old data, if any. 
    // By doing so, we can repeated call CMeshFile::Load(). 
    Unload();

	WCHAR sz[MAX_PATH];
    if (FAILED(DXUTFindDXSDKMediaFileCch(sz, MAX_PATH, szFilename)))
	{
        PrintfError(L"Couldn't find file %s.", szFilename);
        return DXUTERR_MEDIANOTFOUND;
	}

    LPD3DXBUFFER pD3DXMtrlBuffer;

    // Load the mesh from the specified file
	HRESULT hr;
	hr = D3DXLoadMeshFromX( sz, D3DXMESH_SYSTEMMEM,
                                   pd3dDevice, NULL,
                                   &pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
                                   &m_pMesh );
	if (FAILED(hr))
    {
        PrintfError(L"Error loading %s.", sz);
        return hr;
    }

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = ( D3DXMATERIAL* )pD3DXMtrlBuffer->GetBufferPointer();
    m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
    if( m_pMeshMaterials == NULL )
        return E_OUTOFMEMORY;
    m_pMeshTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
    if( m_pMeshTextures == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < m_dwNumMaterials; ++i )
    {
        // Copy the material
        m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

        m_pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL &&
            lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
        {
			WCHAR wsz[MAX_PATH];
			size_t cwcs;
			mbstowcs_s(&cwcs, wsz, MAX_PATH, d3dxMaterials[i].pTextureFilename, MAX_PATH);
			if (FAILED(DXUTFindDXSDKMediaFileCch(sz, MAX_PATH, wsz)))
			{
				PrintfError(L"Couldn't find file %s.", wsz);
				return DXUTERR_MEDIANOTFOUND;
			}

            // Create the texture
			HRESULT hr = D3DXCreateTextureFromFile( pd3dDevice,
                                                    sz,
                                                    &m_pMeshTextures[i] );
            if( FAILED(hr) )
            {
				PrintfError(L"Error loading %s.", wsz);
                return hr;
            }
        }
    }

    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


//-----------------------------------------------------------------------------
// Name: Unload()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
void CMeshFile::Unload()
{
    SAFE_DELETE_ARRAY( m_pMeshMaterials );

    if ( m_pMeshTextures )
    {
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
        {
            SAFE_RELEASE( m_pMeshTextures[i] );
        }
        SAFE_DELETE_ARRAY( m_pMeshTextures );
    }
	SAFE_RELEASE(m_pMesh);
}


void CMeshFile::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
	if (m_pMesh)
	{
		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		// Turn on D3D lighting 
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

		// Meshes are divided into subsets, one for each material. Render them in 
		// a loop
		for( DWORD i = 0; i < m_dwNumMaterials; ++i )
		{
			// Set the material and texture for this subset
			pd3dDevice->SetMaterial( &m_pMeshMaterials[i] );
			pd3dDevice->SetTexture( 0, m_pMeshTextures[i] );

			// Draw the mesh subset
			m_pMesh->DrawSubset( i );
		}
	}
}
