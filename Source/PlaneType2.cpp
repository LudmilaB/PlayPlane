/*
    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 

 * Portions copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */


#include "DXUT.h"
#include "PlaneType2.h"
#include "collision.h"
#include "database.h"
#include "game.h"
#include "movement.h"
#include "body.h"
#include "WorldCollData.h"
#include "Utilities.h"


enum StateName {
	STATE_Initialize,
	STATE_Idle,
	STATE_MoveToRandomTarget,
	STATE_Circle
};

enum SubstateName {
};


PlaneType2::PlaneType2( GameObject & object) : 
	StateMachine( object )
{
	m_Angle = 0;
}


PlaneType2::~PlaneType2( )
{
}


bool PlaneType2::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses
	OnMsg( MSG_Reset )
		ResetStateMachine();


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			g_database.SendMsgFromSystem( m_owner, MSG_LeaderSpeedChanged);
		OnTimeInState(1)
				ChangeState( STATE_Idle );


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			g_database.SendMsgFromSystem( m_owner, MSG_LeaderSpeedChanged);
			ChangeStateDelayed( RandDelay( 1.0f, 5.0f ), STATE_MoveToRandomTarget );

        OnUpdate

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_MoveToRandomTarget )

		OnEnter
			m_owner->GetMovement().SetAirSpeed();
			g_database.SendMsgFromSystem( MSG_LeaderSpeedChanged);
			SetRandomTarget();

        OnMsg( MSG_CollideEnv )
            ChangeState( STATE_Idle );
		OnMsg( MSG_Arrived )
			ChangeState( STATE_Circle );
		OnUpdate
			
///////////////////////////////////////////////////////////////
	DeclareState( STATE_Circle )
		OnEnter
		   Circle();
		 OnUpdate
			Circle();
			if (m_Angle > 3*D3DX_PI)
			{				
				ChangeState( STATE_MoveToRandomTarget );
				m_Angle = 0;
			}
           	
EndStateMachine
}

void PlaneType2::SetRandomTarget()
{
	m_vTarget.x = RangedRand(0.f, 25.f);
	m_vTarget.z = RangedRand(0.f, 25.f);
	m_vTarget.y = RangedRand(0.f, 5.f);
	m_owner->GetMovement().SetTarget( m_vTarget );
//	g_database.SendMsgFromSystem( m_owner,MSG_LeaderDirChanged);
	g_database.SendMsgFromSystem(MSG_LeaderDirChanged);
}

void PlaneType2::Circle()
{
	D3DXVECTOR3 dir = m_owner->GetBody().GetDir();
	m_Angle += D3DX_PI/180.f;
	dir.x = sin( m_Angle );
	dir.z = cos( m_Angle );
	dir.y = 0;
	m_owner->GetMovement().SetDirection(dir);
	g_database.SendMsgFromSystem(MSG_LeaderDirChanged);
}
