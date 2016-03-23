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

#pragma once

#include "statemch.h"

#define VISUALIZENPCVISION  0
#define fCollRadiusPlane .6f

#if VISUALIZENPCVISION
#include "LineNode.h"
#endif

class Plane : public StateMachine
{
public:

	Plane( GameObject & object );
	~Plane( );

private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

    bool CanSeePlayer();
	//Put member variables or functions here
	objectID m_curTarget;

	D3DXVECTOR3 m_LineOfSightSource, m_LineOfSightTarget;
    bool        m_bSeePlayer;

#if VISUALIZENPCVISION
	std::tr1::shared_ptr<LineNode>	m_pNPCLineOfSight;
#endif
};
