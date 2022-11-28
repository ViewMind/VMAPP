#ifndef WAIT_H
#define WAIT_H

#include <QElapsedTimer>

namespace Wait {

   static void For(qint32 ms){

       QElapsedTimer timer;
       timer.start();
       while (timer.elapsed() < ms);

   }

}

#endif // WAIT_H
