#pragma once
class Stars
{
public:
	Stars(IDirect3DDevice9* pd3dDevice);
	~Stars(void);

	#define D3DFVF_STARCUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

	void Render(IDirect3DDevice9* pd3dDevice);
	void Init (D3DXVECTOR3 pos);
	void FrameMove(FLOAT fElapsedTime);

private:
	struct CUSTOMVERTEX
	{
		D3DXVECTOR3 pos;      // The untransformed, 3D position for the vertex
		DWORD color;        // The vertex color
	};
	CUSTOMVERTEX Particles[12];
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	IDirect3DTexture9*		m_pTexture;
	IDirect3DStateBlock9*	m_pSaveStateBlock;
	bool m_bRender;
	FLOAT m_fLifeTime;
	FLOAT m_fLifeCycle;
	D3DXVECTOR3 m_Vel[12];
	D3DXMATRIX m_matWorld;
};
