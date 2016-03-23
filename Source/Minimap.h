#pragma once
#include "MiniMapSquare.h"
#include "WorldFile.h"
#include "Body.h"
class Minimap
{
public:
   Minimap(IDirect3DDevice9* pd3dDevice, WorldFile* File);
  ~Minimap();
   void Render(IDirect3DDevice9* pd3dDevice);
   void UpdatePlayer(int index);
private:
	MinimapSquare** m_Map; 
	int m_nHeight;
	int m_nWidth;
	float m_nFromTop;
	float m_nFromLeft;
	float m_nSide;
	D3DXVECTOR3 PlayerPos[NumberOfMobiles];
	MinimapSquare* m_Player[NumberOfMobiles];
	IDirect3DDevice9* m_pd3dDevice;
	ID3DXLine* pLine;
	D3DXVECTOR2 vLine[5];


public:
	void SetPlayers();
};