QDOC = qdoc
QHELPGENERATOR = qhelpgenerator
QDOCCONF = config/nemo-qml-plugin-notifications.qdocconf
QHELPFILE = html/nemo-qml-plugin-notifications.qhp
QCHFILE = html/nemo-qml-plugin-notifications.qch

docs.commands = ($$QDOC $$PWD/$$QDOCCONF) && \
                ($$QHELPGENERATOR $$PWD/$$QHELPFILE -o $$PWD/$$QCHFILE)

QMAKE_EXTRA_TARGETS += docs
