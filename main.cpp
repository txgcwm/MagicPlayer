#include <QtGui/QApplication>
#include "magicplayer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MagicPlayer w;
    w.show();
    
    return a.exec();
}
