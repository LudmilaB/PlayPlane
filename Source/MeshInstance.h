//-----------------------------------------------------------------------------
// File: MeshInstance.h
//
// Desc: Header file for the CMeshInstance class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H

#include "Instance.h"

class GameObject;

class CMeshInstance : public CInstance
{
public:
	CMeshInstance(GameObject& owner);
	~CMeshInstance();

    HRESULT     GetInstance(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename);
    void        Unload();

    HRESULT     ResetTime();
    HRESULT     AdvanceTime( double dTimeDelta, const D3DXVECTOR3* pvEye );
    HRESULT     Draw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld );
    void        SetOrientation();
	HRESULT     RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );

};

#endif // #ifndef MESHINSTANCE_H
