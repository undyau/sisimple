#ifndef CHTMLOPTIONS_H
#define CHTMLOPTIONS_H

#include <QString>

class CHtmlOptions
{
public:
    CHtmlOptions();
    ~CHtmlOptions();
    bool getWholePage() {return m_WholePage;}
    bool getShowNameTooltip() {return m_ShowNameTooltip;}
    QString getFastestCss() {return m_FastestCss;}
    QString getFastCss() {return m_FastCss;}
    QString getSlowCss() {return m_SlowCss;}
    QString getElapsedCss() {return m_ElapsedCss;}
    QString getLegCss() {return m_LegCss;}
    QString getBehindCss() {return m_BehindCss;}
    QString getHeaderCss() {return m_HeaderCss;}

    void setWholePage(bool a_WholePage) {m_WholePage = a_WholePage;}
    void setShowNameTooltip(bool a_ShowNameTooltip) {m_ShowNameTooltip = a_ShowNameTooltip;}
    void setFastestCss(QString a_FastestCss) {m_FastestCss = a_FastestCss;}
    void setFastCss(QString a_FastCss) {m_FastCss = a_FastCss;}
    void setSlowCss(QString a_SlowCss) {m_SlowCss = a_SlowCss;}
    void setElapsedCss(QString a_ElapsedCss) {m_ElapsedCss = a_ElapsedCss;}
    void setLegCss(QString a_LegCss) {m_LegCss = a_LegCss;}
    void setBehindCss(QString a_BehindCss) {m_BehindCss = a_BehindCss;}
    void setHeaderCss(QString a_HeaderCss) {m_HeaderCss = a_HeaderCss;}

private:
    bool m_WholePage;
    bool m_ShowNameTooltip;
    QString m_FastestCss;
    QString m_FastCss;
    QString m_SlowCss;
    QString m_ElapsedCss;
    QString m_LegCss;
    QString m_BehindCss;
    QString m_HeaderCss;
};

#endif // CHTMLOPTIONS_H
