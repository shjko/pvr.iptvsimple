#include <ctime>
#include "CatchupUrlHelper.h"
#include "../../client.h"

using namespace ADDON;
using namespace iptv;

bool CatchupUrlHelper::IsArchiveSupportedOnChannel(const PVRIptvChannel &channel)
{
  return !channel.strCatchupSource.empty();
}

bool CatchupUrlHelper::IsEpgTagPlayable(const EPG_TAG* tag, const PVRIptvChannel &channel)
{
  const time_t now = time(nullptr);
  return (tag && tag->startTime < now && IsArchiveSupportedOnChannel(channel) &&
    tag->startTime >= (now - static_cast<time_t>(channel.iCatchupLength)));
}

std::string CatchupUrlHelper::FormatCatchupUrl(const PVRIptvChannel &channel)
{
  const time_t dateTimeNow = std::time(0);
  struct tm dateTime = {0};
  localtime_r(&channel.tTimeshiftStartTime, &dateTime);
  std::string fmt = channel.strCatchupSource;
  FormatTime('Y', &dateTime, fmt);
  FormatTime('m', &dateTime, fmt);
  FormatTime('d', &dateTime, fmt);
  FormatTime('H', &dateTime, fmt);
  FormatTime('M', &dateTime, fmt);
  FormatTime('S', &dateTime, fmt);
  FormatUtc("{utc}", channel.tTimeshiftStartTime, fmt);
  FormatUtc("${start}", channel.tTimeshiftStartTime, fmt);
  FormatUtc("{lutc}", dateTimeNow, fmt);
  XBMC->Log(LOG_DEBUG, "CatchupUrlHelper::FormatCatchupUrl - \"%s\"", fmt.c_str());
  return fmt;
}

void CatchupUrlHelper::FormatTime(const char ch, const struct tm *pTime, std::string &fmt)
{
  char str[] = { '{', ch, '}', 0 };
  auto pos = fmt.find(str);
  if (pos != std::string::npos)
  {
    char buff[256], timeFmt[3];
    snprintf(timeFmt, sizeof(timeFmt), "%%%c", ch);
    strftime(buff, sizeof(buff), timeFmt, pTime);
    if (strlen(buff) > 0)
      fmt.replace(pos, 3, buff);
  }
}

void CatchupUrlHelper::FormatUtc(const char *str, time_t tTime, std::string &fmt)
{
  auto pos = fmt.find(str);
  if (pos != std::string::npos)
  {
    char buff[256];
    snprintf(buff, sizeof(buff), "%lu", tTime);
    fmt.replace(pos, strlen(str), buff);
  }
}
