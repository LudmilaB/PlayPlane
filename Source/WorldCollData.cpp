/* 
    WorldCollData.cpp - the collision data for the world. 
    The structure of the code that processes the WorldFile data is virtually 
    identical to the code found in WorldNode.cpp. 

    The implication is that if you change WorldCollData.cpp, there is a good 
    chance you'll need to apply the same change to WorldNode.cpp. 

    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 
 */

#include "DXUT.h"
#include "DXUT/SDKmisc.h"
#include "GameError.h"
#include "WorldCollData.h"

////////////////////////////////////////////////////////////////////////////

#define NUM_ELEMENTS(a)         (sizeof(a) / sizeof(a[0]))

// Define vertices in a triangle list for an OCCUPIED_CELL. 
// This consists of the top five faces of a box. Each face of the box 
// is constructed from two clockwise winding order triangles. 
static _CollQuad _OccupiedCellCollQuads[] = 
{
    { { {  0.0f,  1.0f,  0.0f },   // -z ("back") 
        {  1.0f,  1.0f,  0.0f }, 
        {  1.0f,  0.0f,  0.0f }, 
        {  0.0f,  0.0f,  0.0f } }, 
        {  0.0f,  0.0f, -1.0f } }, 


    { { {  0.0f,  1.0f,  1.0f },   // -x ("left") 
        {  0.0f,  1.0f,  0.0f }, 
        {  0.0f,  0.0f,  0.0f }, 
        {  0.0f,  0.0f,  1.0f } }, 
        { -1.0f,  0.0f,  0.0f } }, 


    { { {  1.0f,  1.0f,  1.0f },   // +z ("front") 
        {  0.0f,  1.0f,  1.0f }, 
        {  0.0f,  0.0f,  1.0f }, 
        {  1.0f,  0.0f,  1.0f } }, 
        {  0.0f,  0.0f,  1.0f } }, 


    { { {  1.0f,  1.0f,  0.0f },   // +x ("right") 
        {  1.0f,  1.0f,  1.0f }, 
        {  1.0f,  0.0f,  1.0f }, 
        {  1.0f,  0.0f,  0.0f } }, 
        {  1.0f,  0.0f,  0.0f } } 
};

#define OccupiedCellCollQuads   (reinterpret_cast<CollQuad*>(_OccupiedCellCollQuads))


WorldCollData::WorldCollData() : 

    m_pCollQuads()
,   m_cCollQuads(0),pwf(0)
{
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
// allocate to store all OCCUPIED_CELLs. We keep the count 
// in m_cCellVertices[]. 

// We test the "front" (y+1) and "right" (x+1) side of each grid location, 
// and insert a wall if there is a transition from or to OCCUPIED_CELL. 
// For completeness, we start at x = -1 and y = -1. This relies on the 
// fact that WorldFile operator() returns INVALID_CELL when out of range. 

void WorldCollData::CalculateBufferSize(WorldFile* pwf)
{
	for (int y = -1; y < pwf->GetHeight(); ++y)
    {
        for (int x = -1; x < pwf->GetWidth(); ++x)
        {
            WorldFile::ECell cell = (*pwf)(y, x);

            if (cell == WorldFile::OCCUPIED_CELL)
            {
                // Count vertices needed to copy front quad only if y+1 neighbor is not occupied. 
                if ((*pwf)(y+1, x  ) != WorldFile::OCCUPIED_CELL)
                {
                    ++m_cCollQuads;
                }

                // Count vertices needed to copy right quad only if x+1 neighbor is not occupied. 
                if ((*pwf)(y  , x+1) != WorldFile::OCCUPIED_CELL)
                {
                    ++m_cCollQuads;
                }
            }
            else    // Current cell != OCCUPIED_CELL (could be INVALID_CELL or EMPTY_CELL)
            {
                // Count vertices needed to copy back quad only if y+1 neighbor is not occupied. 
                if ((cell = (*pwf)(y+1, x  )) == WorldFile::OCCUPIED_CELL)
                {
                    ++m_cCollQuads;
                }

                // Count vertices needed to copy left quad only if x+1 neighbor is not occupied. 
                if ((cell = (*pwf)(y  , x+1)) == WorldFile::OCCUPIED_CELL)
                {
                    ++m_cCollQuads;
                }
            }
        }
    }
}


void WorldCollData::FillBuffer(WorldFile* pwf)
{
    CollQuad* pCollQuad = m_pCollQuads;

    // In the second pass, let's copy the vertices from the reference geometry. 
    // Use CopyCollQuads() to offset the vertices in model space. 

	for (int y = -1; y < pwf->GetHeight(); ++y)
    {
        for (int x = -1; x < pwf->GetWidth(); ++x)
        {
            WorldFile::ECell cell = (*pwf)(y, x);

            if (cell == WorldFile::OCCUPIED_CELL)
            {
                // Copy front quad only if y+1 neighbor is not occupied. 
                if ((*pwf)(y+1, x  ) != WorldFile::OCCUPIED_CELL)
                {
                    pCollQuad = CopyCollQuads(pCollQuad, OccupiedCellCollQuads + 2, 1, y, x);
                }

                // Copy right quad only if x+1 neighbor is not occupied. 
                if ((*pwf)(y  , x+1) != WorldFile::OCCUPIED_CELL)
                {
                    pCollQuad = CopyCollQuads(pCollQuad, OccupiedCellCollQuads + 3, 1, y, x); 
                }
            }
            else    // Current cell != OCCUPIED_CELL (could be INVALID_CELL or EMPTY_CELL)
            {
                // Copy back quad only if y+1 neighbor is not occupied. 
                if ((cell = (*pwf)(y+1, x  )) == WorldFile::OCCUPIED_CELL)
                {
                    pCollQuad = CopyCollQuads(pCollQuad, OccupiedCellCollQuads +  0, 1, y+1, x);
                }

                // Copy left quad only if x+1 neighbor is not occupied. 
                if ((cell = (*pwf)(y  , x+1)) == WorldFile::OCCUPIED_CELL)
                {
                    pCollQuad = CopyCollQuads(pCollQuad, OccupiedCellCollQuads +  1, 1, y, x+1);
                }
            }
        }
    }
	// Post-condition: we filled every byte that was allocated. 
	assert(m_cCollQuads == (pCollQuad - m_pCollQuads));
}


// Call WorldFile::Load() to load the <szFilename> grid file, then figure out 
// what quads need to be generated for the level collision data. 

bool WorldCollData::Load(const LPCWSTR szFilename)
{
    // Clear out old data, if any. 
    // By doing so, we can repeated call WorldCollData::Load(). 
    Unload();
    m_cCollQuads = 0;

 //L   WorldFile *pwf = new WorldFile;
	pwf = new WorldFile;
    if (!pwf->Load(szFilename))
    {
        PrintfError(L"Error loading file %s.", szFilename);
        delete pwf;
        return false;
    }

	CalculateBufferSize(pwf);

    // Now we know exactly how many vertices to allocate, so let's do it. 
    m_pCollQuads = new CollQuad[m_cCollQuads];

    FillBuffer(pwf);

//L    SAFE_DELETE(pwf);
    return true;
}


void WorldCollData::Unload()
{
    SAFE_DELETE_ARRAY(m_pCollQuads);
//L 
	SAFE_DELETE(pwf);
}


WorldCollData::~WorldCollData()
{
    Unload();
}


const CollQuad& WorldCollData::GetCollQuad(UINT i) const
{
    return m_pCollQuads[i];
}


bool WorldCollData::CollideSphereVsWorld(CollSphere& cs)
{
    bool bCollided = false;

    gCollOutput.Reset();
    for (size_t iquad = 0; iquad < m_cCollQuads; ++iquad)
    {
        if (cs.VsQuad(m_pCollQuads[iquad]))
        {
            bCollided = true;
			return bCollided;
        }
    }
    return bCollided;
}


bool WorldCollData::CollideLineVsWorld(CollLine& cl)
{
    bool bCollided = false;

    gCollOutput.Reset();
    for (size_t iquad = 0; iquad < m_cCollQuads; ++iquad)
    {
        if (cl.VsQuad(m_pCollQuads[iquad]))
        {
            bCollided = true;
			return bCollided;
        }
    }
    return bCollided;
}


// Copy <cVertexSource> CUSTOMVERTEXes from <pCollQuadSource> to <pCollQuadDest>. 
// During the copy, also offset the vertex by (<x>, <y>). 
// 
// Returns <pCollQuadDest> + <cVertexSource>. 
CollQuad* WorldCollData::CopyCollQuads(CollQuad* pCollQuadDest, 
                               CollQuad* pCollQuadSource, UINT cCollQuadSource, 
                               int y, int x)
{
    for (size_t iquad = 0; iquad < cCollQuadSource; ++iquad)
    {
        D3DVECTOR* pVertSource = pCollQuadSource->point;
        D3DVECTOR* pVertDest = pCollQuadDest->point;
        for (size_t ivert = 0; ivert < NUM_ELEMENTS(pCollQuadSource->point); ++ivert)
        {
            *pVertDest = *pVertSource++;
            pVertDest->x += float (x);
            pVertDest->z += float (y);
            ++pVertDest;
        }
        pCollQuadDest->normal = pCollQuadSource->normal;
        ++pCollQuadSource;
        ++pCollQuadDest;
    }
    return pCollQuadDest;
}
