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
#include "PlaneType3.h"
#include "collision.h"
#include "database.h"
#include "game.h"
#include "movement.h"
#include "body.h"
#include "WorldCollData.h"
#include "Stars.h"

//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state
	STATE_Idle,
    STATE_SeekPlayer,
	STATE_GO,
	STATE_BACK
};

//Add new substates here
enum SubstateName {
	//empty
};


PlaneType3::PlaneType3( GameObject & object ) : 
	StateMachine( object )
,   m_bSeePlayer(false)
{
#if VISUALIZENPCVISION
	std::tr1::shared_ptr<LineNode>	pLineNode(new LineNode(DXUTGetD3D9Device()));
	m_pNPCLineOfSight = pLineNode;
    g_pScene->AddChild(std::tr1::shared_ptr<LineNode>(m_pNPCLineOfSight));
#endif
}


PlaneType3::~PlaneType3( )
{
}


bool PlaneType3::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses
	OnMsg( MSG_Reset )
		ResetStateMachine();


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			ChangeStateDelayed( 15.0f, STATE_SeekPlayer );


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			ChangeStateDelayed( 1.0f, STATE_SeekPlayer );

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_SeekPlayer )
		OnEnter
			m_owner->GetBody().SetSpeed( 1.5f );
			if (CanSeePlayer())
			{
				GameObject* go = g_database.Find( g_objectIDPlayer );
				if (go)
				{
					D3DXVECTOR3 target = go->GetBody().GetPos();
					m_owner->GetMovement().SetTarget( target );
					ChangeState( STATE_GO );
				}
			}
			else
			{
				m_owner->GetMovement().ComputePath( ); 
				ChangeState( STATE_GO );
			}

		OnMsg( MSG_CollideMoby )
            g_database.SendMsgFromSystem(MSG_PlaySound, MSG_Data( 1.0f ));
			ChangeState( STATE_BACK );

		OnMsg( MSG_CollideEnv )
			ChangeState( STATE_BACK );
///////////////////////////////////////////////////////////////
	DeclareState( STATE_GO )
		OnMsg( MSG_CollideMoby )
            g_database.SendMsgFromSystem(MSG_PlaySound, MSG_Data( 1.0f ));
			/*GameObject* go = g_database.Find( g_objectIDPlayer );
			D3DXVECTOR3 pos1= go->GetBody().GetPos();
			D3DXVECTOR3 pos2 =  m_owner->GetBody().GetPos();

			g_pStar->Init( (pos1 + pos2)/2.f );*/
			ChangeState( STATE_BACK );
		OnMsg( MSG_CollideEnv )
			ChangeState( STATE_BACK );
		OnMsg( MSG_Arrived )
			ChangeState( STATE_Idle );

///////////////////////////////////////////////////////////////
	DeclareState( STATE_BACK )
		OnEnter
			D3DXVECTOR3 dir = m_owner->GetBody().GetDir();
			m_owner->GetBody().SetSpeed( -1.f );
			m_owner->GetMovement().SetDirection( dir );
			ChangeStateDelayed( 1.1f, STATE_Idle );

EndStateMachine
}


bool PlaneType3::CanSeePlayer()
{
    m_LineOfSightSource = m_owner->GetBody().GetPos();
    m_LineOfSightSource.y += 0.4f;

	objectID m_curTarget = m_owner->GetID();
    GameObject* goPlayer = g_database.Find(g_objectIDPlayer);
//L	GameObject* goPlayer = g_database.Find(--m_curTarget);
    if (goPlayer)
    {
        // OBJECT_Player is found. Can we see it? 
        m_LineOfSightTarget = goPlayer->GetBody().GetPos();
        m_LineOfSightTarget.y += 0.4f;


        // Check the line segment between the NPC and the player. 
        CollLine cl;
        cl.Set(&m_LineOfSightSource, &m_LineOfSightTarget);
        gCollOutput.Reset();
        m_bSeePlayer = !g_pWorldCollData->CollideLineVsWorld(cl);

        // Can't see the player?
        if (!m_bSeePlayer)
        {
            // On the line segment between the NPC and the player, mark the 
            // intersection with the wall nearest the NPC. 
            m_LineOfSightTarget = gCollOutput.point;
        }
    }
    else
    {
        // If OBJECT_Player isn't found, then we don't see the player. 
        m_bSeePlayer = false;
        m_LineOfSightTarget = m_LineOfSightSource;
    }

#if VISUALIZENPCVISION
    if (m_bSeePlayer)
    {
        m_pNPCLineOfSight->SetVertex(0, m_LineOfSightSource.x, m_LineOfSightSource.y, m_LineOfSightSource.z, 0xffff0000);
        m_pNPCLineOfSight->SetVertex(1, m_LineOfSightTarget.x, m_LineOfSightTarget.y, m_LineOfSightTarget.z, 0xffff0000);
    }
    else
    {
        m_pNPCLineOfSight->SetVertex(0, m_LineOfSightSource.x, m_LineOfSightSource.y, m_LineOfSightSource.z, 0xffffffff);
        m_pNPCLineOfSight->SetVertex(1, gCollOutput.point.x, gCollOutput.point.y, gCollOutput.point.z, 0xffffffff);
    }
    m_pNPCLineOfSight->Update(0.0f);

#endif
    return m_bSeePlayer;
}
