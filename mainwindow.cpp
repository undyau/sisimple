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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    QFont* plainfont = new QFont("Lucinda Console");
//    plainfont->setStyleHint(QFont::TypeWriter);

//    ui->textEdit->setFont(*plainfont);

    // Maybe need to use Tango theme ??
    ui->setupUi(this);

    ui->textEdit->setReadOnly(true);
    ui->textEdit_2->setReadOnly(true);

    QWidget *viewPort = ui->textEdit->viewport();
    viewPort->setCursor(Qt::ArrowCursor);
    ui->textEdit->setViewport(viewPort);

    m_OpenAct = new QAction(QIcon::fromTheme("document-open", QIcon(":icons/icons/document-open.svg")), tr("&Open..."), this);
    m_OpenAct->setShortcuts(QKeySequence::Open);
    m_OpenAct->setStatusTip(tr("Open an existing CSV dump"));
    m_OpenAct->setToolTip(tr("Open an existing CSV dump"));
    m_OpenAct->setIconVisibleInMenu(true);
    ui->menu_File->addAction(m_OpenAct);
    ui->mainToolBar->addAction(m_OpenAct);
    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->textEdit_2->setContextMenuPolicy(Qt::NoContextMenu);

    m_SaveAct = new QAction(QIcon::fromTheme("document-save", QIcon(":icons/icons/document-save.svg")), tr("&Save..."), this);
    m_SaveAct->setShortcuts(QKeySequence::Save);
    m_SaveAct->setStatusTip(tr("Save Results"));
    m_SaveAct->setToolTip(tr("SaveResults"));
    m_SaveAct->setIconVisibleInMenu(true);
    ui->menu_File->addAction(m_SaveAct);
    ui->mainToolBar->addAction(m_SaveAct);

    m_ExportAct = new QAction(tr("&Export to IOF XML..."), this);
    m_ExportAct->setStatusTip(tr("Export to IOF XML..."));
    m_ExportAct->setToolTip(tr("Export to IOF XML..."));
    ui->menu_File->addAction(m_ExportAct);

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


    connect(m_OpenAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_SaveAct, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_ExportAct, SIGNAL(triggered()), this, SLOT(exportIOF()));
    connect(m_RefreshAct, SIGNAL(triggered()), this, SLOT(refresh()));
    connect(m_QuitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_UseHtmlCheck, SIGNAL(stateChanged(int)), this, SLOT(setUseHTML(int)));
    connect(m_ShowSplitsCheck, SIGNAL(stateChanged(int)), this, SLOT(setShowSplits(int)));
    connect(m_EventText, SIGNAL(textChanged(QString)), this, SLOT(setEventTitle(QString)));
    connect(m_AboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(CEvent::Event(), SIGNAL(logMsg(QString)), ui->textEdit, SLOT(append(QString)));
    connect(CEvent::Event(), SIGNAL(updatedResultsOutput(std::vector<QString>&)), this, SLOT(showResults(std::vector<QString>&)));
    connect(ui->textEdit, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showDownloadContextMenu(const QPoint&)));
    connect(this, SIGNAL(dnf(long)), CEvent::Event(), SLOT(dnfResult(long)));
    connect(this, SIGNAL(reinstate(long)), CEvent::Event(), SLOT(reinstateResult(long)));
    connect(ui->actionManage, SIGNAL(triggered()), this, SLOT(runcoursesdialog()));
    connect(ui->actionGuess, SIGNAL(triggered()), CEvent::Event(), SLOT(guessCourses()));
    connect(ui->actionImport, SIGNAL(triggered()), this, SLOT(importCourses));
    connect(this, SIGNAL(importCourses(QString)), CEvent::Event(), SLOT(importCourses(QString)));
    connect(CEvent::Event(), SIGNAL(coursesGuessed()), this, SLOT(runcoursesdialog()));
}

MainWindow::~MainWindow()
{
    delete m_OpenAct;
    delete ui;
}

void MainWindow::open()
{
    CEvent* oevent = CEvent::Event();
    QString dir = QFileDialog::getExistingDirectory(this,tr("Select event directory"), oevent->Directory());

    if (dir.isEmpty())
        return;

// Test that each required file exists
    if (oevent->FindRawDataFile(dir).isEmpty())
        {
        SIMessageBox("No raw data file in specified directory !", QMessageBox::Warning);
        return;
        }
     if (oevent->FindCourseFile(dir).isEmpty())
        {
        SIMessageBox("No courses file in specified directory !", QMessageBox::Warning);
        return;
        }
     oevent->SetDirectory(dir);
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
QMessageBox::about(this, tr("About SI Simple"),
    tr("<h2>SI Simple 0.4</h2>"
       "<p>&copy; 2010 Andy Simpson</p>"
       "<p>This is an Open Source project hosted at "
       "http://sisimple.sourceforge.net, licensed under the GPL</p>"
       "<p>Acknowledgement and thanks to Ken Hanson and his amazing TCL program SIDResults</p>"
       ));
}

void MainWindow::showResults(std::vector<QString>& a_Lines)
{
    ui->textEdit_2->clear();
    for (unsigned int i = 0; i < a_Lines.size(); i++)
        ui->textEdit_2->append(a_Lines[i]);
}

void MainWindow::showDownloadContextMenu(const QPoint& a_Pos)
{
    QPoint globalPos = a_Pos;
    QTextCursor cursor = ui->textEdit->cursorForPosition(globalPos);
    cursor.select(QTextCursor::LineUnderCursor);
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
    if (!CEvent::Event()->GetResultFinished(index) && !CEvent::Event()->GetResultInvalid(index))
        myMenu.addAction("Reinstate");
    if (CEvent::Event()->GetResultFinished(index))
        myMenu.addAction("DNF");
    myMenu.addAction("Alter");

    QAction* selectedItem = myMenu.exec(ui->textEdit->viewport()->mapToGlobal(a_Pos));
    if (selectedItem)
    {
    if (selectedItem->text() == "Reinstate")
        emit reinstate(index);
    else if (selectedItem->text() == "DNF")
        emit dnf(index);
    else if (selectedItem->text() == "Alter")
        runAlterDialog(index);
    }

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

void MainWindow::importCourses()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select course file"), CEvent::Event()->Directory(), QString("*.xml"));

    if (file.isEmpty())
        return;
    else
        emit importCourses(file);

}
