#include <QDialog>
#include <QGridLayout>
#include <QToolButton>
#include "spriteeditor.h"

const QVector<QRgb> SpriteEditor::_palette = QVector<QRgb>({
    QColor(0, 0, 255).rgb(),
    QColor(16, 16, 16).rgb(),
    QColor(32, 32, 32).rgb(),
    QColor(48, 48, 48).rgb(),
    QColor(64, 64, 64).rgb(),
    QColor(80, 80, 80).rgb(),
    QColor(100, 100, 100).rgb(),
    QColor(116, 116, 116).rgb(),
    QColor(132, 132, 132).rgb(),
    QColor(148, 148, 148).rgb(),
    QColor(168, 168, 168).rgb(),
    QColor(184, 184, 184).rgb(),
    QColor(200, 200, 200).rgb(),
    QColor(216, 216, 216).rgb(),
    QColor(232, 232, 232).rgb(),
    QColor(252, 252, 252).rgb(),
    QColor(52, 60, 72).rgb(),
    QColor(68, 76, 92).rgb(),
    QColor(88, 96, 112).rgb(),
    QColor(108, 116, 132).rgb(),
    QColor(132, 140, 152).rgb(),
    QColor(156, 160, 172).rgb(),
    QColor(176, 184, 196).rgb(),
    QColor(204, 208, 220).rgb(),
    QColor(48, 44, 4).rgb(),
    QColor(64, 60, 12).rgb(),
    QColor(80, 76, 20).rgb(),
    QColor(96, 92, 28).rgb(),
    QColor(120, 120, 64).rgb(),
    QColor(148, 148, 100).rgb(),
    QColor(176, 176, 132).rgb(),
    QColor(204, 204, 168).rgb(),
    QColor(72, 44, 4).rgb(),
    QColor(88, 60, 20).rgb(),
    QColor(104, 80, 44).rgb(),
    QColor(124, 104, 72).rgb(),
    QColor(152, 132, 92).rgb(),
    QColor(184, 160, 120).rgb(),
    QColor(212, 188, 148).rgb(),
    QColor(244, 220, 176).rgb(),
    QColor(64, 0, 4).rgb(),
    QColor(88, 4, 16).rgb(),
    QColor(112, 16, 32).rgb(),
    QColor(136, 32, 52).rgb(),
    QColor(160, 56, 76).rgb(),
    QColor(188, 84, 108).rgb(),
    QColor(204, 104, 124).rgb(),
    QColor(220, 132, 144).rgb(),
    QColor(236, 156, 164).rgb(),
    QColor(252, 188, 192).rgb(),
    QColor(252, 208, 0).rgb(),
    QColor(252, 232, 60).rgb(),
    QColor(252, 252, 128).rgb(),
    QColor(76, 40, 0).rgb(),
    QColor(96, 60, 8).rgb(),
    QColor(116, 88, 28).rgb(),
    QColor(136, 116, 56).rgb(),
    QColor(156, 136, 80).rgb(),
    QColor(176, 156, 108).rgb(),
    QColor(196, 180, 136).rgb(),
    QColor(68, 24, 0).rgb(),
    QColor(96, 44, 4).rgb(),
    QColor(128, 68, 8).rgb(),
    QColor(156, 96, 16).rgb(),
    QColor(184, 120, 24).rgb(),
    QColor(212, 156, 32).rgb(),
    QColor(232, 184, 16).rgb(),
    QColor(252, 212, 0).rgb(),
    QColor(252, 248, 128).rgb(),
    QColor(252, 252, 192).rgb(),
    QColor(32, 4, 0).rgb(),
    QColor(64, 20, 8).rgb(),
    QColor(84, 28, 16).rgb(),
    QColor(108, 44, 28).rgb(),
    QColor(128, 56, 40).rgb(),
    QColor(148, 72, 56).rgb(),
    QColor(168, 92, 76).rgb(),
    QColor(184, 108, 88).rgb(),
    QColor(196, 128, 108).rgb(),
    QColor(212, 148, 128).rgb(),
    QColor(8, 52, 0).rgb(),
    QColor(16, 64, 0).rgb(),
    QColor(32, 80, 4).rgb(),
    QColor(48, 96, 4).rgb(),
    QColor(64, 112, 12).rgb(),
    QColor(84, 132, 20).rgb(),
    QColor(104, 148, 28).rgb(),
    QColor(128, 168, 44).rgb(),
    QColor(28, 52, 24).rgb(),
    QColor(44, 68, 32).rgb(),
    QColor(60, 88, 48).rgb(),
    QColor(80, 104, 60).rgb(),
    QColor(104, 124, 76).rgb(),
    QColor(128, 148, 92).rgb(),
    QColor(152, 176, 108).rgb(),
    QColor(180, 204, 124).rgb(),
    QColor(16, 52, 24).rgb(),
    QColor(32, 72, 44).rgb(),
    QColor(56, 96, 72).rgb(),
    QColor(76, 116, 88).rgb(),
    QColor(96, 136, 108).rgb(),
    QColor(120, 164, 136).rgb(),
    QColor(152, 192, 168).rgb(),
    QColor(184, 220, 200).rgb(),
    QColor(32, 24, 0).rgb(),
    QColor(56, 28, 0).rgb(),
    QColor(72, 40, 4).rgb(),
    QColor(88, 52, 12).rgb(),
    QColor(104, 64, 24).rgb(),
    QColor(124, 84, 44).rgb(),
    QColor(140, 108, 64).rgb(),
    QColor(160, 128, 88).rgb(),
    QColor(76, 40, 16).rgb(),
    QColor(96, 52, 24).rgb(),
    QColor(116, 68, 40).rgb(),
    QColor(136, 84, 56).rgb(),
    QColor(164, 96, 64).rgb(),
    QColor(184, 112, 80).rgb(),
    QColor(204, 128, 96).rgb(),
    QColor(212, 148, 112).rgb(),
    QColor(224, 168, 128).rgb(),
    QColor(236, 188, 148).rgb(),
    QColor(80, 28, 4).rgb(),
    QColor(100, 40, 20).rgb(),
    QColor(120, 56, 40).rgb(),
    QColor(140, 76, 64).rgb(),
    QColor(160, 100, 96).rgb(),
    QColor(184, 136, 136).rgb(),
    QColor(36, 40, 68).rgb(),
    QColor(48, 52, 84).rgb(),
    QColor(64, 64, 100).rgb(),
    QColor(80, 80, 116).rgb(),
    QColor(100, 100, 136).rgb(),
    QColor(132, 132, 164).rgb(),
    QColor(172, 172, 192).rgb(),
    QColor(212, 212, 224).rgb(),
    QColor(40, 20, 112).rgb(),
    QColor(64, 44, 144).rgb(),
    QColor(88, 64, 172).rgb(),
    QColor(104, 76, 196).rgb(),
    QColor(120, 88, 224).rgb(),
    QColor(140, 104, 252).rgb(),
    QColor(160, 136, 252).rgb(),
    QColor(188, 168, 252).rgb(),
    QColor(0, 24, 108).rgb(),
    QColor(0, 36, 132).rgb(),
    QColor(0, 52, 160).rgb(),
    QColor(0, 72, 184).rgb(),
    QColor(0, 96, 212).rgb(),
    QColor(24, 120, 220).rgb(),
    QColor(56, 144, 232).rgb(),
    QColor(88, 168, 240).rgb(),
    QColor(128, 196, 252).rgb(),
    QColor(188, 224, 252).rgb(),
    QColor(16, 64, 96).rgb(),
    QColor(24, 80, 108).rgb(),
    QColor(40, 96, 120).rgb(),
    QColor(52, 112, 132).rgb(),
    QColor(80, 140, 160).rgb(),
    QColor(116, 172, 192).rgb(),
    QColor(156, 204, 220).rgb(),
    QColor(204, 240, 252).rgb(),
    QColor(172, 52, 52).rgb(),
    QColor(212, 52, 52).rgb(),
    QColor(252, 52, 52).rgb(),
    QColor(252, 100, 88).rgb(),
    QColor(252, 144, 124).rgb(),
    QColor(252, 184, 160).rgb(),
    QColor(252, 216, 200).rgb(),
    QColor(252, 244, 236).rgb(),
    QColor(72, 20, 112).rgb(),
    QColor(92, 44, 140).rgb(),
    QColor(112, 68, 168).rgb(),
    QColor(140, 100, 196).rgb(),
    QColor(168, 136, 224).rgb(),
    QColor(200, 176, 248).rgb(),
    QColor(208, 184, 255).rgb(),
    QColor(232, 208, 252).rgb(),
    QColor(60, 0, 0).rgb(),
    QColor(92, 0, 0).rgb(),
    QColor(128, 0, 0).rgb(),
    QColor(160, 0, 0).rgb(),
    QColor(196, 0, 0).rgb(),
    QColor(224, 0, 0).rgb(),
    QColor(252, 0, 0).rgb(),
    QColor(252, 80, 0).rgb(),
    QColor(252, 108, 0).rgb(),
    QColor(252, 136, 0).rgb(),
    QColor(252, 164, 0).rgb(),
    QColor(252, 192, 0).rgb(),
    QColor(252, 220, 0).rgb(),
    QColor(252, 252, 0).rgb(),
    QColor(204, 136, 8).rgb(),
    QColor(228, 144, 4).rgb(),
    QColor(252, 156, 0).rgb(),
    QColor(252, 176, 48).rgb(),
    QColor(252, 196, 100).rgb(),
    QColor(252, 216, 152).rgb(),
    QColor(8, 24, 88).rgb(),
    QColor(12, 36, 104).rgb(),
    QColor(20, 52, 124).rgb(),
    QColor(28, 68, 140).rgb(),
    QColor(40, 92, 164).rgb(),
    QColor(56, 120, 188).rgb(),
    QColor(72, 152, 216).rgb(),
    QColor(100, 172, 224).rgb(),
    QColor(92, 156, 52).rgb(),
    QColor(108, 176, 64).rgb(),
    QColor(124, 200, 76).rgb(),
    QColor(144, 224, 92).rgb(),
    QColor(224, 244, 252).rgb(),
    QColor(200, 236, 248).rgb(),
    QColor(180, 220, 236).rgb(),
    QColor(132, 188, 216).rgb(),
    QColor(88, 152, 172).rgb(),
    QColor(244, 0, 244).rgb(),
    QColor(245, 0, 245).rgb(),
    QColor(246, 0, 246).rgb(),
    QColor(247, 0, 247).rgb(),
    QColor(248, 0, 248).rgb(),
    QColor(249, 0, 249).rgb(),
    QColor(250, 0, 250).rgb(),
    QColor(251, 0, 251).rgb(),
    QColor(252, 0, 252).rgb(),
    QColor(253, 0, 253).rgb(),
    QColor(254, 0, 254).rgb(),
    QColor(255, 0, 255).rgb(),
    QColor(76, 24, 8).rgb(),
    QColor(108, 44, 24).rgb(),
    QColor(144, 72, 52).rgb(),
    QColor(176, 108, 84).rgb(),
    QColor(210, 146, 126).rgb(),
    QColor(252, 60, 0).rgb(),
    QColor(252, 84, 0).rgb(),
    QColor(252, 104, 0).rgb(),
    QColor(252, 124, 0).rgb(),
    QColor(252, 148, 0).rgb(),
    QColor(252, 172, 0).rgb(),
    QColor(252, 196, 0).rgb(),
    QColor(64, 0, 0).rgb(),
    QColor(255, 0, 0).rgb(),
    QColor(48, 48, 0).rgb(),
    QColor(64, 64, 0).rgb(),
    QColor(80, 80, 0).rgb(),
    QColor(255, 255, 0).rgb(),
    QColor(32, 68, 112).rgb(),
    QColor(36, 72, 116).rgb(),
    QColor(40, 76, 120).rgb(),
    QColor(44, 80, 124).rgb(),
    QColor(48, 84, 128).rgb(),
    QColor(72, 100, 144).rgb(),
    QColor(100, 132, 168).rgb(),
    QColor(216, 244, 252).rgb(),
    QColor(96, 128, 164).rgb(),
    QColor(68, 96, 140).rgb(),
    QColor(255, 255, 255).rgb()
});

SpriteEditor::SpriteEditor(const QString &fileName):
    _toolBar(QObject::tr("&Image tools")),
    _image(QImage(fileName).convertToFormat(QImage::Format_Indexed8, _palette)),
    _previousImage(_image),
    _zoomIn(new QAction(QIcon(":/icons/zoom-in.svg"), QObject::tr("Zoom &in"))),
    _zoomOut(new QAction(QIcon(":/icons/zoom-out.svg"), QObject::tr("Zoom &out"))),
    _zoom(1),
    _currentColor(_palette[0]),
    _currentlyPressed(false),
    _hasUnsavedChanges(false)
{
    //Insert the image
    this->updateImage();

    //Set the cursor
    this->setCursor(QCursor(QPixmap::fromImage(QImage(":/icons/pencil.svg")).scaledToHeight(32), 0, 32));

    //Create the toolbar
    this->_toolBar.addAction(this->_zoomIn);
    this->_toolBar.addAction(this->_zoomOut);

    QObject::connect(this->_zoomIn, &QAction::triggered, [this](){
        this->_zoom = qBound(1, this->_zoom + 1, 10);
        this->updateImage();
    });

    QObject::connect(this->_zoomOut, &QAction::triggered, [this](){
        this->_zoom = qBound(1, this->_zoom - 1, 10);
        this->updateImage();
    });

    this->_toolBar.addSeparator();

    QPixmap colorPixmap(24, 24);
    colorPixmap.fill(QColor(this->_currentColor));
    QToolButton *colorPicker = new QToolButton;
    colorPicker->setIcon(QIcon(colorPixmap));
    colorPicker->setText(QObject::tr("Color picker..."));
    colorPicker->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->_toolBar.addWidget(colorPicker);

    QObject::connect(colorPicker, &QToolButton::pressed, [this, colorPicker](){
        QDialog dialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        dialog.setWindowTitle(QObject::tr("Color picker"));
        QGridLayout layout;
        layout.setSizeConstraint(QLayout::SetFixedSize);
        layout.setSpacing(2);
        layout.setMargin(2);
        for(int i = 0; i < this->_palette.length(); i++){
            QToolButton *colorButton = new QToolButton;
            colorButton->setFixedSize(24, 24);
            colorButton->setStyleSheet("QToolButton{background:" + QColor(this->_palette[i]).name() + ((this->_palette[i] == this->_currentColor) ? ";" : ";border:none}QToolButton:hover{") + "border:3px inset gray}");
            layout.addWidget(colorButton, i / 32, i % 32);

            QObject::connect(colorButton, &QToolButton::pressed, [this, colorPicker, i, &dialog](){
                this->_currentColor = this->_palette[i];
                QPixmap colorPixmap(24, 24);
                colorPixmap.fill(QColor(this->_currentColor));
                colorPicker->setIcon(QIcon(colorPixmap));
                dialog.accept();
            });
        }
        dialog.setLayout(&layout);
        dialog.exec();
        colorPicker->setDisabled(true);    //We have to disable it and re-enable it otherwise it will just stay checked forever
        colorPicker->setDisabled(false);
    });
}

QToolBar *SpriteEditor::toolBar(){
    return &this->_toolBar;
}

bool SpriteEditor::save(const QString &fileName){
    if(!this->_image.save(fileName, "png")){
        return false;
    }
    this->_hasUnsavedChanges = false;
    return true;
}

bool SpriteEditor::hasUnsavedChanges() const{
    return this->_hasUnsavedChanges;
}

void SpriteEditor::undo(){
    if(!this->undoIsAvailable()){
        return;
    }

    this->_redoHistory.append(this->_image);
    this->_image = this->_undoHistory.last();
    this->_undoHistory.removeLast();
    this->updateImage();

    emit this->undoAvailable(this->undoIsAvailable());
    emit this->redoAvailable(this->redoIsAvailable());
}

void SpriteEditor::redo(){
    if(!this->redoIsAvailable()){
        return;
    }

    this->_undoHistory.append(this->_image);
    this->_image = this->_redoHistory.last();
    this->_redoHistory.removeLast();
    this->updateImage();

    emit this->undoAvailable(this->undoIsAvailable());
    emit this->redoAvailable(this->redoIsAvailable());
}

bool SpriteEditor::undoIsAvailable() const{
    return this->_undoHistory.length() > 0;
}

bool SpriteEditor::redoIsAvailable() const{
    return this->_redoHistory.length() > 0;
}

void SpriteEditor::mousePressEvent(QMouseEvent *event){
    this->_currentlyPressed = true;
    this->mouseMoveEvent(event);
}

void SpriteEditor::mouseReleaseEvent(QMouseEvent*){
    this->_currentlyPressed = false;
    this->_hasUnsavedChanges = true;
    emit this->imageChanged();

    this->_undoHistory.append(this->_previousImage);
    this->_previousImage = this->_image;
    this->updateImage();

    emit this->undoAvailable(this->undoIsAvailable());
    emit this->redoAvailable(this->redoIsAvailable());
}

void SpriteEditor::mouseMoveEvent(QMouseEvent *event){
    if(this->_currentlyPressed){
        this->_image.setPixel(event->x() / this->_zoom, event->y() / this->_zoom, this->_palette.indexOf(this->_currentColor));
        this->updateImage();
    }
}

void SpriteEditor::wheelEvent(QWheelEvent *event){
    const int delta = event->angleDelta().y() / 120;
    this->_zoom = qBound(1, this->_zoom + delta, 10);
    this->updateImage();
}

void SpriteEditor::updateImage(){
    this->_zoomIn->setDisabled(this->_zoom >= 10);
    this->_zoomOut->setDisabled(this->_zoom <= 1);
    this->setPixmap(QPixmap::fromImage(this->_image).scaled(this->_image.width() * this->_zoom, this->_image.height() * this->_zoom));
    this->setFixedSize(this->_image.width() * this->_zoom, this->_image.height() * this->_zoom);
}
