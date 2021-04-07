#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include <QLabel>
#include <QImage>
#include <QToolBar>
#include <QMouseEvent>

class SpriteEditor : public QLabel{
    Q_OBJECT

public:
    SpriteEditor(const QString &fileName);

    QToolBar *toolBar();

    bool save(const QString &fileName);
    bool hasUnsavedChanges() const;

    void undo();
    void redo();

    bool undoIsAvailable() const;
    bool redoIsAvailable() const;

signals:
    void imageChanged();

    void undoAvailable(bool);
    void redoAvailable(bool);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void updateImage();

    QToolBar _toolBar;
    QImage _image, _previousImage;
    QList<QImage> _undoHistory, _redoHistory;
    QAction *const _zoomIn, *const _zoomOut;

    int _zoom;
    QRgb _currentColor;
    bool _currentlyPressed;
    bool _hasUnsavedChanges;

    static const QVector<QRgb> _palette;
};

#endif // SPRITEEDITOR_H
