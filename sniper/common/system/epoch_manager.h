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
    UInt64 getPersistedEID() const { return m_persisted_eid; }
    void setPersistedEID(const UInt64 persisted_eid) { m_persisted_eid = persisted_eid; }

    static UInt64 getGlobalSystemEID();
    static void setGlobalPersistedEID(const UInt64 persisted_eid);

private:
    static SInt64 __increment(UInt64 arg, UInt64 val)
    {
        ((EpochManager *)arg)->increment(val);
        return 0;
    }
    void increment(UInt64 time);

    UInt64 m_system_eid;
    UInt64 m_persisted_eid;
};

#endif
