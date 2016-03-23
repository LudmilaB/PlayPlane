There are 4 Type of planes in my project that can fly different routes.

"Plane" can SeekPlayer and move to random target. 
"PlaneType1" starts, takes off, flights, lands, slows down. All that in one direction. Then it chooses another direction.	
"PlaneType2" flights on straights lines or circles.
"PlaneInFormation" can flight in formations.

Like always -  it was difficult to start, but now I can't say what was really difficult.
Now I feel that can do the project better, but no time left. 

Features implemented:
Substates  -  PlaneType1.cpp
Global Message Response  -  OnMsg(MSG_LeaderDirChanged)   PlaneInFormation.cpp,  Ln.8
data passed in msg -  g_database.SendMsgFromSystem(MSG_PlaySound, MSG_Data( 1.0f )  Plane, Ln.135
OnTimeInSubstate  -     PlaneType1.cpp,  Ln.127
persistent state variable  DeclareStateInt( CurTarget );-    Plane.cpp,  Ln.119
ChangeStateDelayed - PlaneType1.cpp,  Ln.8
PushStateMachine - world.cpp  Ln.217
OnTimeInState - PlaneType2.cpp,  Ln.68
PopState() - Plane.cpp, Ln.137
SetTimerState - Plane.cpp, Ln.160

Extra credit:
I've implemented formation. You can see 3 planes flying together - NPC5, NPC6, NPC7 