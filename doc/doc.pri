equals(QT_MAJOR_VERSION, 4): QDOC = qdoc3
equals(QT_MAJOR_VERSION, 5): QDOC = qdoc
QDOCCONF = config/nemo-qml-plugin-notifications.qdocconf

docs.commands = ($$QDOC $$PWD/$$QDOCCONF)

QMAKE_EXTRA_TARGETS += docs

