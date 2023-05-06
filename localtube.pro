#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T19:57:59
#
#-------------------------------------------------

QT       += core gui network networkauth xml core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



RC_FILE = localtube.rc

TARGET = localtube
TEMPLATE = app


SOURCES +=  main.cpp\
            mainwindow.cpp \
            video.cpp \
            about.cpp \
            feedFetcher/feedfetcheruser.cpp \
            feedFetcher/feedfetcherplaylist.cpp \
            feedFetcher/feedfetcher.cpp \
            feedFetcher/feedfetcherchannel.cpp \
            appsettings.cpp \
    networkisonline.cpp

HEADERS  += mainwindow.h \
            video.h \
            about.h \
            feedFetcher/feedfetcheruser.h \
            feedFetcher/feedfetcherplaylist.h \
            feedFetcher/feedfetcher.h \
            feedFetcher/feedfetcherchannel.h \
            appsettings.h \
    constants.h \
    networkisonline.h

FORMS    += mainwindow.ui \
    about.ui \
    appsettings.ui

RESOURCES += \
    ressource.qrc

win32{
    LIBS += -LC:/OpenSSL-Win32/lib
    INCLUDEPATH += C:/OpenSSL-Win32/include
}

DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix
win32:OBJECTS_DIR = ../build/o/win32

INSTALLS += target man link icon16 icon22 icon32 icon48 icon64 icon128 iconscal

target.path = /$(DESTDIR)/usr/bin

man.files = doc/localtube.1.gz
man.path = /$(DESTDIR)/usr/share/man/man1

link.files = link/localtube.desktop
link.path = /$(DESTDIR)/usr/share/applications

icon16.files = icon/debian/usr/share/icons/hicolor/16x16/apps/localtube.png
icon16.path = /$(DESTDIR)/usr/share/icons/hicolor/16x16/apps

icon22.files = icon/debian/usr/share/icons/hicolor/22x22/apps/localtube.png
icon22.path = /$(DESTDIR)/usr/share/icons/hicolor/22x22/apps

icon32.files = icon/debian/usr/share/icons/hicolor/32x32/apps/localtube.png
icon32.path = /$(DESTDIR)/usr/share/icons/hicolor/32x32/apps

icon48.files = icon/debian/usr/share/icons/hicolor/48x48/apps/localtube.png
icon48.path = /$(DESTDIR)/usr/share/icons/hicolor/48x48/apps

icon64.files = icon/debian/usr/share/icons/hicolor/64x64/apps/localtube.png
icon64.path = /$(DESTDIR)/usr/share/icons/hicolor/64x64/apps

icon128.files = icon/debian/usr/share/icons/hicolor/128x128/apps/localtube.png
icon128.path = /$(DESTDIR)/usr/share/icons/hicolor/128x128/apps

iconscal.files = icon/debian/usr/share/icons/hicolor/scalable/apps/localtube.svgz
iconscal.path = /$(DESTDIR)/usr/share/icons/hicolor/scalable/apps

DISTFILES +=





