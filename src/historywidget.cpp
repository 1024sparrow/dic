#include "historywidget.h"
#include "debug.h"

#include <QTableView>
#include <QBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>

#include <QFile>
#include <QTextStream>
#include <QDateTime>

HistoryWidget::HistoryWidget(QWidget *parent)
    :QDockWidget("history", parent)
{
    model = new QStandardItemModel(0, 2, this);
    {
        model->setHorizontalHeaderLabels(QStringList()<<"key"<<"value");
    }

    QTableView *tv = new QTableView(this);
    {
        tv->horizontalHeader()->setStretchLastSection(true);

        tv->setSelectionBehavior(QAbstractItemView::SelectRows);
        tv->setSelectionMode(QAbstractItemView::SingleSelection);
        connect(tv, SIGNAL(clicked(QModelIndex)), SLOT(onItemClicked(QModelIndex)));
    }

    tv->setModel(model);

    setWidget(tv);
}

void HistoryWidget::registerRequest(int searchMode, const QString &searchPattern, const QString &wordValue)
{
    QStandardItem *col0item = new QStandardItem(searchPattern);
    QStandardItem *col1item = new QStandardItem(wordValue);
    model->insertRow(0, QList<QStandardItem *>()<<col0item<<col1item);
    model->setHeaderData(0,Qt::Vertical, searchMode, Qt::UserRole + 1);
}

void HistoryWidget::exportToFile(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::Text | QIODevice::Append | QIODevice::WriteOnly))
    {
        debug_show_message("oops. unchancy.");
        return;
    }
    QTextStream stream(&file);
    stream.setCodec("utf8");
    stream << "\n-- " << QDateTime::currentDateTime().toString("dd.MM.yyyy, hh:mm") << "--\ncodepage utf8\n";

    for (int i = 0, c = model->rowCount() ; i < c ; ++i)
    {
        QString key = model->index(i, 0).data(Qt::DisplayRole).toString();
        if (key.isEmpty())
            continue;
        QString value = model->index(i, 1).data(Qt::DisplayRole).toString();
        if (value.isEmpty())
            continue;
        QString row = QString("%1 -- %2\n").arg(key).arg(value.simplified());
        stream << row;
    }
    file.close();
}

void HistoryWidget::onItemClicked(const QModelIndex &index)
{
    int searchMode = model->headerData(index.row(), Qt::Vertical, Qt::UserRole + 1).toInt();
    QString searchPattern = index.sibling(index.row(), 0).data(Qt::DisplayRole).toString();

    emit historyResultsRequested(searchMode, searchPattern);
}
