#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T19:57:59
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



!win32{

  INCLUDEPATH += /usr/include/libappindicator-0.1 \
          /usr/include/gtk-2.0 \
          /usr/lib/gtk-2.0/include \
          /usr/include/glib-2.0 \
          /usr/lib/glib-2.0/include \
          /usr/include/cairo \
          /usr/include/atk-1.0 \
          /usr/include/pango-1.0

  PKGCONFIG = gtk+-2.0

  LIBS += -L/usr/lib -lappindicator -lnotify

  CONFIG += link_pkgconfig
}

RC_FILE = rss-youtube-dl.rc

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

RESOURCES += \
    ressource.qrc







