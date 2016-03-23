/* 
    WorldModelFile.cpp - this file contains the data to render the world. 
    The structure of the code that processes the WorldFile data is virtually 
    identical to the code found in WorldCollData.cpp. 

    The implication is that if you change WorldModelFile.cpp, there is a good 
    chance you'll need to apply the same change to WorldCollData.cpp. 
 */

#include "DXUT.h"
#include "DXUT/SDKmisc.h"
#include "GameError.h"
#include "WorldModelFile.h"

////////////////////////////////////////////////////////////////////////////

const WCHAR szTexture[WorldFile::CELL_MAX][MAX_PATH] = 
{
   L"grass_turf_v2_tex.dds", 
    L"stones.bmp"
};

#define NUM_ELEMENTS(a)         (sizeof(a) / sizeof(a[0]))

// Define vertices in a triangle list for an EMPTY_CELL. 
// This consists of a quad floor constructed out of two clockwise winding 
// order triangles. 
static CUSTOMQUAD EmptyCellQuads[] = 
{
//                             0x00RRGGBB 
    { { {  0.0f,  0.0f,  1.0f, 0xffffffff,  0.0f,  1.0f },   // -y ("floor") 
        {  1.0f,  0.0f,  1.0f, 0xffffffff,  1.0f,  1.0f }, 
        {  0.0f,  0.0f,  0.0f, 0xffffffff,  0.0f,  0.0f }, 
        {  1.0f,  0.0f,  0.0f, 0xffffffff,  1.0f,  0.0f }, 
        {  0.0f,  0.0f,  0.0f, 0xffffffff,  0.0f,  0.0f }, 
        {  1.0f,  0.0f,  1.0f, 0xffffffff,  1.0f,  1.0f } } } 
};

// Define vertices in a triangle list for an OCCUPIED_CELL. 
// This consists of the top five faces of a box. Each face of the box 
// is constructed from two clockwise winding order triangles. 
static CUSTOMQUAD OccupiedCellQuads[] = 
{
//                             0x00RRGGBB 
    { { {  0.0f,  1.0f,  1.0f, 0xffffffff,  0.0f,  0.0f },   // +y ("top") 
        {  1.0f,  1.0f,  1.0f, 0xffffffff,  1.0f,  0.0f }, 
        {  0.0f,  1.0f,  0.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  1.0f,  1.0f,  0.0f, 0xffffffff,  1.0f,  1.0f }, 
        {  0.0f,  1.0f,  0.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  1.0f,  1.0f,  1.0f, 0xffffffff,  1.0f,  0.0f } } }, 

    { { {  0.0f,  1.0f,  0.0f, 0xffffffff,  0.0f,  0.0f },   // -z ("back") 
        {  1.0f,  1.0f,  0.0f, 0xffffffff,  1.0f,  0.0f }, 
        {  0.0f,  0.0f,  0.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  1.0f,  0.0f,  0.0f, 0xffffffff,  1.0f,  1.0f }, 
        {  0.0f,  0.0f,  0.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  1.0f,  1.0f,  0.0f, 0xffffffff,  1.0f,  0.0f } } }, 

    { { {  0.0f,  1.0f,  1.0f, 0xffffffff,  0.0f,  0.0f },   // -x ("left") 
        {  0.0f,  1.0f,  0.0f, 0xffffffff,  1.0f,  0.0f }, 
        {  0.0f,  0.0f,  1.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  0.0f,  0.0f,  0.0f, 0xffffffff,  1.0f,  1.0f }, 
        {  0.0f,  0.0f,  1.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  0.0f,  1.0f,  0.0f, 0xffffffff,  1.0f,  0.0f } } }, 

    { { {  1.0f,  1.0f,  1.0f, 0xffffffff,  0.0f,  0.0f },   // +z ("front") 
        {  0.0f,  1.0f,  1.0f, 0xffffffff,  1.0f,  0.0f }, 
        {  1.0f,  0.0f,  1.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  0.0f,  0.0f,  1.0f, 0xffffffff,  1.0f,  1.0f }, 
        {  1.0f,  0.0f,  1.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  0.0f,  1.0f,  1.0f, 0xffffffff,  1.0f,  0.0f } } }, 

    { { {  1.0f,  1.0f,  0.0f, 0xffffffff,  0.0f,  0.0f },   // +x ("right") 
        {  1.0f,  1.0f,  1.0f, 0xffffffff,  1.0f,  0.0f }, 
        {  1.0f,  0.0f,  0.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  1.0f,  0.0f,  1.0f, 0xffffffff,  1.0f,  1.0f }, 
        {  1.0f,  0.0f,  0.0f, 0xffffffff,  0.0f,  1.0f }, 
        {  1.0f,  1.0f,  1.0f, 0xffffffff,  1.0f,  0.0f } } } 
};


CUSTOMQUAD* CWorldModelFile::m_pCellQuads[WorldFile::CELL_MAX] = 
{
    EmptyCellQuads, 
    OccupiedCellQuads
};


UINT CWorldModelFile::m_cCellQuads[WorldFile::CELL_MAX] = 
{
    NUM_ELEMENTS(EmptyCellQuads), 
    NUM_ELEMENTS(OccupiedCellQuads)
};


CWorldModelFile::CWorldModelFile()
	: m_pWorldFile(NULL)
{
    memset(&m_pVB, 0, sizeof(m_pVB));
    memset(&m_pVertices, 0, sizeof(m_cVertices));
    memset(&m_cVertices, 0, sizeof(m_cVertices));
    memset(&m_pTexture, 0, sizeof(m_pTexture));
}


CWorldModelFile::~CWorldModelFile()
{
    Unload();
}


HRESULT CWorldModelFile::LoadTextures(IDirect3DDevice9* pd3dDevice)
{
    // Load textures 
    WCHAR str[MAX_PATH];
    for (int icell = 0; icell < WorldFile::CELL_MAX; ++icell)
    {
		HRESULT hr = DXUTFindDXSDKMediaFileCch(str, MAX_PATH, szTexture[icell]);
        if (FAILED(hr))
        {
			PrintfError(L"Can't find file %s.", szTexture[icell]);
            Unload();
            return hr;
        }
		hr = D3DXCreateTextureFromFile(pd3dDevice, str, &m_pTexture[icell]);
        if (FAILED(hr))
        {
			PrintfError(L"Error creating texture from file %s.", szTexture[icell]);
            Unload();
            return hr;
        }
    }
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


// Do not insert interior wall when two OCCUPIED_CELLs are adjacent. 
// If we do, we'll see z-fighting between the top of occupied cells and 
// the interior walls as you move around the level. When the z-fighting 
// occurs it looks like gaps are appearing on the top of the occupied cells. 

// Calculate the exact number of bytes to allocate. The design goals are 
// (1) don't grow the buffer, 
// (2) avoid reallocating memory if possible, and 
// (3) don't allocate more than necessary. 

// This decision makes sense on a machine with limited memory, but 
// probably doesn't make any sense on a machine with virtual memory 
// (especially considering how few memory allocations we are making). 

// The first pass through the data is just to count how many vertices to 
// allocate to draw all EMPTY_CELLs and OCCUPIED_CELLs. We keep the count 
// in m_cVertices[]. 

// We test the "front" (y+1) and "right" (x+1) side of each grid location, 
// and insert a wall if there is a transition from or to OCCUPIED_CELL. 
// For completeness, we start at x = -1 and y = -1. This relies on the 
// fact that WorldFile operator() returns INVALID_CELL when out of range. 

void CWorldModelFile::CalculateBufferSizes()
{
	for (int y = -1; y < m_pWorldFile->GetHeight(); ++y)
    {
        for (int x = -1; x < m_pWorldFile->GetWidth(); ++x)
        {
            WorldFile::ECell cell = (*m_pWorldFile)(y, x);

            if (cell == WorldFile::OCCUPIED_CELL)
            {
                // Count vertices needed to copy the top quad of the box. 
                m_cVertices[cell] += NUM_ELEMENTS(m_pCellQuads[cell]->vertex);
                // Changed my mind: decided OCCUPIED_CELLs should get ground as well. 
                m_cVertices[WorldFile::EMPTY_CELL] += NUM_ELEMENTS(m_pCellQuads[WorldFile::EMPTY_CELL]->vertex);

                // Count vertices needed to copy front quad only if y+1 neighbor is not occupied. 
                if ((*m_pWorldFile)(y+1, x  ) != WorldFile::OCCUPIED_CELL)
                {
                    m_cVertices[cell] += NUM_ELEMENTS(m_pCellQuads[cell]->vertex);
                }

                // Count vertices needed to copy right quad only if x+1 neighbor is not occupied. 
                if ((*m_pWorldFile)(y  , x+1) != WorldFile::OCCUPIED_CELL)
                {
                    m_cVertices[cell] += NUM_ELEMENTS(m_pCellQuads[cell]->vertex);
                }
            }
            else    // Current cell != OCCUPIED_CELL (could be INVALID_CELL or EMPTY_CELL)
            {
                if (cell == WorldFile::EMPTY_CELL)
                {
                    // Count vertices needed to copy the ground. 
                    m_cVertices[cell] += m_cCellQuads[cell] * NUM_ELEMENTS(m_pCellQuads[cell]->vertex);
                }

                // Count vertices needed to copy back quad only if y+1 neighbor is not occupied. 
                if ((cell = (*m_pWorldFile)(y+1, x  )) == WorldFile::OCCUPIED_CELL)
                {
                    m_cVertices[cell] += NUM_ELEMENTS(m_pCellQuads[cell]->vertex);
                }

                // Count vertices needed to copy left quad only if x+1 neighbor is not occupied. 
                if ((cell = (*m_pWorldFile)(y  , x+1)) == WorldFile::OCCUPIED_CELL)
                {
                    m_cVertices[cell] += NUM_ELEMENTS(m_pCellQuads[cell]->vertex);
                }
            }
        }
    }
}


HRESULT CWorldModelFile::CreateAndLockBuffers(IDirect3DDevice9* pd3dDevice)
{
    // Now we know exactly how many vertices to allocate, so let's do it. 
    for (int icell = 0; icell < WorldFile::CELL_MAX; ++icell)
    {
        // Create the vertex buffer.
		HRESULT hr = pd3dDevice->CreateVertexBuffer( m_cVertices[icell] * sizeof(CUSTOMVERTEX),
                                                      D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
                                                      D3DPOOL_DEFAULT, &m_pVB[icell], NULL );
        if (FAILED(hr))
        {
			PrintfError(L"Error creating vertex buffer.");
            Unload();       
            return hr;
        }
        // Get pointer to vertex buffer.
        hr = m_pVB[icell]->Lock( 0, m_cVertices[icell] * sizeof(CUSTOMVERTEX), (void**)&m_pVertices[icell], 0 );
        if (FAILED(hr))
        {
			PrintfError(L"Error locking vertex buffer.");
            Unload();
            return hr;
        }
    }
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


void CWorldModelFile::FillAndUnlockBuffers(IDirect3DDevice9* pd3dDevice)
{
    CUSTOMVERTEX* pVertex[WorldFile::CELL_MAX];

    for (int icell = 0; icell < WorldFile::CELL_MAX; ++icell)
    {
        pVertex[icell] = m_pVertices[icell];
    }

    // Use CopyCellQuads() to copy and offset reference quad vertices from model space into world space. 
	for (int y = -1; y < m_pWorldFile->GetHeight(); ++y)
    {
        for (int x = -1; x < m_pWorldFile->GetWidth(); ++x)
        {
            WorldFile::ECell cell = (*m_pWorldFile)(y, x);

            if (cell == WorldFile::OCCUPIED_CELL)
            {
                // Copy the top quad of the box. 
                pVertex[cell] = CopyCellQuads(pVertex[cell], m_pCellQuads[cell], 1, y, x);
                // Since the camera can enter OCCUPIED_CELLs, it looks slightly less bad if it gets the ground quad as well. 
                pVertex[WorldFile::EMPTY_CELL] = CopyCellQuads(pVertex[WorldFile::EMPTY_CELL], m_pCellQuads[WorldFile::EMPTY_CELL], 1, y, x);

                // Copy front quad only if y+1 neighbor is not occupied. 
                if ((*m_pWorldFile)(y+1, x  ) != WorldFile::OCCUPIED_CELL)
                {
                    pVertex[cell] = CopyCellQuads(pVertex[cell], m_pCellQuads[cell] + 3, 1, y, x);
                }

                // Copy right quad only if x+1 neighbor is not occupied. 
                if ((*m_pWorldFile)(y  , x+1) != WorldFile::OCCUPIED_CELL)
                {
                    pVertex[cell] = CopyCellQuads(pVertex[cell], m_pCellQuads[cell] + 4, 1, y, x); 
                }
            }
            else    // cell != OCCUPIED_CELL; could be INVALID_CELL or EMPTY_CELL 
            {
                if (cell == WorldFile::EMPTY_CELL)
                {
                    // Copy the ground. 
                    pVertex[cell] = CopyCellQuads(pVertex[cell], m_pCellQuads[cell], m_cCellQuads[cell], y, x);
                }

                // Copy back quad only if y+1 neighbor is not occupied. 
                if ((cell = (*m_pWorldFile)(y+1, x  )) == WorldFile::OCCUPIED_CELL)
                {
                    pVertex[cell] = CopyCellQuads(pVertex[cell], m_pCellQuads[cell] + 1, 1, y+1, x);
                }

                // Copy left quad only if x+1 neighbor is not occupied. 
                if ((cell = (*m_pWorldFile)(y  , x+1)) == WorldFile::OCCUPIED_CELL)
                {
                    pVertex[cell] = CopyCellQuads(pVertex[cell], m_pCellQuads[cell] + 2, 1, y, x+1);
                }
            }
        }
    }

    for (int icell = 0; icell < WorldFile::CELL_MAX; ++icell)
    {
		// Post-condition: we filled exactly the number of bytes that was created. 
		assert(m_cVertices[icell] == (pVertex[icell] - m_pVertices[icell]));
        m_pVB[icell]->Unlock();
    }
}


// Call WorldFile::Load() to load the <szFilename> grid file, then figure out 
// what triangles need to be generated to draw the level data. Since we are 
// rendering using D3DPT_TRIANGLELIST, each triangle always requires three vertices. 
HRESULT CWorldModelFile::Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename)
{
    // Clear out old data, if any. 
    // By doing so, we can repeated call CWorldModelFile::Load(). 
    Unload();
    memset(&m_cVertices[0], 0, sizeof(m_cVertices));

	HRESULT hr = LoadTextures(pd3dDevice);
	if (FAILED(hr))
	{
		return hr;
	}

    m_pWorldFile = new WorldFile;

    if (!m_pWorldFile->Load(szFilename))
    {
		PrintfError(L"Error loading file %s.", szFilename);
        SAFE_DELETE(m_pWorldFile);
        return DXUTERR_MEDIANOTFOUND;
    }

	CalculateBufferSizes();
	hr = CreateAndLockBuffers(pd3dDevice);
	if (FAILED(hr))
	{
        SAFE_DELETE(m_pWorldFile);
		return hr;
	}
	FillAndUnlockBuffers(pd3dDevice);

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


void CWorldModelFile::Unload()
{
    for (int icell = 0; icell < WorldFile::CELL_MAX; ++icell)
    {
        SAFE_RELEASE(m_pTexture[icell]);
        SAFE_RELEASE(m_pVB[icell]);
    }

    SAFE_DELETE(m_pWorldFile);
}


// Render traversal for drawing objects 
void CWorldModelFile::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
    if (m_pVB)
    {
        // Set the world space transform
        pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

        // Set CCW culling
        pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

        // Turn off D3D lighting, since we are providing our own vertex colors
        pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

        // Enable trilinear filtering. 
        pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

        pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

        // Draw the EMPTY_CELLs followed by the OCCUPIED_CELLs 
        for (int icell = 0; icell < WorldFile::CELL_MAX; ++icell)
        {
            // Set the texture
            pd3dDevice->SetTexture(0, m_pTexture[icell]);

            // Draw the triangles in the vertex buffer. 
            pd3dDevice->SetStreamSource( 0, m_pVB[icell], 0, sizeof(CUSTOMVERTEX) );
            pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_cVertices[icell] / 3 );
        }
    }
}


// Copy <cVertexSource> CUSTOMVERTEXes from <pVertexSource> to <pVertexDest>. 
// During the copy, also offset the vertex by (<x>, <y>). 
// 
// Returns <pVertexDest> + <cVertexSource>. 
CUSTOMVERTEX* CWorldModelFile::CopyCellQuads(CUSTOMVERTEX* pVertexDest, 
                                       CUSTOMQUAD* pQuadSource, UINT cQuadSource, 
                                       int y, int x)
{
    for (size_t iquad = 0; iquad < cQuadSource; ++iquad)
    {
        CUSTOMVERTEX* pVertexSource = pQuadSource->vertex;
        for (size_t ivert = 0; ivert < NUM_ELEMENTS(pQuadSource->vertex); ++ivert)
        {
            *pVertexDest = *pVertexSource++;
            pVertexDest->x += float (x);
            pVertexDest->z += float (y);
            ++pVertexDest;
        }
        ++pQuadSource;
    }
    return pVertexDest;
}
