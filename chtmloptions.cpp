#include "chtmloptions.h"
#include <QSettings>

CHtmlOptions::CHtmlOptions()
{
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("HTML Options");
    m_WholePage = settings.value("WholePage",false).toBool();
    m_ShowNameTooltip = settings.value("ShowToolTip",true).toBool();
    m_FastestCss = settings.value("FastestCSS", "color:blue").toString();
    m_FastCss = settings.value("FastCSS", "background-color:LightCyan").toString();
    m_SlowCss = settings.value("SlowCSS", "background-color:Tomato").toString();
    m_ElapsedCss = settings.value("ElapsedCSS", "").toString();
    m_LegCss = settings.value("LegCSS", "").toString();
    m_BehindCss = settings.value("BehindCSS", "").toString();
    m_HeaderCss = settings.value("HeaderCSS", "").toString();
    settings.endGroup();
}

CHtmlOptions::~CHtmlOptions()
{
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("HTML Options");
    settings.setValue("WholePage",m_WholePage);
    settings.setValue("ShowToolTip",m_ShowNameTooltip);
    settings.setValue("FastestCSS", m_FastestCss);
    settings.setValue("FastCSS", m_FastCss);
    settings.setValue("SlowCSS", m_SlowCss);
    settings.setValue("ElapsedCSS", m_ElapsedCss);
    settings.setValue("LegCSS", m_LegCss);
    settings.setValue("BehindCSS", m_BehindCss);
    settings.setValue("HeaderCSS", m_HeaderCss);
    settings.endGroup();
}
