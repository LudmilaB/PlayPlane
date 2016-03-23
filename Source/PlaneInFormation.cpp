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
#include "PlaneInFormation.h"
#include "collision.h"
#include "database.h"
#include "game.h"
#include "movement.h"
#include "body.h"
#include "WorldCollData.h"


enum StateName {
	STATE_Initialize,
	STATE_Following,
};

enum SubstateName {
	
};


PlaneInFormation::PlaneInFormation( GameObject & object, GameObject & Leader, D3DXVECTOR3 &offset ) : 
	StateMachine( object )
{
	m_bLeader = &Leader;
	m_offset = offset;
	m_owner->GetBody().SetPos(m_bLeader->GetBody().GetPos() + m_offset);
#if VISUALIZENPCVISION
	std::tr1::shared_ptr<LineNode>	pLineNode(new LineNode(DXUTGetD3D9Device()));
	m_pNPCLineOfSight = pLineNode;
    g_pScene->AddChild(std::tr1::shared_ptr<LineNode>(m_pNPCLineOfSight));
#endif
}


PlaneInFormation::~PlaneInFormation( )
{
}


bool PlaneInFormation::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses
	OnMsg( MSG_Reset )
		ResetStateMachine();


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )

		OnEnter
			Follow();
			m_owner->GetBody().SetSpeed( m_bLeader->GetBody().GetSpeed() );
			ChangeStateDelayed( 1.0f, STATE_Following );


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Following )

		OnEnter		

        OnMsg(MSG_LeaderDirChanged)
			Follow();
		OnMsg(MSG_LeaderSpeedChanged)
			float speed = m_bLeader->GetBody().GetSpeed();
			m_owner->GetBody().SetSpeed( speed );
		OnUpdate
			Follow();


EndStateMachine
}

void PlaneInFormation::Follow()
{
	D3DXVECTOR3 Dir = m_bLeader->GetBody().GetDir();
	m_owner->GetMovement().SetDirection( Dir );
	D3DXVECTOR3 offset(0,0,0);
	/*offset.x = m_offset.x *Dir.x;  
	offset.y = m_offset.y *Dir.y; 
	offset.z = m_offset.z *Dir.z; 
	D3DXVECTOR3 Pos = m_bLeader->GetBody().GetPos()-offset;*/
	D3DXVECTOR3 Pos = m_bLeader->GetBody().GetPos();
	Pos.x -= m_offset.x +Dir.x;  
	Pos.y -= m_offset.y +Dir.y; 
	Pos.z -= m_offset.z +Dir.z; 
	m_owner->GetBody().SetPos(Pos);
}
