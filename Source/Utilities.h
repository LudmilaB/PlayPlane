#pragma once
HRESULT FindAndLoadTexture(IDirect3DDevice9* pd3dDevice, WCHAR const* fname, IDirect3DTexture9** ppTexture);

DWORD F2DW( FLOAT f );

float round( float x);

void swap( int* a, int* b);

float RangedRand( float range_min, float range_max );
