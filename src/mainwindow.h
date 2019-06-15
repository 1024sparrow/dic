#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class HistoryWidget;
class SelectModeWidget;

class QTimer;
class QLineEdit;
class QTableView;

#include <QMainWindow>
#include <QWidget>
#include <QSqlDatabase>
#include <QStandardItemModel>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
//    void onInitFromTxt();
    void onFilterStringChanged(const QString &);
    void onChangeRequestPattern(const QVariant &newPattern);

    void onToggleFullscreen(bool);
    void onToggleRegisterHistory(bool);
    void onToggleAutoHistory(bool);
    void onTimer();
    void onHistoryResultsRequested(int requestPattern_, const QString &searchPattern_);
    void saveinHistory();
    void onSaveInFile();
private:
    void updateResultList();

private:
    QSqlDatabase db;
    QStandardItemModel *resultModel;
    QString requestPattern;
    QString searchPattern;
    HistoryWidget *historyWidget;
    bool modeAutoHistoryEnabled;//by focus leaving save request (and results if possible) in history and clear search field
    QTimer *m_timer;

    SelectModeWidget *modeSwitcher;
    QLineEdit *leFilter;
    QTableView *listView;

    bool m_isWindowActive;
};

#endif // MAINWINDOW_H
