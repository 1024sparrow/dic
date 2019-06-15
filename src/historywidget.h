#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

class QStandardItemModel;
class QModelIndex;

#include <QDockWidget>

class HistoryWidget : public QDockWidget
{
    Q_OBJECT
public:
    HistoryWidget(QWidget *parent = 0);
    void registerRequest(int searchMode, const QString &searchPattern, const QString &wordValue = QString());
    void exportToFile(const QString &filepath);
signals:
    void historyResultsRequested(int searchMode, const QString &searchPattern);
private slots:
    void onItemClicked(const QModelIndex &index);

private:
    QStandardItemModel *model;
};

#endif // HISTORYWIDGET_H
