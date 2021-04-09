#ifndef TEXTEDITORLIST_H
#define TEXTEDITORLIST_H

#include <QObject>
#include "texteditor.h"

class TextEditorList : public QObject{
    Q_OBJECT

public:
    TextEditorList();
    virtual ~TextEditorList();

    QMap<QString, TextEditor*>::iterator begin();
    QMap<QString, TextEditor*>::const_iterator begin() const;
    QMap<QString, TextEditor*>::iterator end();
    QMap<QString, TextEditor*>::const_iterator end() const;

    TextEditor *addTextEditor(const QString &fileName, SyntaxHighlighter::Type syntaxHighlighter = SyntaxHighlighter::None, QWidget *parentWindow = nullptr);
    bool removeTextEditor(const QString &fileName);

    TextEditor *textEditorFromFileName(const QString &fileName) const;
    bool undoEnabled(TextEditor *editor) const;
    bool undoEnabled(const QString &fileName) const;
    bool redoEnabled(TextEditor *editor) const;
    bool redoEnabled(const QString &fileName) const;
    bool copyEnabled(TextEditor *editor) const;
    bool copyEnabled(const QString &fileName) const;

    bool hasUnsavedChanges(TextEditor *editor) const;
    bool hasUnsavedChanges(const QString &fileName) const;
    void markAsSaved(TextEditor *editor);
    void markAsSaved(const QString &fileName);

signals:
    void changesInTextEditor(TextEditor*);

private:
    QMap<QString, TextEditor*> _textEditors;
    QMap<TextEditor*, bool> _undoEnabled, _redoEnabled, _copyEnabled, _hasUnsavedChanges;
};

#endif // TEXTEDITORLIST_H
