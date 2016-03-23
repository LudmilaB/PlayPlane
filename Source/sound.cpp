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

/* 
    Two ways to add support for a new sound effect event: 

    1. Modify STATE_Idle below to listen for specific events. 

    2. Explicitly send a MSG_PlaySound event (from elsewhere in the game): 

            g_database.SendMsgFromSystem(MSG_PlaySound, 1.0f);

       The float value get converted to an int, and then is used to index 
       both the szSounds[] and m_apSounds[] arrays. 
 */

#include "DXUT.h"
#include "DXUT/SDKmisc.h"
#include "sound.h"
extern CSoundManager g_DSound;          // DirectSound class

#define NUM_ELEMENTS(a)         (sizeof(a) / sizeof(a[0]))

WCHAR *szSounds[] = 
{
//	L"TOCCATA.wav",
	L"Paul_Mauria_Love_Is_Blue.wav",
    L"metalplate.wav"
};


//Add new states here
enum StateName
{
    STATE_Initialize,   
    STATE_Idle
};

//Add new substates here
enum SubstateName
{
    //empty
};


Sound::Sound( GameObject & object ) : 
    StateMachine( object )
{
    WCHAR sPath[ MAX_PATH ];
    HRESULT hr;
    for (int i = 0; i < NUM_ELEMENTS(m_apSounds); ++i)
    {
        hr = DXUTFindDXSDKMediaFileCch( sPath, MAX_PATH, szSounds[i] );
        if( FAILED( hr ) )
            StringCchCopy( sPath, MAX_PATH, szSounds[i] );

        hr = g_DSound.Create( & m_apSounds[i], sPath, DSBCAPS_CTRLVOLUME );
        if( FAILED( hr ) )
        {
            WCHAR sz[2048];
            wsprintf(sz, L"%s not found; continuing without sound.\n", szSounds[i]);
            OutputDebugString(sz);
        }
    }
}


Sound::~Sound()
{
    for (int i = 0; i < NUM_ELEMENTS(m_apSounds); ++i)
    {
        SAFE_DELETE(m_apSounds[i]);
    }
}


bool Sound::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

    //Global message responses
    OnMsg( MSG_Reset )
        ResetStateMachine();


    ///////////////////////////////////////////////////////////////
    DeclareState( STATE_Initialize )

        OnEnter
		    m_apSounds[0]->Play( 0, DSBPLAY_LOOPING, (LONG) DSBVOLUME_MAX );
            ChangeState(STATE_Idle);
	

    ///////////////////////////////////////////////////////////////
    DeclareState( STATE_Idle )

        OnEnter
            ;

        OnMsg( MSG_PlaySound )
            int i = (int) msg->GetFloatData();
            if (0 <= i && i < NUM_ELEMENTS(m_apSounds) && m_apSounds[i] )
                m_apSounds[i]->Play( 0, 0, (LONG) DSBVOLUME_MAX );


EndStateMachine
}
