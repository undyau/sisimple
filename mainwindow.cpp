/*
Copyright 2010 Andy Simpson

This file is part of SI Simple.

SI Simple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SI Simple is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SI Simple.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QIcon>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>
#include "Utils.h"
#include "CEvent.h"
#include <QDebug>
#include "alterdialog.h"
#include "coursesdialog.h"
#include "importsidialog.h"
#include "clockerrorsdialog.h"
#include <QInputDialog>
#include <QSettings>
#include <QCloseEvent>
#include "downloaddialog.h"
#include "htmloptionsdialog.h"
#include "version.h"
#include "QScrollBar"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Maybe need to use Tango theme ??
    ui->setupUi(this);

    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("General");
    bool showSplits = settings.value("showSplits", false).toBool();
    bool showHtml = settings.value("showHtml", false).toBool();
    settings.endGroup();

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->textEdit->setFont(font);
    ui->textEdit->font();
    ui->textEdit_2->setFont(font);

    ui->textEdit->setReadOnly(true);
    ui->textEdit_2->setReadOnly(true);

    QWidget *viewPort = ui->textEdit->viewport();
    viewPort->setCursor(Qt::ArrowCursor);
    ui->textEdit->setViewport(viewPort);

    viewPort = ui->textEdit_2->viewport();
    viewPort->setCursor(Qt::ArrowCursor);
    ui->textEdit_2->setViewport(viewPort);
    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->textEdit_2->setContextMenuPolicy(Qt::CustomContextMenu);

    m_NewAct = new QAction(QIcon::fromTheme("document-new", QIcon(":icons/icons/document-new.svg")), tr("&New Event"), this);
    m_NewAct->setShortcuts(QKeySequence::New);
    m_NewAct->setStatusTip(tr("New Event"));
    m_NewAct->setToolTip(tr("New Event"));
    m_NewAct->setIconVisibleInMenu(true);
    ui->menu_File->addAction(m_NewAct);
    ui->mainToolBar->addAction(m_NewAct);

    m_DownloadAct = new QAction( QIcon(":icons/icons/device-siunit.svg"), tr("&Download..."), this);
    //m_DownloadAct->setShortcuts(QKeySequence::Download);
    m_DownloadAct->setStatusTip(tr("Download from SI unit"));
    m_DownloadAct->setToolTip(tr("Download SI unit"));
    m_DownloadAct->setIconVisibleInMenu(true);
    ui->menu_File->addAction(m_DownloadAct);
    ui->mainToolBar->addAction(m_DownloadAct);

    m_OpenAct = new QAction(QIcon::fromTheme("document-open", QIcon(":icons/icons/document-open.svg")), tr("&Open..."), this);
    m_OpenAct->setShortcuts(QKeySequence::Open);
    m_OpenAct->setStatusTip(tr("Open an existing CSV dump or XML results"));
    m_OpenAct->setToolTip(tr("Open an existing CSV dump or XML results"));
    m_OpenAct->setIconVisibleInMenu(true);
    ui->menu_File->addAction(m_OpenAct);
    ui->mainToolBar->addAction(m_OpenAct);

    m_SaveAct = new QAction(QIcon::fromTheme("document-save", QIcon(":icons/icons/document-save.svg")), tr("&Save..."), this);
    m_SaveAct->setShortcuts(QKeySequence::Save);
    m_SaveAct->setStatusTip(tr("Save Results"));
    m_SaveAct->setToolTip(tr("Save Results"));
    m_SaveAct->setIconVisibleInMenu(true);
    ui->menu_File->addAction(m_SaveAct);
    ui->mainToolBar->addAction(m_SaveAct);

    ui->menu_File->addSeparator();
    m_ExportMenu = ui->menu_File->addMenu(tr("&Export"));

    m_ExportAct = new QAction(tr("IOF XML(2.0.3)..."), this);
    m_ExportAct->setStatusTip(tr("Export to IOF XML(2.0.3) - for RouteGadget, AttackPoint"));
    m_ExportAct->setToolTip(tr("Export to IOF XML(2.0.3) - for Eventor"));
    m_ExportMenu->addAction(m_ExportAct);
    m_Export3Act = new QAction(tr("IOF XML(3.0)..."), this);
    m_Export3Act->setStatusTip(tr("Export to IOF XML(3.0) - for RouteGadget, AttackPoint"));
    m_Export3Act->setToolTip(tr("Export to IOF XML(3.0) - for Eventor"));
    m_ExportMenu->addAction(m_Export3Act);

    ui->menu_File->addSeparator();

    m_QuitAct = new QAction(tr("&Quit"), this);
    m_QuitAct->setShortcuts(QKeySequence::Quit);
    m_QuitAct->setStatusTip(tr("Quit"));
    m_QuitAct->setToolTip(tr("Quit"));
    ui->menu_File->addAction(m_QuitAct);

    m_RefreshAct = new QAction(QIcon::fromTheme("view-refresh", QIcon(":icons/icons/view-refresh.svg")), tr("Recalculate"), this);
    m_RefreshAct->setShortcuts(QKeySequence::Refresh);
    m_RefreshAct->setStatusTip(tr("Recalculate Results"));
    m_RefreshAct->setToolTip(tr("Recalculate"));
    m_RefreshAct->setIconVisibleInMenu(true);
    ui->menu_Results->addAction(m_RefreshAct);
    ui->mainToolBar->addAction(m_RefreshAct);

    m_FormatHtmlAct = new QAction(tr("&HTML customisation"), this);
    m_FormatHtmlAct->setStatusTip(tr("Customise HTML Appearance"));
    m_FormatHtmlAct->setToolTip(tr("Customise HTML Appearance"));
    ui->menu_Results->addAction(m_FormatHtmlAct);

    m_UseHtmlCheck = new ::QCheckBox(tr("HTML"), this);
    m_ShowSplitsCheck = new QCheckBox(tr("Splits"), this);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(m_UseHtmlCheck);
    ui->mainToolBar->addWidget(m_ShowSplitsCheck);
    m_ShowSplitsCheck->setChecked(true);

    m_EventLabel = new QLabel(tr("Event:"), this);
    m_EventText = new QLineEdit(this);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(m_EventLabel);
    ui->mainToolBar->addWidget(m_EventText);

    m_AboutAct = new QAction(tr("&About"), this);
    m_AboutAct->setStatusTip(tr("About"));
    m_AboutAct->setToolTip(tr("About"));
    ui->menuHelp->addAction(m_AboutAct);

    m_UseHtmlCheck->setChecked(showHtml);
    m_ShowSplitsCheck->setChecked(showSplits);
    CEvent::Event()->SetShowSplits(showSplits);
    CEvent::Event()->SetShowHTML(showHtml);

    connect(m_NewAct, SIGNAL(triggered()), this, SLOT(newEvent()));
    connect(m_OpenAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_SaveAct, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_ExportAct, SIGNAL(triggered()), this, SLOT(exportIOF()));
    connect(m_Export3Act, SIGNAL(triggered()), this, SLOT(exportIOF3()));
    connect(m_RefreshAct, SIGNAL(triggered()), this, SLOT(refresh()));
    connect(m_FormatHtmlAct, SIGNAL(triggered()), this, SLOT(formatHTML()));
    connect(m_QuitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_UseHtmlCheck, SIGNAL(stateChanged(int)), this, SLOT(setUseHTML(int)));
    connect(m_ShowSplitsCheck, SIGNAL(stateChanged(int)), this, SLOT(setShowSplits(int)));
    connect(m_EventText, SIGNAL(textChanged(QString)), this, SLOT(setEventTitle(QString)));
    connect(m_AboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(CEvent::Event(), SIGNAL(logMsg(QString)), ui->textEdit, SLOT(append(QString)));
    connect(CEvent::Event(), SIGNAL(updatedResultsOutput(std::vector<QString>&)), this, SLOT(showResults(std::vector<QString>&)));
    connect(ui->textEdit, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showDownloadContextMenu(const QPoint&)));
    connect(ui->textEdit_2, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showResultContextMenu(const QPoint&)));
    connect(this, SIGNAL(dnf(long)), CEvent::Event(), SLOT(dnfResult(long)));
    connect(this, SIGNAL(dsq(long)), CEvent::Event(), SLOT(dsqResult(long)));
    connect(this, SIGNAL(reinstate(long)), CEvent::Event(), SLOT(reinstateResult(long)));
    connect(ui->actionManage, SIGNAL(triggered()), this, SLOT(runcoursesdialog()));
    connect(ui->actionGuess, SIGNAL(triggered()), CEvent::Event(), SLOT(guessCourses()));
    connect(ui->actionFix_Clock_Errors, SIGNAL(triggered()), this, SLOT(runclockerrorsdialog()));
    connect(ui->actionImport, SIGNAL(triggered()), this, SLOT(importCourses()));
    connect(this, SIGNAL(importCourses(QString)), CEvent::Event(), SLOT(importCourses(QString)));
    connect(CEvent::Event(), SIGNAL(coursesGuessed()), this, SLOT(runcoursesdialog()));
    connect(ui->actionLoadSI, SIGNAL(triggered()), this, SLOT(loadSI()));
    connect(CEvent::Event(), SIGNAL(resetLog()), ui->textEdit, SLOT(clear()));
    connect(CEvent::Event(), SIGNAL(loadedSIArchive(QString)), ui->statusBar, SLOT(showMessage(QString)));
    connect(ui->actionRental_Sticks, SIGNAL(triggered()), this, SLOT(rentalStickNames()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_DownloadAct, SIGNAL(triggered()), this,SLOT(rundownloaddialog()));
    connect(CEvent::Event(), SIGNAL(exportIOF()), this, SLOT(exportIOF()));
    connect(this, SIGNAL(deleteDownload(long)), CEvent::Event(), SLOT(deleteDownload(long)));
    connect(CEvent::Event(), SIGNAL(eventNameSet(QString)), m_EventText, SLOT(setText(QString)));

    // Initialise the event
    CEvent::Event()->LoadLastEvent();
}

MainWindow::~MainWindow()
{
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("General");
    settings.setValue("showSplits", m_ShowSplitsCheck->isChecked());
    settings.setValue("showHtml", m_UseHtmlCheck->isChecked());
    settings.endGroup();

    delete m_OpenAct;
    delete ui;
}

void MainWindow::open()
{
    CEvent* oevent = CEvent::Event();
    QString file = QFileDialog::getOpenFileName(this,tr("Select event data file"), oevent->Directory(), "Result files(*.csv *.xml);;All files (* *.*)");

    if (file.isEmpty())
        return;

    oevent->SetResultsInputFile(file);
}

void MainWindow::newEvent()
{
    CEvent* oevent = CEvent::Event();
    oevent->Reset();
}

void MainWindow::save()
{
    CEvent* oevent = CEvent::Event();
    QString filter;
    if (oevent->GetShowHTML())
       filter = "HTML files (*.htm *.html)";
    else
       filter = "Text files (*.txt)";

    QString file = QFileDialog::getSaveFileName(this, "Save results to", oevent->Directory(), filter);
if (!file.isEmpty())
    CEvent::Event()->SaveResults(file);
}

void MainWindow::exportIOF()
{
    CEvent* oevent = CEvent::Event();

    QString file = QFileDialog::getSaveFileName(this, "Export results to", oevent->Directory(), tr("XML files (*.xml)"));
    if (!file.isEmpty())
        CEvent::Event()->ExportXML(file);
}

void MainWindow::exportIOF3()
{
    CEvent* oevent = CEvent::Event();

    QString file = QFileDialog::getSaveFileName(this, "Export results to", oevent->Directory(), tr("XML files (*.xml)"));
    if (!file.isEmpty())
        CEvent::Event()->ExportXML3(file);
}

void MainWindow::refresh()
{
    CEvent::Event()->RecalcResults();
}

void MainWindow::setUseHTML(int a_Value)
{
    CEvent::Event()->SetShowHTML(!!a_Value);
}

void MainWindow::setEventTitle(QString a_Title)
{
    CEvent::Event()->SetEventName(a_Title);
}

void MainWindow::setShowSplits(int a_Value)
{
    CEvent::Event()->SetShowSplits(!!a_Value);
}

void MainWindow::about()
{
QString version(VER_FILEVERSION_STR);
QMessageBox::about(this, tr("About SI Simple"),
    QString("<h2>SI Simple " + version + "</h2>"
       "<p>&copy; 2010-2021 Andy Simpson</p>"
       "<p>This is an Open Source project hosted at "
       "https://github.com/undyau/sisimple, licensed under the GPL.</p>"
       "<p>Acknowledgement and thanks to Ken Hanson and his amazing TCL program SIDResults.</p>"
       ));
}

void MainWindow::showResults(std::vector<QString>& a_Lines)
{
    ui->textEdit_2->clear();
    for (unsigned int i = 0; i < a_Lines.size(); i++)
        ui->textEdit_2->append(a_Lines[i]);
    QScrollBar *vScrollBar = ui->textEdit_2->verticalScrollBar();
    if (vScrollBar)
        vScrollBar->triggerAction(QScrollBar::SliderToMinimum);
}

void MainWindow::populateContextMenu(QMenu& a_Menu, int a_Index)
{
    if (!CEvent::Event()->GetResultFinished(a_Index) && !CEvent::Event()->GetResultInvalid(a_Index))
        a_Menu.addAction("Reinstate");
    if (CEvent::Event()->GetResultFinished(a_Index))
        {
        a_Menu.addAction("DNF");
        a_Menu.addAction("Disqualify");
        }
    a_Menu.addAction("Alter");
    a_Menu.addAction("Delete");
}


void MainWindow::doContextMenuAction(QAction *selectedItem, int a_Index)
{
    if (selectedItem)
    {
    if (selectedItem->text() == "Reinstate")
        emit reinstate(a_Index);
    else if (selectedItem->text() == "DNF")
        emit dnf(a_Index);
    else if (selectedItem->text() == "Disqualify")
        emit dsq(a_Index);
    else if (selectedItem->text() == "Alter")
        runAlterDialog(a_Index);
    else if (selectedItem->text() == "Delete")
        {
        QString msg = QString(tr("Are you sure that you want to delete this download ?"));
        int result = SIMessageBox(msg, QMessageBox::Question, QMessageBox::Yes|QMessageBox::No);
        if (result == QMessageBox::Yes)
            emit deleteDownload(a_Index);
        }
    }
}

void MainWindow::showDownloadContextMenu(const QPoint& a_Pos)
{
    QPoint globalPos = a_Pos;
    QTextCursor cursor = ui->textEdit->cursorForPosition(globalPos);
    cursor.select(QTextCursor::LineUnderCursor);

    QTextEdit::ExtraSelection highlight;
    highlight.cursor = cursor;
    highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    highlight.format.setBackground( Qt::green );

    QList<QTextEdit::ExtraSelection> extras;
    extras << highlight;
    ui->textEdit->setExtraSelections( extras );

    QString line = cursor.selectedText();
    int i = line.indexOf("(");
    int j = line.indexOf(")");
    bool ok(false);
    long index;

    if ( i >= 0 && j > i)
        index = line.mid(i+1, j-i-1).toLong(&ok);
    else
        return;
    if (!ok)
        return;

    QMenu myMenu;
    populateContextMenu(myMenu, index);

    QAction* selectedItem = myMenu.exec(ui->textEdit->viewport()->mapToGlobal(a_Pos));
    doContextMenuAction(selectedItem, index);
}

void MainWindow::showResultContextMenu(const QPoint& a_Pos)
{
    QPoint globalPos = a_Pos;
    QTextCursor cursor = ui->textEdit_2->cursorForPosition(globalPos);
    cursor.select(QTextCursor::LineUnderCursor);

    QTextEdit::ExtraSelection highlight;
    highlight.cursor = cursor;
    highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    highlight.format.setBackground( Qt::green );

    QList<QTextEdit::ExtraSelection> extras;
    extras << highlight;
    ui->textEdit_2->setExtraSelections( extras );

    QString line = cursor.selectedText();
    QRegExp resultRE("^[ ]*[0-9]{1,4} ([^:]{10,26}) [^:]*([^ ]*:[0-9][0-9])$");
    QRegExp dnfRE("^([^:]*).{3,3}\\s(DSQ|DNF)$");
    if (!resultRE.exactMatch(line) && !dnfRE.exactMatch(line))
        return;

    long index(0);
    if (resultRE.exactMatch(line))
        {
        index = CEvent::Event()->LookupResult(resultRE.capturedTexts().at(1).trimmed(),resultRE.capturedTexts().at(2));
        if (index <0)
            return;
        }
    else
        {
        index = CEvent::Event()->LookupResult(dnfRE.capturedTexts().at(1).trimmed(),dnfRE.capturedTexts().at(2));
        if (index <0)
            return;
        }

    QMenu myMenu;
    populateContextMenu(myMenu, index);

    QAction* selectedItem = myMenu.exec(ui->textEdit_2->viewport()->mapToGlobal(a_Pos));
    doContextMenuAction(selectedItem, index);
}


void MainWindow::runAlterDialog(long a_Index)
{
    if (!CEvent::Event()->ResultExists(a_Index))
        return;
    QStringList courses;
    CEvent::Event()->GetCourseNames(courses);

    AlterDialog dlg(this, courses, CEvent::Event()->GetResult(a_Index));
    dlg.exec();
}

void MainWindow::runcoursesdialog()
{
    coursesdialog dlg(this);
    dlg.exec();
}

void MainWindow::runclockerrorsdialog()
{
    clockerrorsdialog dlg(this);
    dlg.exec();
}

void MainWindow::importCourses()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select course file"), CEvent::Event()->Directory(), QString("*.xml"));

    if (file.isEmpty())
        return;
    else
        emit importCourses(file);

}

void MainWindow::loadSI()
{
    ImportSIDialog dlg(this);
    dlg.exec();
}

void MainWindow::rentalStickNames()
{
     bool ok;
     QString names = CEvent::Event()->GetRentalNames();
     QString text = QInputDialog::getText(this, tr("Rental Stick Names"),
                                          tr("Rental stick names (comma separated):"), QLineEdit::Normal,
                                          names, &ok);
     if (ok && !text.isEmpty())
         CEvent::Event()->SetRentalNames(text);
}

void MainWindow::closeEvent ( QCloseEvent * event )
{
    if (!CEvent::Event()->CanClose())
       event->ignore();
    else
        event->accept();
}

void MainWindow::rundownloaddialog()
{
    DownloadDialog dlg(this);
    dlg.exec();
}

void MainWindow::formatHTML()
{
    HtmlOptionsDialog dlg(this);
    dlg.exec();
}
