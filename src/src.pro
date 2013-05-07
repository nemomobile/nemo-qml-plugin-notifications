system(qdbusxml2cpp org.freedesktop.Notifications.xml -p notificationmanagerproxy -c NotificationManagerProxy -i notification.h)

TEMPLATE = lib
TARGET = nemonotifications
CONFIG += qt hide_symbols create_pc create_prl
QT += dbus

SOURCES += notification.cpp \
    notificationmanagerproxy.cpp

HEADERS += \
    notification.h \
    notificationmanagerproxy.h

OTHER_FILES += notifications.qdoc notifications.qdocconf

target.path = $$[QT_INSTALL_LIBS]
pkgconfig.files = $$TARGET.pc
pkgconfig.path = $$target.path/pkgconfig
headers.files = notification.h
headers.path = /usr/include/nemonotifications

QMAKE_PKGCONFIG_NAME = lib$$TARGET
QMAKE_PKGCONFIG_DESCRIPTION = Convenience library or sending notifications
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

INSTALLS += target headers pkgconfig
