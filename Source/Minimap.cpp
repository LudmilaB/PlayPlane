#include "DXUT.h"
#include "Minimap.h"
#include "GameObject.h"
#include "Database.h"
//#include "Body.h"
#include "Game.h"



Minimap::Minimap(IDirect3DDevice9* pd3dDevice, WorldFile* File):m_Map(0)
{
	m_pd3dDevice = pd3dDevice;
	if(!File)
		return;
		m_nSide = 5; 
		m_nHeight = File->GetHeight();
		m_nWidth  = File->GetWidth();
		D3DVIEWPORT9 viewport = { 0 };
		pd3dDevice->GetViewport(&viewport);

	//	m_nFromTop = 350;
		m_nFromTop = viewport.Height-150;
		m_nFromLeft = 30;

		m_Map = new MinimapSquare*[(m_nWidth) * (m_nHeight)];
		for (int i = 0; i<m_nHeight; i++)
			for (int j=0; j< m_nWidth; j++)
			{
				WorldFile::ECell cell = (*File)(i,j);
				DWORD Color;
				switch(cell)
				{
					case WorldFile::EMPTY_CELL:
						Color  = D3DCOLOR_XRGB(200, 200, 120);
						break;
					case WorldFile::OCCUPIED_CELL:
						Color  = D3DCOLOR_XRGB(60, 60, 60);
						break;
					default:
						Color  = D3DCOLOR_XRGB(0, 0, 125);
						break;
				}
				m_Map[(m_nHeight - i-1)*m_nWidth +j] =  new MinimapSquare(pd3dDevice, Color, m_nFromLeft+ j*m_nSide, m_nFromTop + (m_nHeight - i -1 )*m_nSide, m_nSide  );
			}

			for(int i = 0; i < NumberOfMobiles; i++)
				m_Player[i] = NULL;
			
			D3DXCreateLine(pd3dDevice, &pLine);
			pLine->SetWidth(1);
			vLine[0].x = m_nFromLeft;
			vLine[0].y = m_nFromTop;
			vLine[1].x = m_nFromLeft + m_nSide*m_nWidth;
			vLine[1].y = m_nFromTop;
			vLine[2].x = m_nFromLeft + m_nSide*m_nWidth;
			vLine[2].y = m_nFromTop + m_nSide*m_nHeight;
			vLine[3].x =  m_nFromLeft;
			vLine[3].y = m_nFromTop + m_nSide*m_nHeight;
			vLine[4].x = vLine[0].x;
			vLine[4].y = vLine[0].y;
				 

}

Minimap::~Minimap()
{
	if(m_Map)
	{
		for (int i = 0; i<m_nHeight; i++)
			for (int j=0; j<m_nWidth; j++)
					delete m_Map[i*m_nWidth +j];

		delete  m_Map;
	}
	for (int i = 0; i < NumberOfMobiles; i++)
	{
		if(m_Player[i])
			delete m_Player[i];
	}
	SAFE_RELEASE(pLine);
}

 void Minimap::Render(IDirect3DDevice9* pd3dDevice)
 {
	D3DXMATRIX matIdentity;		// identity matrix
    D3DXMatrixIdentity( &matIdentity );
	for (int i = 0; i<m_nHeight; i++)
			for (int j=0; j< m_nWidth; j++)
				m_Map[i*m_nWidth +j]->Render(pd3dDevice, matIdentity);
	SetPlayers();

	for( int i=0; i<NumberOfMobiles; i++)
		if(PlayerPos[i].x> m_nFromLeft-2.5f && PlayerPos[i].x< m_nFromLeft + m_nSide*m_nWidth &&
			PlayerPos[i].y > m_nFromTop && PlayerPos[i].y < m_nFromTop +m_nHeight * m_nSide)
				m_Player[i]->Render(pd3dDevice, matIdentity);

	pLine->Begin();
	pLine->Draw( vLine, 5, D3DCOLOR_XRGB(0,0,0));
	pLine->End();
 }

 void Minimap::SetPlayers() 
 {
	 for(int i = 0; i < NumberOfMobiles; i++)
	 {
		GameObject* goPlayer = g_database.Find(g_objectIDPlayer+i);

		D3DXVECTOR3 const playerPos = goPlayer->GetBody().GetPos();
		float Left = 0;
		float Xcoef = (playerPos.x - Left)/m_nWidth;
		PlayerPos[i].x = m_nFromLeft + Xcoef*m_nWidth * m_nSide-2.0f;

		float Ycoef = (playerPos.z)/m_nHeight;
		PlayerPos[i].y = m_nFromTop + (1- Ycoef)* m_nHeight * m_nSide -1.5 ;
		UpdatePlayer(i);
	 }

 }

 void Minimap::UpdatePlayer(int index)
 {
		if(m_Player[index])
			delete m_Player[index]; 
		DWORD Color;
		if(index)
			Color = RGB(0, 170, 0);
		else
			Color = RGB(255, 0, 200);
		
		m_Player[index] = new MinimapSquare( m_pd3dDevice, Color, PlayerPos[index].x,  PlayerPos[index].y, 3);
 }
