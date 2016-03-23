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

#include "DXUT.h"
#include "DebugCamera.h"

const float fRotateSpeed = 2.0f;            // debug camera rotation speed 
const float fMoveSpeed = 5.0f;              // debug camera move speed 

static enum D3DUtil_DebugCameraKeys 
{
    DEBUG_CAM_ROTATE_LEFT = 0,
    DEBUG_CAM_ROTATE_RIGHT,
    DEBUG_CAM_MOVE_FORWARD,
    DEBUG_CAM_MOVE_BACKWARD,
    DEBUG_CAM_PITCH_UP,
    DEBUG_CAM_PITCH_DOWN,
    DEBUG_CAM_MOVE_UP, 
    DEBUG_CAM_MOVE_DOWN, 
    DEBUG_CAM_MAX_KEYS,
    DEBUG_CAM_UNKNOWN = 0xFF
};

static BYTE	s_aKeys[DEBUG_CAM_MAX_KEYS];    // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK

//--------------------------------------------------------------------------------------
// Maps a windows virtual key to an enum
//--------------------------------------------------------------------------------------
static D3DUtil_DebugCameraKeys MapDebugCameraKey( UINT nKey )
{
    // This could be upgraded to a method that's user-definable but for 
    // simplicity, we'll use a hardcoded mapping.
    switch( nKey )
    {
        case 'A':      return DEBUG_CAM_ROTATE_LEFT;
        case 'D':      return DEBUG_CAM_ROTATE_RIGHT;
        case 'W':      return DEBUG_CAM_MOVE_FORWARD;
        case 'S':      return DEBUG_CAM_MOVE_BACKWARD;
        case VK_PRIOR: return DEBUG_CAM_PITCH_UP;   // pgup
        case VK_NEXT:  return DEBUG_CAM_PITCH_DOWN; // pgdn

        case 'Q':      return DEBUG_CAM_MOVE_UP;
        case 'Z':      return DEBUG_CAM_MOVE_DOWN;

        case VK_NUMPAD9: return DEBUG_CAM_PITCH_UP;
        case VK_NUMPAD3: return DEBUG_CAM_PITCH_DOWN;
    }

    return DEBUG_CAM_UNKNOWN;
}

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------
CDebugCamera::CDebugCamera()
{
    ZeroMemory( s_aKeys, sizeof(BYTE)*DEBUG_CAM_MAX_KEYS );
}

//--------------------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------------------
CDebugCamera::~CDebugCamera()
{
}

//--------------------------------------------------------------------------------------
// Call this from your message proc so this class can handle window messages
//--------------------------------------------------------------------------------------
LRESULT CDebugCamera::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER( hWnd );
    UNREFERENCED_PARAMETER( lParam );

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            // Map this key to a D3DUtil_DebugCameraKeys enum and update the
            // state of s_aKeys[] by adding the KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK mask
            // only if the key is not down
            D3DUtil_DebugCameraKeys mappedKey = MapDebugCameraKey( (UINT)wParam );
            if( mappedKey != DEBUG_CAM_UNKNOWN && (DWORD)mappedKey < DEBUG_CAM_MAX_KEYS )
            {
                if( FALSE == IsKeyDown(s_aKeys[mappedKey]) )
                {
                    s_aKeys[ mappedKey ] = KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
                    ++m_cKeysDown;
                }
            }
            break;
        }

        case WM_KEYUP:
        {
            // Map this key to a D3DUtil_DebugCameraKeys enum and update the
            // state of s_aKeys[] by removing the KEY_IS_DOWN_MASK mask.
            D3DUtil_DebugCameraKeys mappedKey = MapDebugCameraKey( (UINT)wParam );
            if( mappedKey != DEBUG_CAM_UNKNOWN && (DWORD)mappedKey < DEBUG_CAM_MAX_KEYS )
            {
                s_aKeys[ mappedKey ] &= ~KEY_IS_DOWN_MASK;
                --m_cKeysDown;
            }
            break;
        }
    }

    return FALSE;
}

//--------------------------------------------------------------------------------------
// Update the view matrix based on user input & elapsed time
//--------------------------------------------------------------------------------------
VOID CDebugCamera::FrameMove( FLOAT fElapsedTime )
{
    D3DXVECTOR3 vPosDeltaWorld(0.0f, 0.0f, 0.0f);

    if( DXUTGetGlobalTimer()->IsStopped() )
        fElapsedTime = 1.0f / DXUTGetFPS();

    if (IsKeyDown(s_aKeys[DEBUG_CAM_ROTATE_LEFT]))
    {
        m_fCameraYawAngle -= fRotateSpeed * fElapsedTime;
    }
    if (IsKeyDown(s_aKeys[DEBUG_CAM_ROTATE_RIGHT]))
    {
        m_fCameraYawAngle += fRotateSpeed * fElapsedTime;
    }
	if (IsKeyDown(s_aKeys[DEBUG_CAM_MOVE_FORWARD]))
	{
        vPosDeltaWorld.x =  sin(m_fCameraYawAngle) * fMoveSpeed * fElapsedTime;
        vPosDeltaWorld.z =  cos(m_fCameraYawAngle) * fMoveSpeed * fElapsedTime;
	}
	if (IsKeyDown(s_aKeys[DEBUG_CAM_MOVE_BACKWARD]))
	{
        vPosDeltaWorld.x = -sin(m_fCameraYawAngle) * fMoveSpeed * fElapsedTime;
        vPosDeltaWorld.z = -cos(m_fCameraYawAngle) * fMoveSpeed * fElapsedTime;
	}
    if (IsKeyDown(s_aKeys[DEBUG_CAM_PITCH_UP]))
    {
        m_fCameraPitchAngle -= fRotateSpeed * fElapsedTime;

        // Limit pitch to straight up 
        m_fCameraPitchAngle = __max( -D3DX_PI/2.0f,  m_fCameraPitchAngle );
    }
    if (IsKeyDown(s_aKeys[DEBUG_CAM_PITCH_DOWN]))
    {
        m_fCameraPitchAngle += fRotateSpeed * fElapsedTime;

        // Limit pitch to straight down 
        m_fCameraPitchAngle = __min( +D3DX_PI/2.0f,  m_fCameraPitchAngle );
    }
    if (IsKeyDown(s_aKeys[DEBUG_CAM_MOVE_UP]))
    {
        vPosDeltaWorld.y += fMoveSpeed * fElapsedTime;
    }
    if (IsKeyDown(s_aKeys[DEBUG_CAM_MOVE_DOWN]))
    {
        vPosDeltaWorld.y -= fMoveSpeed * fElapsedTime;
    }

    // Make a rotation matrix based on the camera's yaw & pitch
    D3DXMATRIX mCameraRot;
    D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle, m_fCameraPitchAngle, 0 );

    // Transform vectors based on camera's rotation matrix
    D3DXVECTOR3 vWorldUp, vWorldAhead;
    D3DXVECTOR3 vLocalUp    = D3DXVECTOR3(0,1,0);
    D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0,0,1);
    D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mCameraRot );
    D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mCameraRot );

    // Move the eye position 
    m_vEye += vPosDeltaWorld;

    // Update the lookAt position based on the eye position 
    m_vLookAt = m_vEye + vWorldAhead;
    
    // Update the view matrix
    D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vWorldUp );

    D3DXMatrixInverse( &m_mCameraWorld, NULL, &m_mView );
}
