#pragma once
#include "FSUtils.h"

namespace FS
{
	// templated class to store callbacks set/get functions for FSObject derived items 
	// this one gets fancy, but basically we are keeping a pointer to two distinct functions
	// the first is the Set<whatever> function and the 2nd is the Get<whatever> function
	// this allows us to not only modify variables, but allows the variables to be
	// validated by the object class. 
	// example SetName(char* n) { // code here to make sure it is only 8 characters long };
	// example SetWidth(int w)  { // code here to make sure it is between 10 and 400 };
	template <class O>
	class  CCallback
	{
	private:
		void (O::*fptPut)(stringc);
		stringc(O::*fptGet)();
		O* Objptr;
	public:
		// class constructor
		CCallback(O* obj, void(O::*fpt1)(stringc), stringc(O::*fpt2)(void))
		{
			Objptr = obj; fptPut = fpt1; fptGet = fpt2;
		};

		// class destructor
		virtual ~CCallback() { };

		// set property
		virtual void setProperty(stringc data) { if (fptPut) (*Objptr.*fptPut)(data); };

		// get property
		virtual stringc getProperty() { return (*Objptr.*fptGet)(); };
	};


	// structure used by FireStorm editor for tracking variable data 
	class  varData
	{
	public:
		stringc	vName;			// variable name				example	m_Id
		stringc	vType;			// variable type				example	int
		stringc	vControl;		// variable control type		example D_INT
		bool	vAccess;		// variable access				example true
		void*	vCallback;		// variable callback			used to validate data
		stringc	vDescription;	// description of variable		example this is the unique id of the object
		stringc	vComm;			// text to pass back to user	example Variable Modified
		stringc vMin;			// the minimum value, in the same format as the expected value
		stringc vMax;			// the maximum value, in the same format as the expected value

		varData();					// class constructor
		~varData();					// class destructor
		void initialize();			// set all variables to a known value
		bool cleanup();				// cleanup whatever ememory mess we made
		void setComm(stringc c);	// set the comm message
		void clearComm();			// clear the comm message
	};

	class FSPropertyManager
	{
	public:
		// the list of varData structures
		list<varData*> m_List;

		FSPropertyManager(void) {}
		virtual ~FSPropertyManager(void) {}

		virtual void initialize();	// set all variables to a known value
		virtual bool create();		// dual creation allows for better error handling
		virtual bool cleanup();		// cleanup whatever memory mess we made

		virtual stringc	 getVariable(stringc variable);					// get variable value in string format
		virtual stringc	 getVariable(int index);					// get variable value in string format
		virtual bool	 setVariable(stringc variable, stringc value);	// set variable value via string format
		virtual void	 add(varData* data);							// add a variable to the list
		virtual void	 remove(varData* data);							// delete a variable from the list
		varData*		 getVarData(stringc variable);

	};

} // end namespace