/*
    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 

 * Portions copyright Steve Rabin, 2005. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2005"
 */


#include "DXUT.h"
#include "game.h"
#include "player.h"
#include "database.h"
#include "movement.h"
#include "body.h"
#include "global.h"

const float accelerationForward = 0.01f;
const float accelerationBackward = 0.01f;
const float accelerationCoast = 0.05f;  // how quickly player stops when keys are released 
const float accelerationMax = 0.2f;

const float speedRotate = 0.05f;        // in radians 
const float speedRotateJog = 0.06f;     // in radians 

//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state
	STATE_Idle,
    STATE_GO
};

//Add new substates here
enum SubstateName {
	//empty
};

//--------------------------------------------------------------------------------------
// Maps a windows virtual key to an enum
//--------------------------------------------------------------------------------------
static PlayerEvents MapPlayerKey( UINT nKey )
{
    // This could be upgraded to a method that's user-definable but for 
    // simplicity, we'll use a hardcoded mapping.
    switch( nKey )
    {
        case VK_LEFT:  return PLAYER_ROTATE_LEFT;
        case VK_RIGHT: return PLAYER_ROTATE_RIGHT;
        case VK_UP:    return PLAYER_MOVE_FORWARD;
        case VK_DOWN:  return PLAYER_MOVE_BACKWARD;

        case VK_NUMPAD4: return PLAYER_ROTATE_LEFT;
        case VK_NUMPAD6: return PLAYER_ROTATE_RIGHT;
        case VK_NUMPAD8: return PLAYER_MOVE_FORWARD;
        case VK_NUMPAD2: return PLAYER_MOVE_BACKWARD;

        case VK_SHIFT: return PLAYER_JOG_MODIFIER;
    }

    return PLAYER_UNKNOWN;
}

// Translate Windows WM_KEYDOWN and WM_KEYUP message into MSG_KeyDown and MSG_KeyUp messages 
// and pass them to the player. 

LRESULT MapPlayerMessageFromWindowsMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER( hWnd );
    UNREFERENCED_PARAMETER( lParam );

    if (g_objectIDPlayer != INVALID_OBJECT_ID && MsgRoute::DoesSingletonExist())
    {
        PlayerEvents mappedEvent = MapPlayerKey( (UINT)wParam );
        if( mappedEvent != PLAYER_UNKNOWN && (DWORD)mappedEvent < PLAYER_MAX_KEYS )
        {
            MSG_Data msgdata((int) mappedEvent);

            switch( uMsg )
            {
                case WM_KEYDOWN:
                {
                    g_msgroute.SendMsg( 0.0f, MSG_KeyDown, g_objectIDPlayer, g_objectIDPlayer, SCOPE_TO_STATE_MACHINE, 0, STATE_MACHINE_QUEUE_ALL, msgdata, false, false );
                    break;
                }

                case WM_KEYUP:
                {
                    g_msgroute.SendMsg( 0.0f, MSG_KeyUp, g_objectIDPlayer, g_objectIDPlayer, SCOPE_TO_STATE_MACHINE, 0, STATE_MACHINE_QUEUE_ALL, msgdata, false, false );
                    break;
                }
            }
        }
    }

    return FALSE;
}

Player::Player( GameObject & object ) : 
    StateMachine( object )
,   m_cKeysDown(0)
,   m_acceleration(0.f)
{
    ZeroMemory( m_aKeys, sizeof(BYTE) * PLAYER_MAX_KEYS );
}

bool Player::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses
	OnMsg( MSG_Reset )
		ResetStateMachine();

	OnMsg( MSG_KeyDown )
        ProcessKeyDown(msg);

	OnMsg( MSG_KeyUp )
        ProcessKeyUp(msg);


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			m_owner->GetMovement().SetAirSpeed(1.5f);
			ChangeState( STATE_Idle );
	

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();

        OnUpdate
            ProcessMovement();
            // Transition to walk? 
            if (fabs(m_owner->GetBody().GetSpeed()) > 0.0f)
            {
    			ChangeState( STATE_GO );
            }

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_GO )

		OnEnter
	//		m_owner->GetBody().SetSpeed( 13.f );
        OnUpdate
            ProcessMovement();
            // Transition to idle? 
            if (m_owner->GetBody().GetSpeed() == 0.0f)
            {
                ChangeState( STATE_Idle );
            }
EndStateMachine	
}

void Player::ProcessKeyDown(MSG_Object* msg)
{
    // Update the state of s_aKeys[] by adding the KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK 
    // mask only if the key is not down
    int mappedEvent = msg->GetIntData();
//L    if( FALSE == IsKeyDown(m_aKeys[mappedEvent]) )
    {
        m_aKeys[ mappedEvent ] = KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
        ++m_cKeysDown;
    }
	/*if( mappedEvent == PLAYER_MOVE_BACKWARD )
		m_aKeys[ PLAYER_MOVE_FORWARD ] &= ~KEY_IS_DOWN_MASK;*/
}

void Player::ProcessKeyUp(MSG_Object* msg)
{
    // Update the state of s_aKeys[] by removing the KEY_IS_DOWN_MASK mask. 
    int mappedEvent = msg->GetIntData();
//	if( mappedEvent != PLAYER_MOVE_FORWARD )
		 m_aKeys[ mappedEvent ] &= ~KEY_IS_DOWN_MASK;

	m_aKeys[ mappedEvent ] &= ~KEY_WAS_DOWN_MASK;
    --m_cKeysDown;
}

void Player::ProcessMovement()
{
 //   if (WasKeyDown(m_aKeys[PLAYER_MOVE_FORWARD]))
	if (IsKeyDown(m_aKeys[PLAYER_MOVE_FORWARD]))
    {
        m_acceleration += accelerationForward;
        if (m_acceleration > accelerationMax)
            m_acceleration = accelerationMax;
    }
    if (IsKeyDown(m_aKeys[PLAYER_MOVE_BACKWARD]))
    {
        m_acceleration -= accelerationBackward;
        if (m_acceleration < -accelerationMax)
            m_acceleration = -accelerationMax;
    }

    if (IsKeyDown(m_aKeys[PLAYER_ROTATE_LEFT]))
    {
        D3DXVECTOR3 dir;
        dir = m_owner->GetBody().GetDir();
        float angle = atan2(dir.z, dir.x);
        if (IsKeyDown(m_aKeys[PLAYER_JOG_MODIFIER]))
		{
	        angle += speedRotateJog;
		}
		else
		{
	        angle += speedRotate;
		}
        dir.x = cos(angle);     dir.z = sin(angle);
        m_owner->GetBody().SetDir(dir);
    }
    if (IsKeyDown(m_aKeys[PLAYER_ROTATE_RIGHT]))
    {
        D3DXVECTOR3 dir;
        dir = m_owner->GetBody().GetDir();
        float angle = atan2(dir.z, dir.x);
        if (IsKeyDown(m_aKeys[PLAYER_JOG_MODIFIER]))
		{
	        angle -= speedRotateJog;
		}
		else
		{
	        angle -= speedRotate;
		}
        dir.x = cos(angle);     dir.z = sin(angle);
        m_owner->GetBody().SetDir(dir);
    }

    // Update the velocity. 
    float velocity = m_owner->GetBody().GetSpeed();
    // This is the auto decelerate when the keys have been released. 
    if (!IsKeyDown(m_aKeys[PLAYER_MOVE_FORWARD]) && !IsKeyDown(m_aKeys[PLAYER_MOVE_BACKWARD]))
    {
		if (velocity > 0.0f)
		{
			m_acceleration -= accelerationCoast;
			if (m_acceleration < -accelerationMax)
				m_acceleration = -accelerationMax;
		    velocity += m_acceleration;
            if (velocity < 0.0f)
			{
                velocity = 0.0f;
    			m_acceleration = 0.0f;
			}
		}
		else if (velocity < 0.0f)
		{
			m_acceleration += accelerationCoast;
			if (m_acceleration > accelerationMax)
				m_acceleration = accelerationMax;
		    velocity += m_acceleration;
            if (velocity > 0.0f)
			{
                velocity = 0.0f;
    			m_acceleration = 0.0f;
			}
		}
    }
	else
	{
	    velocity += m_acceleration;
	}
#if 0	// allow player to move backwards 
    if (velocity < 0.0f)
        velocity = 0.0f;
#endif
    if (IsKeyDown(m_aKeys[PLAYER_JOG_MODIFIER]))
    {
        if (velocity > 2 * m_owner->GetMovement().GetAirSpeed())
            velocity = 2 * m_owner->GetMovement().GetAirSpeed();
		// don't allow players to run backwards; only allow backwards walking 
        if (velocity < -m_owner->GetMovement().GetLandSpeed())
            velocity = -m_owner->GetMovement().GetLandSpeed();
    }
    else
    {
        if (velocity > m_owner->GetMovement().GetAirSpeed())
            velocity = m_owner->GetMovement().GetAirSpeed();
        if (velocity < -m_owner->GetMovement().GetLandSpeed())
            velocity = -m_owner->GetMovement().GetLandSpeed();
    }
    m_owner->GetBody().SetSpeed(velocity);
	
	D3DXVECTOR3 dir = m_owner->GetBody().GetDir();
	m_owner->GetMovement().SetDirection( m_owner->GetBody().GetDir());

 //   // Move the player by setting m_target. We set it farther than we can reach 
 //   // in one frame. 
 //   // The other approach would be to change the Movement code so it moves in 
 //   // m_dir instead of towards m_target. 
 //   D3DXVECTOR3 dir = m_owner->GetBody().GetDir();
 //   dir *= velocity * 200.f;            // set it far enough away that we won't reach it. 

 //   D3DXVECTOR3 target = m_owner->GetBody().GetPos();
 //   target += dir;
 //   m_owner->GetMovement().SetTarget(target);
	//m_owner->GetMovement().SetTarget(target);
}
