/*
Copyright (c) 2015 Clement Roblot

This file is part of localtube.

Localtube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Localtube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with localtube.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>


QString *pathToFiles;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Localtube");
    QApplication::setApplicationVersion(CURRENT_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("Download your youtube subscription feed");
    parser.addHelpOption();
    parser.addVersionOption();

    //parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
    //parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));

    QCommandLineOption configFileOption(QStringList() << "c" << "Config file",
            QCoreApplication::translate("main", "Config file to use"),
            QCoreApplication::translate("main", "File"));
    parser.addOption(configFileOption);

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    QString configFile = parser.value(configFileOption);

    MainWindow w(configFile);
    w.show();

    return a.exec();




}

