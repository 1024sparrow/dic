#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <QString>
#include <stdio.h>
#include <QMessageBox>
#include <QApplication>

#define currentSrcCodePosition QString("%1, %2 (%3):").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__)
#define debug_legal(T) qDebug() << __FILE__<< ", " << __LINE__ << "(" << __FUNCTION__ << "): " << T
#define boolToString(T) (T?"true":"false")
#define debug_failed debug("failed")

//#ifdef CONSOLE_DEBUG

#define debug(T) qDebug()<<"debug" << __FILE__<< ", " << __LINE__ << "(" << __FUNCTION__ << "): " << T
#define debug_profiling qDebug()<<"debug" << __FILE__<< ", " << __LINE__ << "(" << __FUNCTION__ << ")";

#define debug_show_profiling QMessageBox::information(0,"",currentSrcCodePosition)
#define debug_show_message(T) QMessageBox::information(0,"",currentSrcCodePosition + QString("\n%1").arg(T))
//#define debug_show_message(A, B) QMessageBox::information(0,"",currentSrcCodePosition + QString("\n"A)B)

#define debug_show_and_die(T) {debug(T);qApp->closeAllWindows();QMessageBox::critical(0, "debug_show_and_die", currentSrcCodePosition + QString("%1").arg(T));qApp->exit(-1);}
#define debug_close_app qApp->closeAllWindows();

//#else // CONSOLE_DEBUG

//#define debug(T)
//#define debug_profiling ;

//#define debug_show_profiling
//#define debug_show_message(T)

//#define debug_show_and_die(T) ;
//#define debug_close_app ;

//#endif // CONSOLE_DEBUG

#endif // DEBUG_H
