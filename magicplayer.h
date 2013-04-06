#ifndef MAGICPLAYER_H
#define MAGICPLAYER_H

#include <QMainWindow>
#include <QTimer>
#include <vlc/vlc.h>

namespace Ui {
class MagicPlayer;
}

class MagicPlayer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MagicPlayer(QWidget *parent = 0);
    ~MagicPlayer();
    void setplayStatus(int);
    int getplayStatus();
    //void updateSize(const libvlc_event_t *event,void *data);

    int player_initialize(libvlc_media_player_t *);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void videoPlay();
    void videoStop();

    void windowResize();

    void updateProgress(int);
    void updateVolume(int);
    void updateInterface();

    void updatestartTime();
    void updateendTime();

    void actionIncreaseVolume();
    void actionDecreaseVolume();
    void actionMute();

    void actionBackward();
    void actionForward();
    void actionSpecified();

    void actionOnTop(bool);
    void actionFullscreen(bool);
    void actionFitWindow(bool);

    void actionAbout();
    void actionOpen();
private:
    Ui::MagicPlayer *ui;
    libvlc_instance_t * inst;
    libvlc_media_player_t *mp;
    int playStatus;//0----play,1----pause
    QTimer *timer;//used to flesh the interface
};

#endif // MAGICPLAYER_H
