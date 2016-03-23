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
#include "PlaneType1.h"
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
	STATE_MoveToRandomTarget
};

enum SubstateName {
	SUBSTATE_Starting,
	SUBSTATE_TakeOff,
	SUBSTATE_Flight,
	SUBSTATE_Landing,
	SUBSTATE_SlowingDown
};


PlaneType1::PlaneType1( GameObject & object ) : 
	StateMachine( object )
{

}


PlaneType1::~PlaneType1( )
{
}


bool PlaneType1::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses
	OnMsg( MSG_Reset )
		ResetStateMachine();


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			ChangeStateDelayed( 1.0f, STATE_Idle );


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			ChangeStateDelayed( RandDelay( 1.0f, 5.0f ), STATE_MoveToRandomTarget );

        OnUpdate

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_MoveToRandomTarget )

		OnEnter
			ChangeSubstate( SUBSTATE_Starting );

        OnMsg( MSG_CollideEnv )
            ChangeState( STATE_Idle );

			DeclareSubstate( SUBSTATE_Starting )
				DeclareSubstateFloat( Scale1 );
				OnEnter
					Scale1 = .05f;
					m_owner->GetMovement().SetLandSpeed();
					ChooseDir();
					D3DXVec3Scale( &m_vTarget, &m_vDir, Scale1 );
					m_vTarget = m_owner->GetBody().GetPos()+  m_vTarget;
					m_vTarget.y = 0.f;
					m_owner->GetMovement().SetTarget(m_vTarget );
				OnMsg( MSG_Arrived )
					ChangeSubstate( SUBSTATE_TakeOff );
			
			DeclareSubstate(SUBSTATE_TakeOff)
				OnEnter
					m_owner->GetMovement().SetAirSpeed();
					D3DXVec3Scale( &m_vTarget, &m_vDir, .2f );
					m_vTarget = m_owner->GetBody().GetPos()+  m_vTarget;
					m_vTarget.y = RangedRand(0.f, 6.f);
					m_owner->GetMovement().SetTarget(m_vTarget );
				OnMsg( MSG_Arrived )
					ChangeSubstate( SUBSTATE_Flight );
				

			DeclareSubstate(SUBSTATE_Flight)
				OnEnter
					D3DXVec3Scale( &m_vTarget, &m_vDir, .5f );
					m_vTarget = m_owner->GetBody().GetPos()+  m_vTarget;
					m_vTarget.y = m_owner->GetBody().GetPos().y;
					m_owner->GetMovement().SetTarget( m_vTarget );
				OnMsg( MSG_Arrived )
					ChangeSubstate( SUBSTATE_Landing );
				OnTimeInSubstate(2.f)
					ChangeSubstate( SUBSTATE_Landing );

			DeclareSubstate(SUBSTATE_Landing)
				OnEnter
					D3DXVec3Scale( &m_vTarget, &m_vDir, .2f );
					m_vTarget = m_owner->GetBody().GetPos()+  m_vTarget;
					m_vTarget.y = 0.f;
					m_owner->GetMovement().SetTarget( m_vTarget );
				OnMsg( MSG_Arrived )
					ChangeSubstate( SUBSTATE_SlowingDown );

			DeclareSubstate(SUBSTATE_SlowingDown)
				OnEnter
					m_owner->GetMovement().SetLandSpeed();
					D3DXVec3Scale( &m_vTarget, &m_vDir, .05f );
					m_vTarget = m_owner->GetBody().GetPos()+  m_vTarget;
					m_vTarget.y = 0.f;
					m_owner->GetMovement().SetTarget( m_vTarget );
				OnMsg( MSG_Arrived )
					ChangeState( STATE_Idle );

EndStateMachine
}

void PlaneType1::ChooseDir()
{
	m_vTarget.x = RangedRand(0.f, 25.f);
	m_vTarget.z = RangedRand(0.f, 25.f);
	m_vTarget.y = 0;
	D3DXVECTOR3 Pos = m_owner->GetBody().GetPos();
	m_vDir = m_vTarget - Pos;
}
