#ifndef NMLPROJECT_H
#define NMLPROJECT_H

#include <QtWidgets>
#include "texteditor.h"
#include "texteditorlist.h"
#include "spriteeditor.h"
#include "windowwithclosesignal.hpp"

class NMLProject : public MainWindow{
    Q_OBJECT

public:
    NMLProject(const NMLProject&) = delete;
    NMLProject &operator=(const NMLProject&) = delete;

    NMLProject(const QString &nmlFile, const QString &fileToOpen = "");
    static NMLProject *newNMLProject(QWidget *parent = nullptr);
    static NMLProject *openNMLProject(QWidget *parent = nullptr);

    void addLanguage(const QString &language, bool openLanguageNow);    //Takes the name of the language, for example "English (US)"
    bool removeLanguage(const QString &language);    //Takes the name of the file without the extension, for example "english"

    void addSprite(const QString &fileName, const QString &destinationName);    //fileName is the complete path of the file to import, for example "C:/sprites.png", destinationName is the name of the destination file without the extension, for example "sprites"
    bool renameSprite(const QString &oldName, const QString &newName);    //Takes the name of the file with the extension, for example "oldsprites.png" and "newsprites.png"
    bool removeSprite(const QString &fileName);    //Takes the name of the file with the extension, for example "sprites.png"

    bool saveFile(const QString &fileName);

public slots:
    bool saveAll();

    void reloadLanguageList();
    void reloadSpriteList();

    void showSettingsWindow();

private:
    QString fileFromModelIndex(const QModelIndex &index) const;
    bool setActiveFile(const QString &fileName);

    QPair<QString, int> fileAndLineNumber(const QString &message);
    QPair<TextEditor*, int> editorAndLineNumber(const QString &message);

    const QString _nmlFile;
    const QDir _projectDir, _langDir, _gfxDir;
    QStringList _languageFiles;
    QStringList _spriteFiles;
    QStandardItemModel _fileListModel, _logModel;

    TextEditorList _textEditors;
    QMap<QString, SpriteEditor*> _spriteEditors;
    QString _activeFile;
    QDockWidget _fileListDockWidget, _logDockWidget;

    QAction *const _undoButton, *const _redoButton, *const _cutButton, *const _copyButton, *const _pasteButton, *const _findButton, *const _selectAllButton;
    QAction *const _toggleEditToolBar, *const _toggleImageTools;
    QToolBar *const _editToolBar;
    QDialog _findWindow;

    static const QMap<QString, int> _languageCodes;
};

#endif // NMLPROJECT_H
