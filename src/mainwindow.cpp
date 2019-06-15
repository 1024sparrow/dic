#include "mainwindow.h"
#include "selectmodewidget.h"
#include "historywidget.h"
#include "debug.h"

#include <QGroupBox>
#include <QBoxLayout>
#include <QToolBar>
#include <QLineEdit>
#include <QTableView>
#include <QAction>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QFileDialog>
#include <QByteArray>
#include <QVariant>
#include <QFrame>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent), modeAutoHistoryEnabled(true), m_isWindowActive(true)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(onTimer()));
    m_timer->start(200);

    historyWidget = new HistoryWidget(this);
    {
        historyWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
        connect(historyWidget, SIGNAL(historyResultsRequested(int,QString)), this,
                SLOT(onHistoryResultsRequested(int,QString)));
        //connect(historyWidget, SIGNAL())
//        historyWidget->setVisible(false);
    }

    QMenuBar *menubar = new QMenuBar(this);
    {
//        QAction *actInitFromTxt = new QAction(tr("fromTxt"), this);
        QMenu *menuView = new QMenu("view", this);
        {
            QAction *actionToggleFullscreen = new QAction("toggle [fullscreen]", menuView);
            {
                actionToggleFullscreen->setCheckable(true);
                connect(actionToggleFullscreen, SIGNAL(toggled(bool)), SLOT(onToggleFullscreen(bool)));
                actionToggleFullscreen->setShortcut(Qt::Key_F11);
            }

            QAction *actionToggleRegisterHistory = new QAction("toggle [history]", menuView);
            {
                actionToggleRegisterHistory->setCheckable(true);
                actionToggleRegisterHistory->setChecked(true);
                connect(actionToggleRegisterHistory, SIGNAL(toggled(bool)), historyWidget, SLOT(setVisible(bool)));
//                connect(historyWidget, SIGNAL(visibilityChanged(bool)), actionToggleRegisterHistory, SLOT(setChecked(bool)));
            }            
            menuView->addAction(actionToggleFullscreen);
            menuView->addAction(actionToggleRegisterHistory);
        }
        QMenu *menuActions = new QMenu("actions", this);
        {
            QAction *actionToggleAutoHistory = new QAction("toggle [auto save and clean]", menuActions);
            {
                actionToggleAutoHistory->setCheckable(true);
                actionToggleAutoHistory->setChecked(modeAutoHistoryEnabled);
                connect(actionToggleAutoHistory, SIGNAL(toggled(bool)), SLOT(onToggleAutoHistory(bool)));
            }

            menuActions->addAction(actionToggleAutoHistory);
        }
        QAction *actSaveInHistory = new QAction(QIcon(":pi/save_to_file.png"), QString("save in request history"), this);
        QAction *actSaveInFile = new QAction(QIcon(":pi/save.png"), QString("export to a file"), this);
        QAction *actClose = new QAction(QIcon(":pi/cross.png"), QString("close"), this);

//        connect(actInitFromTxt, SIGNAL(triggered()), SLOT(onInitFromTxt()));
        connect(actSaveInHistory, SIGNAL(triggered()), SLOT(saveinHistory()));
        connect(actSaveInFile, SIGNAL(triggered()), SLOT(onSaveInFile()));
        connect(actClose, SIGNAL(triggered()), SLOT(close()));

//        toolbar->addAction(actInitFromTxt);
        menubar->addMenu(menuView);
        menubar->addMenu(menuActions);

        {
            QToolBar *rightcorner = new QToolBar(menubar);
            rightcorner->addAction(actSaveInHistory);
            rightcorner->addAction(actSaveInFile);
            rightcorner->addSeparator();
            rightcorner->addAction(actClose);

            rightcorner->layout()->setMargin(0);

            menubar->setCornerWidget(rightcorner);
        }
    }
    QFrame *filterWidget = new QFrame(this);
    {
        QList<QPair<QPixmap, QVariant> > listForModeSwitcher;
        listForModeSwitcher << QPair<QPixmap, QVariant>(QPixmap(":pi/starts.png"), "select key, value from t where key like ':pattern%'");
        listForModeSwitcher << QPair<QPixmap, QVariant>(QPixmap(":pi/contains.png"), "select key, value from t where key like '%:pattern%'");
        int defMode = 0;
        modeSwitcher = new SelectModeWidget(listForModeSwitcher, defMode, filterWidget);
        requestPattern = listForModeSwitcher[defMode].second.toString();
        connect(modeSwitcher, SIGNAL(methodChanged(QVariant)), SLOT(onChangeRequestPattern(QVariant)));

        leFilter = new QLineEdit(filterWidget);
        connect(leFilter, SIGNAL(textChanged(QString)), SLOT(onFilterStringChanged(QString)));

        QHBoxLayout *lay = new QHBoxLayout(filterWidget);
        lay->setMargin(0);
        lay->addWidget(modeSwitcher);
        lay->addWidget(leFilter, 1);
    }
    listView = new QTableView(this);
    {
        listView->horizontalHeader()->setStretchLastSection(true);
        listView->setSelectionBehavior(QAbstractItemView::SelectRows);
        listView->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    resultModel = new QStandardItemModel(0, 2, this);
    {
        resultModel->setHorizontalHeaderLabels(QStringList()<<"key"<<"value");
    }
    listView->setModel(resultModel);

    QWidget *centralWidgetCandidate = new QWidget(this);
    {
        QVBoxLayout *mainLay = new QVBoxLayout(centralWidgetCandidate);
        mainLay->setMargin(0);
        mainLay->addWidget(filterWidget);
        mainLay->addWidget(listView);
    }
    setMenuBar(menubar);
    setCentralWidget(centralWidgetCandidate);
    addDockWidget(Qt::RightDockWidgetArea, historyWidget, Qt::Vertical);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/boris/.dic/dic.db");
//    db.setDatabaseName(":memory:");
    if (db.open())
    {
        QSqlQuery query(db);
        query.exec("create table t(key text, value text);");
    }
    else
        debug("epic fail");

    {
        QString styleFilePath = ":style.qss";
#ifdef STYLE_INDEVELOP
        styleFilePath = "style.qss";//for develop time
#endif
        QFile styleFile(styleFilePath);
        if (styleFile.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&styleFile);
            QString styleString;
            while (!stream.atEnd())
                styleString.append(stream.readLine() + "\n");
            setStyleSheet(styleString);
        }
    }
    leFilter->setFocus();
}

MainWindow::~MainWindow()
{
    db.close();
}

//void MainWindow::onInitFromTxt()
//{
//    QSqlQuery query(db);
//    QFile file(QFileDialog::getOpenFileName(this));
//    if (!file.open(QIODevice::ReadOnly))
//    {
//        debug("can't open file:"<<file.errorString());
//        return;
//    }
//    QByteArray kusok = file.readAll();
//    file.close();
//    QList<QByteArray> lines = kusok.split('\n');
//    for (int row = 0, rowCount = lines.count() ; row < rowCount ; ++row)
//    {
//        QString srcString = QString::fromUtf8(lines[row].constData(), lines[row].count());
//        if (srcString.startsWith("//"))
//            continue;
//        QStringList pair = srcString.split("--");
//        if (pair.count() != 2)
//            continue;
//        if (row < 10)
//            debug(pair);
//        query.prepare("insert into t(key, value) values(:key, :value);");
//        query.bindValue(":key", pair.first().simplified());
//        query.bindValue(":value", pair.last().simplified());
//        query.exec();
//    }
//    debug("finished");
//}

void MainWindow::onFilterStringChanged(const QString &pattern)
{
    searchPattern = pattern;
    updateResultList();
}

void MainWindow::onChangeRequestPattern(const QVariant &newPattern)
{
    requestPattern = newPattern.toString();
    updateResultList();
}

void MainWindow::onToggleFullscreen(bool b)
{
    setWindowState(b ? Qt::WindowFullScreen : Qt::WindowNoState);
}

void MainWindow::onToggleRegisterHistory(bool b)
{
    //
}

void MainWindow::onToggleAutoHistory(bool b)
{
    modeAutoHistoryEnabled = b;
}

void MainWindow::onTimer()
{
    if (isWindow())
    {
        if (modeAutoHistoryEnabled)
        {
            if (isActiveWindow())
            {
                m_isWindowActive = true;
            }
            else
            {
                if (m_isWindowActive == true)
                {
                    //мы потеряли управление. Скидываем запрос в историю и чистим поле запроса.
                    if (resultModel->rowCount())
                    {
                        saveinHistory();
                        leFilter->clear();
                    }
                }
                m_isWindowActive = false;
            }
        }
    }
}

void MainWindow::onHistoryResultsRequested(int requestPattern_, const QString &searchPattern_)
{
    modeSwitcher->setCurrentState(requestPattern_);
    leFilter->setText(searchPattern_);
    leFilter->selectAll();
    leFilter->setFocus();
}

void MainWindow::saveinHistory()
{
    int rowCount = resultModel->rowCount();

    QString pattern = searchPattern;
    QString wordValue;
    if (rowCount == 1)
        wordValue = resultModel->index(0, 1).data(Qt::DisplayRole).toString();
    else if (!listView->selectionModel()->selection().isEmpty())
    {
        int row = listView->selectionModel()->selection().indexes().first().row();
        pattern = resultModel->index(row, 0).data(Qt::DisplayRole).toString();
        wordValue = resultModel->index(row, 1).data(Qt::DisplayRole).toString();
    }
    historyWidget->registerRequest(modeSwitcher->currentState(), pattern, wordValue);
}

void MainWindow::onSaveInFile()
{
    const QString filepath = QFileDialog::getSaveFileName(this, "where to save?", QDir::homePath(), "*.txt");
    if (!filepath.isEmpty())
        historyWidget->exportToFile(filepath);
}

void MainWindow::updateResultList()
{
    resultModel->setRowCount(0);
    if (searchPattern.length() < 2)
        return;

    QSqlQuery query(db);
    QString queryStr = requestPattern;
    queryStr = queryStr.replace(":pattern", searchPattern);
    if (query.exec(queryStr))
    {
        while(query.next())
        {
            QList<QStandardItem *> items;
            items.append(new QStandardItem(query.value(0).toString()));
            items.append(new QStandardItem(query.value(1).toString()));
            resultModel->appendRow(items);
        }
    }
    else
        debug("error:"<<query.lastError().text());
}
