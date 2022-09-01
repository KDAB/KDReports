/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSREPORT_H
#define KDREPORTSREPORT_H

#include "KDReportsErrorDetails.h"
#include "KDReportsGlobal.h"

#include <QColor>
#include <QFont>
#include <QObject>
#include <QPrinter>
#include <QSizeF>
#include <QTextOption>

#include <functional>

QT_BEGIN_NAMESPACE
class QDomDocument;
class QTextDocumentFragment;
class QTextDocument;
class QDomElement;
class QDomNode;
class QAbstractItemModel;
class QIODevice;
class QRect;
class QPainter;
class ReportData;
class EditorData;
QT_END_NAMESPACE

namespace KDReports {

class MainTable;
class XmlParser;
class Element;
class HeaderReportBuilder;
class Header;
typedef Header Footer;
class ReportPrivate;
class ReportBuilder;
class TextDocument;
class TableBreakingSettingsDialog;
class AutoTableElement;
class XmlElementHandler;

/**
 * If no header was specified for the first and last pages, then the
 * even/odd headers are used for those.
 */
enum HeaderLocation
{
    FirstPage = 1, ///< The first page of the report
    EvenPages = 2, ///< The even pages of the report: 2, 4, 6 etc.
    OddPages = 4, ///< The odd pages of the report: 1 (unless FirstPage has its own header), 3, 5, 7 etc.
    LastPage = 8, ///< The last page of the report.
    AllPages = OddPages | EvenPages ///< All pages (except first and last if FirstPage or LastPage have their own headers)
};
Q_DECLARE_FLAGS(HeaderLocations, HeaderLocation)

/**
 * The Report class represents a report.
 *
 * Initially the report is empty.
 * Content can be added to the report using the addElement API,
 * or using loadFromXML to load an equivalent definition from an XML file.
 *
 * The report can then be previewed with PreviewDialog, printed, exported to PDF, or exported to an image.
 */
class KDREPORTS_EXPORT Report : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString documentName READ documentName WRITE setDocumentName)

public:
    /**
     * Constructs a report.
     * \param parent optional parent object, for memory management
     */
    explicit Report(QObject *parent = nullptr);
    /**
     * Destroys the report.
     */
    ~Report() override;

    enum ReportMode
    {
        WordProcessing = 0,
        SpreadSheet = 1
    };

    /**
     * Sets the main mode of the report: word-processing or spread-sheet (one large table).
     * The word-processing mode is the most generic one, where text, images, tables etc.
     * can be mixed into a single report.
     * The spread-sheet mode is specific to "one large table", which gives additional features
     * like table breaking over multiple pages horizontally.
     * This is very similar to the difference between word processors and spreadsheets
     * in most office suites.
     *
     * A note about tables: in word-processing mode, the text of table cells can wrap, if the
     * table is too wide for the page. In spread-sheet mode, table cells do not wrap. Instead,
     * the font is adapted so that the table fits. In spread-sheet mode, the same font is used
     * for all table cells, for layouting and performance reasons.
     *
     * It is very important to set the mode of the report before doing anything else
     * with the report.
     *
     * \since 1.3
     */
    void setReportMode(ReportMode reportMode);

    /**
     * Returns the mode set with setReportMode();
     * \since 1.3
     */
    ReportMode reportMode() const;

    /**
     * Sets the default font used for text in this report.
     * \since 1.1
     */
    void setDefaultFont(const QFont &font);

    /**
     * \return the default font used for text in this report
     * \since 1.4
     */
    QFont defaultFont() const;

    /**
     * Spreadsheet mode: returns the main table of the report.
     */
    MainTable *mainTable() const;

    /**
     * Adds an element to the report, next to the previous element, in the same paragraph.
     *
     * An element defines what is added to the report. The report does not store
     * the element, though. For this reason, any changes to the element after insertion
     * will have no effect on the report. Set all settings in the element before calling addInlineElement.
     */
    void addInlineElement(const Element &element);

    /**
     * Adds an element to the report, creating a new paragraph for it.
     * You can specify the alignment of that paragraph.
     *
     * An element defines what is added to the report. The report does not store
     * the element, though. For this reason, any changes to the element after insertion
     * will have no effect on the report. Set all settings in the element before calling addElement.
     *
     * \param element the definition of what to add to the report
     * \param horizontalAlignment the alignment of the paragraph created for this element
     * \param backgroundColor the background color of the paragraph created for this element
     * The difference with Element::setBackground is that it affects the whole paragraph, including
     * any inline elements as well as the unused area up to the right margin of the page.
     */
    void addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft, const QColor &backgroundColor = QColor());

    /**
     * Adds vertical spacing between paragraphs.
     * Make sure to call addElement after that, not addInlineElement.
     * \param space the space in millimeters
     */
    void addVerticalSpacing(qreal space);

    /**
     * Adds a piece of formatted text from a QTextDocument.
     * This method is an optimization for the case where the initial text
     * is in a QTextDocument (e.g. in a QTextEdit). In general you
     * probably want to use addElement(HtmlElement()) instead.
     */
    void addFragment(const QTextDocumentFragment &fragment);

    /**
     * Adds a page break to the report.
     * The next element added to the report will be at the top of a page.
     *
     * Note that you cannot call addPageBreak twice in a row, it will only insert one page break.
     * To get a completely empty page you need to insert an empty text element between both calls
     * to addPageBreak.
     *
     * Also note that exporting to an image, or using an endless printer, will remove
     * all page breaks in the report.
     */
    void addPageBreak();

    /**
     * Set the list of tabs (tabulations) to use when adding paragraph elements.
     * Those tabs will be set for any paragraph element that is added from now on,
     * until calling this method again.
     *
     * This method allows to align numbers based on their decimal separator, for instance.
     * Example:
     * \code
     QList<QTextOption::Tab> tabs;
     QTextOption::Tab tab;
     tab.position = 50; // in mm
     tab.type = QTextOption::DelimiterTab;
     tab.delimiter = QLatin1Char( '.' );
     tabs.append( tab );
     report.setTabPositions( tabs );

     Then insert text using a tab, like TextElement("\t123.456").
     * \endcode
     *
     * \since 1.2
     */
    void setTabPositions(const QList<QTextOption::Tab> &tabs);

    /**
     * Returns a tab position that is always right-aligned to the right edge of the paper.
     * This is convenient for having left-aligned and right-aligned text on the same line.
     * \since 1.3, static \since 2.0
     */
    static QTextOption::Tab rightAlignedTab();

    /**
     * Returns a tab position that is always center-aligned to the middle of the paper.
     * This is convenient for having left-aligned, center-aligned (and potentially right-aligned)
     * text on the same line.
     * \since 1.3, static \since 2.0
     */
    static QTextOption::Tab middleAlignedTab();

    /**
     * Sets the paragraph margins for all paragraph elements to be created from now on.
     * The default margins are 0,0,0,0.
     * \since 1.3
     */
    void setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * Sets the page size of the report. Example: setPageSize(QPrinter::A4) or setPageSize(QPrinter::Letter);
     * Note that for performance reasons, it is recommended to do this after
     * adding all the elements to the report rather than before.
     * Deprecated, use setPageSize(QPageSize::PageSizeId), or setPageSize(QPageSize)
     */
    void setPageSize(QPrinter::PageSize size);
#endif

    /**
     * Sets the page size of the report. Example: setPageSize(QPageSize::A4) or setPageSize(QPageSize::Letter);
     * Note that for performance reasons, it is recommended to do this after
     * adding all the elements to the report rather than before.
     */
    void setPageSize(QPageSize::PageSizeId size);

    /**
     * Sets the page size of the report. Example: setPageSize(QPageSize::A4) or setPageSize(QPageSize::Letter);
     * Note that for performance reasons, it is recommended to do this after
     * adding all the elements to the report rather than before.
     */
    void setPageSize(const QPageSize &size);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * \return the page size set by setPageSize. The default page size is A4.
     */
    QPrinter::PageSize pageSize() const;
#else
    /**
     * \return the page size set by setPageSize. The default page size is A4.
     */
    QPageSize pageSize() const;
#endif

    /**
     * Sets a custom paper size for the report.
     * Supported units are DevicePixel, Millimeter, Point and Inch.
     */
    void setPaperSize(QSizeF paperSize, QPrinter::Unit unit);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * Sets the orientation, QPrinter::Portrait or QPrinter::Landscape.
     * Deprecated, use setPageOrientation
     */
    Q_DECL_DEPRECATED void setOrientation(QPrinter::Orientation orientation);
    /**
     * \return the orientation set by setOrientation. The default orientation is QPrinter::Portrait.
     * Deprecated, use pageOrientation
     */
    Q_DECL_DEPRECATED QPrinter::Orientation orientation() const;
#endif

    /**
     * Sets the orientation, QPageLayout::Portrait or QPageLayout::Landscape.
     */
    void setPageOrientation(QPageLayout::Orientation orientation);
    /**
     * \return the orientation set by setPageOrientation. The default orientation is QPageLayout::Portrait.
     */
    QPageLayout::Orientation pageOrientation() const;

    /**
     * Set a custom page size for an endless printer.
     * The page width is known, the document is laid out without pagination
     * within that width. The page height is set automatically so that the
     * entire document fits within one page.
     *
     * When calling setWidthForEndlessPrinter you don't have to call setPageSize or setOrientation.
     *
     * Call setWidthForEndlessPrinter(0) to reset to normal (pagesize-dependent) behavior.
     *
     * NOTE: with Qt-4.3, this feature only works reliably on Unix, using Qt compiled with CUPS support.
     * With Qt-4.4, there is no such limitation and this feature should work everywhere.
     */
    void setWidthForEndlessPrinter(qreal widthMM);

    /**
     * Sets the page margins in mm. The default margins are 20 mm.
     */
    void setMargins(qreal top, qreal left, qreal bottom, qreal right);

    /**
     * \since 1.1
     */
    void getMargins(qreal *top, qreal *left, qreal *bottom, qreal *right) const;

    /**
     * Sets the top page margins in mm. The default margin is 20 mm.
     * \since 1.4
     */
    void setTopPageMargin(qreal top);

    /**
     * \return the top page margin in mm.
     * \since 1.4
     */
    qreal topPageMargins() const;

    /**
     * Sets the left page margins in mm. The default margin is 20 mm.
     * \since 1.4
     */
    void setLeftPageMargin(qreal left);

    /**
     * \return the left page margin in mm.
     * \since 1.4
     */
    qreal leftPageMargins() const;

    /**
     * Sets the right page margins in mm. The default margin is 20 mm.
     * \since 1.4
     */
    void setRightPageMargin(qreal right);

    /**
     * \return the right page margin in mm.
     * \since 1.4
     */
    qreal rightPageMargins() const;

    /**
     * Sets the bottom page margins in mm. The default margin is 20 mm.
     * \since 1.4
     */
    void setBottomPageMargin(qreal bottom);

    /**
     * \return the bottom page margin in mm.
     * \since 1.4
     */
    qreal bottomPageMargins() const;

    /**
     * Sets the margin between the header and the body of the report, in mm.
     * This margin is 0 by default, i.e. the body starts immediately under the header.
     * \since 1.1
     */
    void setHeaderBodySpacing(qreal spacing);

    /**
     * \return the margin between the header and the body of the report, in mm.
     * \since 1.4
     */
    qreal headerBodySpacing() const;

    /**
     * Sets the margin between the footer and the body of the report, in mm.
     * This margin is 0 by default, i.e. the footer starts immediately under the body.
     * \since 1.1
     */
    void setFooterBodySpacing(qreal spacing);

    /**
     * \return the margin between the footer and the body of the report, in mm.
     * \since 1.4
     */
    qreal footerBodySpacing() const;

    /**
     * Returns a reference to a header object.
     * Calling this method makes the report have a header on the specified pages.
     *
     * Calling the method with different sets of intersecting flags leads to undefined behavior.
     * For instance header(EvenPages|FirstPage) and header(OddPages) is fine,
     * but header(EvenPages|FirstPage) and header(FirstPage) leads to two headers being
     * defined for the first page, anyone of the two could be picked.
     * Calling the method with the same set multiple times, for instance header(EvenPages|FirstPage),
     * is ok though: the same header instance will be returned every time.
     *
     * Note that all headers will occupy the same height: the height of the
     * tallest header.
     *
     * Specify the contents of the header by calling Header::addElement.
     */
    Header &header(HeaderLocations hl = AllPages);

    /**
     * \return a reference to the footer object.
     * Calling this method makes the report have a footer on the specified pages.
     * See header() for caveats.
     *
     * Specify the contents of the footer by calling Footer::addElement.
     */
    Footer &footer(HeaderLocations hl = AllPages);

    /**
     * Associates a model with a model key. This is only used if the
     * report description is going to be loaded from an XML file, using \a
     * loadFromXML. This method needs to be called before \a
     * loadFromXML is called.
     */
    void associateModel(const QString &modelKey, QAbstractItemModel *model);

    /**
     * Associate a text string with the id of a text or html element.
     *
     * \param id the id of the text or html element, specified using id="foo" in the XML,
     *  or using TextElement::setId().
     * \param value the text for this element, which will appear in the report.
     * The value can include newlines.
     * If associateTextValue was already called with the id \p id, the new value
     * replaces the old one.
     */
    void associateTextValue(const QString &id, const QString &value);

    /**
     * Associate a pixmap with the id of an image element.
     *
     * \param id the id of the image element, specified using id="foo" in the XML,
     *  or using ImageElement::setId().
     * \param value the pixmap to use in the report.
     * If associateImageValue was already called with the id \p id, the new value
     * replaces the old one.
     */
    void associateImageValue(const QString &id, const QPixmap &value);

    /**
     * Associate an image with the id of an image element. This is only used if the
     * report description is going to be loaded from an XML file, using \a
     * loadFromXML. This method needs to be called before \a
     * loadFromXML is called.
     * \param id the id of the image element, specified using id="foo" in the XML.
     * \param value the pixmap to use in the report.
     * If associateImageValue was already called with the id \p id, the new value
     * replaces the old one.
     *
     * This overload is especially useful in threads.
     * \since 1.3
     */
    void associateImageValue(const QString &id, const QImage &value);

    /**
     * Loads a report definition from an XML document. Notice that if the
     * report definition contains Chart or Table elements, you
     * need to call \a associate for each such element. \a iodevice
     * needs to be open and readable.
     * If you have any chart or table elements in your report, you
     * must associate the models using associateModel() first,
     * otherwise these elements will stay empty.
     * \return true if the XML document was successfully loaded, false otherwise
     */
    bool loadFromXML(QIODevice *iodevice, ErrorDetails *details = nullptr);

    /**
     * This is an overloaded member function, provided for convenience.
     * With this method you can modify the QDomDocument before giving it to KDReports.
     *
     * Note: if the QDomDocument is simply created with a file or QIODevice as argument,
     * it will ignore whitespace-only nodes like &lt;text> &lt;/text>. The other loadFromXML
     * takes care of this, while this overload requires that the QDomDocument has been
     * properly set up.
     *
     * \return true if the XML document was successfully loaded, false otherwise
     * \since 1.1
     */
    bool loadFromXML(const QDomDocument &doc, ErrorDetails *details = nullptr);

    /**
     * Sets an xml element handler.
     * The report does not take ownership of the xml element handler.
     *
     * If you want it to be deleted with the report, derive from both QObject and XmlElementHandler
     * and give the report as parent QObject.
     * \since 1.2
     */
    void setXmlElementHandler(KDReports::XmlElementHandler *handler);

    /**
     * Sets the current row for the given model.
     * When this value is set, it overrides the values
     * of the 'row' attributes within XML tags
     * \since 1.3
     */
    void setCurrentRow(const QAbstractItemModel *model, int row);

    /**
     * Sets the name of the report.
     * This is used for QPrinter::setDocName(), which gives a name to the print job.
     * \since 1.7
     */
    void setDocumentName(const QString &name);

    /**
     * Returns the name of the report.
     * \since 1.7
     */
    QString documentName() const;

    /**
     * Show the print dialog to let the user choose a printer, and print.
     * \param parent the parent widget for the progress dialog that appears when printing
     * \return false if the print dialog was cancelled
     */
    bool printWithDialog(QWidget *parent);

    /**
     * Print directly using the setup values from a preexisting
     * QPrinter object.
     * Notice that this will only change settings like page sizes or
     * orientation temporarily, during this particular print
     * operation. After the operation, these values will be reset to
     * their previous values.
     *
     * Returns false if cancelled (not possible yet).
     * \param printer the printer to use for printing
     * \param parent the parent widget for the progress dialog that appears when printing
     */
    bool print(QPrinter *printer, QWidget *parent = nullptr);

    /**
     * Export the whole report to a PS or PDF file.
     * If the file name has the suffix ".ps" then PostScript is automatically
     * selected as output format.
     * If the file name has the ".pdf" suffix PDF is generated.
     */
    bool exportToFile(const QString &fileName, QWidget *parent = nullptr);

    /**
     * Export the whole report to an image file.
     * \param size the size of the image in pixels
     * \param fileName the name of the image file
     * \param format the format of the image, for instance: BMP, JPG, PNG.
     */
    bool exportToImage(QSize size, const QString &fileName, const char *format);

    /**
     * Export the whole report to HTML.
     * Note that HTML export does not include headers and footers, nor watermark.
     *
     * Images are saved into separate files.
     * \since 1.2
     */
    bool exportToHtml(const QString &fileName);

    /**
     * Paints a given page of the report into the painter.
     * Note that this method can be used for printing and for on-screen rendering
     * so it doesn't fill the background with white, the caller has do to that.
     * @param pageNumber the page to paint, starting at 0
     * @param painter the QPainter to paint into
     */
    void paintPage(int pageNumber, QPainter &painter);

    /**
     * Sets the number of the first page, so that the variable PageNumber
     * starts at another value than 1. This is useful when splitting a
     * large report into smaller documents.
     */
    void setFirstPageNumber(int num);

    /**
     * \return the number given to setFirstPageNumber
     */
    int firstPageNumber() const;

    /**
     * \return the report's number of pages (with the current page size).
     */
    int numberOfPages() const;

    /**
     * \return true when scaleTo was called with numPagesHorizontally > 0
     */
    bool isTableBreakingEnabled() const;

    enum TableBreakingPageOrder
    {
        DownThenRight,
        RightThenDown
    };

    /**
     * Sets the page order to use when breaking tables.
     * This method is kept for compatibility, use
     * mainTable()->setTableBreakingPageOrder() in new code, instead.
     *
     * \since 1.1
     */
    void setTableBreakingPageOrder(TableBreakingPageOrder pageOrder);

    /**
     * \return the table breaking page order given to setTableBreakingPageOrder
     * \since 1.1
     */
    TableBreakingPageOrder tableBreakingPageOrder() const;

    /**
     * \short Scale the fonts in the document by a given factor.
     *
     * Scale all the fonts used in the main part of the report
     * (not headers and footers) by the given amount.
     * Therefore setFontScalingFactor(1.0) is a no-op.
     *
     * \since 1.1
     */
    void setFontScalingFactor(qreal factor);

    /**
     * \return the font scaling factor given to setFontScalingFactor
     * \since 1.1
     */
    qreal fontScalingFactor() const;

    /**
     * \short Ensure that the report fits into a number of pages.
     *
     * By default tables are layouted so that all columns fit into the page width,
     * but for large tables this can wrap the text too much.
     * This method allows to scale the report so that it fits into a number
     * of pages horizontally and vertically. For instance, 3 pages horizontally
     * means that tables columns can span over 3 pages, arranged side by side.
     * This is a maximum number of pages; if the table is really small it will still
     * occupy only a single page.
     *
     * Scaling also means that the font sizes can be reduced (similar to what setFontScalingFactor does)
     * so that the report fits into the number of pages specified by this method.
     *
     * \param numPagesHorizontally number of pages in the horizontal direction,
     *         1 for no table breaking in the horizontal direction.
     * \param numPagesVertically number of pages in the vertical direction, 0 for no limit
     *
     * \since 1.1
     */
    void scaleTo(int numPagesHorizontally, int numPagesVertically);

    /**
     * \return the number of horizontal pages set with scaleTo, otherwise 1.
     * \since 1.1
     */
    int maximumNumberOfPagesForHorizontalScaling() const;
    /**
     * \return the number of vertical pages set with scaleTo, otherwise 0.
     * \since 1.1
     */
    int maximumNumberOfPagesForVerticalScaling() const;

    /**
     * Sets a fixed row height. Only available in spreadsheet mode.
     * The main use case is printing of fixed size labels.
     * \since 1.7
     */
    void setFixedRowHeight(qreal mm);

    /**
     * Returns a list of the auto-table elements that were used to create this report.
     * You wouldn't normally need this method; this is only to be able to change some
     * settings like showing table headers or table borders at runtime.
     * \since 1.1
     *
     * Note: use mainTable() instead, in spreadsheet mode
     */
    QList<KDReports::AutoTableElement *> autoTableElements() const;

    /**
     * Regenerate the contents of auto tables. This can be useful if the data in the item model
     * changes frequently, to save time compared to recreating the full report.
     * \since 1.1
     */
    void regenerateAutoTables();

    /**
     * Regenerate the contents of a single auto table. This can be useful if the data in
     * this specific model changes frequently, to save time compared to recreating the full report.
     * \since 1.2
     */
    void regenerateAutoTableForModel(QAbstractItemModel *model);

    /**
     * Sets the text to be used as a watermark. By default, the text
     * will be displayed with 0 degrees rotation, in a light gray, in
     * a 48pt Helvetica font, centered on the page.
     * You can set both a watermark text and a watermark image, but
     * the two will overwrite each other so this is not
     * recommended. Also notice that if you choose a watermark color
     * that is too dark, this will make the reports less legible.
     */
    void setWatermarkText(const QString &text, int rotation = 0, const QColor &color = QColor(204, 204, 204), const QFont &font = QFont(QStringLiteral("Helvetica"), 48));

    /**
     * \return the watermark text.
     */
    QString watermarkText() const;

    /**
     * \return the watermark text rotation.
     */
    int watermarkRotation() const;

    /**
     * \return the watermark text color.
     */
    QColor watermarkColor() const;

    /**
     * \return the watermark text font.
     */
    QFont watermarkFont() const;

    /**
     * Sets the pixmap to be used as a watermark. By default, the image
     * will be "grayed out" much like QIcon does for disabled
     * pixmaps. If you do not want this behavior, set \a autoGrayOut
     * to false. However, you need to ensure then that the image does
     * prevent the legibility of the report. The pixmap will not be
     * scaled, it will be centered on the page.
     * You can set both a watermark text and a watermark image, but
     * the two will overwrite each other so this is not recommended.
     */
    void setWatermarkPixmap(const QPixmap &pixmap, bool autoGrayOut = true);

    /**
     * \return the image to be used as watermark. If autoGrayOut was
     * used when calling \a setWatermarkPixmap, this is not the same
     * pixmap as was passed to setWatermarkPixmap.
     */
    QPixmap watermarkPixmap() const;

    /**
     * Same as setWatermarkPixmap but with a QImage. Especially useful in threads.
     * \since 1.2
     */
    void setWatermarkImage(const QImage &image);

    /**
     * \return the image to be used as watermark.
     * \since 1.2
     */
    QImage watermarkImage() const;

    using WatermarkFunction = std::function<void(QPainter &, int)>;
    /**
     * Sets a function to be called for painting the watermark with QPainter
     * The page area to be painted is defined by (0,0) and paperSize().
     * To draw at an absolute position in millimeters, use mmToPixels() for conversion.
     * The arguments to the function are: QPainter *painter, int pageNumber
     * \since 2.2
     */
    void setWatermarkFunction(WatermarkFunction function);

    /**
     * \return the watermark function passed to setWatermarkFunction
     * \since 2.2
     */
    WatermarkFunction watermarkFunction() const;

    /**
     * \return the location for the given header.
     * \since 1.4
     */
    KDReports::HeaderLocations headerLocation(Header *header) const;

    /**
     * \return the location for the given footer.
     * \since 1.4
     */
    KDReports::HeaderLocations footerLocation(KDReports::Footer *footer) const;

    /**
     * Set the header location
     * \since 1.4
     */
    void setHeaderLocation(HeaderLocations hl, Header *header);

    /**
     * Set the footer location
     * \since 1.4
     */
    void setFooterLocation(HeaderLocations hl, Footer *footer);

    /**
     * mmToPixels converts a distance in millimeters, to pixels, for drawing in the watermarkFunction.
     * @param mm distance in millimeters
     * @return distance in pixels
     * \since 2.2
     */
    static qreal mmToPixels(qreal mm);

    /**
     * \internal Returns the paper size in pixels.
     */
    QSizeF paperSize() const;

    /**
     * \internal (debug)
     */
    void dump() const;

    /**
     * \internal Returns the current builder cursor position.
     */
    int currentPosition() const;

    /**
     * Indicates that the elements are about to be added to the report.
     * This makes report generation much faster, since the report doesn't
     * have to be relayouted until endEdit() is called.
     * It is possible to nest calls to beginEdit()/endEdit(), in which case
     * only the topmost pair of calls will have an effect.
     *
     * \since 1.6
     */
    void beginEdit();

    /**
     * Indicates that the end of a block of editing operations.
     * It is possible to call beginEdit()/endEdit() again afterwards, for
     * another block of editing operations.
     *
     * \since 1.6
     */
    void endEdit();

    /**
     * Returns the target of the anchor at position pos, or an empty string
     * if no anchor exists at that position.
     * @param pageNumber The page on which the mouse is, starting at 0
     * @param pos The position of the mouse, in pixels
     *
     * \since 2.0
     */
    QString anchorAt(int pageNumber, QPoint pos) const;

    /**
     * Returns the QTextDocument that contains the main part of the report,
     * assuming reportMode() is WordProcessing.
     * Note that this does not include any headers or footers, nor watermark.
     *
     * This method can be used to display the report in a readonly QTextEdit
     * or in a QTextBrowser.
     *
     * Modifying this QTextDocument will lead to undefined behaviour in KDReports.
     * Returns nullptr if reportMode() is set to SpreadSheet.
     * @since 2.1
     */
    QTextDocument *mainTextDocument() const;

private:
    friend class Test;
    friend class ::ReportData;
    friend class ::EditorData;
    KDReports::TextDocument &doc() const;

    QString asHtml() const;

    void setupPrinter(QPrinter *printer);

private:
    Q_DISABLE_COPY(Report)
    friend class HeaderReportBuilder; // for headerChanged()
    friend class ImageElement; // for textDocumentWidth()
    friend class ChartElement; // for textDocumentWidth()
    friend class XmlParser; // d->m_builder
    friend class Header; // doc()
    friend class PreviewDialogPrivate; // setupPrinter
    friend class PreviewWidgetPrivate; // setupPrinter
    friend class ReportPrivate; // setupPrinter
    std::unique_ptr<ReportPrivate> d;
};

}

#endif /* KDREPORTSREPORT_H */
