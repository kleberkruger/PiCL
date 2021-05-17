#ifndef EPOCH_MANAGER_H
#define EPOCH_MANAGER_H

#include "fixed_types.h"

class EpochManager
{
public:
    EpochManager();
    ~EpochManager();

    void start();

    UInt64 getSystemEID() const { return m_system_eid; }
    UInt64 getPersistedEID() const { return 0; }
    UInt64 getTaggedEID() const { return 0; }

    static UInt64 getGlobalSystemEID();

private:
    static SInt64 __increment(UInt64 arg, UInt64 val)
    {
        ((EpochManager *)arg)->increment(val);
        return 0;
    }
    void increment(UInt64 time);

    UInt64 m_system_eid;
};

#endif
