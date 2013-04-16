system(qdbusxml2cpp org.freedesktop.Notifications.xml -p notificationmanagerproxy -c NotificationManagerProxy -i notification.h)

TARGET = nemonotifications
PLUGIN_IMPORT_PATH = org/nemomobile/notifications
QT += dbus

TEMPLATE = lib
CONFIG += qt plugin hide_symbols
QT += declarative

target.path = $$[QT_INSTALL_IMPORTS]/$$PLUGIN_IMPORT_PATH
INSTALLS += target

qmldir.files += $$_PRO_FILE_PWD_/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$$$PLUGIN_IMPORT_PATH
INSTALLS += qmldir

SOURCES += plugin.cpp \
    notification.cpp \
    notificationmanagerproxy.cpp

HEADERS += \
    notification.h \
    notificationmanagerproxy.h

OTHER_FILES += qmldir notifications.qdoc notifications.qdocconf

headers.files = notification.h
headers.path = /usr/include/nemo-qml-plugins/notifications
pkgconfig.files = nemonotifications.pc
pkgconfig.path = /usr/lib/pkgconfig
INSTALLS += headers pkgconfig
