/*
	Debug Camera 

	Described in CP100A-Homework4.doc 

    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 
 */

#include "DXUT/DXUTcamera.h"

class CDebugCamera : public CBaseCamera
{
public:
    CDebugCamera();
    ~CDebugCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void FrameMove( FLOAT fElapsedTime );

    // Functions to change behavior
    void SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown = false );

    // Functions to get state
    D3DXMATRIX*  GetWorldMatrix()            { return &m_mCameraWorld; }

    D3DXVECTOR3 GetWorldRight() const { return *(D3DXVECTOR3*)&m_mCameraWorld._11; } 
    D3DXVECTOR3 GetWorldUp() const    { return *(D3DXVECTOR3*)&m_mCameraWorld._21; }
    D3DXVECTOR3 GetWorldAhead() const { return *(D3DXVECTOR3*)&m_mCameraWorld._31; }
    D3DXVECTOR3 GetEyePt() const      { return *(D3DXVECTOR3*)&m_mCameraWorld._41; }

protected:
    D3DXMATRIX m_mCameraWorld;       // World matrix of the camera (inverse of the view matrix)
};
