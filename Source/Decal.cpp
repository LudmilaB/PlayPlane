#include "DXUT.h"
#include "Decal.h"
#include "GameError.h"
#include "Utilities.h"

////////////////////////////////////////////////////////////////////////////


Decal::Decal(IDirect3DDevice9* pd3dDevice,FLOAT Scale, FLOAT X , FLOAT Y , FLOAT Z ) :
	m_pVB(0),  m_pIB(0)
{

	D3DXMatrixTranslation(&m_matWorld,X,Y,Z);
	D3DXMATRIX RotationY;
	D3DXMatrixRotationY(&RotationY, D3DX_PI/2);
	D3DXMatrixMultiply(&m_matWorld,&RotationY, &m_matWorld );

	DWORD Color  = RGB(255, 255, 255);

    CUSTOMVERTEX Vertices[] =                 // wall
    {   
		 { -1.0f,  1.0f,  1.0f, Color, 0.0f, 0.0f},  
         {  1.0f,  1.0f,  1.0f, Color, 1.0f, 0.0f},
         { -1.0f, -1.0f,  1.0f, Color, 0.0f, 1.0f}, 	 
		 {  1.0f, -1.0f,  1.0f, Color, 1.0f, 1.0f},
    };
	
	for(int i =0; i<4; i++)
	{
		Vertices[i].x*=Scale;
		Vertices[i].y*=Scale;
		Vertices[i].z*=Scale;
	}

    // Create the vertex buffer.
    if( FAILED( pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return;	// E_FAIL;
    }

    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, sizeof(Vertices), (void**)&pVertices, 0 ) ) )
        return;	// E_FAIL;
    memcpy( pVertices, Vertices, sizeof(Vertices) );
    m_pVB->Unlock();

	if( FAILED( pd3dDevice->CreateIndexBuffer(6*3*2,0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL) ) )
		return;	// E_FAIL;
	WORD *indices=NULL;
	if( FAILED( m_pIB->Lock( 0, 0, (void**)&indices, 0 ) ) )
        return;	// E_FAIL;

	indices[0]=0;
	indices[1]=1;
	indices[2]=2;

	indices[3]=2;
	indices[4]=1;
	indices[5]=3;

	 m_pIB->Unlock();

	HRESULT hr = pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pSaveStateBlock);
	if (FAILED(hr))
	{
		PrintfError(L"Error creating decal's save state block.");
		return;
	}
	if (FAILED(FindAndLoadTexture(pd3dDevice, L"Avatar1.dds", &m_pTexture)))
		return;	

}


Decal::~Decal()
{
    SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pSaveStateBlock);
	SAFE_RELEASE(m_pTexture);
}

void Decal::Render(IDirect3DDevice9* pd3dDevice)
{
	// Set the world space transform
	pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);

	
	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetPixelShader(NULL);
	m_pSaveStateBlock->Capture();
//	pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(1.f) );
    pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(-.00001f) );

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW  );	
	pd3dDevice->SetTexture(0, m_pTexture);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,					D3DTOP_SELECTARG1);
	/*	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,				D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,					D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,				D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,			0);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_DISABLE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_RESULTARG,				D3DTA_CURRENT);

		pd3dDevice->SetSamplerState     (0, D3DSAMP_ADDRESSU,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_ADDRESSV,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_MAGFILTER,				D3DTEXF_POINT);*/





	pd3dDevice->SetTexture(1, m_pTexture);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,					D3DTOP_BLENDTEXTUREALPHA);
 //   pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1,				D3DTA_TEXTURE);
 //   pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2,				D3DTA_CURRENT);
	//pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,					D3DTOP_BLENDTEXTUREALPHA);
	//pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1,				D3DTA_CONSTANT);
	//pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2,				D3DTA_CURRENT);
	//pd3dDevice->SetTextureStageState(1, D3DTSS_CONSTANT,				0xFFFFFFFF); // opaque white
	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,			0);
//	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_DISABLE);
//	pd3dDevice->SetTextureStageState(1, D3DTSS_RESULTARG,				D3DTA_CURRENT);

//	pd3dDevice->SetSamplerState     (1, D3DSAMP_ADDRESSU,				D3DTADDRESS_CLAMP);
//	pd3dDevice->SetSamplerState     (1, D3DSAMP_ADDRESSV,				D3DTADDRESS_CLAMP);
	pd3dDevice->SetSamplerState     (1, D3DSAMP_MAGFILTER,				D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState     (1, D3DSAMP_MINFILTER,				D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState     (1, D3DSAMP_MIPFILTER,				D3DTEXF_POINT);

    pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
    pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

	pd3dDevice->SetIndices( m_pIB);
	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0,4,0,2);
//	pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.f) );
 //   pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS, F2DW(0.f) );
	m_pSaveStateBlock->Apply();
}
