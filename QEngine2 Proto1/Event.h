#ifndef EVENT_H
#define EVENT_H

class Game;

class Event
{
protected:
	Game* engine;

public:
	Event();
	Event(Game* eng);
	virtual void EventHandler();
};

#endif