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

#pragma once

#include "statemch.h"

#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

enum PlayerEvents
{
    PLAYER_ROTATE_LEFT = 0,
    PLAYER_ROTATE_RIGHT,
    PLAYER_MOVE_FORWARD,
    PLAYER_MOVE_BACKWARD,
    PLAYER_JOG_MODIFIER,
    PLAYER_MAX_KEYS,
    PLAYER_UNKNOWN = 0xFF
};

LRESULT MapPlayerMessageFromWindowsMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

class Player : public StateMachine
{
public:

	Player( GameObject & object );
	~Player( void ) {}


private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

    void ProcessKeyDown(MSG_Object* msg);
    void ProcessKeyUp(MSG_Object* msg);
    void ProcessMovement();

    bool IsKeyDown( BYTE key ) const { return( (key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK ); }
	bool WasKeyDown( BYTE key ) const { return( (key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK ); }

    int m_cKeysDown;
    BYTE m_aKeys[PLAYER_MAX_KEYS];  // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK

    float m_acceleration;
};
