#include "nmlproject.h"
#include "version.h"

#ifdef _WIN32
    #define NMLCOMPILER (qApp->applicationDirPath() + "/nmlc.exe")    //On Windows, run the nmlc.exe file in the same folder as the NMLCreator executable
#else
    #define NMLCOMPILER "nmlc"    //On Linux, run the nmlc command that the user is supposed to install from the command line
#endif

const QMap<QString, int> NMLProject::_languageCodes = QMap<QString, int>({
    {QObject::tr("Afrikaans"), 0x1b},
    {QObject::tr("Arabic"), 0x14},
    {QObject::tr("Basque"), 0x21},
    {QObject::tr("Belarusian"), 0x10},
    {QObject::tr("Bulgarian"), 0x18},
    {QObject::tr("Catalan"), 0x22},
    {QObject::tr("Chinese (Simplified)"), 0x56},
    {QObject::tr("Chinese (Traditional)"), 0x0c},
    {QObject::tr("Croatian"), 0x38},
    {QObject::tr("Czech"), 0x15},
    {QObject::tr("Danish"), 0x2d},
    {QObject::tr("Dutch"), 0x1f},
    {"English (Australia)", 0x3d},
    {"English (UK)", 0x01},
    {"English (US)", 0x00},
    {QObject::tr("Esperanto"), 0x05},
    {QObject::tr("Estonian"), 0x34},
    {QObject::tr("Faroese"), 0x12},
    {QObject::tr("Finnish"), 0x35},
    {QObject::tr("French"), 0x03},
    {QObject::tr("Gaelic"), 0x13},
    {QObject::tr("Galician"), 0x31},
    {QObject::tr("German"), 0x02},
    {QObject::tr("Greek"), 0x1e},
    {QObject::tr("Hebrew"), 0x61},
    {QObject::tr("Hungarian"), 0x24},
    {QObject::tr("Icelandic"), 0x29},
    {QObject::tr("Indonesian"), 0x5a},
    {QObject::tr("Irish"), 0x08},
    {QObject::tr("Italian"), 0x27},
    {QObject::tr("Japanese"), 0x39},
    {QObject::tr("Korean"), 0x3a},
    {QObject::tr("Latin"), 0x66},
    {QObject::tr("Latvian"), 0x2a},
    {QObject::tr("Lithuanian"), 0x2b},
    {QObject::tr("Luxembourgish"), 0x23},
    {QObject::tr("Malay"), 0x3c},
    {QObject::tr("Norwegian (Bokmal)"), 0x2f},
    {QObject::tr("Norwegian (Nynorsk)"), 0x0e},
    {QObject::tr("Polish"), 0x30},
    {QObject::tr("Portuguese (Brazil)"), 0x37},
    {QObject::tr("Portuguese (Portugal)"), 0x36},
    {QObject::tr("Romanian"), 0x28},
    {QObject::tr("Russian"), 0x07},
    {QObject::tr("Serbian"), 0x0d},
    {QObject::tr("Slovak"), 0x16},
    {QObject::tr("Slovenian"), 0x2c},
    {QObject::tr("Spanish (Mexico)"), 0x55},
    {QObject::tr("Spanish (Spain)"), 0x04},
    {QObject::tr("Swedish"), 0x2e},
    {QObject::tr("Tamil"), 0x0a},
    {QObject::tr("Thai"), 0x42},
    {QObject::tr("Turkish"), 0x3e},
    {QObject::tr("Ukrainian"), 0x33},
    {QObject::tr("Vietnamese"), 0x54},
    {QObject::tr("Welsh"), 0x0f}
});

NMLProject::NMLProject(const QString &nmlFile, const QString &fileToOpen):
    _nmlFile(nmlFile),
    _projectDir(QFileInfo(nmlFile).dir()),
    _langDir(_projectDir.path() + "/lang"),
    _gfxDir(_projectDir.path() + "/gfx"),
    _undoButton(new QAction(QIcon(":/icons/undo.svg"), QObject::tr("&Undo"))),
    _redoButton(new QAction(QIcon(":/icons/redo.svg"), QObject::tr("&Redo"))),
    _cutButton(new QAction(QIcon(":/icons/cut.svg"), QObject::tr("Cu&t"))),
    _copyButton(new QAction(QIcon(":/icons/copy.svg"), QObject::tr("&Copy"))),
    _pasteButton(new QAction(QIcon(":/icons/paste.svg"), QObject::tr("&Paste"))),
    _selectAllButton(new QAction(QObject::tr("Select &all"))),
    _toggleEditToolBar(new QAction(QObject::tr("&Edit"))),
    _toggleImageTools(new QAction(QObject::tr("&Image tools"))),
    _editToolBar(new QToolBar(QObject::tr("&Edit")))
{
    //Create the lang and gfx folders if they don't already exist
    this->_langDir.mkpath(".");
    this->_gfxDir.mkpath(".");

    //Initialize the file lists
    QStandardItem *nmlItem = new QStandardItem(QFileInfo(this->_nmlFile).fileName());
    nmlItem->setIcon(QIcon(":/icons/nml.svg"));
    this->_fileListModel.appendRow(nmlItem);

    QStandardItem *languagesItem = new QStandardItem("lang");
    languagesItem->setIcon(QFileIconProvider().icon(QFileIconProvider::Folder));
    this->_fileListModel.appendRow(languagesItem);
    QStandardItem *spritesItem = new QStandardItem("gfx");
    spritesItem->setIcon(QFileIconProvider().icon(QFileIconProvider::Folder));
    this->_fileListModel.appendRow(spritesItem);

    this->reloadLanguageList();
    this->reloadSpriteList();

    QObject::connect(&this->_textEditors, &TextEditorList::changesInTextEditor, [this](TextEditor *editor){
        if(editor == this->centralWidget()){
            this->setWindowTitle("*" + QFileInfo(this->_activeFile).fileName() + " @ " + QFileInfo(this->_nmlFile).fileName() + " - NMLCreator");
        }
    });

    //Create the logging area
    QListView *logView = new QListView;
    logView->setModel(&this->_logModel);
    logView->setEditTriggers(QTreeView::NoEditTriggers);
    this->_logDockWidget.setWidget(logView);
    this->_logDockWidget.setWindowTitle(QObject::tr("Errors and Warnings"));
    this->addDockWidget(Qt::BottomDockWidgetArea, &this->_logDockWidget);

    QObject::connect(logView, &QListView::clicked, [this](const QModelIndex &index){
        const QPair<QString, int> fileAndLineNumber = this->fileAndLineNumber(this->_logModel.itemFromIndex(index)->text());
        const QString file = fileAndLineNumber.first;
        TextEditor *editor = this->_textEditors.textEditorFromFileName(file);
        const int lineNumber = fileAndLineNumber.second;
        if(file.isEmpty() || lineNumber == 0){
            return;
        }

        this->setActiveFile(file);
        editor->setTextCursor(QTextCursor(editor->document()->findBlockByLineNumber(lineNumber - 1)));
    });

    //Create the file list
    QTreeView *fileListView = new QTreeView;
    fileListView->setModel(&this->_fileListModel);
    fileListView->header()->hide();
    fileListView->setEditTriggers(QTreeView::NoEditTriggers);
    fileListView->setContextMenuPolicy(Qt::CustomContextMenu);
    fileListView->expandAll();
    this->_fileListDockWidget.setWidget(fileListView);
    this->_fileListDockWidget.setWindowTitle(QObject::tr("Files"));
    this->addDockWidget(Qt::LeftDockWidgetArea, &this->_fileListDockWidget);

    QObject::connect(fileListView->itemDelegate(), &QAbstractItemDelegate::closeEditor, [this](){
        for(int i = 0; i < this->_spriteFiles.length(); i++){
            QStandardItem *item = this->_fileListModel.item(2)->child(i);
            QString newName = item->text();
            const QString oldName = QFileInfo(this->_spriteFiles[i]).fileName();
            if(oldName == newName){
                continue;
            }

            if(!newName.contains(QRegExp("\\.png$", Qt::CaseInsensitive))){
                newName += ".png";
            }
            if(this->renameSprite(oldName, newName)){
                item->setText(newName);
            }
            else{
                item->setText(oldName);
            }
        }
    });

    QObject::connect(fileListView, &QTreeView::clicked, [this](const QModelIndex &index){
        const QString &file = this->fileFromModelIndex(index);
        if(file.isEmpty() || QFileInfo(file).isDir()){
            return;
        }
        this->setActiveFile(file);
    });

    QObject::connect(fileListView, &QTreeView::customContextMenuRequested, [this, fileListView](const QPoint &point){
        const QModelIndex &index = fileListView->indexAt(point);
        const QString &file = this->fileFromModelIndex(index);
        if(file.isEmpty()){
            return;
        }

        QMenu *contextMenu = new QMenu(fileListView);
        if(QFileInfo(file).isDir()){
            QAction *open = contextMenu->addAction(QObject::tr("&Open in file explorer"));
            QObject::connect(open, &QAction::triggered, [file](){
                QDesktopServices::openUrl(file);
            });

            QAction *reload = contextMenu->addAction(QObject::tr("Re&load file list"));
            if(QFileInfo(file).fileName() == "lang"){
                QObject::connect(reload, &QAction::triggered, this, &NMLProject::reloadLanguageList);

                QAction *addLanguage = contextMenu->addAction(QObject::tr("&Add language..."));
                QObject::connect(addLanguage, &QAction::triggered, [this](){
                    QDialog dialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                    QGridLayout layout;

                    QComboBox languageList;
                    languageList.addItems(this->_languageCodes.keys());
                    languageList.setCurrentText("English (US)");
                    layout.addWidget(&languageList, 0, 0, 1, 2);

                    QPushButton okButton(QObject::tr("OK"));
                    QObject::connect(&okButton, &QPushButton::pressed, &dialog, &QDialog::accept);
                    layout.addWidget(&okButton, 1, 0);

                    QPushButton cancelButton(QObject::tr("Cancel"));
                    QObject::connect(&cancelButton, &QPushButton::pressed, &dialog, &QDialog::reject);
                    layout.addWidget(&cancelButton, 1, 1);

                    dialog.setLayout(&layout);
                    dialog.setWindowTitle(QObject::tr("Add Language"));
                    if(dialog.exec()){
                        this->addLanguage(languageList.currentText(), true);
                    }
                });
            }

            else{
                QObject::connect(reload, &QAction::triggered, this, &NMLProject::reloadSpriteList);

                QAction *emptySprite = contextMenu->addAction(QObject::tr("Create an &empty sprite"));
                QObject::connect(emptySprite, &QAction::triggered, [this](){
                    this->addSprite(":/sprites/emptysprite.png", "sprites");
                });

                QAction *importSprite = contextMenu->addAction(QObject::tr("&Import sprite..."));
                QObject::connect(importSprite, &QAction::triggered, [this](){
                    const QString &fileName = QFileDialog::getOpenFileName(this, QObject::tr("Import Sprite"), "", QObject::tr("All supported image files") + " (*.png;*.bmp;*.gif;*.jpg;*.jpeg;*.pbm;*.pgm;*.ppm;*.xbm;*.xpm);;" + QObject::tr("PNG files") + " (*.png)");
                    if(!fileName.isEmpty()){
                        this->addSprite(fileName, QFileInfo(fileName).baseName());
                    }
                });
            }
        }
        else{
            const QString type = QFileInfo(file).suffix();
            QAction *open = contextMenu->addAction(QObject::tr("&Open"));
            QFont font = open->font();
            font.setBold(true);
            open->setFont(font);
            QObject::connect(open, &QAction::triggered, [this, file](){
                this->setActiveFile(file);
            });
            QAction *reload = contextMenu->addAction(QObject::tr("Re&load"));
            if(type == "png"){
                QObject::connect(reload, &QAction::triggered, [this, file](){
                    SpriteEditor *oldEditor = this->_spriteEditors[file];
                    if(oldEditor != nullptr && oldEditor->hasUnsavedChanges()){
                        if(QMessageBox::warning(this, "", QObject::tr("The file %1 has unsaved changes. Do you really want to reload it?").arg(QFileInfo(file).fileName()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
                            return;
                        }
                    }
                    if(!QFile(file).exists()){
                        QMessageBox::information(this, "", QObject::tr("The file %1 has been deleted.").arg(file));
                        this->removeSprite(QFileInfo(file).fileName());
                        return;
                    }
                    this->_spriteEditors[file] = new SpriteEditor(file);
                    if(this->_activeFile == file){
                        this->setActiveFile(file);
                    }
                    if(oldEditor != nullptr){
                        delete oldEditor;
                    }
                    this->setActiveFile(this->_activeFile);
                });
            }
            else{
                QObject::connect(reload, &QAction::triggered, [this, file, type](){
                    if(this->_textEditors.hasUnsavedChanges(file)){
                        if(QMessageBox::warning(this, "", QObject::tr("The file %1 has unsaved changes. Do you really want to reload it?").arg(QFileInfo(file).fileName()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
                            return;
                        }
                    }
                    if(type == "lng" && !QFile(file).exists()){
                        QMessageBox::information(this, "", QObject::tr("The file %1 has been deleted.").arg(file));
                        this->removeLanguage(QFileInfo(file).baseName());
                        return;
                    }
                    TextEditor *editor = this->_textEditors.textEditorFromFileName(file);
                    if(editor != nullptr){
                        QFile f(file);
                        if(!f.open(QFile::ReadOnly)){
                            QMessageBox::critical(this, "", QObject::tr("Could not open file %1.").arg(file));
                            return;
                        }
                        editor->setPlainText(QString::fromUtf8(f.readAll()));
                        f.close();
                        this->_textEditors.markAsSaved(file);
                    }
                    this->setActiveFile(this->_activeFile);
                });
            }
            if(type == "png"){
                QAction *rename = contextMenu->addAction(QObject::tr("&Rename"));
                QObject::connect(rename, &QAction::triggered, [this, fileListView, index](){
                    fileListView->edit(index);
                });
            }
            if(type != "nml"){
                QAction *remove = contextMenu->addAction(QObject::tr("&Delete"));
                QObject::connect(remove, &QAction::triggered, [this, file, type](){
                    if(QMessageBox::warning(this, "", QObject::tr("Do you really want to delete the file %1? This can not be undone.").arg(file), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
                        if(type == "lng"){
                            this->removeLanguage(QFileInfo(file).baseName());
                        }
                        else{
                            this->removeSprite(QFileInfo(file).fileName());
                        }
                    }
                });
            }
            QAction *showInFolder = contextMenu->addAction(QObject::tr("&Show in file explorer"));
            QObject::connect(showInFolder, &QAction::triggered, [file](){
                #ifdef _WIN32
                QProcess::startDetached("explorer.exe", {"/select,", QDir::toNativeSeparators(file)});
                #else
                QDesktopServices::openUrl(QFileInfo(file).dir().path());
                #endif
            });
        }
        contextMenu->exec(fileListView->viewport()->mapToGlobal(point));
    });

    //Create the menu bar
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu(QObject::tr("&File"));
    QAction *newProject = fileMenu->addAction(QIcon(":/icons/new.svg"), QObject::tr("Create &new project"));
    newProject->setShortcut(QKeySequence("CTRL+N"));
    QAction *openProject = fileMenu->addAction(QIcon(":/icons/open.svg"), QObject::tr("&Open project..."));
    openProject->setShortcut(QKeySequence("CTRL+O"));
    fileMenu->addSeparator();
    QAction *save = fileMenu->addAction(QIcon(":/icons/save.svg"), QObject::tr("&Save"));
    save->setShortcut(QKeySequence("CTRL+S"));
    QAction *saveAll = fileMenu->addAction(QObject::tr("Save &all"));
    saveAll->setShortcut(QKeySequence("CTRL+Shift+S"));
    QAction *compileButton = fileMenu->addAction(QIcon(":/icons/hammer.svg"), QObject::tr("&Compile"));
    compileButton->setShortcut(QKeySequence("F5"));
    fileMenu->addSeparator();
    QAction *settings = fileMenu->addAction(QIcon(":/icons/settings.svg"), QObject::tr("S&ettings"));
    fileMenu->addSeparator();
    QAction *close = fileMenu->addAction(QObject::tr("Close this &project"));
    close->setShortcut(QKeySequence("Alt+F4"));

    QObject::connect(newProject, &QAction::triggered, [this](){
        NMLProject::newNMLProject(this);
    });
    QObject::connect(openProject, &QAction::triggered, [this](){
        NMLProject::openNMLProject(this);
    }); 
    QObject::connect(save, &QAction::triggered, [this](){
        this->saveFile(this->_activeFile);
    });
    QObject::connect(saveAll, &QAction::triggered, this, &NMLProject::saveAll);
    auto *connections = new QMap<TextEditor*, QMetaObject::Connection>;    //Contains connections between the TextChanged signal of text editors and a lambda to remove warnings and errors in that editor
    QObject::connect(compileButton, &QAction::triggered, [this, compileButton, connections](){
        if(!this->saveAll()){
            return;
        }

        compileButton->setDisabled(true);
        for(TextEditor *editor: this->_textEditors){
            editor->removeAllErrors();
            editor->removeAllWarnings();
            if(connections->contains(editor)){
                QObject::disconnect((*connections)[editor]);
            }
        }
        connections->clear();
        this->_logModel.removeRows(0, this->_logModel.rowCount());
        this->_logModel.appendRow(new QStandardItem(QObject::tr("Compiling, please wait...")));

        const QString destination = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/OpenTTD/newgrf/" + QFileInfo(this->_nmlFile).baseName() + ".grf";
        QProcess *compiler = new QProcess;
        compiler->setWorkingDirectory(this->_projectDir.path());
        QObject::connect(compiler, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, compileButton, compiler, connections](bool failed){
            const QStringList output = QString::fromLatin1(compiler->readAllStandardError()).split("\n");
            this->_logModel.removeRows(0, this->_logModel.rowCount());
            if(failed){
                this->_logModel.appendRow(new QStandardItem(QObject::tr("NewGRF was not compiled because of the following errors:")));
            }
            else{
                this->_logModel.appendRow(new QStandardItem(QObject::tr("NewGRF was compiled successfully. To test it, run OpenTTD, go to NewGRF Settings, select your NewGRF in the list and click \"Add\".")));
            }
            for(const QString &message: output){
                QStandardItem *modelItem = new QStandardItem(message);
                QIcon icon;
                const QPair<TextEditor*, int> editorAndLineNumber = this->editorAndLineNumber(message);
                TextEditor *editor = editorAndLineNumber.first;
                const int lineNumber = editorAndLineNumber.second;
                const QString filterWarnings = QSettings("OpenTTD", "NMLCreator").value("compiler/filterWarnings", "").toString();

                if(message.contains(QRegExp("^\\s*nmlc\\s*error", Qt::CaseInsensitive))){
                    icon = QIcon(":/icons/error.svg");
                    if(editor != nullptr && lineNumber > 0){
                        editor->addError(lineNumber);
                    }
                }
                else if(message.contains(QRegExp("^\\s*nmlc\\s*warning", Qt::CaseInsensitive))){
                    if(!filterWarnings.isEmpty() && message.contains(QRegularExpression(filterWarnings))){
                        continue;
                    }
                    icon = QIcon(":/icons/warning.svg");
                    if(editor != nullptr && lineNumber > 0){
                        editor->addWarning(lineNumber);
                    }
                }
                if(editor != nullptr && !connections->contains(editor)){
                    connections->insert(editor, QObject::connect(editor, &TextEditor::textChanged, [this, editor, modelItem, connections](){
                        editor->removeAllErrors();
                        editor->removeAllWarnings();
                        this->_logModel.removeRow(this->_logModel.indexFromItem(modelItem).row());
                        QObject::disconnect((*connections)[editor]);
                        connections->remove(editor);
                    }));
                }

                modelItem->setIcon(icon);
                this->_logModel.appendRow(modelItem);
            }

            compileButton->setDisabled(false);
            delete compiler;
        });
        QObject::connect(compiler, &QProcess::errorOccurred, [this, compileButton, compiler](){
            const QString compilerPath = QSettings("OpenTTD", "NMLCreator").value("compiler/path", NMLCOMPILER).toString();
            this->_logModel.removeRows(0, this->_logModel.rowCount());
            this->_logModel.appendRow(new QStandardItem(QIcon(":/icons/error.svg"), QObject::tr("NewGRF was not compiled because the compiler was not found.")));
            #ifdef _WIN32
                QMessageBox::critical(this, "", QObject::tr("Could not find the file %1.").arg(compilerPath) + "\n\n" + QObject::tr("To fix this error, try resetting the NMLCreator settings. If this error persists, reinstalling NMLCreator."));
            #else
                if(compilerPath == "nmlc"){
                    QMessageBox::critical(this, "", QObject::tr("Command nmlc not found. Please install NML by running the following command in the command line:") + "\n\npython3 -m pip install nml\n\n" + QObject::tr("If you receive an error doing so, try installing Python 3 and pip by running the following commands:") + "\n\nsudo apt install python3\nsudo apt install python3-pip");
                }
                else{
                    QMessageBox::critical(this, "", QObject::tr("Command %1 not found. Please open the NMLCreator settings and specify a valid command under \"Compiler path\".").arg(compilerPath));
                }
            #endif

            compileButton->setDisabled(false);
            delete compiler;
        });
        QSettings settings("OpenTTD", "NMLCreator");
        QStringList args = {"-c", "--grf", destination, this->_nmlFile, "--cache-dir=" + settings.value("compiler/cacheDir", ".nmlcache").toString()};
        if(!settings.value("compiler/enableCache", true).toBool()){
            args.append("--no-cache");
        }
        if(!settings.value("compiler/clearUnusedCache", false).toBool()){
            args.append("--clear-orphaned");
        }
        if(!settings.value("compiler/enableWarnings", true).toBool()){
            args.append("--quiet");
        }
        compiler->start(settings.value("compiler/path", NMLCOMPILER).toString(), args);
    });
    QObject::connect(settings, &QAction::triggered, this, &NMLProject::showSettingsWindow);
    QObject::connect(close, &QAction::triggered, [this](){
        emit this->aboutToClose(nullptr);
    });

    QMenu *editMenu = menuBar->addMenu(QObject::tr("&Edit"));
    editMenu->addAction(this->_undoButton);
    this->_undoButton->setShortcut(QKeySequence("CTRL+Z"));
    editMenu->addAction(this->_redoButton);
    this->_redoButton->setShortcut(QKeySequence("CTRL+Y"));
    editMenu->addSeparator();
    editMenu->addAction(this->_cutButton);
    this->_cutButton->setShortcut(QKeySequence("CTRL+X"));
    editMenu->addAction(this->_copyButton);
    this->_copyButton->setShortcut(QKeySequence("CTRL+C"));
    editMenu->addAction(this->_pasteButton);
    this->_pasteButton->setShortcut(QKeySequence("CTRL+V"));
    editMenu->addSeparator();
    editMenu->addAction(this->_selectAllButton);
    this->_selectAllButton->setShortcut(QKeySequence("CTRL+A"));

    if(fileToOpen.isEmpty() || !this->setActiveFile(fileToOpen)){
        this->setActiveFile(this->_nmlFile);
    }

    QMenu *viewMenu = menuBar->addMenu(QObject::tr("&View"));
    QMenu *toolBarsList = viewMenu->addMenu(QObject::tr("&Toolbars"));
    QAction *toggleFileList = viewMenu->addAction(QObject::tr("&Files"));
    toggleFileList->setCheckable(true);
    toggleFileList->setChecked(true);
    QObject::connect(toggleFileList, &QAction::triggered, &this->_fileListDockWidget, &QDockWidget::setVisible);
    QObject::connect(&this->_fileListDockWidget, &QDockWidget::visibilityChanged, toggleFileList, &QAction::setChecked);
    QAction *toggleLogsList = viewMenu->addAction(QObject::tr("&Errors and Warnings"));
    toggleLogsList->setCheckable(true);
    toggleLogsList->setChecked(true);
    QObject::connect(toggleLogsList, &QAction::triggered, &this->_logDockWidget, &QDockWidget::setVisible);
    QObject::connect(&this->_logDockWidget, &QDockWidget::visibilityChanged, toggleLogsList, &QAction::setChecked);
    viewMenu->addSeparator();
    QAction *clearLogs = viewMenu->addAction(QObject::tr("&Clear errors and warnings"));
    QObject::connect(clearLogs, &QAction::triggered, [this, connections](){
        for(TextEditor *editor: this->_textEditors){
            editor->removeAllErrors();
            editor->removeAllWarnings();
            if(connections->contains(editor)){
                QObject::disconnect((*connections)[editor]);
            }
        }
        connections->clear();
        this->_logModel.removeRows(0, this->_logModel.rowCount());
    });

    QMenu *helpMenu = menuBar->addMenu(QObject::tr("&Help"));
    QAction *help = helpMenu->addAction(QIcon(":/icons/help.svg"), QObject::tr("NMLCreator &Help"));
    help->setShortcut(QKeySequence("F1"));
    QAction *documentation = helpMenu->addAction(QObject::tr("&NML documentation"));
    QAction *tutorial = helpMenu->addAction(QObject::tr("NML &tutorial"));
    helpMenu->addSeparator();
    QAction *about = helpMenu->addAction(QObject::tr("&About NMLCreator"));
    QAction *aboutQt = helpMenu->addAction(QObject::tr("About &Qt"));

    QObject::connect(help, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://github.com/DonaldDuck313/NMLCreator/blob/main/README.md"));
    });
    QObject::connect(documentation, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://newgrf-specs.tt-wiki.net/wiki/NML:Main"));
    });
    QObject::connect(tutorial, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://www.tt-wiki.net/wiki/NMLTutorial"));
    });
    QObject::connect(about, &QAction::triggered, [this](){
        QMessageBox::about(this, QObject::tr("About NMLCreator"), QObject::tr("NMLCreator version " PROGRAMVERSION) + "<br><br>" + QObject::tr("This program is licensed under GNU General Public License v2.0.") + "<br><br>" + QObject::tr("This program uses the <a %1>NML compiler</a> by the OpenTTD team, licensed under GNU General Public License v2.0.").arg("href='https://github.com/OpenTTD/nml'") + "<br><br>" + QObject::tr("Icons made by %2, %3 and %4 from %1. Some icons have been modified.").arg("<a href='https://www.flaticon.com/'>www.flaticon.com</a></div>", "<a href='https://www.freepik.com'>Freepik</a>", "<a href='https://www.flaticon.com/authors/pixel-perfect'>Pixel perfect</a>", "<a href='https://www.flaticon.com/authors/smashicons'>Smashicons</a>"));
    });
    QObject::connect(aboutQt, &QAction::triggered, [this](){
        QMessageBox::aboutQt(this);
    });

    //Create the toolbar
    QToolBar *fileToolBar = this->addToolBar(QObject::tr("File"));
    fileToolBar->addAction(newProject);
    fileToolBar->addAction(openProject);
    fileToolBar->addSeparator();
    fileToolBar->addAction(compileButton);
    fileToolBar->addAction(settings);

    QAction *toggleFileToolBar = toolBarsList->addAction(QObject::tr("&File"));
    toggleFileToolBar->setCheckable(true);
    toggleFileToolBar->setChecked(true);
    QObject::connect(toggleFileToolBar, &QAction::triggered, fileToolBar, &QToolBar::setVisible);
    QObject::connect(fileToolBar, &QToolBar::visibilityChanged, toggleFileToolBar, &QAction::setChecked);

    this->addToolBar(this->_editToolBar);
    this->_editToolBar->addAction(this->_undoButton);
    this->_editToolBar->addAction(this->_redoButton);
    this->_editToolBar->addSeparator();
    this->_editToolBar->addAction(this->_cutButton);
    this->_editToolBar->addAction(this->_copyButton);
    this->_editToolBar->addAction(this->_pasteButton);

    toolBarsList->addAction(this->_toggleEditToolBar);
    this->_toggleEditToolBar->setCheckable(true);
    this->_toggleEditToolBar->setChecked(true);
    toolBarsList->addAction(this->_toggleImageTools);
    this->_toggleImageTools->setCheckable(true);
    this->_toggleImageTools->setDisabled(true);
    QObject::connect(this->_toggleEditToolBar, &QAction::triggered, this->_editToolBar, &QToolBar::setVisible);
    QObject::connect(this->_editToolBar, &QToolBar::visibilityChanged, this->_toggleEditToolBar, &QAction::setChecked);

    //Ask for confirmation before closing if there are unsaved files
    QObject::connect(this, &MainWindow::aboutToClose, [this](QCloseEvent *event){
        QStringList unsavedFiles;

        if(this->_textEditors.hasUnsavedChanges(this->_textEditors.textEditorFromFileName(this->_nmlFile))){
            unsavedFiles.append(QFileInfo(this->_nmlFile).fileName());
        }
        for(const QString &languageFile: qAsConst(this->_languageFiles)){
            if(this->_textEditors.hasUnsavedChanges(this->_textEditors.textEditorFromFileName(languageFile))){
                unsavedFiles.append(QFileInfo(languageFile).fileName());
            }
        }
        for(const QString &spriteFile: qAsConst(this->_spriteFiles)){
            if(this->_spriteEditors[spriteFile]->hasUnsavedChanges()){
                unsavedFiles.append(QFileInfo(spriteFile).fileName());
            }
        }

        if(unsavedFiles.isEmpty()){
            this->close();
        }
        else switch(QMessageBox::warning(this, "", QObject::tr("The following files have unsaved changes:") + "\n\n" + unsavedFiles.join("\n") + "\n\n" + QObject::tr("Do you want to save them before closing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)){
        case QMessageBox::Yes:
            this->saveAll();
            //No break, so continue with closing the window
        case QMessageBox::No:
            this->close();
            break;
        default:
            if(event != nullptr){
                event->ignore();
            }
            break;
        }
    });

    //Open the main window
    this->setWindowIcon(QIcon(":/icons/icon.svg"));
    this->showMaximized();
}

NMLProject *NMLProject::newNMLProject(QWidget *parent){
    QDir projectsDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/OpenTTD/nmlprojects");
    if(!projectsDir.exists()){
        projectsDir.mkpath(".");
    }

    QDialog newProjectWindow(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    newProjectWindow.setWindowTitle(QObject::tr("New Project - NMLCreator"));
    newProjectWindow.setWindowIcon(QIcon(":/icons/icon.svg"));
    QFormLayout newProjectLayout;

    QLineEdit projectName(QObject::tr("MyProject"));
    newProjectLayout.addRow(QObject::tr("Project name"), &projectName);

    QWidget projectFolder;
    QHBoxLayout projectFolderLayout;
    QLineEdit projectFolderEdit(projectsDir.path());
    projectFolderLayout.addWidget(&projectFolderEdit);
    QPushButton browseButton(QObject::tr("Browse..."));
    projectFolderLayout.addWidget(&browseButton);
    projectFolderLayout.setMargin(0);
    QObject::connect(&browseButton, &QPushButton::pressed, [&](){
        const QString path = QFileDialog::getExistingDirectory(&newProjectWindow);
        if(!path.isEmpty()){
            projectFolderEdit.setText(path);
        }
    });
    projectFolder.setLayout(&projectFolderLayout);
    newProjectLayout.addRow(QObject::tr("Folder to create the project in"), &projectFolder);

    QWidget buttonsContainer;
    QHBoxLayout buttonsLayout;
    buttonsLayout.setMargin(0);

    QPushButton okButton(QObject::tr("OK"));
    QString projectPath;
    QObject::connect(&okButton, &QPushButton::pressed, [&](){
        //Create the project folder
        QDir projectDir(projectFolderEdit.text() + "/" + projectName.text());
        if(projectDir.exists()){
            if(QMessageBox::warning(&newProjectWindow, "", QObject::tr("The folder %1 already exists. If you select it as your project location, its current contents may be overwritten. Do you really want to continue?").arg(projectDir.path()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
                return;
            }
        }
        else if(!projectDir.mkpath(".")){
            QMessageBox::critical(&newProjectWindow, "", QObject::tr("You do not have permission to create the folder %1.").arg(projectDir.path()));
            return;
        }

        //Create the NML file
        projectPath = projectDir.path() + "/" + projectName.text() + ".nml";
        QFile nmlFile(projectPath);
        if(!nmlFile.open(QFile::WriteOnly | QFile::Truncate)){
            if(nmlFile.exists()){
                QMessageBox::critical(&newProjectWindow, "", QObject::tr("You do not have permission to edit the file %1.").arg(nmlFile.fileName()));
            }
            else{
                QMessageBox::critical(&newProjectWindow, "", QObject::tr("You do not have permission to create the file %1.").arg(nmlFile.fileName()));
            }
            return;
        }
        const QString username = QDir::home().dirName().left(2).toUpper();
        const QString initialContent =
            "grf{\n"
            "    grfid: \"" + username + "\\xx\\xx\";\n"
            "    name: string(STR_GRF_NAME);\n"
            "    desc: string(STR_GRF_DESCRIPTION);\n"
            "    version: 0;\n"
            "    min_compatible_version: 0;\n"
            "}"
        ;
        nmlFile.write(initialContent.toUtf8());
        nmlFile.close();

        //Delete the lang and gfx folders in case there is already stuff in them (they will be created again in the NMLProject constructor)
        QDir gfxDir(projectDir.path() + "/gfx");
        gfxDir.removeRecursively();
        QDir langDir(projectDir.path() + "/lang");
        langDir.removeRecursively();

        //Close the dialog
        newProjectWindow.accept();
    });
    buttonsLayout.addWidget(&okButton);

    QPushButton cancelButton(QObject::tr("Cancel"));
    QObject::connect(&cancelButton, &QPushButton::pressed, &newProjectWindow, &QDialog::reject);
    buttonsLayout.addWidget(&cancelButton);

    buttonsContainer.setLayout(&buttonsLayout);
    newProjectLayout.addRow(&buttonsContainer);

    newProjectWindow.setLayout(&newProjectLayout);
    const bool accepted = newProjectWindow.exec();

    if(accepted){
        NMLProject *project = new NMLProject(projectPath);
        project->addLanguage("English (US)", false);
        return project;
    }
    return nullptr;
}

NMLProject *NMLProject::openNMLProject(QWidget *parent){
    const QString fileName = QFileDialog::getOpenFileName(parent, "", "", QObject::tr("NML project files") + " (*.nml)");
    if(fileName.isEmpty()){
        return nullptr;
    }
    else if(QFileInfo(fileName).suffix() != "nml"){
        QMessageBox::critical(parent, "", QObject::tr("The file %1 is not a valid NML file.").arg(fileName));
        return nullptr;
    }
    else{
        return new NMLProject(fileName);
    }
}

void NMLProject::addLanguage(const QString &language, bool openLanguageNow){
    if(!this->_languageCodes.contains(language)){
        QMessageBox::critical(this, "", QObject::tr("The language %1 is not supported by OpenTTD.").arg(language));
        return;
    }
    QFile file(this->_langDir.path() + "/" + QString(language).replace(QRegExp("\\s|\\s*\\([a-z]*\\)", Qt::CaseInsensitive), "").toLower() + ".lng");
    if(file.exists()){
        if(QMessageBox::warning(this, "", QObject::tr("A language file for %1 already exists for this project. Do you want to overwrite it?").arg(language), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
            if(!this->removeLanguage(language)){
                return;
            }
        }
        else{
            return;
        }
    }
    if(!file.open(QFile::WriteOnly | QFile::Truncate)){
        QMessageBox::critical(this, "", QObject::tr("You do not have permission to edit the file %1.").arg(file.fileName()));
        return;
    }
    const QString initialContent =
        "##grflangid 0x" + QString::number(this->_languageCodes[language], 16) + "\n\n"
        "STR_GRF_NAME: " + QFileInfo(this->_nmlFile).baseName() + "\n"
        "STR_GRF_DESCRIPTION: Enter description here"
    ;
    file.write(initialContent.toUtf8());
    file.close();

    this->_languageFiles.append(file.fileName());

    QStandardItem *languageItem = new QStandardItem(QFileInfo(file).fileName());
    languageItem->setIcon(QIcon(":/icons/lng.svg"));
    this->_fileListModel.item(1)->appendRow(languageItem);

    if(openLanguageNow){
        this->setActiveFile(file.fileName());
    }
}

bool NMLProject::removeLanguage(const QString &language){
    const QString file = this->_langDir.path() + "/" + language + ".lng";
    if(QFile(file).exists() && !QFile(file).remove()){
        QMessageBox::critical(this, "", QObject::tr("Could not delete file %1.").arg(file));
        return false;
    }

    if(this->centralWidget() == this->_textEditors.textEditorFromFileName(file)){
        this->setActiveFile(this->_nmlFile);
    }
    this->_textEditors.removeTextEditor(file);
    this->_fileListModel.item(1)->removeRow(this->_languageFiles.indexOf(file));
    this->_languageFiles.removeAll(file);

    return true;
}

void NMLProject::addSprite(const QString &fileName, const QString &destinationName){
    if(QImage(fileName).isNull()){
        QMessageBox::critical(this, "", QObject::tr("The file %1 is not a valid image file.").arg(fileName));
        return;
    }
    QString destination = this->_gfxDir.path() + "/" + destinationName + ".png";
    for(qulonglong i = 2; QFile(destination).exists(); i++){
        destination = this->_gfxDir.path() + "/" + destinationName + "-" + QString::number(i) + ".png";
    }

    SpriteEditor *editor = new SpriteEditor(fileName);
    if(!editor->save(destination)){
        QMessageBox::critical(this, "", QObject::tr("You do not have permission to create the file %1.").arg(destination));
        delete editor;
        return;
    }

    this->_spriteFiles.append(destination);
    this->_spriteEditors.insert(destination, editor);

    QStandardItem *spriteItem = new QStandardItem(QFileInfo(destination).fileName());
    spriteItem->setIcon(QIcon(":/icons/png.svg"));
    this->_fileListModel.item(2)->appendRow(spriteItem);

    this->setActiveFile(destination);
}

bool NMLProject::renameSprite(const QString &oldName, const QString &newName){
    const QString oldPath = this->_gfxDir.path() + "/" + oldName;
    const QString newPath = this->_gfxDir.path() + "/" + newName;
    if(QFile(newPath).exists()){
        QMessageBox::critical(this, "", QObject::tr("The file %1 already exists.").arg(newPath));
        return false;
    }
    if(!QFile(oldPath).rename(newPath)){
        QMessageBox::critical(this, "", QObject::tr("Could not rename file %1.").arg(oldPath));
        return false;
    }

    const int index = this->_spriteFiles.indexOf(oldPath);
    this->_spriteFiles[index] = newPath;
    this->_spriteEditors.insert(newPath, this->_spriteEditors[oldPath]);
    this->_spriteEditors.remove(oldPath);
    this->_fileListModel.item(2)->child(index)->setText(QFileInfo(newPath).fileName());

    if(this->_activeFile == oldPath){
        this->setActiveFile(newPath);
    }

    return true;
}

bool NMLProject::removeSprite(const QString &fileName){
    const QString file = this->_gfxDir.path() + "/" + fileName;
    if(QFile(file).exists() && !QFile(file).remove()){
        QMessageBox::critical(this, "", QObject::tr("Could not delete file %1.").arg(file));
        return false;
    }

    if(QFileInfo(this->_activeFile).fileName() == fileName){
        this->setActiveFile(this->_nmlFile);
    }
    delete this->_spriteEditors[file];
    this->_spriteEditors.remove(file);
    this->_fileListModel.item(2)->removeRow(this->_spriteFiles.indexOf(file));
    this->_spriteFiles.removeAll(file);

    return true;
}

bool NMLProject::saveFile(const QString &fileName){
    if(QFileInfo(fileName).suffix() == "png"){
        SpriteEditor *editor = this->_spriteEditors[fileName];
        if(editor == nullptr){
            return true;
        }

        if(!editor->save(fileName)){
            return false;
        }
    }
    else{
        TextEditor *editor = this->_textEditors.textEditorFromFileName(fileName);
        if(editor == nullptr){    //If the editor for this file isn't open yet, do nothing
            return true;
        }

        QFile file(fileName);
        if(!file.open(QFile::WriteOnly | QFile::Truncate)){
            QMessageBox::critical(this, "", QObject::tr("An error occurred when saving the file %1. The file might be used by another program or you might not have sufficient permissions to edit this file.").arg(fileName));
            return false;
        }
        file.write(editor->toPlainText().replace(QRegExp("[^\\S\r\n]*([\r\n]|$)"), "\n").replace(QRegExp("[\r\n]*$"), "").toUtf8());
        file.close();

        this->_textEditors.markAsSaved(editor);
    }

    if(fileName == this->_activeFile){
        this->setWindowTitle(QFileInfo(this->_activeFile).fileName() + " @ " + QFileInfo(this->_nmlFile).fileName() + " - NMLCreator");
    }

    return true;
}

bool NMLProject::saveAll(){
    if(!this->saveFile(this->_nmlFile)){
        return false;
    }
    for(const QString &fileName: qAsConst(this->_languageFiles)){
        if(!this->saveFile(fileName)){
            return false;
        }
    }
    for(const QString &fileName: qAsConst(this->_spriteFiles)){
        if(!this->saveFile(fileName)){
            return false;
        }
    }
    return true;
}

void NMLProject::reloadLanguageList(){
    for(const QString &file: qAsConst(this->_languageFiles)){
        if(!QFile(file).exists()){
            this->removeLanguage(QFileInfo(file).baseName());
        }
    }

    const QStringList &languageFiles = this->_langDir.entryList({"*.lng"}, QDir::Files);
    for(const QString &languageFile: languageFiles){
        const QString completePath = this->_langDir.path() + "/" + languageFile;
        this->_languageFiles.append(completePath);

        QStandardItem *languageItem = new QStandardItem(languageFile);
        languageItem->setIcon(QIcon(":/icons/lng.svg"));
        this->_fileListModel.item(1)->appendRow(languageItem);
    }
}

void NMLProject::reloadSpriteList(){
    for(const QString &file: qAsConst(this->_spriteFiles)){
        if(!QFile(file).exists()){
            this->removeSprite(QFileInfo(file).fileName());
        }
    }

    const QStringList &spriteFiles = this->_gfxDir.entryList({"*.png"}, QDir::Files);
    for(const QString &spriteFile: spriteFiles){
        const QString filePath = this->_gfxDir.path() + "/" + spriteFile;
        if(QImage(filePath).isNull()){
            QMessageBox::critical(nullptr, "", QObject::tr("The file %1 is not a valid image file.").arg(filePath));
            continue;
        }
        this->_spriteFiles.append(filePath);
        this->_spriteEditors.insert(filePath, new SpriteEditor(filePath));

        QStandardItem *spriteItem = new QStandardItem(spriteFile);
        spriteItem->setIcon(QIcon(":/icons/png.svg"));
        this->_fileListModel.item(2)->appendRow(spriteItem);
    }
}

void NMLProject::showSettingsWindow(){
    QSettings settings("OpenTTD", "NMLCreator");

    QDialog settingsWindow(this);
    settingsWindow.setWindowTitle(QObject::tr("Settings"));
    QGridLayout layout;

    QTabWidget tabs;
    layout.addWidget(&tabs, 0, 0, 1, 4);

    //Compiler tab
    QWidget compilerTab;
    QVBoxLayout compilerLayout;

    QGroupBox compilerPathBox(QObject::tr("General"));
    QHBoxLayout compilerPathLayout;
    QLabel compilerLabel;
    QLineEdit compilerPath(settings.value("compiler/path", NMLCOMPILER).toString());
    QPushButton resetCompiler(QObject::tr("Reset"));
    QObject::connect(&resetCompiler, &QPushButton::pressed, [&](){
        compilerPath.setText(NMLCOMPILER);
    });
    compilerPathLayout.addWidget(&compilerLabel);
    compilerPathLayout.addWidget(&compilerPath);
    #ifdef _WIN32
        compilerLabel.setText(QObject::tr("Compiler path"));
        compilerPath.setWhatsThis(QObject::tr("Specifies the path to the NML compiler."));
        resetCompiler.setWhatsThis(QObject::tr("Resets the NML compiler path to the file nmlc.exe in the same folder as the NMLCreator executable."));
        QPushButton browseForCompilerButton(QObject::tr("Browse..."));
        browseForCompilerButton.setWhatsThis("Opens a dialog where you can select which executable file to use as NML compiler.");
        QObject::connect(&browseForCompilerButton, &QPushButton::pressed, [&](){
            const QString file = QFileDialog::getOpenFileName(&settingsWindow, QObject::tr("Select NML Compiler"), "", QObject::tr("All executable files") + " (*.exe;*.bat;*.bin;*.cmd;*.com;*.job;*.jse;*.ps1;*.py;*.pyc;*.vb;*.vbe;*.vbs;*.vbscript;*.ws;*.wsf);;" + QObject::tr("Executables") + " (*.exe);;" + QObject::tr("Python files") + " (*.py;*.pyc);;" + QObject::tr("All files") + " (*.*)");
            if(!file.isEmpty()){
                compilerPath.setText(file);
            }
        });
        compilerPathLayout.addWidget(&browseForCompilerButton);
    #else
        compilerLabel.setText(QObject::tr("Compiler command"));
        compilerPath.setWhatsThis(QObject::tr("Specifies the command which runs the NML compiler. By default, this is the command 'nmlc', which can be installed using 'python3 -m pip install nml'."));
        resetCompiler.setWhatsThis(QObject::tr("Resets the NML compiler command to 'nmlc'."));
    #endif
    compilerPathLayout.addWidget(&resetCompiler);
    compilerPathBox.setLayout(&compilerPathLayout);
    compilerLayout.addWidget(&compilerPathBox);

    QGroupBox cacheBox(QObject::tr("Cache"));
    QFormLayout cacheLayout;
    QCheckBox enableCache(QObject::tr("Enable caching of sprites"));
    enableCache.setChecked(settings.value("compiler/enableCache", true).toBool());
    enableCache.setWhatsThis(QObject::tr("Unchecking this box passes the --no-cache flag to the compiler, which disables caching of sprites in .cache[index] files."));
    cacheLayout.addRow(&enableCache);
    QLineEdit cacheDir(settings.value("compiler/cacheDir", ".nmlcache").toString());
    cacheDir.setWhatsThis(QObject::tr("Stores the cached files in this folder. This value is passed to the compiler using the --cache-dir flag.") + "\n\n" + QObject::tr("Both relative and absolute path are accepted. Relative paths are relative to the folder containing the .nml file."));
    cacheDir.setEnabled(enableCache.isChecked());
    QObject::connect(&enableCache, &QCheckBox::clicked, &cacheDir, &QLineEdit::setEnabled);
    cacheLayout.addRow(QObject::tr("Location of cache files"), &cacheDir);
    QCheckBox clearUnusedCache(QObject::tr("Clear unused cache files"));
    clearUnusedCache.setChecked(settings.value("compiler/clearUnusedCache", false).toBool());
    clearUnusedCache.setWhatsThis(QObject::tr("Checking this box passes the --clear-orphaned flag to the compiler, which makes the compiler automatically remove unused items from cache files."));
    cacheLayout.addRow(&clearUnusedCache);
    cacheBox.setLayout(&cacheLayout);
    compilerLayout.addWidget(&cacheBox);

    QGroupBox warningBox(QObject::tr("Warnings"));
    QFormLayout warningLayout;
    QCheckBox enableWarnings(QObject::tr("Enable warnings"));
    enableWarnings.setChecked(settings.value("compiler/enableWarnings", true).toBool());
    enableWarnings.setWhatsThis("Enables or disables warnings. Unchecking this box passes the --quiet flag to the compiler.");
    warningLayout.addRow(&enableWarnings);
    QLineEdit filterWarnings(settings.value("compiler/filterWarnings", "").toString());
    filterWarnings.setWhatsThis("Does not show warnings that match the specified regular expression. Leave empty if you don't want to filter out any warnings.");
    filterWarnings.setEnabled(enableWarnings.isChecked());
    QObject::connect(&enableWarnings, &QCheckBox::clicked, &filterWarnings, &QLineEdit::setEnabled);
    warningLayout.addRow(QObject::tr("Filter warnings according to the regular expression"), &filterWarnings);
    warningBox.setLayout(&warningLayout);
    compilerLayout.addWidget(&warningBox);

    compilerTab.setLayout(&compilerLayout);
    tabs.addTab(&compilerTab, QObject::tr("Compiler"));

    //Text editor tab
    QWidget textEditorTab;
    QVBoxLayout textEditorLayout;

    QGroupBox fontBox(QObject::tr("Font"));
    QGridLayout fontLayout;
    QLabel exampleText(QObject::tr("Example text"));
    exampleText.setFont(settings.value("textEditor/font", QFontDatabase::systemFont(QFontDatabase::FixedFont)).value<QFont>());
    exampleText.setWhatsThis(QObject::tr("Shows how the selected font will be displayed in the text editor."));
    fontLayout.addWidget(&exampleText, 0, 0, 1, 2);
    QPushButton changeFont(QObject::tr("Change font..."));
    changeFont.setWhatsThis(QObject::tr("Opens a dialog where you can select a new font to be used in the text editor."));
    QObject::connect(&changeFont, &QPushButton::pressed, [&](){
        bool ok = false;
        const QFont newFont = QFontDialog::getFont(&ok, exampleText.font(), &settingsWindow);
        if(ok){
            exampleText.setFont(newFont);
        }
    });
    fontLayout.addWidget(&changeFont, 1, 0);
    QPushButton resetFont(QObject::tr("Reset font"));
    resetFont.setWhatsThis(QObject::tr("Sets the font of the text editor to the default font."));
    QObject::connect(&resetFont, &QPushButton::pressed, [&](){
        exampleText.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    });
    fontLayout.addWidget(&resetFont, 1, 1);
    fontBox.setLayout(&fontLayout);
    textEditorLayout.addWidget(&fontBox);

    class ColorButton: public QPushButton{
    public:
        ColorButton(const QString &name, QColor *color):
            QPushButton(),
            _pixmap(32, 24),
            _color(*color),
            _colorRef(color),
            _name(name)
        {
            this->setColor(this->_color);
            this->setFixedWidth(40);
            QObject::connect(this, &QPushButton::pressed, [this](){
                const QColor color = QColorDialog::getColor(this->_color, this);
                if(color.isValid()){
                    this->setColor(color);
                }
            });
        }

        void setColor(QColor color){
            this->_color = color;
            this->_pixmap.fill(color);
            this->setIcon(QIcon(this->_pixmap));
        }

        void saveColorSettings(){
            *this->_colorRef = this->_color;
            QSettings("OpenTTD", "NMLCreator").setValue("textEditor/" + this->_name, this->_color);
        }

    private:
        QPixmap _pixmap;
        QColor _color;
        QColor *_colorRef;
        const QString _name;
    };

    QGroupBox syntaxHighlightingBox(QObject::tr("Syntax highlighting"));
    QHBoxLayout syntaxHighlightingLayout;
    QWidget syntaxHighlightingColumn1, syntaxHighlightingColumn2;
    QFormLayout syntaxHighlightingLayout1, syntaxHighlightingLayout2;
    ColorButton commentsButton("commentColor", &SyntaxHighlighter::commentColor);
    commentsButton.setWhatsThis(QObject::tr("Sets the color that comments will be displayed in in the text editor."));
    syntaxHighlightingLayout1.addRow(QObject::tr("Comments"), &commentsButton);
    ColorButton literalStringButton("literalStringColor", &SyntaxHighlighter::literalStringColor);
    literalStringButton.setWhatsThis(QObject::tr("Sets the color that literal strings (enclosed in double quotes) will be displayed in in the text editor."));
    syntaxHighlightingLayout1.addRow(QObject::tr("Literal strings"), &literalStringButton);
    ColorButton numberButton("numberColor", &SyntaxHighlighter::numberColor);
    numberButton.setWhatsThis(QObject::tr("Sets the color that numbers will be displayed in in the text editor."));
    syntaxHighlightingLayout1.addRow(QObject::tr("Numbers"), &numberButton);
    ColorButton constantButton("constantColor", &SyntaxHighlighter::constantColor);
    constantButton.setWhatsThis(QObject::tr("Sets the color that constants will be displayed in in the text editor. This includes string names defined in .lng files and built-in constants."));
    syntaxHighlightingLayout1.addRow(QObject::tr("Constants"), &constantButton);
    ColorButton keywordButton("keywordColor", &SyntaxHighlighter::keywordColor);
    keywordButton.setWhatsThis(QObject::tr("Sets the color that keywords will be displayed in in the text editor."));
    syntaxHighlightingLayout2.addRow(QObject::tr("Keywords"), &keywordButton);
    ColorButton propertyButton("propertyColor", &SyntaxHighlighter::propertyColor);
    propertyButton.setWhatsThis(QObject::tr("Sets the color that property names of an item will be displayed in in the text editor."));
    syntaxHighlightingLayout2.addRow(QObject::tr("Properties"), &propertyButton);
    ColorButton blockNameButton("blockNameColor", &SyntaxHighlighter::blockNameColor);
    blockNameButton.setWhatsThis(QObject::tr("Sets the color that block names will be displayed in in the text editor. This includes string names defined in .lng files and built-in blockNames."));
    syntaxHighlightingLayout2.addRow(QObject::tr("Block names"), &blockNameButton);
    QPushButton resetSyntaxHighlighting(QObject::tr("Reset colors"));
    resetSyntaxHighlighting.setWhatsThis(QObject::tr("Resets all the syntax highlighting colors to their default values."));
    QObject::connect(&resetSyntaxHighlighting, &QPushButton::pressed, [&](){
        commentsButton.setColor(QColor(128, 128, 128));
        literalStringButton.setColor(QColor(175, 175, 0));
        numberButton.setColor(QColor(128, 0, 128));
        constantButton.setColor(QColor(200, 128, 0));
        keywordButton.setColor(QColor(255, 0, 100));
        propertyButton.setColor(QColor(0, 100, 255));
        blockNameButton.setColor(QColor(50, 150, 0));
    });
    syntaxHighlightingLayout2.addRow(&resetSyntaxHighlighting);
    syntaxHighlightingColumn1.setLayout(&syntaxHighlightingLayout1);
    syntaxHighlightingColumn2.setLayout(&syntaxHighlightingLayout2);
    syntaxHighlightingLayout.addWidget(&syntaxHighlightingColumn1);
    syntaxHighlightingLayout.addWidget(&syntaxHighlightingColumn2);
    syntaxHighlightingBox.setLayout(&syntaxHighlightingLayout);
    textEditorLayout.addWidget(&syntaxHighlightingBox);

    textEditorTab.setLayout(&textEditorLayout);
    tabs.addTab(&textEditorTab, QObject::tr("Text editor"));

    //OK and cancel buttons
    QPushButton okButton(QObject::tr("OK"));
    okButton.setWhatsThis(QObject::tr("Closes this window and saves the changes."));
    QObject::connect(&okButton, &QPushButton::pressed, [&](){
        if(!QRegularExpression(filterWarnings.text()).isValid()){
            QMessageBox::critical(&settingsWindow, "", QObject::tr("The regular expression specified in \"Filter warnings according to the regular expression\" is not valid."));
            return;
        }
        settingsWindow.accept();
    });
    layout.addWidget(&okButton, 1, 1);

    QPushButton cancelButton(QObject::tr("Cancel"));
    cancelButton.setWhatsThis(QObject::tr("Closes this window and discards the changes."));
    QObject::connect(&cancelButton, &QPushButton::pressed, &settingsWindow, &QDialog::reject);
    layout.addWidget(&cancelButton, 1, 2);

    QPushButton resetButton(QObject::tr("Reset all"));
    resetButton.setWhatsThis(QObject::tr("Closes this window and resets all the settings to their default values."));
    QObject::connect(&resetButton, &QPushButton::pressed, [&](){
        if(QMessageBox::warning(&settingsWindow, "", QObject::tr("Do you really want to reset all the settings to their default values?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
            return;
        }

        emit resetCompiler.pressed();
        emit resetFont.pressed();
        emit resetSyntaxHighlighting.pressed();

        enableCache.setChecked(true);
        cacheDir.setEnabled(true);
        cacheDir.setText(".nmlcache");
        enableWarnings.setChecked(true);
        filterWarnings.setEnabled(true);
        filterWarnings.setText("");

        settingsWindow.accept();
    });
    layout.addWidget(&resetButton, 1, 3);

    layout.setColumnStretch(0, 5);
    settingsWindow.setLayout(&layout);
    const bool accepted = settingsWindow.exec();

    //Save the settings
    if(accepted){
        settings.setValue("compiler/path", compilerPath.text());
        settings.setValue("compiler/enableCache", enableCache.isChecked());
        settings.setValue("compiler/cacheDir", cacheDir.text());
        settings.setValue("compiler/enableWarnings", enableWarnings.isChecked());
        settings.setValue("compiler/filterWarnings", filterWarnings.text());

        settings.setValue("textEditor/font", exampleText.font());
        commentsButton.saveColorSettings();
        literalStringButton.saveColorSettings();
        numberButton.saveColorSettings();
        constantButton.saveColorSettings();
        keywordButton.saveColorSettings();
        propertyButton.saveColorSettings();
        blockNameButton.saveColorSettings();
        SyntaxHighlighter::updateAllSyntaxHighlighters();
    }
}

QString NMLProject::fileFromModelIndex(const QModelIndex &index) const{
    if(!index.isValid()){
        return "";
    }
    else if(!index.parent().isValid()){
        switch(index.row()){
        case 0:
            return this->_nmlFile;
        case 1:
            return this->_projectDir.path() + "/lang";
        case 2:
            return this->_projectDir.path() + "/gfx";
        }
        return "";
    }
    else switch(index.parent().row()){
    case 1:
        return this->_languageFiles[index.row()];
    case 2:
        return this->_spriteFiles[index.row()];
    }
    return "";
}

bool NMLProject::setActiveFile(const QString &fileName){
    QWidget *editor = nullptr;
    if(QFileInfo(fileName).suffix() == "png"){
        if(!this->_spriteEditors.contains(fileName)){
            return false;
        }
        editor = new QScrollArea;
    }
    else{
        editor = this->_textEditors.textEditorFromFileName(fileName);
        if(editor == nullptr && !(editor = this->_textEditors.addTextEditor(fileName, (QFileInfo(fileName).suffix() == "nml") ? SyntaxHighlighter::NML : SyntaxHighlighter::LNG))){
            return false;
        }
    }

    this->_undoButton->disconnect();
    this->_redoButton->disconnect();
    this->_cutButton->disconnect();
    this->_copyButton->disconnect();
    this->_pasteButton->disconnect();
    this->_selectAllButton->disconnect();
    QWidget *activeEditor = this->centralWidget();
    TextEditor *activeTextEditor = dynamic_cast<TextEditor*>(activeEditor);
    SpriteEditor *activeSpriteEditor = nullptr;
    QScrollArea *activeScrollArea = dynamic_cast<QScrollArea*>(activeEditor);
    if(activeScrollArea != nullptr){
        activeSpriteEditor = dynamic_cast<SpriteEditor*>(activeScrollArea->widget());
        activeSpriteEditor->setParent(nullptr);
        activeSpriteEditor->hide();
        activeSpriteEditor->disconnect();    //This is all we need, the buttons are disconnected above

        this->_toggleImageTools->setDisabled(true);
        this->_toggleImageTools->disconnect();
        this->removeToolBar(activeSpriteEditor->toolBar());
        activeSpriteEditor->toolBar()->disconnect();
    }
    if(activeTextEditor != nullptr){
        //We don't need to disconnect the buttons here, that's done above
        QObject::disconnect(activeTextEditor, &TextEditor::undoAvailable, this->_undoButton, &QAction::setEnabled);
        QObject::disconnect(activeTextEditor, &TextEditor::redoAvailable, this->_redoButton, &QAction::setEnabled);
        QObject::disconnect(activeTextEditor, &TextEditor::copyAvailable, this->_cutButton, &QAction::setEnabled);
        QObject::disconnect(activeTextEditor, &TextEditor::copyAvailable, this->_copyButton, &QAction::setEnabled);
        activeTextEditor->setParent(nullptr);    //Otherwise the program crashes
        activeTextEditor->hide();    //Otherwise it will open the editor in a new window
    }
    this->_activeFile = fileName;
    this->setCentralWidget(editor);
    editor->show();    //To compensate for hiding the widget above

    TextEditor *textEditor = dynamic_cast<TextEditor*>(editor);
    QScrollArea *scrollArea = dynamic_cast<QScrollArea*>(editor);
    if(textEditor != nullptr){
        QObject::connect(this->_undoButton, &QAction::triggered, textEditor, &TextEditor::undo);
        QObject::connect(textEditor, &TextEditor::undoAvailable, this->_undoButton, &QAction::setEnabled);
        this->_undoButton->setEnabled(this->_textEditors.undoEnabled(textEditor));
        QObject::connect(this->_redoButton, &QAction::triggered, textEditor, &TextEditor::redo);
        QObject::connect(textEditor, &TextEditor::redoAvailable, this->_redoButton, &QAction::setEnabled);
        this->_redoButton->setEnabled(this->_textEditors.redoEnabled(textEditor));
        QObject::connect(this->_cutButton, &QAction::triggered, textEditor, &TextEditor::cut);
        QObject::connect(textEditor, &TextEditor::copyAvailable, this->_cutButton, &QAction::setEnabled);
        this->_cutButton->setEnabled(this->_textEditors.copyEnabled(textEditor));
        QObject::connect(this->_copyButton, &QAction::triggered, textEditor, &TextEditor::copy);
        QObject::connect(textEditor, &TextEditor::copyAvailable, this->_copyButton, &QAction::setEnabled);
        this->_copyButton->setEnabled(this->_textEditors.copyEnabled(textEditor));
        QObject::connect(this->_pasteButton, &QAction::triggered, textEditor, &TextEditor::paste);
        this->_pasteButton->setEnabled(true);
        QObject::connect(this->_selectAllButton, &QAction::triggered, textEditor, &TextEditor::selectAll);
        this->_selectAllButton->setEnabled(true);

        this->setWindowTitle((this->_textEditors.hasUnsavedChanges(textEditor) ? "*" : "") + QFileInfo(fileName).fileName() + " @ " + QFileInfo(this->_nmlFile).fileName() + " - NMLCreator");
    }
    if(scrollArea != nullptr){
        SpriteEditor *spriteEditor = this->_spriteEditors[fileName];
        scrollArea->setWidget(spriteEditor);
        spriteEditor->show();

        QObject::connect(this->_undoButton, &QAction::triggered, spriteEditor, &SpriteEditor::undo);
        QObject::connect(spriteEditor, &SpriteEditor::undoAvailable, this->_undoButton, &QAction::setEnabled);
        this->_undoButton->setEnabled(spriteEditor->undoIsAvailable());
        QObject::connect(this->_redoButton, &QAction::triggered, spriteEditor, &SpriteEditor::redo);
        QObject::connect(spriteEditor, &SpriteEditor::redoAvailable, this->_redoButton, &QAction::setEnabled);
        this->_redoButton->setEnabled(spriteEditor->redoIsAvailable());
        this->_cutButton->setEnabled(false);
        this->_copyButton->setEnabled(false);
        this->_pasteButton->setEnabled(false);
        this->_selectAllButton->setEnabled(false);

        this->setWindowTitle((spriteEditor->hasUnsavedChanges() ? "*" : "") + QFileInfo(fileName).fileName() + " @ " + QFileInfo(this->_nmlFile).fileName() + " - NMLCreator");
        QObject::connect(spriteEditor, &SpriteEditor::imageChanged, [this, spriteEditor, fileName](){
            this->setWindowTitle((spriteEditor->hasUnsavedChanges() ? "*" : "") + QFileInfo(fileName).fileName() + " @ " + QFileInfo(this->_nmlFile).fileName() + " - NMLCreator");
        });

        this->_toggleImageTools->setDisabled(false);
        this->_toggleImageTools->setChecked(true);
        QObject::connect(this->_toggleImageTools, &QAction::triggered, spriteEditor->toolBar(), &QToolBar::setVisible);
        this->addToolBar(spriteEditor->toolBar());
        spriteEditor->toolBar()->show();
        QObject::connect(spriteEditor->toolBar(), &QToolBar::visibilityChanged, this->_toggleImageTools, &QAction::setChecked);
    }

    return true;
}

QPair<QString, int> NMLProject::fileAndLineNumber(const QString &message){
    QRegExp regex("^\\s*nmlc\\s*(?:error|warning)\\s*:\\s*\"([^\"]+)\"\\s*,\\s*line\\s*([0-9]+)", Qt::CaseInsensitive);
    QString file = "";
    int lineNumber = 0;
    if(regex.indexIn(message) != -1){
        file = regex.cap(1).replace("\\", "/");
        if(this->_textEditors.textEditorFromFileName(file) == nullptr){
            file = this->_projectDir.path() + "/" + regex.cap(1).replace("\\", "/");
        }
        if(this->_textEditors.textEditorFromFileName(file) == nullptr){
            file = "";
        }
        lineNumber = regex.cap(2).toInt();
    }
    return QPair<QString, int>(file, lineNumber);
}

QPair<TextEditor*, int> NMLProject::editorAndLineNumber(const QString &message){
    const QPair<QString, int> fileAndLineNumber = this->fileAndLineNumber(message);
    return QPair<TextEditor*, int>(this->_textEditors.textEditorFromFileName(fileAndLineNumber.first), fileAndLineNumber.second);
}
