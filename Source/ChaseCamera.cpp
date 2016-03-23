/*
	Chase Camera 

	Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 
 */

#include "DXUT.h"
#include "ChaseCamera.h"
#include "body.h"
#include "database.h"
#include "Game.h"
#include "gameobject.h"
#include "global.h"
#include "Tiny.h"
//#include "collision.h"
#include "WorldCollData.h"

extern std::vector< CTiny* >     g_v_pCharacters;

const float fRotateSpeed = 1.0f;            // chase camera rotation speed 
const float fMoveSpeed = 5.0f;              // chase camera move speed 

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------
CChaseCamera::CChaseCamera()
{
}

//--------------------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------------------
CChaseCamera::~CChaseCamera()
{
}

//--------------------------------------------------------------------------------------
// Call this from your message proc so this class can handle window messages
//--------------------------------------------------------------------------------------
LRESULT CChaseCamera::HandleMessages( HWND , UINT , WPARAM , LPARAM )
{
    return FALSE;
}

//--------------------------------------------------------------------------------------
// Update the view matrix based on user input & elapsed time
//--------------------------------------------------------------------------------------
VOID CChaseCamera::FrameMove( FLOAT fElapsedTime )
{
    // set follow transforms
	GameObject* player = g_database.Find(g_objectIDPlayer);
	CInstance * pChar = player->GetInstance();

    D3DXVECTOR3 vCharPos = pChar->GetOwner()->GetBody().GetPos();
    D3DXVECTOR3 vCharFacing = pChar->GetOwner()->GetBody().GetDir();
	D3DXVECTOR3 DesiredEye = D3DXVECTOR3( vCharPos.x, vCharPos.y+.5f, vCharPos.z );
    m_vLookAt = D3DXVECTOR3( vCharPos.x, vCharPos.y, vCharPos.z );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	vCharFacing.x *= .8f; vCharFacing.y = 0.f; vCharFacing.z *= .8f;
    DesiredEye -= vCharFacing;

	D3DXVECTOR3 temp = m_vEye + (DesiredEye - m_vEye)*.06f;

	/*vCharFacing.x *= .01f; vCharFacing.y = 0.f; vCharFacing.z *= .01f;
	D3DXVECTOR3 temp1 = temp -vCharFacing;

	int r = floor(temp1.z);
	int c = floor(temp1.x);*/
	//int r = (int)floor(temp.z);
	//int c = (int)floor(temp.x);
	//WorldFile::ECell cell = (*g_pWorldCollData->pwf)(r, c);
	//bool bTooClose = false;
	//if (cell != WorldFile::OCCUPIED_CELL) 
	//{
	//	if( temp.z - r < .1f ||  temp.x - c < .1f )
	//		bTooClose = true;

	//}

	//CollLine cl;
 //   cl.Set(&vCharPos, &temp);
 //   gCollOutput.Reset();
 //   bool bSeePlayer = !g_pWorldCollData->CollideLineVsWorld(cl);

	//if ((cell != WorldFile::OCCUPIED_CELL && !bTooClose || temp.y>vCharPos.y+1.5f)  && bSeePlayer)
	//	m_vEye  =  temp;

	int r = (int)floor(temp.z);
	int c = (int)floor(temp.x);
	WorldFile::ECell cell = (*g_pWorldCollData->pwf)(r, c);


	CollLine cl;
    cl.Set(&vCharPos, &temp);
    gCollOutput.Reset();
    bool bSeePlayer = !g_pWorldCollData->CollideLineVsWorld(cl);

	if ((cell != WorldFile::OCCUPIED_CELL || temp.y>vCharPos.y+1.5f) && bSeePlayer)
		m_vEye  =  temp;
	else 
	{
	/*	float difx = m_vEye.x -temp.x;
		if( difx < 0.1f)
			m_vEye.x = m_vEye.x + difx;
		float difz = m_vEye.z -temp.z;
		if( difz < 0.1f)h
			m_vEye.z = m_vEye.z + difz;*/
	}

 //   m_vLookAt += vCharFacing;

    // Update the view matrix
    D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vUp );

    D3DXMatrixInverse( &m_mCameraWorld, NULL, &m_mView );
}


void CChaseCamera::SetAfterPlayer()
{
	GameObject* player = g_database.Find(g_objectIDPlayer);
	CInstance * pChar = player->GetInstance();

    D3DXVECTOR3 vCharPos = pChar->GetOwner()->GetBody().GetPos();
    D3DXVECTOR3 vCharFacing = pChar->GetOwner()->GetBody().GetDir();
	m_vEye = D3DXVECTOR3( vCharPos.x, vCharPos.y+.5f, vCharPos.z );
    float offset = .8f;
	vCharFacing.x *= .8f; vCharFacing.y = 0.f; vCharFacing.z *= .8f;
    m_vEye -= vCharFacing;
	m_vLookAt = vCharPos;
	D3DXVECTOR3 vWorldUp = D3DXVECTOR3(0,1,0);
	D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vWorldUp );
    D3DXMatrixInverse( &m_mCameraWorld, NULL, &m_mView );
}