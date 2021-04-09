#include <QRegularExpression>
#include <QSettings>
#include "syntaxhighlighter.h"

QColor SyntaxHighlighter::commentColor(QSettings("OpenTTD", "NMLCreator").value("textEditor/commentColor", QColor(128, 128, 128)).value<QColor>());
QColor SyntaxHighlighter::literalStringColor(QSettings("OpenTTD", "NMLCreator").value("textEditor/literalStringColor", QColor(175, 175, 0)).value<QColor>());
QColor SyntaxHighlighter::numberColor(QSettings("OpenTTD", "NMLCreator").value("textEditor/numberColor", QColor(128, 0, 128)).value<QColor>());
QColor SyntaxHighlighter::constantColor(QSettings("OpenTTD", "NMLCreator").value("textEditor/constantColor", QColor(200, 128, 0)).value<QColor>());
QColor SyntaxHighlighter::keywordColor(QSettings("OpenTTD", "NMLCreator").value("textEditor/keywordColor", QColor(255, 0, 100)).value<QColor>());
QColor SyntaxHighlighter::propertyColor(QSettings("OpenTTD", "NMLCreator").value("textEditor/propertyColor", QColor(0, 100, 255)).value<QColor>());
QColor SyntaxHighlighter::blockNameColor(QSettings("OpenTTD", "NMLCreator").value("textEditor/blockNameColor", QColor(50, 150, 0)).value<QColor>());

const QList<QPair<const QRegularExpression, QColor*>> SyntaxHighlighter::_nmlHighlightingRules({
    {QRegularExpression("\\b[A-Z0-9_]+\\b"), &constantColor},
    {QRegularExpression("\\b([0-9\\.]+|0x[0-9A-Fa-f]+)\\b"), &numberColor},
    {QRegularExpression("\\b(string|date|bitmask)(?=\\s*\\()"), &keywordColor},
    {QRegularExpression("(?<=[;{])(\\s*//.*)?\\s*return\\s+"), &keywordColor},
    {QRegularExpression("\\b\\w+(?=\\s*(\\(([^)]|(\\([^)]*\\)))*\\))?\\s*\\{)"), &blockNameColor},
    {QRegularExpression("(?<=[;{])(\\s*//.*)?[\\s\\w]+(?=:)"), &propertyColor},
    {QRegularExpression("\"[^\"]*([\"\r\n]|$)"), &literalStringColor},
    {QRegularExpression("//.*([\r\n]|$)"), &commentColor}
});

const QList<QPair<const QRegularExpression, QColor*>> SyntaxHighlighter::_lngHighlightingRules({
    {QRegularExpression("(?<=[\r\n])[\\s\\w]+(?=:)"), &constantColor},
    {QRegularExpression("(?<=:).+(?=([\r\n]|$))"), &literalStringColor},
    {QRegularExpression("\\{[^{}\r\n]*\\}"), &keywordColor},
    {QRegularExpression("#.*([\r\n]|$)"), &commentColor}
});

QList<SyntaxHighlighter*> SyntaxHighlighter::_syntaxHighlighters;

SyntaxHighlighter::SyntaxHighlighter(QPlainTextEdit *parent, Type type):
    QSyntaxHighlighter(parent->document()),
    _type(type),
    _parent(parent)
{
    this->_syntaxHighlighters.append(this);
}

SyntaxHighlighter::~SyntaxHighlighter(){
    this->_syntaxHighlighters.removeAll(this);
}

void SyntaxHighlighter::updateAllSyntaxHighlighters(){
    for(SyntaxHighlighter *highlighter: qAsConst(_syntaxHighlighters)){
        highlighter->_parent->blockSignals(true);    //Block the signals otherwise it will think that the file has been edited
        const int cursorPosition = highlighter->_parent->textCursor().position();
        highlighter->_parent->setFont(QSettings("OpenTTD", "NMLCreator").value("textEditor/font", QFontDatabase::systemFont(QFontDatabase::FixedFont)).value<QFont>());
        highlighter->_parent->setPlainText(highlighter->_parent->toPlainText());
        QTextCursor textCursor = highlighter->_parent->textCursor();
        textCursor.setPosition(cursorPosition);
        highlighter->_parent->setTextCursor(textCursor);
        highlighter->_parent->blockSignals(false);
    }
}

void SyntaxHighlighter::highlightBlock(const QString &text){
    const QList<QPair<const QRegularExpression, QColor*>> *highlightingRules = nullptr;
    switch(this->_type){
    case NML:
        highlightingRules = &this->_nmlHighlightingRules;
        break;
    case LNG:
        highlightingRules = &this->_lngHighlightingRules;
        break;
    default:
        return;
    }

    QString textBefore, textAfter;
    for(int i = 0; i < this->currentBlock().blockNumber(); i++){
        textBefore += this->document()->findBlockByNumber(i).text();
    }
    for(int i = this->currentBlock().blockNumber() + 1; i < this->document()->blockCount(); i++){
        textAfter += this->document()->findBlockByNumber(i).text();
    }

    const QString completeText = textBefore + "\n" + text + "\n" + textAfter;

    for(const auto &rule: *highlightingRules){
        QTextCharFormat format;
        format.setForeground(QBrush(*rule.second));
        QRegularExpressionMatchIterator iterator = rule.first.globalMatch(completeText);
        while(iterator.hasNext()){
            QRegularExpressionMatch match = iterator.next();
            const int start = qBound(0, match.capturedStart() - textBefore.length() - 1, text.length());
            const int end = qBound(0, match.capturedStart() + match.capturedLength() - textBefore.length() - 1, text.length());
            this->setFormat(start, end - start, format);
        }
    }
}
