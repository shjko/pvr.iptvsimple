#pragma once

#include <ctime>

#include "libXBMC_addon.h"

namespace iptv
{
  class IStreamReader
  {
  public:
    virtual ~IStreamReader(void) = default;
    virtual bool Start(time_t timestamp = 0) = 0;
    virtual ssize_t ReadData(unsigned char *buffer, unsigned int size) = 0;
    virtual int64_t Seek(long long position, int whence) = 0;
    virtual int64_t Position() = 0;
    virtual int64_t Length() = 0;
    virtual time_t TimeStart() = 0;
    virtual time_t TimeEnd() = 0;
    virtual bool IsRealTime() = 0;
    virtual bool IsTimeshifting() = 0;
    virtual bool IsTimeshiftSupported() = 0;
  };
}
