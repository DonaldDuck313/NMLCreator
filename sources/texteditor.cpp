#include <QSettings>
#include <QMimeData>
#include <QScrollBar>
#include "texteditor.h"

TextEditor::TextEditor(SyntaxHighlighter::Type syntaxHighlighter, QWidget *parent):
    QPlainTextEdit(parent),
    _lineNumberArea(this),
    _syntaxHighligher(this, syntaxHighlighter)
{
    //Set simple properties
    this->setFont(QSettings("OpenTTD", "NMLCreator").value("textEditor/font", QFontDatabase::systemFont(QFontDatabase::FixedFont)).value<QFont>());
    this->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

    //Line numbers
    const auto updateLineNumberAreaWidth = [this](){
        this->setViewportMargins(this->lineNumberAreaWidth(), 0, 0, 0);
    };

    QObject::connect(this, &TextEditor::blockCountChanged, updateLineNumberAreaWidth);
    QObject::connect(this, &TextEditor::updateRequest, [this, updateLineNumberAreaWidth](const QRect &rect, int dy){
        if(dy != 0){
            this->_lineNumberArea.scroll(0, dy);
        }
        else{
            this->_lineNumberArea.update(0, rect.y(), this->_lineNumberArea.width(), rect.height());
        }
        if(rect.contains(this->viewport()->rect())){
            updateLineNumberAreaWidth();
        }
    });
    updateLineNumberAreaWidth();

    //Highlight the current line
    QObject::connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);
    this->highlightCurrentLine();
}

int TextEditor::lineNumberAreaWidth() const{
    int digits = 1;
    int max = qMax(1, this->blockCount());
    while(max >= 10){
        max /= 10;
        digits++;
    }
    return 3 + this->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void TextEditor::resizeEvent(QResizeEvent *event){
    QPlainTextEdit::resizeEvent(event);
    const QRect cr = this->contentsRect();
    this->_lineNumberArea.setGeometry(cr.left(), cr.top(), this->lineNumberAreaWidth(), cr.height());
}

void TextEditor::addError(int line){
    this->_linesWithErrors.append(line);
    this->highlightCurrentLine();
}

void TextEditor::addWarning(int line){
    this->_linesWithWarnings.append(line);
    this->highlightCurrentLine();
}

void TextEditor::removeError(int line){
    this->_linesWithErrors.removeAll(line);
    this->highlightCurrentLine();
}

void TextEditor::removeWarning(int line){
    this->_linesWithWarnings.removeAll(line);
    this->highlightCurrentLine();
}

void TextEditor::removeAllErrors(){
    this->_linesWithErrors.clear();
    this->highlightCurrentLine();
}

void TextEditor::removeAllWarnings(){
    this->_linesWithWarnings.clear();
    this->highlightCurrentLine();
}

void TextEditor::highlightCurrentLine(){
    QList<QTextEdit::ExtraSelection> extraSelections;
    if(!this->isReadOnly()){
        QTextEdit::ExtraSelection selection;
        const QColor lineColor = QColor(Qt::blue).lighter(190);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = this->textCursor();
        selection.cursor.clearSelection();    //selection.cursor is a copy of this->textCursor() so this won't clear the actual selection
        extraSelections.append(selection);
    }
    const auto highlightLine = [&](int lineNumber, const QColor lineColor){
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = QTextCursor(this->document()->findBlockByLineNumber(lineNumber - 1));    //-1 because findBlockByLineNumber counts line numbers starting at 0
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    };
    for(int lineNumber: qAsConst(this->_linesWithWarnings)){
        highlightLine(lineNumber, QColor(Qt::yellow).lighter((lineNumber == this->textCursor().blockNumber() + 1) ? 120 : 160));
    }
    for(int lineNumber: qAsConst(this->_linesWithErrors)){
        highlightLine(lineNumber, QColor(Qt::red).lighter((lineNumber == this->textCursor().blockNumber() + 1) ? 140 : 170));
    }
    this->setExtraSelections(extraSelections);
}

void TextEditor::LineNumberArea::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = this->_textEditor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(this->_textEditor->blockBoundingGeometry(block).translated(this->_textEditor->contentOffset()).top());
    int bottom = top + qRound(this->_textEditor->blockBoundingRect(block).height());

    while(block.isValid() && top <= event->rect().bottom()){
        if(block.isVisible() && bottom >= event->rect().top()){
            painter.setPen(Qt::black);
            painter.drawText(0, top, this->width(), this->_textEditor->fontMetrics().height(), Qt::AlignRight, QString::number(blockNumber + 1));
            const int imageSize = qMin(this->_textEditor->lineNumberAreaWidth(), this->_textEditor->fontMetrics().height());
            const QRect imagePosition(0, top, imageSize, imageSize);
            if(this->_textEditor->_linesWithWarnings.contains(blockNumber + 1)){
                painter.drawImage(imagePosition, QImage(":/icons/warning.svg"));
            }
            if(this->_textEditor->_linesWithErrors.contains(blockNumber + 1)){
                painter.drawImage(imagePosition, QImage(":/icons/error.svg"));
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(this->_textEditor->blockBoundingRect(block).height());
        blockNumber++;
    }
}

void TextEditor::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Tab){
        const int start = this->textCursor().selectionStart();
        const int end = this->textCursor().selectionEnd();

        //Indent selection if pressing tab when text is selected
        if(start != end){
            const int initialScroll = this->verticalScrollBar()->value();
            const QString initialText = this->toPlainText().replace("\r\n", "\n");
            QStringList linesBeforeSelection = initialText.left(start).split(QRegExp("[\r\n]"));
            QStringList linesInSelection = initialText.mid(start, end - start).split(QRegExp("[\r\n]"));
            const QString afterSelection = initialText.right(initialText.length() - end);

            linesBeforeSelection[linesBeforeSelection.length() - 1] = "    " + linesBeforeSelection.last();
            for(int i = 1; i < linesInSelection.length(); i++){    //Start at 1 because the selection might start in the middle of a line and we don't want to insert spaces there
                linesInSelection[i] = "    " + linesInSelection[i];
            }

            this->setPlainText(linesBeforeSelection.join("\n") + linesInSelection.join("\n") + afterSelection);
            QTextCursor textCursor = this->textCursor();
            textCursor.setPosition(start + 4, QTextCursor::MoveAnchor);
            textCursor.setPosition(end + 4 * linesInSelection.length(), QTextCursor::KeepAnchor);
            this->setTextCursor(textCursor);
            this->verticalScrollBar()->setValue(initialScroll);
        }

        //Insert spaces if pressing tab when text isn't selected
        else{
            this->insertPlainText("    ");
        }
    }
    else if(event->key() == Qt::Key_Backtab){
        const int start = this->textCursor().selectionStart();
        const int end = this->textCursor().selectionEnd();

        //Unindent selection if pressing backtab when text is selected
        if(start != end){
            const int initialScroll = this->verticalScrollBar()->value();
            const QString initialText = this->toPlainText().replace("\r\n", "\n");
            QStringList linesBeforeSelection = initialText.left(start).split(QRegExp("[\r\n]"));
            QStringList linesInSelection = initialText.mid(start, end - start).split(QRegExp("[\r\n]"));
            const QString afterSelection = initialText.right(initialText.length() - end);

            const int lastLineBeforeSelectionInitialLength = linesBeforeSelection.last().length();
            linesBeforeSelection.last().replace(QRegExp("^ {1,4}"), "");
            for(int i = 1; i < linesInSelection.length(); i++){    //Start at 1 because the selection might start in the middle of a line and we don't want to insert spaces there
                linesInSelection[i].replace(QRegExp("^ {1,4}"), "");
            }

            this->setPlainText(linesBeforeSelection.join("\n") + linesInSelection.join("\n") + afterSelection);
            QTextCursor textCursor = this->textCursor();
            textCursor.setPosition(start - lastLineBeforeSelectionInitialLength + linesBeforeSelection.last().length(), QTextCursor::MoveAnchor);
            textCursor.setPosition(end - initialText.length() + this->toPlainText().length(), QTextCursor::KeepAnchor);
            this->setTextCursor(textCursor);
            this->verticalScrollBar()->setValue(initialScroll);
        }
    }
    else{
        QPlainTextEdit::keyPressEvent(event);

        //Auto-indent if the user just inserted a newline
        if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
            int pos = this->textCursor().position() - 1;
            const QString text = this->toPlainText();
            while(pos >= 0 && text[pos].isSpace()){
                pos--;
            }
            if(text[pos] == '{'){
                this->insertPlainText("    ");
            }
            while(pos >= 0 && text[pos] != '\r' && text[pos] != '\n'){
                pos--;
            }
            pos++;
            while(pos < text.length() && text[pos] == ' '){
                this->insertPlainText(" ");
                pos++;
            }
        }

        //Auto-unindent if the user just inserted a } character
        if(event->key() == Qt::Key_BraceRight){
            const int initialPos = this->textCursor().position();
            const int initialScroll = this->verticalScrollBar()->value();
            int pos = initialPos - 2;
            int removedCharacters = 0;
            QString text = this->toPlainText();
            for(int i = 0; i < 4; i++){
                if(pos < 0 || text[pos] != ' '){
                    break;
                }
                text = text.remove(pos, 1);
                removedCharacters++;
                pos--;
            }
            this->setPlainText(text);
            QTextCursor textCursor = this->textCursor();
            textCursor.setPosition(initialPos - removedCharacters);
            this->setTextCursor(textCursor);
            this->verticalScrollBar()->setValue(initialScroll);
        }
    }
}

void TextEditor::insertFromMimeData(const QMimeData *source){
    if(source->hasText() && source->text().contains("\t")){
        QString text = source->text();
        text.replace("\t", "    ");
        QMimeData newMimeData;
        newMimeData.setText(text);
        QPlainTextEdit::insertFromMimeData(&newMimeData);
    }
    else{
        QPlainTextEdit::insertFromMimeData(source);
    }
}
