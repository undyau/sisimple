#include "CControlAdjustments.h"

CControlAdjustments::CControlAdjustments()
{
}

CControlAdjustments::~CControlAdjustments()
{
}

CControlAdjustments* CControlAdjustments::ControlAdjustments()
{
    static CControlAdjustments instance; // Guaranteed to be destroyed.
                               // Instantiated on first use.
    return &instance;
}

void CControlAdjustments::SetAllAdjustments(std::map<int,int>& a_Adjustments)
{
    m_Adjustments = a_Adjustments;
}

int  CControlAdjustments::GetAdjustment(int a_Cn)
{
    if (m_Adjustments.find(a_Cn) != m_Adjustments.end())
        return m_Adjustments[a_Cn];

    return 0;
}
