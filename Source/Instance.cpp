//-----------------------------------------------------------------------------
// File: Instance.cpp
//
// Desc: Defines the instance base class. 
//       It holds an instance of something to draw and a transform matrix 
//       that specifies where to draw that something. 
//       This class is intended to be a field within a GameObject class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#include "DXUT.h"
#include "Instance.h"

CInstance::CInstance(GameObject& owner) : 
	m_owner(&owner)
,	m_pNode()
{
}

CInstance::~CInstance()
{
}
