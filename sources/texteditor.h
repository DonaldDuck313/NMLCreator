#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>
#include <QPainter>
#include <QTextBlock>
#include "syntaxhighlighter.h"

class TextEditor : public QPlainTextEdit{
    Q_OBJECT

    class LineNumberArea : public QWidget{
    public:
        LineNumberArea(TextEditor *editor): QWidget(editor), _textEditor(editor){}

        QSize sizeHint() const override{
            return QSize(this->_textEditor->lineNumberAreaWidth(), 0);
        }

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        TextEditor *_textEditor;
    };

public:
    TextEditor(SyntaxHighlighter::Type syntaxHighlighter = SyntaxHighlighter::None, QWidget *parent = nullptr);

    int lineNumberAreaWidth() const;

    void addError(int line);
    void addWarning(int line);

    void removeError(int line);
    void removeWarning(int line);

public slots:
    void removeAllErrors();
    void removeAllWarnings();

private slots:
    void highlightCurrentLine();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void insertFromMimeData(const QMimeData *source) override;

private:
    LineNumberArea _lineNumberArea;
    QList<int> _linesWithErrors, _linesWithWarnings;
    SyntaxHighlighter _syntaxHighligher;
};

#endif // TEXTEDITOR_H
