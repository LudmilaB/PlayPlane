/* 
    LineNode.cpp - render one or more line segments. 

    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 
 */

#include "DXUT.h"
#include "LineNode.h"

////////////////////////////////////////////////////////////////////////////

#define NUM_ELEMENTS(a)         (sizeof(a) / sizeof(a[0]))


// Construct LineNode to hold <cLines> number of lines. 

LineNode::LineNode(IDirect3DDevice9* pd3dDevice, UINT cLines) : 
	Node(pd3dDevice)
,	m_pVertices(0)
,	m_pVB(0)
,   m_cVertices(cLines * 2)
{
	Reload(pd3dDevice);
}


LineNode::~LineNode()
{
	Unload();
}


// Each even and odd pair of vertices make up the endpoints of a line segment. 

void LineNode::SetVertex(unsigned int index, FLOAT x, FLOAT y, FLOAT z, DWORD color)
{
	assert(index < m_cVertices);
    m_pVertices[index].x = x;
    m_pVertices[index].y = y;
    m_pVertices[index].z = z;
    m_pVertices[index].color = color;
}


void LineNode::Reload(IDirect3DDevice9* pd3dDevice)
{
	Unload();
    memset(&m_pVB, 0, sizeof(m_pVB));

    m_pVertices = new CUSTOMVERTEXLINE[m_cVertices];
    memset(m_pVertices, 0, m_cVertices * sizeof(CUSTOMVERTEXLINE));

    // Create the vertex buffer.
    if( FAILED( pd3dDevice->CreateVertexBuffer( m_cVertices * sizeof(CUSTOMVERTEXLINE), 
                                                  D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEXLINE, 
                                                  D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return;	// E_FAIL;
    }

	// Call base class
	Node::Reload(pd3dDevice);
}

//-----------------------------------------------------------------------------
// Name: Unload()
// Desc: Releases all D3D objects (as part of OnDestroyDevice) 
//-----------------------------------------------------------------------------
void LineNode::Unload()
{
    SAFE_RELEASE(m_pVB);
    SAFE_DELETE_ARRAY(m_pVertices);

	// Call base class
	Node::Unload();
}


// If the line segment positions have been change, update the contents of the 
// vertex buffer. 

void LineNode::Update(double /* fTime */)
{
	CUSTOMVERTEXLINE* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
    {
        return;	// E_FAIL;
    }

    memcpy(pVertices, m_pVertices, m_cVertices * sizeof(CUSTOMVERTEXLINE));
    m_pVB->Unlock();
}


// Render the line segments. 

void LineNode::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
    if (m_pVB)
    {
        // Set the world space transform
        pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

        // Set CCW culling
        pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

        // Turn off D3D lighting, since we are providing our own vertex colors
        pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

        pd3dDevice->SetTexture(0, NULL);
        pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEXLINE));
        pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEXLINE);
        pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_cVertices / 2);
    }

	// Call base class
	Node::Render(pd3dDevice, matWorld);
}
