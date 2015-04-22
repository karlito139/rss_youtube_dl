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

TARGET = rss-youtube-dl
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



DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix
win32:OBJECTS_DIR = ../build/o/win32

INSTALLS += target man link icon16 icon22 icon32 icon48 icon64 icon128 iconscal

target.path = /$(DESTDIR)/usr/bin

man.files = doc/rss-youtube-dl.1.gz
man.path = /$(DESTDIR)/usr/share/man/man1

link.files = link/rss-youtube-dl.desktop
link.path = /$(DESTDIR)/usr/share/applications

icon16.files = icon/debian/usr/share/icons/hicolor/16x16/apps/rss-youtube-dl.png
icon16.path = /$(DESTDIR)/usr/share/icons/hicolor/16x16/apps

icon22.files = icon/debian/usr/share/icons/hicolor/22x22/apps/rss-youtube-dl.png
icon22.path = /$(DESTDIR)/usr/share/icons/hicolor/22x22/apps

icon32.files = icon/debian/usr/share/icons/hicolor/32x32/apps/rss-youtube-dl.png
icon32.path = /$(DESTDIR)/usr/share/icons/hicolor/32x32/apps

icon48.files = icon/debian/usr/share/icons/hicolor/48x48/apps/rss-youtube-dl.png
icon48.path = /$(DESTDIR)/usr/share/icons/hicolor/48x48/apps

icon64.files = icon/debian/usr/share/icons/hicolor/64x64/apps/rss-youtube-dl.png
icon64.path = /$(DESTDIR)/usr/share/icons/hicolor/64x64/apps

icon128.files = icon/debian/usr/share/icons/hicolor/128x128/apps/rss-youtube-dl.png
icon128.path = /$(DESTDIR)/usr/share/icons/hicolor/128x128/apps

iconscal.files = icon/debian/usr/share/icons/hicolor/scalable/apps/rss-youtube-dl.svgz
iconscal.path = /$(DESTDIR)/usr/share/icons/hicolor/scalable/apps





