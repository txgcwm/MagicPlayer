#include <QResizeEvent>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QThread>
#include <QString>
#include <QTime>
#include <QFile>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "magicplayer.h"
#include "ui_magicplayer.h"



MagicPlayer::MagicPlayer(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MagicPlayer)
{
    ui->setupUi(this);
    setplayStatus(1);

    inst = NULL;
    mp = NULL;

    inst = libvlc_new(0, NULL); /* Load the VLC engine */

    //set the default size
    this->resize(600, 400);
    ui->progressSlider->setMaximum(1000);
    ui->volumeSlider->setMaximum(100);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateInterface()));

    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(videoPlay()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(videoStop()));

    connect(ui->action_Play, SIGNAL(triggered()), this, SLOT(videoPlay()));
    connect(ui->action_Stop, SIGNAL(triggered()), this, SLOT(videoPlay()));

    connect(ui->action_Jump_Forward, SIGNAL(triggered()), this, SLOT(actionForward()));
    connect(ui->action_Jump_Backward, SIGNAL(triggered()), this, SLOT(actionBackward()));
    connect(ui->action_Jump_Specified_Time, SIGNAL(triggered()), this, SLOT(actionSpecified()));

    connect(ui->action_Always_On_Top, SIGNAL(triggered(bool)), this, SLOT(actionOnTop(bool)));
    connect(ui->action_Always_Fit_Window, SIGNAL(triggered(bool)), this, SLOT(actionFitWindow(bool)));
    connect(ui->action_Fullscreen, SIGNAL(triggered(bool)), this, SLOT(actionFullscreen(bool)));

    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(actionAbout()));

    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->action_Open_File, SIGNAL(triggered()), this, SLOT(actionOpen()));

    //connect(ui->progressSlider, SIGNAL(sliderMoved(int)), this, SLOT(updateProgress(int)));
    connect(ui->progressSlider, SIGNAL(valueChanged(int)), this, SLOT(updateProgress(int)));
    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(updateVolume(int)));
    //libvlc_event_manager_t *emp=libvlc_media_player_event_manager(mp);
    //ui->gridLayout->setGeometry(QRect(0,0,this->width(),this->height()));
    //libvlc_event_attach(emp,libvlc_MediaPlayerPlaying,updateSize,this);

    connect(ui->action_Increase_Volume, SIGNAL(triggered()), this, SLOT(actionIncreaseVolume()));
    connect(ui->action_Decrease_Volume, SIGNAL(triggered()), this, SLOT(actionDecreaseVolume()));
    connect(ui->action_Mute, SIGNAL(triggered()), this, SLOT(actionMute()));
}

MagicPlayer::~MagicPlayer()
{
    libvlc_media_player_release(mp);  /* Free the media_player */

    libvlc_release(inst);
    delete ui;
}

void MagicPlayer::resizeEvent(QResizeEvent *event)
{
    (void) event;
    //ui->gridLayout->setGeometry(QRect(0,0,this->width(),this->height()));
    //event->accept();

    return;
}

void MagicPlayer::videoPlay()
{
    if(mp != NULL) {
        if(getplayStatus() == 1) {
            libvlc_media_player_play (mp);
            setplayStatus(0);
            //wait for video loading ,or the resize will fail
            QTimer::singleShot(100,this,SLOT(windowResize()));
        } else {
            libvlc_media_player_pause (mp);
            setplayStatus(1);
        }
    }

    return;
}

void MagicPlayer::setplayStatus(int t)
{
    playStatus=t;
}

int MagicPlayer::getplayStatus()
{
    return playStatus;
}

//void MagicPlayer::updateSize(const libvlc_event_t *event,void *data)
//{
//    if(event==1)

//}

void MagicPlayer::windowResize()
{
    if(mp != NULL) {
        unsigned int width,height;
        libvlc_video_get_size(mp,0,&width,&height);
        this->resize(width,height);
    }

    return;
}

void MagicPlayer::updateProgress(int pos)
{
    if((mp != NULL) && (libvlc_media_player_is_playing(mp) == 1)) {
        float num=(float)pos/(float)ui->progressSlider->maximum();
        libvlc_media_player_set_position(mp,num);
    }

    return;
}

void MagicPlayer::updateVolume(int pos)
{
    if((mp != NULL) && (libvlc_media_player_is_playing(mp) == 1)) {
        int num=pos;
        libvlc_audio_set_volume(mp,num);
    }

    return;
}

void MagicPlayer::updateInterface()
{
    if((mp != NULL) && (libvlc_media_player_is_playing(mp) == 1)) {
        int videoPosition = libvlc_media_player_get_position(mp)*ui->progressSlider->maximum();
        int volumePosition = libvlc_audio_get_volume(mp);

        //ignore the valueChanged signal
        bool oldState=ui->progressSlider->blockSignals(true);
        ui->progressSlider->setValue(videoPosition);
        //reset the signal state
        ui->progressSlider->blockSignals(oldState);

        ui->volumeSlider->setValue(volumePosition);

        updatestartTime();
        updateendTime();
    }

    return;
}

void MagicPlayer::updatestartTime()
{
    if(mp != NULL) {
        QTime time;
        time = time.addMSecs(libvlc_media_player_get_time(mp));
        ui->startTimeLabel->setText(time.toString("hh:mm:ss"));
    }

    return;
}

void MagicPlayer::updateendTime()
{
    if(mp != NULL) {
        QTime time;
        time = time.addMSecs(libvlc_media_player_get_length(mp)-libvlc_media_player_get_time(mp));
        ui->endTimeLabel->setText(time.toString("hh:mm:ss"));
    }

    return;
}

void MagicPlayer::actionIncreaseVolume()
{
    int value = ui->volumeSlider->value();

    if(value < ui->volumeSlider->maximum()) {
        ui->volumeSlider->setValue(++value);
    }

    return;
}

void MagicPlayer::actionDecreaseVolume()
{
    int value = ui->volumeSlider->value();

    if(value > 1) {
        ui->volumeSlider->setValue(--value);
    }

    return;
}

void MagicPlayer::actionMute()
{
    ui->volumeSlider->setValue(ui->volumeSlider->minimum());

    return;
}

void MagicPlayer::actionForward()
{
    int value = ui->progressSlider->value();

    if(value + ui->progressSlider->pageStep() <= ui->progressSlider->maximum()) {
        ui->progressSlider->setValue(ui->progressSlider->pageStep() + value);
    }

    return;
}

void MagicPlayer::actionBackward()
{
    int value = ui->progressSlider->value();

    if(value-ui->progressSlider->pageStep() >= 0) {
        ui->progressSlider->setValue(value-ui->progressSlider->pageStep());

    }

    return;
}

void MagicPlayer::actionSpecified()
{
    if(mp != NULL) {
        bool ok;
        int maxMinute = (int)(libvlc_media_player_get_length(mp)/(60*1000));
        int minute=QInputDialog::getInt(this,
                                        tr("Input Time"), //dialog title
                                        tr("Please Specified the minutes you want to jump:\n(Max Value:")+QString::number(maxMinute)+tr(" min)"),
                                        0, //default value
                                        0, //min value
                                        maxMinute, //max value
                                        1, //step value
                                        &ok);
        if(ok) {
            ui->progressSlider->setValue((float)minute/maxMinute*ui->progressSlider->maximum());
        }
    }

    return;
}

void MagicPlayer::actionOnTop(bool t)
{
    if(t) {
        setWindowFlags(Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(windowFlags()^Qt::WindowStaysOnTopHint);
    }

    this->show();

    return;
}

void MagicPlayer::actionFullscreen(bool t)
{
    if(t) {
        setWindowState(Qt::WindowFullScreen);
    } else {
        setWindowState(windowState()^Qt::WindowFullScreen);
    }

    this->show();

    return;
}

void MagicPlayer::actionFitWindow(bool t)
{
    (void) t;

    return;
}

void MagicPlayer::actionAbout()
{
    QString text;
    QFile fin("about.html");

    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        text=tr("<h2>MagicPlayer</h2><br />Version 1.0");
    } else {
        text=QTextStream(&fin).readAll();
    }

    fin.close();

    QMessageBox::about(NULL, tr("About"), text);

    return;
}

void MagicPlayer::actionOpen()
{
    if(mp!=NULL) {
        videoStop();
    }

    QString path=QFileDialog::getOpenFileName(this,tr("Open File"),".",tr("Video Files(*.avi *.mkv *.mp4 *.ogg)"));
    if(path.length()!=0) {
        // Create a new item
        libvlc_media_t *m = libvlc_media_new_path (inst, path.toStdString().c_str());
        // Create a media player playing environement
        mp = libvlc_media_player_new_from_media (m);
        // No need to keep the media now
        libvlc_media_release (m);
        player_initialize(mp);
        videoPlay();
    }

    return;
}

int MagicPlayer::player_initialize(libvlc_media_player_t *_mp)
{
    /* Get our media instance to use our window */
#if defined(Q_OS_WIN)
    libvlc_media_player_set_drawable(_mp,ui->videoFrame->winId());
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_drawable(_mp,ui->videoFrame->winId());
#else //Linux
    libvlc_media_player_set_xwindow (_mp,ui->videoFrame->winId());
#endif
    timer->start(100);

    return 0;
}

void MagicPlayer::videoStop()
{
    if(mp != NULL) {
        libvlc_media_player_release(mp);
        setplayStatus(1);
        mp = NULL;
    }

    return;
}


