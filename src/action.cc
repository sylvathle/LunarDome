#include "action.hh"

MyActionInitialization::MyActionInitialization()
{
}

MyActionInitialization::~MyActionInitialization()
{
}

void MyActionInitialization::BuildForMaster() const
{
	MyRunAction *runAction = new MyRunAction();
	SetUserAction(runAction);
}

void MyActionInitialization::Build() const
{

	MyRunAction *runAction = new MyRunAction();
	SetUserAction(runAction);
	
	MyPrimaryGenerator *generator = new MyPrimaryGenerator();
	SetUserAction(generator);

	MyEventAction *eventAction = new MyEventAction(runAction);
	SetUserAction(eventAction);

	//MySteppingAction *steppingAction = new MySteppingAction(eventAction);
	//SetUserAction(steppingAction);

}
