// Class automatically generated by Dev-C++ New Class wizard

#ifndef CLEGSTAT_H
#define CLEGSTAT_H
#include <wx/datetime.h>
#include "CPunch.h"

class CResult;

// lightweight class holding statistical information for a single leg for a single runner
class CLegStat
{
	public:
		// class constructor
		CLegStat(CResult* a_Result, int a_Index, CPunch a_Punch);
		// class destructor
		~CLegStat();
		
	   CResult* m_Result;
       long m_Index; // index of leg within course
       wxTimeSpan m_ElapsedTime;
       long m_ElapsedPos;
       wxTimeSpan m_ElapsedBehind;

       wxTimeSpan m_LegTime;
       wxTimeSpan m_LegBehind; 
       CPunch m_Punch;  
       long m_LegPos;           
       
};

#endif // CLEGSTAT_H