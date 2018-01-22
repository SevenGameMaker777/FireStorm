

#include "FS_BlackBoard.h"

#include "FSLevel.h"



namespace FS

{

	// set all variables to a known value

	void FSBlackBoard::initialize()

	{

		m_Level = 0;

		m_List.clear();

	}



	// dual creation allows for better error handling

	bool FSBlackBoard::create(FSLevel* level)

	{

		// remember this

		m_Level = level;



		// everything went fine

		return true;

	}



	// cleanup whatever memory mess we made

	bool FSBlackBoard::cleanup()

	{

		// forget this

		m_Level = 0;



		// always rturn false from a cleanup function

		return false;

	}



	void FSBlackBoard::addEvent(FSBlackBoardEntry* event)

	{

		m_List.insert(event->getName(), event);

	}



	void FSBlackBoard::setValue(int who, stringc name, bool value)

	{

		map<stringc, FSBlackBoardEntry*>::Node *n = m_List.find(name);

		if (n)

		{

			FSBlackBoardEntry* e = n->getValue();

			if (e)

			{

				e->setValue(value);

				e->setSource(who);

			}

		}

	}



	bool FSBlackBoard::getValue(stringc name)

	{

		map<stringc, FSBlackBoardEntry*>::Node *n = m_List.find(name);

		if (n)

		{

			FSBlackBoardEntry* e = n->getValue();

			if (e) return e->getValue();

		}

		return false;

	}



	// do whatever this class does each frame

	void FSBlackBoard::preFrame(const float &elapsedtime)

	{

		// if the list is emtpy then bail

		if (m_List.empty()) return;



		// run through the list, if a variable changed to true, send a message

		map<stringc, FSBlackBoardEntry*>::Iterator it;



		for (it = m_List.getIterator(); !it.atEnd(); it++)

		{

			if (it.getNode()->getValue()->getValue() == true)

			{

				if (it.getNode()->getValue()->getHasFired() == false)

				{

					it.getNode()->getValue()->setHasFired(true);

					it.getNode()->getValue()->setSource(it.getNode()->getValue()->getSource());

					FSObjectMessage *msg = it.getNode()->getValue();

					getLevel()->sendMessage(msg->getType(),msg->getType2(),msg->getSource(),msg->getDest(),msg->getRTime(),msg->getDTime(),msg->getData());

					stringc text("Object # ");

					text += msg->getSource();

					text += " set the BlackBoard variable ";

					text += it.getNode()->getValue()->getName();

					getLevel()->sendMessage(FSMT::LEVELTEXT, FSMT::MSG_NULL, msg->getSource(), FSID_LEVEL_MESSAGE, 0, 0, text);

					if (it.getNode()->getValue()->getDestroyAfterFired())

					{

						// destroy it!!!!!!!!!!

					}

					else

						if (it.getNode()->getValue()->getOneTimeEvent())

						{

							it.getNode()->getValue()->setValue(false);

							it.getNode()->getValue()->setHasFired(false);

						}

				}

			}

		}

	}



} // end namespace