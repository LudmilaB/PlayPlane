#include "DXUT.h"
#include "Utilities.h"
#include "GameError.h"
#include "DXUT\SDKmisc.h"

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

DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

float round( float x)
{
  return floor((x + .5f)* 1000)/1000.f; 
}
void swap( int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

float RangedRand( float range_min, float range_max )
{  
    float u = (float)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
	return u;
}