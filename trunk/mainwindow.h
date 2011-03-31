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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QCheckBox;
class QLabel;
class QLineEdit;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    virtual void closeEvent ( QCloseEvent * event );

signals:
    void reinstate(long a_Index);
    void dnf(long a_Index);
    void deleteDownload(long a_Index);
    void alter(long a_Index, QString a_Name, QString a_Club);
    void importCourses(QString a_Filename);

private slots:
    void open(); // Open new dump from SI-Config
    void save(); // Save results to file
    void refresh(); // Refresh display
    void exportIOF(); // Export in IOF format
    void about(); // Display About dialog
    void setUseHTML(int a_Value); // Toggle use of HTML and plain text
    void setShowSplits(int a_Value); // Toggle display of splits
    void setEventTitle(QString a_Title); // Set title of event
    void showResults(std::vector<QString>& a_Lines); // Show the results
    void showDownloadContextMenu(const QPoint& a_Pos); // Show the context menu for the download pane
    void showResultContextMenu(const QPoint& a_Pos); // Show the context menu for the download pane
    void runcoursesdialog(); // Show the course management dialog
    void runclockerrorsdialog(); // Show the clock errors dilog
    void importCourses(); // Identify a course file for loading
    void loadSI(); // download an SI database
    void rentalStickNames(); // obtain the names present on rental sticks
    void rundownloaddialog(); // download data from an SI unit

private:
    Ui::MainWindow *ui;
    QAction* m_OpenAct;
    QAction* m_SaveAct;
    QAction* m_ExportAct;
    QAction* m_DownloadAct;
    QAction* m_QuitAct;
    QAction* m_RefreshAct;
    QAction* m_AboutAct;

    QCheckBox* m_UseHtmlCheck;
    QCheckBox* m_ShowSplitsCheck;

    QLabel*  m_EventLabel;
    QLineEdit*  m_EventText;

    void runAlterDialog(long a_Index);
};

#endif // MAINWINDOW_H
