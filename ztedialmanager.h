
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ZTEDIALMANAGER_H
#define __ZTEDIALMANAGER_H

class ZteDialManager
{
private:
    void            scan_device();

public:
                    ZteDialManager();
    virtual         ~ZteDialManager();

    int             main_loop_run();
};

#endif
