#pragma once

#include "IStreamReader.h"
#include "../PVRIptvData.h"
#include "../ArchiveConfig.h"

namespace iptv
{
  class ChannelStreamReader : public IStreamReader
  {
    public:
      ChannelStreamReader(const PVRIptvChannel &channel, const unsigned int readTimeout = 0);
      ~ChannelStreamReader(void);

      bool Start(time_t timestamp = 0) override;
      ssize_t ReadData(unsigned char *buffer, unsigned int size) override;
      int64_t Seek(long long position, int whence) override;
      int64_t Position() override;
      int64_t Length() override;
      std::time_t TimeStart() override;
      std::time_t TimeEnd() override;
      bool IsRealTime() override;
      bool IsTimeshifting() override;
      inline bool IsTimeshiftSupported() override { return IsArchiveSupportedOnChannel(m_channel); }

      static bool IsArchiveSupportedOnChannel(const PVRIptvChannel &channel);

    private:
      void Create(const std::string &streamURL);
      void Close();

      PVRIptvChannel m_channel;
      CArchiveConfig m_config;
      void *m_streamHandle = nullptr;
      unsigned int m_readTimeout = 0;
      int64_t m_timeshiftOffset = 0;
      bool m_firstSeek = false;
      bool m_isTimeshifting = false;
  };
}
