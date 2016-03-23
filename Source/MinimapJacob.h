#pragma once

#include <d3d9.h>

class WorldFile;

//--------------------------------------------------------------------------------------
// This class has a render function that  will be called at the end of every frame
// to render the minimap. This is advanced graphics homework assignment week1.
//--------------------------------------------------------------------------------------

class Minimap
{
public:
	Minimap(IDirect3DDevice9* pd3dDevice, WorldFile& worldFile);
	~Minimap();

	void Render(IDirect3DDevice9* pd3dDevice);

private:
	bool					m_Ok;
	WorldFile&				m_WorldFile;
	IDirect3DTexture9*		m_pMinimapTexture;
	IDirect3DTexture9*		m_pMinimapMask;
	IDirect3DTexture9*		m_pMinimapFrame;
	IDirect3DStateBlock9*	m_pMinimapSaveStateBlock;

	// Prevent value-copying of this object (wouldn't work):
	Minimap(Minimap const&);
	Minimap& operator=(Minimap const&);
};