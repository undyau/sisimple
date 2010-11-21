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

signals:
    void reinstate(long a_Index);
    void dnf(long a_Index);
    void alter(long a_Index, QString a_Name, QString a_Club);

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

private:
    Ui::MainWindow *ui;
    QAction* m_OpenAct;
    QAction* m_SaveAct;
    QAction* m_ExportAct;
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
