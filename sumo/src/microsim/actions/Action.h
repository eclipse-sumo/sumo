#ifndef Action_h
#define Action_h

class MSNet;

class Action {
public:
	Action();
	virtual ~Action();
	virtual void execute(/*MSNet &*/) = 0;
};

#endif
