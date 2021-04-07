#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

class SyntaxHighlighter : public QSyntaxHighlighter{
public:
    enum Type{None, NML, LNG};

    SyntaxHighlighter(QPlainTextEdit *parent, Type type);
    virtual ~SyntaxHighlighter();

    static void updateAllSyntaxHighlighters();

    static QColor commentColor, literalStringColor, numberColor, constantColor, keywordColor, propertyColor, blockNameColor;

protected:
    void highlightBlock(const QString &text) override;

private:
    const Type _type;
    QPlainTextEdit *const _parent;
    static const QList<QPair<const QRegularExpression, QColor*>> _nmlHighlightingRules, _lngHighlightingRules;
    static QList<SyntaxHighlighter*> _syntaxHighlighters;
};

#endif // SYNTAXHIGHLIGHTER_H
