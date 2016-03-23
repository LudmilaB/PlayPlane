//-----------------------------------------------------------------------------
// File: Instance.h
//
// Desc: Header file for the CInstance class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#ifndef INSTANCE_H
#define INSTANCE_H

#include "Node.h"

class GameObject;

class CInstance
{
public:
	CInstance(GameObject& owner);
	virtual ~CInstance();

    virtual HRESULT     GetInstance(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename) = 0;
    virtual void        Unload() = 0;

    virtual HRESULT     ResetTime() = 0;
    virtual HRESULT     AdvanceTime( double dTimeDelta, const D3DXVECTOR3* pvEye ) = 0;
    virtual HRESULT     Draw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld ) = 0;
    virtual void        SetOrientation() = 0;
    inline GameObject*  GetOwner( void )			{ return( m_owner ); }
	std::tr1::shared_ptr<Node> GetNode()	{ return m_pNode; }

	virtual HRESULT     RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) = 0;

protected:
    GameObject*          m_owner;
	std::tr1::shared_ptr<Node> m_pNode;
};

#endif // #ifndef INSTANCE_H
