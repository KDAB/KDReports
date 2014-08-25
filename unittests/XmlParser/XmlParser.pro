KDREPORTS_PATH = $$PWD/../..

include( $$KDREPORTS_PATH/unittests/unittests.pri )

QT += xml

DESTDIR = .
HEADERS = 
SOURCES = XmlParser.cpp TestXmlElementHandlerV1.cpp TestXmlElementHandler.cpp

RESOURCES += XmlParser.qrc

test.target = test
test.commands = ./$(TARGET)
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test

OTHER_FILES = \
  broken.xml \
  doc_footer.xml \
  doc_header.xml \
  fonts.xml \
  handlerWithError.xml \
  handler.xml \
  htmlid.xml \
  margins.xml \
  simple.xml \
  spreadsheet.xml \
  tabs.xml \
  textid.xml \
  vspaceInHeader.xml \
  wrongTopElement.xml \

