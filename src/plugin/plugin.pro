TARGET = nemonotifications
PLUGIN_IMPORT_PATH = org/nemomobile/notifications
QT += dbus

TEMPLATE = lib
CONFIG += qt plugin hide_symbols
QT += declarative

INCLUDEPATH += ..
LIBS += -L.. -lnemonotifications
SOURCES += plugin.cpp

target.path = $$[QT_INSTALL_IMPORTS]/$$PLUGIN_IMPORT_PATH
qmldir.files += $$_PRO_FILE_PWD_/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$$$PLUGIN_IMPORT_PATH
INSTALLS += target qmldir

OTHER_FILES += qmldir
