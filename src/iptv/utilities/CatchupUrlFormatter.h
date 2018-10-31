#pragma once

#include "../../PVRIptvData.h"

namespace iptv
{
  class CatchupUrlFormatter
  {
    public:
      CatchupUrlFormatter() = delete;
      static std::string FormatCatchupUrl(const PVRIptvChannel &channel);

    private:
      static void FormatTime(const char ch, const struct tm *pTime, std::string &fmt);
      static void FormatUtc(const char *str, time_t tTime, std::string &fmt);
  };
}
