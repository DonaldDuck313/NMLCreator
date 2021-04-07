#include <QMessageBox>
#include "texteditorlist.h"

TextEditorList::TextEditorList(){}

TextEditorList::~TextEditorList(){
    for(TextEditor *editor: qAsConst(this->_textEditors)){
        delete editor;
    }
}

QMap<QString, TextEditor*>::iterator TextEditorList::begin(){
    return this->_textEditors.begin();
}

QMap<QString, TextEditor*>::const_iterator TextEditorList::begin() const{
    return this->_textEditors.begin();
}

QMap<QString, TextEditor*>::iterator TextEditorList::end(){
    return this->_textEditors.end();
}

QMap<QString, TextEditor*>::const_iterator TextEditorList::end() const{
    return this->_textEditors.end();
}

TextEditor *TextEditorList::addTextEditor(const QString &fileName, SyntaxHighlighter::Type syntaxHighlighter, QWidget *parentWindow){
    TextEditor *editor = new TextEditor(syntaxHighlighter);

    //Read the file
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)){
        QMessageBox::critical(parentWindow, "", QObject::tr("Could not open file %1.").arg(fileName));
        return nullptr;
    }
    editor->setPlainText(QString::fromUtf8(file.readAll()));
    file.close();

    //Add the text editor to the list
    this->_textEditors.insert(fileName, editor);

    this->_undoEnabled.insert(editor, false);
    QObject::connect(editor, &TextEditor::undoAvailable, [this, editor](bool enabled){
        this->_undoEnabled[editor] = enabled;
    });
    this->_redoEnabled.insert(editor, false);
    QObject::connect(editor, &TextEditor::redoAvailable, [this, editor](bool enabled){
        this->_redoEnabled[editor] = enabled;
    });
    this->_copyEnabled.insert(editor, false);
    QObject::connect(editor, &TextEditor::copyAvailable, [this, editor](bool enabled){
        this->_copyEnabled[editor] = enabled;
    });
    this->_hasUnsavedChanges.insert(editor, false);
    QObject::connect(editor, &TextEditor::textChanged, [this, editor](){
        this->_hasUnsavedChanges[editor] = true;
        emit this->changesInTextEditor(editor);
    });
    return editor;
}

bool TextEditorList::removeTextEditor(const QString &fileName){
    TextEditor *editor = this->_textEditors[fileName];
    if(editor == nullptr){
        return false;
    }

    this->_textEditors.remove(fileName);
    this->_undoEnabled.remove(editor);
    this->_redoEnabled.remove(editor);
    this->_copyEnabled.remove(editor);
    delete editor;
    return true;
}

TextEditor *TextEditorList::textEditorFromFileName(const QString &fileName) const{
    return this->_textEditors[fileName];
}

bool TextEditorList::undoEnabled(TextEditor *editor) const{
    return this->_undoEnabled[editor];
}

bool TextEditorList::undoEnabled(const QString &fileName) const{
    return this->undoEnabled(this->textEditorFromFileName(fileName));
}

bool TextEditorList::redoEnabled(TextEditor *editor) const{
    return this->_redoEnabled[editor];
}

bool TextEditorList::redoEnabled(const QString &fileName) const{
    return this->redoEnabled(this->textEditorFromFileName(fileName));
}

bool TextEditorList::copyEnabled(TextEditor *editor) const{
    return this->_copyEnabled[editor];
}

bool TextEditorList::copyEnabled(const QString &fileName) const{
    return this->copyEnabled(this->textEditorFromFileName(fileName));
}

bool TextEditorList::hasUnsavedChanges(TextEditor *editor) const{
    return this->_hasUnsavedChanges[editor];
}

bool TextEditorList::hasUnsavedChanges(const QString &fileName) const{
    return this->hasUnsavedChanges(this->textEditorFromFileName(fileName));
}

void TextEditorList::markAsSaved(TextEditor *editor){
    this->_hasUnsavedChanges[editor] = false;
}

void TextEditorList::markAsSaved(const QString &fileName){
    this->markAsSaved(this->textEditorFromFileName(fileName));
}
