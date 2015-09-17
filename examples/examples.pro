TEMPLATE = subdirs
# Because of the tools lib
CONFIG += ordered
SUBDIRS  = tools Letter HelloWorld ReferenceReport PriceList Database HelloWorldXML PriceListXML DatabaseXML \
    LongReport LongTextReport BigImage MailMergeXML ReportSection \
    Labels

include(../kdchart.pri)
have_kdchart: SUBDIRS += ChartXML
