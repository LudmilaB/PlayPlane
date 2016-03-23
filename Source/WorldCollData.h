/* 
    WorldCollData.cpp - the collision data for the world. 

    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 
 */

#pragma once

#include "collision.h"
#include "WorldFile.h"

////////////////////////////////////////////////////////////////////////////

class WorldCollData
{
public:
	WorldCollData();
	~WorldCollData();

    bool                Load(const LPCWSTR szFilename);
    void                Unload();

    bool                CollideSphereVsWorld(CollSphere& cs);
    bool                CollideLineVsWorld(CollLine& cl);

    const CollQuad&     GetCollQuad(UINT i) const;
    UINT                GetCCollQuads()     { return m_cCollQuads; }
//L
	WorldFile *pwf;

private:

    CollQuad*               m_pCollQuads;        // CollQuads in the world 
    UINT                    m_cCollQuads;        // Number of CollQuads in m_pCollQuads 

	void CalculateBufferSize(WorldFile* pwf);
	void FillBuffer(WorldFile* pwf);

    CollQuad* CopyCollQuads(CollQuad* pCollQuadsDest, 
                               CollQuad* pCollQuadSource, UINT cCollQuadSource, 
                               int y, int x);
};

////////////////////////////////////////////////////////////////////////////

extern WorldCollData* g_pWorldCollData; // World collision data 
