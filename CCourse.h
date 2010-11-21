// Class automatically generated by Dev-C++ New Class wizard

#ifndef CCOURSE_H
#define CCOURSE_H

#include <vector>
#include <QString>
#include <list>
#include "CLeg.h"

// Objects in this class represent a course for the event.
class CCourse
{
    public:
        // class constructor
        CCourse(QString& a_Data);
        // class destructor
        ~CCourse();

        static bool ValidData(QString& a_Data);
        void SetUntimedLeg(int a_Index);   
        void SetOptionalLeg(int a_Index);
        bool GetUntimedLeg(int a_Index);   
        bool GetOptionalLeg(int a_Index);  
        QString GetLength() {return m_Length;};
        
        void CompulsoryControls(std::list<long>& a_Controls);                              
        QString GetName() {return m_Name;};
        CLeg GetLeg(int a_Index);
        int GetLegCount() {return m_Controls.size();};
        QString TextDescStr();
        QString TextSplitHdrStr();
	
    private:
        QString m_Name;
        QString m_Length;
        QString m_Climb;

        std::vector<CLeg*> m_Controls;
};

#endif // CCOURSE_H