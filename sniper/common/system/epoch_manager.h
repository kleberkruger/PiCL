#ifndef EPOCH_MANAGER_H
#define EPOCH_MANAGER_H

#include "fixed_types.h"

class EpochManager
{
public:
    EpochManager();
    ~EpochManager();

    void start();

private:
    static SInt64 __record(UInt64 arg, UInt64 val)
    {
        ((EpochManager *)arg)->record(val);
        return 0;
    }
    void record(UInt64 time);

    UInt64 m_eid;
};

#endif
