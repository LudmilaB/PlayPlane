#include "DXUT.h"
#include "DXUT\SDKmisc.h"
#include "GameError.h"
#include "GameObject.h"
#include "Database.h"
#include "Body.h"
#include "WorldFile.h"
#include "Game.h"

#include "Minimap.h"


//--------------------------------------------------------------------------------------
// This utility function encompasses the task of finding and loading a texture file.
//--------------------------------------------------------------------------------------

HRESULT FindAndLoadTexture(IDirect3DDevice9* pd3dDevice, WCHAR const* fname, IDirect3DTexture9** ppTexture)
{
    WCHAR str[MAX_PATH];

	HRESULT hr = DXUTFindDXSDKMediaFileCch(str, MAX_PATH, fname);
    if (FAILED(hr))
    {
		PrintfError(L"Can't find file %s.", fname);
        return hr;
    }

	hr = D3DXCreateTextureFromFile(pd3dDevice, str, ppTexture);
    if (FAILED(hr))
    {
		PrintfError(L"Error creating texture from file %s.", fname);
        return hr;
    }

	return hr;
}


//--------------------------------------------------------------------------------------
// This class has a render function that  will be called at the end of every frame
// to render the minimap. This is advanced graphics homework assignment week1.
//--------------------------------------------------------------------------------------

Minimap::Minimap(IDirect3DDevice9* pd3dDevice, WorldFile& worldFile)
	: m_Ok(false)
	, m_WorldFile(worldFile)
	, m_pMinimapTexture(NULL)
	, m_pMinimapMask(NULL)
	, m_pMinimapFrame(NULL)
	, m_pMinimapSaveStateBlock(NULL)
{
	HRESULT hr;

	UINT const w = UINT(worldFile.GetWidth());
	UINT const h = UINT(worldFile.GetHeight());

	hr = pd3dDevice->CreateTexture(w, h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pMinimapTexture, NULL);
//	hr = FindAndLoadTexture(pd3dDevice, L"Map256x256.bmp", &m_pMinimapTexture);
	if (FAILED(hr))
	{
		return;
	}

	D3DLOCKED_RECT lockedRect = { 0 };
	hr = m_pMinimapTexture->LockRect(0, &lockedRect, NULL, 0);
	if (FAILED(hr))
	{
		return;
	}

	for (UINT y = 0; y < h; ++y)
	{
		DWORD* const pRow = (DWORD*) ((BYTE*)lockedRect.pBits + lockedRect.Pitch * (w-1 - y));

		for (UINT x = 0; x < w; ++x)
		{
			switch(worldFile(y, x))
			{
				case WorldFile::INVALID_CELL:
					pRow[x] = 0xFFFF0000; // Red
					break;

				case WorldFile::OCCUPIED_CELL:
					pRow[x] = D3DCOLOR_XRGB(160, 100, 100);
					break;

				default:
					pRow[x] = D3DCOLOR_XRGB(160, 180, 100);
					break;
			}
		}
	}

	m_pMinimapTexture->UnlockRect(0);

	hr = FindAndLoadTexture(pd3dDevice, L"Circle256x256.tga", &m_pMinimapMask);
	if (FAILED(hr))
	{
		return;
	}

	hr = FindAndLoadTexture(pd3dDevice, L"MinimapFrame256x256.tga", &m_pMinimapFrame);
	if (FAILED(hr))
	{
		return;
	}

	hr = pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pMinimapSaveStateBlock);
	if (FAILED(hr))
	{
		PrintfError(L"Error creating minimap's save state block.");
		return;
	}

	m_Ok = true;
}

Minimap::~Minimap()
{
	SAFE_RELEASE(m_pMinimapSaveStateBlock);
	SAFE_RELEASE(m_pMinimapFrame);
	SAFE_RELEASE(m_pMinimapMask);
	SAFE_RELEASE(m_pMinimapTexture);
}

void Minimap::Render(IDirect3DDevice9* pd3dDevice)
{
	if (!m_Ok)
	{
		// Only render if initialization succeeded.
		return;
	}

	// Find where the player is and which direction she is looking.

	GameObject* goPlayer = g_database.Find(g_objectIDPlayer);
	D3DXVECTOR3 const playerPos = goPlayer->GetBody().GetPos();
	D3DXVECTOR3 const playerDir = goPlayer->GetBody().GetDir();

	// Calculate the player position in minimap space.
	// World space has one unit per square of the map, and is 25x25.
	// Texture space is 0..1 for any texture.
	// Minimap space is just texture space in the minimap texture.
	// So we divide by 25 to world space to get minimap space.
	//
	// We also need to invert the Z coordinate because texture coordinates grow downwards.

	float const px =        playerPos.x / float(m_WorldFile.GetWidth());
	float const py = 1.0f - playerPos.z / float(m_WorldFile.GetHeight());

	// Figure out the vectors that point forward and right in minimap space.
	//
	// From a position in the center of the minimap, we reach the edge with a displacement of 0.5,
	// so we'll divide by 2 (playerDir is a normalized vector, so the coordinates are [-1,1]).
	// Also, we don't want to show the entire minimap, so we divide another 2 on top of that.

	float const ZoomFactor = 4.0f; // Combined divisors.

	float const forwardX =  playerDir.x / ZoomFactor;
	float const forwardY = -playerDir.z / ZoomFactor;

	float const rightX = -forwardY; // 90 degree rotation.
	float const rightY =  forwardX;

	// For the NPC, position will be enough.

	GameObject* goNPC = g_database.FindByName("NPC1");
	D3DXVECTOR3 const npcPos = goNPC->GetBody().GetPos();

	// Find the dimensions of the viewport,
	// so we can render the minimap in the correct location.

	D3DVIEWPORT9 viewport = { 0 };

	pd3dDevice->GetViewport(&viewport);

	float const minimapX1 = float(viewport.X) + float(viewport.Width)  * ( 1.0f / 32.0f);
	float const minimapX2 = minimapX1		  + float(viewport.Width)  * ( 1.0f /  8.0f);
	float const minimapY2 = float(viewport.Y) + float(viewport.Height) * (23.0f / 24.0f);
	float const minimapY1 = minimapY2		  - float(viewport.Height) * ( 1.0f /  6.0f);

	float const minimapCenterX = (minimapX1 + minimapX2) / 2.0f;
	float const minimapCenterY = (minimapY1 + minimapY2) / 2.0f;

	float const minimapHalfSizeX = (minimapX2 - minimapX1) / 2.0f; //float(viewport.Width)  * ( 1.0f / 16.0f);
	float const minimapHalfSizeY = (minimapY2 - minimapY1) / 2.0f; //float(viewport.Height) * ( 1.0f / 12.0f);

	// Save all of D3D's state.

	m_pMinimapSaveStateBlock->Capture();

	// Set some initial states that we'll want.

	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetPixelShader(NULL);

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	{
		// First, render the minimap itself. We'll use three simultaneous textures:
		// 0 = minimap
		// 1 = alpha mask
		// 2 = beautifying frame
		//
		// The pixel pipeline formula will be:
		//
		// color = lerp(minimap.rgb, frame.rgb, frame.a)
		// alpha = lerp(mask.a, 1, frame.a)

		pd3dDevice->SetTexture			(0,									m_pMinimapTexture);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,					D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,				D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,					D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,				D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,			0);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_DISABLE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_RESULTARG,				D3DTA_CURRENT);

		pd3dDevice->SetSamplerState     (0, D3DSAMP_ADDRESSU,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_ADDRESSV,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_MAGFILTER,				D3DTEXF_POINT);

		pd3dDevice->SetTexture			(1,									m_pMinimapMask);
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,					D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1,				D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,					D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1,				D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,			1);
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_DISABLE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_RESULTARG,				D3DTA_CURRENT);

		pd3dDevice->SetSamplerState     (1, D3DSAMP_ADDRESSU,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (1, D3DSAMP_ADDRESSV,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (1, D3DSAMP_MAGFILTER,				D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState     (1, D3DSAMP_MINFILTER,				D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState     (1, D3DSAMP_MIPFILTER,				D3DTEXF_POINT);

		pd3dDevice->SetTexture			(2,									m_pMinimapFrame);
		pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP,					D3DTOP_BLENDTEXTUREALPHA);
		pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG1,				D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG2,				D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP,					D3DTOP_BLENDTEXTUREALPHA);
		pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG1,				D3DTA_CONSTANT);
		pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG2,				D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(2, D3DTSS_CONSTANT,				0xFFFFFFFF); // opaque white
		pd3dDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX,			1);
		pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_DISABLE);
		pd3dDevice->SetTextureStageState(2, D3DTSS_RESULTARG,				D3DTA_CURRENT);

		pd3dDevice->SetSamplerState     (2, D3DSAMP_ADDRESSU,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (2, D3DSAMP_ADDRESSV,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (2, D3DSAMP_MAGFILTER,				D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState     (2, D3DSAMP_MINFILTER,				D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState     (2, D3DSAMP_MIPFILTER,				D3DTEXF_POINT);

		pd3dDevice->SetTextureStageState(3, D3DTSS_COLOROP,					D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAOP,					D3DTOP_DISABLE);

		// Set up the vertices.
		// We use two sets of texture coordinates:
		//
		// 0 = These coordinates rotate as the player rotates. Applied to the minimap.
		// 1 = Static coordinates to map a full texture. Applied to the mask and the frame.

		struct MinimapVertex
		{
			float x, y, z, rhw;
			float mapU, mapV;
			float maskU, maskV;

			enum { FVF = D3DFVF_XYZRHW | D3DFVF_TEX2 };
		};

		pd3dDevice->SetFVF(MinimapVertex::FVF);

		// Note that we can use the direction vector (forwardX, forwardY)
		// and the right vector to find the four texture coordinates directly:
		//
		// coord0 = p + forward - right
		// coord1 = p + forward + right
		// coord2 = p - forward - right
		// coord3 = p - forward + right
		//

		MinimapVertex const vertices[4] =
		{
			{ minimapX1, minimapY1, 0, 1,  px + forwardX - rightX, py + forwardY - rightY,  0, 0 },
			{ minimapX2, minimapY1, 0, 1,  px + forwardX + rightX, py + forwardY + rightY,  1, 0 },
			{ minimapX1, minimapY2, 0, 1,  px - forwardX - rightX, py - forwardY - rightY,  0, 1 },
			{ minimapX2, minimapY2, 0, 1,  px - forwardX + rightX, py - forwardY + rightY,  1, 1 },
		};

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(vertices[0]));
	}

	// Now draw the players.

	{
		pd3dDevice->SetTexture			(0,									m_pMinimapMask);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,					D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,				D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,					D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,				D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,			0);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_DISABLE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_RESULTARG,				D3DTA_CURRENT);

		pd3dDevice->SetSamplerState     (0, D3DSAMP_ADDRESSU,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_ADDRESSV,				D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_MAGFILTER,				D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_MINFILTER,				D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState     (0, D3DSAMP_MIPFILTER,				D3DTEXF_POINT);

		pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		struct DotVertex
		{
			float x, y, z, rhw;
			DWORD color;
			float u, v;

			enum { FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 };
		};

		pd3dDevice->SetFVF(DotVertex::FVF);

		// The player's position is fixed (the center of the minimap).

		DotVertex const vertices[4] =
		{
			{ minimapCenterX-3, minimapCenterY-3, 0, 1,  0xFF00FF00,  0, 0 },
			{ minimapCenterX+3, minimapCenterY-3, 0, 1,  0xFF00FF00,  1, 0 },
			{ minimapCenterX-3, minimapCenterY+3, 0, 1,  0xFF00FF00,  0, 1 },
			{ minimapCenterX+3, minimapCenterY+3, 0, 1,  0xFF00FF00,  1, 1 },
		};

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(vertices[0]));

		// Now draw the NPC.

		// First calculate the minimap-space direction vector, and skip
		// if the NPC is outside of the minimap.

		float const dx =  (npcPos.x - playerPos.x) * (ZoomFactor / float(m_WorldFile.GetWidth()));
		float const dy = -(npcPos.z - playerPos.z) * (ZoomFactor / float(m_WorldFile.GetHeight()));

		float const MinimapEffectiveRadius = 0.9f; // Because the mask doesn't reach the edge of the texture.

		if (sqrtf(dx*dx + dy*dy) < MinimapEffectiveRadius)
		{
			// Normally a 2D rotation matrix looks like this:
			//
			// (  cos(a)  sin(a) )
			// ( -sin(a)  cos(a) )
			//
			// But the right-direction vector already contains (cos(a), sin(a))
			// So we use it directly (no need to calculate angles).
			//
			// We need to multiply ZoomFactor because the right vector has it divided (it's not normalized).

			float const x = (+ dx * rightX + dy * rightY) * ZoomFactor * minimapHalfSizeX;
			float const y = (- dx * rightY + dy * rightX) * ZoomFactor * minimapHalfSizeY;

			DotVertex const verticesNPC[4] =
			{
				{ minimapCenterX-3 + x, minimapCenterY-3 + y, 0, 1,  0xFFFF0000,  0, 0 },
				{ minimapCenterX+3 + x, minimapCenterY-3 + y, 0, 1,  0xFFFF0000,  1, 0 },
				{ minimapCenterX-3 + x, minimapCenterY+3 + y, 0, 1,  0xFFFF0000,  0, 1 },
				{ minimapCenterX+3 + x, minimapCenterY+3 + y, 0, 1,  0xFFFF0000,  1, 1 },
			};

			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verticesNPC, sizeof(verticesNPC[0]));
		}
	}

	m_pMinimapSaveStateBlock->Apply();
}
