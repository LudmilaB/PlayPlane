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


#if VISUALIZENPCVISION
#include "LineNode.h"
#endif

class PlaneType1 : public StateMachine
{
public:

	PlaneType1( GameObject & object );
	~PlaneType1( );

private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

	void ChooseDir();
	objectID m_curTarget;
	D3DXVECTOR3 m_vTarget;
	D3DXVECTOR3 m_vDir;

#if VISUALIZENPCVISION
	std::tr1::shared_ptr<LineNode>	m_pNPCLineOfSight;
#endif
};
