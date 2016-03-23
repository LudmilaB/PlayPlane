#include "DXUT.h"
#include "Stars.h"
#include "GameError.h"
#include "Utilities.h"

Stars::Stars(IDirect3DDevice9* pd3dDevice)
{
	for (int i = 0; i<12; i++)
	{
		float Angle = D3DX_PI/3.f * i;
		if( i<6 )
		{
			m_Vel[i].z = 0;
			m_Vel[i].x = cosf(Angle);
		}
		else
		{
			m_Vel[i].z = cosf(Angle);
			m_Vel[i].x = 0;
		}
		m_Vel[i].y = sinf(Angle);

		Particles[i].color = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
		Particles[i].pos.x = 0;
		Particles[i].pos.z = 0;
		Particles[i].pos.y  = 0;		
	}

	pd3dDevice->CreateVertexBuffer(sizeof(Particles),D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		                                                              D3DFVF_STARCUSTOMVERTEX,
																	  D3DPOOL_DEFAULT,
																	  &m_pVB,NULL);

	if (FAILED(FindAndLoadTexture(pd3dDevice, L"Star1.bmp", &m_pTexture)))
		return;	
	HRESULT hr = pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pSaveStateBlock);

	if (FAILED(hr))
	{
		PrintfError(L"Error creating stars' save state block.");
		return;
	}
	m_bRender = false;
	m_fLifeCycle = .5f;
}

Stars::~Stars(void)
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSaveStateBlock);
}

void Stars::Init(D3DXVECTOR3 pos)
{
	if(m_bRender)
		return;
	for (int i = 0; i<12; i++)
	{
		Particles[i].pos.x = pos.x;
		Particles[i].pos.z = pos.z;
		Particles[i].pos.y  = pos.y;		
	}
	m_fLifeTime = 0.f;
	m_bRender = true;
}

void Stars::FrameMove(FLOAT fElapsedTime)
{
	 if(!m_bRender)
		 return;
	 m_fLifeTime += fElapsedTime;
	 if( m_fLifeTime > m_fLifeCycle)
	 {
		m_bRender = false;
		return;
	 }
	 for (int i = 0; i<12; i++)
		Particles[i].pos += m_Vel[i]* fElapsedTime;
}

void Stars::Render(IDirect3DDevice9* pd3dDevice)
{
	if(m_bRender)
	{
		m_pSaveStateBlock->Capture();
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
//		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pd3dDevice->SetTexture(0, m_pTexture);

		pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );       // Turn on point sprites
		pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
		pd3dDevice->SetRenderState(D3DRS_POINTSIZE, F2DW(.02f));
		pd3dDevice->SetRenderState(D3DRS_POINTSCALE_B,F2DW(1.f));
		
		                          
		
		 // Fill the vertex buffer.
		VOID* pParticles;
		if( FAILED( m_pVB->Lock( 0, 0, (void**)&pParticles, 0 ) ) )
			return;	// E_FAIL;
		memcpy( pParticles, Particles, sizeof(Particles) );
		m_pVB->Unlock();
		pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
		pd3dDevice->SetFVF( D3DFVF_STARCUSTOMVERTEX);

		pd3dDevice->DrawPrimitive(D3DPT_POINTLIST,0,12);
		m_pSaveStateBlock->Apply();
	}
}