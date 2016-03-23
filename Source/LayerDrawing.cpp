

#include "DXUT.h"
#include "layerdrawing.h"
#include "global.h"
#include "collision.h"
#include "WorldCollData.h"
#include "database.h"
#include "game.h"
#include "gameobject.h"
#include "body.h"


LayerDrawing::LayerDrawing( void )
:  m_pVertexBuffer(NULL),
  m_quad(false)
{
	
	
}

LayerDrawing::~LayerDrawing( void )
{
	SAFE_RELEASE( m_pVertexBuffer );
}

void LayerDrawing::OnLostDevice( void )
{
	SAFE_RELEASE( m_pVertexBuffer );
	m_quad = false;
}

struct LAYER_VERTEX
{
	float x, y, z;
	float nx,ny, nz;
};

#define LAYER_VERTEX_FORMAT ( D3DFVF_XYZ | D3DFVF_NORMAL)

void LayerDrawing::OnResetDeviceQuad(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC *pBackBuffer)
{
	UINT length = 4 * sizeof(LAYER_VERTEX); // 4 vertices
	DWORD usage = 0;
	D3DPOOL pool = D3DPOOL_DEFAULT;

	{	//Quad
		pd3dDevice->CreateVertexBuffer( length, usage, LAYER_VERTEX_FORMAT, pool, &m_pVertexBuffer, NULL );

		// created our buffer, copy our stuff into it
		float vertexData[] = {
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
				0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
				1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
				1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
		};
		LAYER_VERTEX *data;
		m_pVertexBuffer->Lock(0,0, (VOID**)&data, 0);
		memcpy(data, vertexData, sizeof(LAYER_VERTEX) * 4); 
		m_pVertexBuffer->Unlock();
	}
	
	ZeroMemory( &m_material, sizeof(D3DMATERIAL9) );
}

void LayerDrawing::DrawQuad(IDirect3DDevice9* pd3dDevice, D3DXVECTOR3* pos, float width, D3DXCOLOR color)
{

	if( !m_quad )
	{
		m_quad = true;
		OnResetDeviceQuad( pd3dDevice, NULL );
	}

	D3DXMATRIX translation;
	D3DXMatrixTranslation( &translation, pos->x, pos->y, pos->z );

	//D3DXMATRIX scale;
	//D3DXMatrixScaling( &scale, width, width, width );	

	//D3DXMATRIX srt;
	//srt = scale * translation;

	//// translate to stand at location, draw our mesh
	//pd3dDevice->SetTransform( D3DTS_WORLD, &srt );
	pd3dDevice->SetTransform( D3DTS_WORLD, &translation );

	pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(LAYER_VERTEX));
	pd3dDevice->SetFVF(LAYER_VERTEX_FORMAT);
	m_material.Diffuse = color;
	m_material.Ambient = color;
	pd3dDevice->SetMaterial(&m_material);
	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2); // draw our 2 triangles.
}

void LayerDrawing::DrawLayer( IDirect3DDevice9* pd3dDevice, int Layer, D3DXCOLOR color)
{
	if( Layer == OCCUPANCYLAYER || Layer == COMBINED)
		InitOccupancyLayer();
	D3DXCOLOR xColor = color;
	int Width = g_pWorldCollData->pwf->GetWidth();		
	int Height = g_pWorldCollData->pwf->GetHeight();

	D3DXVECTOR3 pos( 0.0f, 0.01f, 0.0f );
	for (int r = 0; r < Width; r++)
		for (int c = 0; c < Height; c++)
		{
			pos.x = (float)c;
			pos.z = (float)r;
			switch(Layer)
			{
				//case VISIBILITYLAYER:
				//	xColor.r = color.r *((float)VisibilityLayer[r][c])/200.f;
				//	break;
				case OPENESSLAYER:
					xColor.g = color.g *OpenessLayer[r][c];
					break;
				case OCCUPANCYLAYER:
					xColor.b = color.b *OccupancyLayer[r][c];
					break;
				case COMBINED:
					xColor.r = color.r *OccupancyLayer[r][c]*OpenessLayer[r][c]*2;
					xColor.g = color.g *OccupancyLayer[r][c]*OpenessLayer[r][c]*2;
					break;
			
			}
			DrawQuad( pd3dDevice, &pos, 1, xColor );
		}
}

void LayerDrawing::InitVisibilityLayer()
{

	int Width = g_pWorldCollData->pwf->GetWidth();		
	int Height = g_pWorldCollData->pwf->GetHeight();

	for(int r=0; r<Height; r++)
		for( int c=0; c<Width; c++)
			VisibilityLayer[r][c] = 0;		

	CollLine cl;															 
	
	D3DXVECTOR3 TheNode, Node;
	for(int r=0; r<Height; r++)
	{
		for( int c=0; c<Width; c++)
		{
			WorldFile::ECell cell = (*g_pWorldCollData->pwf)(r, c);
				if (cell == WorldFile::OCCUPIED_CELL)
					continue;
				
			D3DXVECTOR3 TheNode(c+.5f,0,r+.5f);
				for(int r1=r; r1< Height; r1++)
					for( int c1=0; c1<Width; c1++)
					{
						if( r*Width + c >= r1*Width + c1)
							continue;

						WorldFile::ECell cell = (*g_pWorldCollData->pwf)(r1, c1);
						if (cell == WorldFile::OCCUPIED_CELL)
							continue;
						
						D3DXVECTOR3 Node(c1+.5f,0,r1+.5f);
						cl.Set(&TheNode, &Node);
						gCollOutput.Reset();
						bool Visible = !g_pWorldCollData->CollideLineVsWorld(cl);
						if(Visible)
						{
							VisibilityLayer[r][c]++;
							VisibilityLayer[r1][c1]++;
						}
					}
		}
	}
}

void LayerDrawing::InitOpenessLayer()
{

	int Width = g_pWorldCollData->pwf->GetWidth();		
	int Height = g_pWorldCollData->pwf->GetHeight();
	for(int r=0; r<Height; r++)
	for( int c=0; c<Width; c++)
	{
		WorldFile::ECell cell = (*g_pWorldCollData->pwf)(r, c);
		if (cell == WorldFile::OCCUPIED_CELL)
			OpenessLayer[r][c] = 0;
		else
			OpenessLayer[r][c] = 1;
	}

	float LastOpeness = 0;
	while( LastOpeness < .8)
	{
		float NextOpeness = LastOpeness+.2f;
		for(int r=0; r<Height; r++)
			for( int c=0; c<Width; c++)
			{
				
				if(OpenessLayer[r][c] != LastOpeness)
					continue;
				for( int RowOff = -1; RowOff <= 1; RowOff++)
				for( int ColOff = -1; ColOff <= 1; ColOff++)
				{
					int Row  = r + RowOff;
					int Col = c + ColOff;
					if(Row<0 || Col<0 || Row >= Height || Col >= Width)
						continue;
					if( OpenessLayer[Row][Col] > NextOpeness )
						OpenessLayer[Row][Col] = NextOpeness; 

				}
			}
			LastOpeness = NextOpeness;
	}

}

void LayerDrawing::InitOccupancyLayer()
{
	int Width = g_pWorldCollData->pwf->GetWidth();		
	int Height = g_pWorldCollData->pwf->GetHeight();

	for(int r=0; r<Height; r++)
		for( int c=0; c<Width; c++)
			OccupancyLayer[r][c] = 0;	

	objectID IDPlayer = g_objectIDPlayer;
	while(IDPlayer < 5)
	{
		GameObject* Player = g_database.Find(IDPlayer++);
		if(!Player)
			break;
		D3DXVECTOR3 pos = Player->GetBody().GetPos();
		int r = (int)floor(pos.z);
		int c = (int)floor(pos.x);
		if(r>=0 && r<Height && c>=0 && c<Width)
			OccupancyLayer[r][c] = 1;
	}

	float LastOccupancy = 1.f;
		while( LastOccupancy > .4f)
		{
			float NextOccupancy = LastOccupancy - .2f;
			for(int r=0; r<Height; r++)
				for( int c=0; c<Width; c++)
				{
					
					if(OccupancyLayer[r][c] != LastOccupancy)
						continue;
					for( int RowOff = -1; RowOff <= 1; RowOff++)
					for( int ColOff = -1; ColOff <= 1; ColOff++)
					{
						int Row  = r + RowOff;
						int Col = c + ColOff;
						if(Row<0 || Col<0 || Row >= Height || Col >= Width)
							continue;
						if( OccupancyLayer[Row][Col] < NextOccupancy)
							OccupancyLayer[Row][Col] = NextOccupancy; 

					}
				}
				LastOccupancy = NextOccupancy;
		}
}


void LayerDrawing::InitLayers()
{
	InitVisibilityLayer();
	InitOpenessLayer();
}


