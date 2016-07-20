
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdlib.h>

#include "log.h"
#include "ztedialmanager.h"

int main(int argc, char** argv)
/***********************************/
{
    int r = 0;

#if DEBUG
    free( malloc( 1 ) ); // for electric fence
#endif

    Log::Instantiate("console", LOG_PRIO_INFO, "ztedial");

    Log::Info("Ztedial started");

    ZteDialManager* manager = new ZteDialManager();
    r = manager->main_loop_run();
    delete manager;

    Log::Dispose();

    Log::Info("Ztedial exited");

    return r;
}
