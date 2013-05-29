TARGET = nemonotifications
PLUGIN_IMPORT_PATH = org/nemomobile/notifications
QT += dbus
QT -= gui

TEMPLATE = lib
CONFIG += qt plugin hide_symbols
equals(QT_MAJOR_VERSION, 4): QT += declarative
equals(QT_MAJOR_VERSION, 5): QT += qml

INCLUDEPATH += ..
equals(QT_MAJOR_VERSION, 4): LIBS += -L.. -lnemonotifications
equals(QT_MAJOR_VERSION, 5): LIBS += -L.. -lnemonotifications-qt5
SOURCES += plugin.cpp

equals(QT_MAJOR_VERSION, 4): target.path = $$[QT_INSTALL_IMPORTS]/$$PLUGIN_IMPORT_PATH
equals(QT_MAJOR_VERSION, 5): target.path = $$[QT_INSTALL_QML]/$$PLUGIN_IMPORT_PATH
qmldir.files += $$_PRO_FILE_PWD_/qmldir
qmldir.path +=  $$target.path
INSTALLS += target qmldir

OTHER_FILES += qmldir
