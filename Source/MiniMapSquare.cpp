#include "DXUT.h"
#include "MinimapSquare.h"

////////////////////////////////////////////////////////////////////////////


MinimapSquare::MinimapSquare(IDirect3DDevice9* pd3dDevice, DWORD Color , FLOAT X , FLOAT Y, FLOAT Side,LPDIRECT3DTEXTURE9 pTexture) :
Node(pd3dDevice), m_pVB(0)
{
	m_pTexture = pTexture;
	CUSTOMVERTEX Vertices[] =               
    {   
		 {  X,           Y, 0.0f, 1.0f, Color, 0.0f, 0.0f },  
         {  X+Side,      Y, 0.0f, 1.0f, Color, 1.0f, 0.0f },
         {  X,      Y+Side, 0.0f, 1.0f, Color, 0.0f, 1.0f }, 	 
		 {  X+Side, Y+Side, 0.0f, 1.0f, Color, 1.0f, 1.0f }

    };

	HRESULT hr;
	// Create the vertex buffer.
	if(pTexture)	   
	    hr=  pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_MINIMAPVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB, NULL );
	else
	   hr=  pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_MINIMAPVER,
                                                  D3DPOOL_MANAGED, &m_pVB, NULL );
    if (FAILED(hr))
		return; //E_FAIL

    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, sizeof(Vertices), (void**)&pVertices, 0 ) ) )
        return;	// E_FAIL;
    memcpy( pVertices, Vertices, sizeof(Vertices) );
    m_pVB->Unlock();

    return;	// S_OK;

}


MinimapSquare::~MinimapSquare()
{
    SAFE_RELEASE(m_pVB);
}



void MinimapSquare::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
	// Set the world space transform
//	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Set the texture
	if(m_pTexture)
		pd3dDevice->SetTexture(0, m_pTexture);
	else
		pd3dDevice->SetTexture(0, NULL);

	// Turn off culling
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

    // Turn off D3D lighting, since we are providing our own vertex colors
//    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	if(m_pTexture)
	{
		pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
		pd3dDevice->SetFVF( D3DFVF_MINIMAPVERTEX );
	}
	else
	{
		pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
		pd3dDevice->SetFVF( D3DFVF_MINIMAPVER );
	}

	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,0, 2);  
	Node::Render(pd3dDevice, matWorld);

}
