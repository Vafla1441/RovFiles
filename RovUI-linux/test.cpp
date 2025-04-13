#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtWidgets/WidgetVideo.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    VlcInstance instance(VlcCommon::args());
    VlcMedia media("rtsp://192.168.1.6/stream=0", &instance);
    VlcMediaPlayer player(&instance);
    VlcWidgetVideo videoWidget;
    
    player.setVideoWidget(&videoWidget);
    player.open(&media);
    videoWidget.show();
    
    return app.exec();
}