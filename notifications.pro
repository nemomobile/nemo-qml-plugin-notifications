TEMPLATE = subdirs

src_plugins.subdir = src/plugin
src_plugins.target = sub-plugins
src_plugins.depends = src

SUBDIRS += src src_plugins

include (doc/doc.pri)

oneshot.files = oneshot/*
oneshot.path = /usr/lib/oneshot.d

INSTALLS += oneshot

