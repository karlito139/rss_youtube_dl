#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T19:57:59
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rss_youtube_dl
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rssfeed.cpp \
    video.cpp

HEADERS  += mainwindow.h \
    rssfeed.h \
    video.h

FORMS    += mainwindow.ui
