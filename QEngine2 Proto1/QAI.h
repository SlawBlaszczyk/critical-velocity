#ifndef QAI_H
#define QAI_H

class Game;

class QAI
{
	Game* engine;
public:

	QAI(Game* eng);
	~QAI();
	void SeekNewTarget();
	void CheckTargetReached();
};

#endif