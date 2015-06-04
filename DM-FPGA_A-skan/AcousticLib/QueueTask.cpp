/********************************************************************
	author:		Victor Mariinschii
	created:	2011/05/23
	filename: 	Task.cpp
	purpose:	Thread synchronization
*********************************************************************/

#include "QueueTask.h"

void QueueTask::setElement(int cmd, const int data /* = -1 */)
{
	bool need2Wait;
	tx.Cmd = cmd;
	tx.Data = data;
	lock();
	qTie.push(tx);
	need2Wait = (qTie.size() > QUEUE_MAX_SIZE);
	unlock();
	if (need2Wait)	// defense
		Sleep(40);
}

void QueueTask::receiveElement()
{
	lock();
	if (!qTie.empty())
	{
		rx = qTie.front();
		qTie.pop();
	}
	unlock();
}
