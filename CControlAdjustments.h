#ifndef CCONTROLADJUSTMENTS_H
#define CCONTROLADJUSTMENTS_H
#include <QString>
#include <map>

class CControlAdjustments
{
public:
        // Singleton access/creation
    static CControlAdjustments* ControlAdjustments();
    ~CControlAdjustments();

    int  GetAdjustment(int a_Cn);

    void SetAllAdjustments(std::map<int,int>& a_Adjustments);
    void GetAllAdjustments(std::map<int,int>& a_Adjustments) {a_Adjustments = m_Adjustments;}


private:
    static CControlAdjustments* _instance;
    std::map<int, int> m_Adjustments;

    CControlAdjustments();
    CControlAdjustments(CControlAdjustments const&);         // Don't Implement
    void operator=(CControlAdjustments const&); // Don't implement
};

#endif // CCONTROLADJUSTMENTS_H
