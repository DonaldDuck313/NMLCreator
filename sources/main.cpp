#include <QApplication>
#include <QtWidgets>
#include "nmlproject.h"

int main(int argc, char **argv){
    QApplication app(argc, argv);

    if(argc >= 2){
        const QString fileToOpen = QString(argv[1]).replace("\\", "/");
        const QString type = QFileInfo(fileToOpen).suffix();
        if(!QFile(fileToOpen).exists()){
            QMessageBox::critical(nullptr, "", QObject::tr("Could not find file %1.").arg(fileToOpen));
            return 2;    //The system cannot find the file specified.
        }
        else if(!QFile(fileToOpen).open(QFile::ReadOnly)){
            QMessageBox::critical(nullptr, "", QObject::tr("Could not open file %1.").arg(fileToOpen));
            return 4;    //The system cannot open the file.
        }
        else if(type == "nml"){
            new NMLProject(fileToOpen);
        }
        else if(type == "lng" || type == "png"){
            const QDir langDir = QFileInfo(fileToOpen).dir();
            const QDir projectDir = QFileInfo(langDir.path()).dir();
            const QString nmlFile = projectDir.path() + "/" + projectDir.dirName() + ".nml";
            if(QFile(nmlFile).exists() && QFile(nmlFile).open(QFile::ReadOnly)){
                new NMLProject(nmlFile, fileToOpen);
            }
            else{
                const QStringList &nmlFiles = projectDir.entryList({"*.nml"}, QDir::Files);
                if((type == "lng" && langDir.dirName() != "lang") || (type == "png" && langDir.dirName() != "gfx") || nmlFiles.isEmpty()){
                    QMessageBox::critical(nullptr, "", QObject::tr("The file %1 does not seem to be part of an NML project.").arg(argv[1]));
                    return 3;    //The system cannot find the path specified.
                }
                if(!QFile(nmlFiles[0]).open(QFile::ReadOnly)){
                    QMessageBox::critical(nullptr, "", QObject::tr("Could not open file %1.").arg(nmlFiles[0]));
                    return 4;    //The system cannot open the file.
                }
                new NMLProject(nmlFiles[0], fileToOpen);
            }
        }
        else{
            QMessageBox::critical(nullptr, "", QObject::tr("The file %1 is not a valid NML file.").arg(fileToOpen));
            return 1;    //Incorrect function.
        }
    }
    else{
        QToolBar *startWindow = new QToolBar;

        QAction *newProject = startWindow->addAction(QIcon(":/icons/new.svg"), QObject::tr("Create new project"));
        QAction *openProject = startWindow->addAction(QIcon(":/icons/open.svg"), QObject::tr("Open existing project"));

        QObject::connect(newProject, &QAction::triggered, [&](){
            delete startWindow;
            NMLProject::newNMLProject();
        });

        QObject::connect(openProject, &QAction::triggered, [&](){
            if(NMLProject::openNMLProject(startWindow) != nullptr){
                delete startWindow;
            }
        });

        startWindow->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        startWindow->setStyleSheet("QToolButton{margin:8px; padding:8px; font-size:14pt}");
        startWindow->setOrientation(Qt::Vertical);
        startWindow->setWindowIcon(QIcon(":/icons/icon.svg"));
        startWindow->show();
    }

    return app.exec();
}
