/* 
    LineNode.h - render one or more line segments. 

    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 
 */

#pragma once
#include "Node.h"

////////////////////////////////////////////////////////////////////////////

// A structure for our custom vertex type
struct CUSTOMVERTEXLINE
{
	FLOAT x, y, z;      // The untransformed, 3D position for the vertex
	DWORD color;        // The vertex color
};

class LineNode : public Node
{
public:
	LineNode(IDirect3DDevice9* pd3dDevice, UINT cLines = 1);
	~LineNode();

    void                Reload(IDirect3DDevice9* pd3dDevice);
    void                Unload();

	void SetVertex(unsigned int index, FLOAT x, FLOAT y, FLOAT z, DWORD color);

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEXLINE (D3DFVF_XYZ|D3DFVF_DIFFUSE)

						// Update traversal for physics, AI, etc.
	virtual void		Update(double fTime);

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);

private:

    CUSTOMVERTEXLINE*       m_pVertices;                        // Two vertices in line segment 
    UINT                    m_cVertices;                        // Number of vertices in m_pVertices 
    LPDIRECT3DVERTEXBUFFER9 m_pVB;                              // Buffer to hold Vertices
};

////////////////////////////////////////////////////////////////////////////
