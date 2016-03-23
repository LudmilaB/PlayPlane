
#pragma once

#include "singleton.h"

enum Layers
{
	DEFAULT,
//	VISIBILITYLAYER,
	OPENESSLAYER,
	OCCUPANCYLAYER,
	COMBINED,
	COUNT
};

class LayerDrawing /*: public Singleton <LayerDrawing>*/
{
public:

	LayerDrawing( void );
	~LayerDrawing( void );
	void OnLostDevice( void );

	void OnResetDeviceQuad(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC *pBackBuffer);
	void DrawQuad(IDirect3DDevice9* pd3dDevice, D3DXVECTOR3* pos, float width, D3DXCOLOR color);
	void DrawLayer( IDirect3DDevice9* pd3dDevice, int Layer, D3DXCOLOR color);
	void InitLayers();

protected:

	//For quad drawing
	bool m_quad;
	IDirect3DVertexBuffer9* m_pVertexBuffer;
	D3DMATERIAL9 m_material;
	int VisibilityLayer[25][25];
	float OpenessLayer[25][25];
	float OccupancyLayer[25][25];
	void InitOccupancyLayer();
	void InitVisibilityLayer();
	void InitOpenessLayer();
	
};
