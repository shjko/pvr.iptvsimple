#include "ChannelStreamReader.h"
#include "utilities/math.h"
#include "utilities/CatchupUrlFormatter.h"
#include "../client.h"

using namespace ADDON;
using namespace iptv;

ChannelStreamReader::ChannelStreamReader(const PVRIptvChannel &channel, const unsigned int readTimeout)
{
  XBMC->Log(LOG_DEBUG, "%s: Create ChannelStreamReader", __FUNCTION__);
  m_channel = channel;
  m_readTimeout = readTimeout;
  m_config.ReadSettings(XBMC);
}

ChannelStreamReader::~ChannelStreamReader(void)
{
  Close();
}

void ChannelStreamReader::Create(const std::string &streamURL)
{
  Close();
  m_streamHandle = XBMC->CURLCreate(streamURL.c_str());
  if (m_readTimeout > 0)
    XBMC->CURLAddOption(m_streamHandle, XFILE::CURL_OPTION_PROTOCOL,
      "connection-timeout", std::to_string(0).c_str());

  XBMC->Log(LOG_DEBUG, "ChannelStreamReader: Started; url=%s", streamURL.c_str());
}

void ChannelStreamReader::Close(void)
{
  if (m_streamHandle)
  {
    XBMC->CloseFile(m_streamHandle);
    m_streamHandle = nullptr;
    m_timeshiftOffset = 0;
    XBMC->Log(LOG_DEBUG, "ChannelStreamReader: Stopped");
  }
}

bool ChannelStreamReader::Start(time_t timestamp)
{
  XBMC->Log(LOG_DEBUG, "%s", __FUNCTION__);
  const time_t now = time(nullptr);
  time_t timeshiftBuffer = 0;

  if (IsTimeshiftSupported())
  {
    if (m_channel.iCatchupDays > 0)
      timeshiftBuffer = days_to_seconds(m_channel.iCatchupDays);
    else if (m_config.GetTimeshiftBuffer() > -1)
      timeshiftBuffer = m_config.GetTimeshiftBuffer();
    m_channel.etEpgTag.startTime = now - timeshiftBuffer;
    m_channel.etEpgTag.endTime = now;
    m_channel.tTimeshiftStartTime = -1;
    if (timestamp == 0)
      timestamp = now;
    if (timestamp >= m_channel.etEpgTag.startTime && timestamp <= m_channel.etEpgTag.endTime)
      m_channel.tTimeshiftStartTime = timestamp;
    if (m_channel.tTimeshiftStartTime > 0)
    {
      m_isTimeshifting = timestamp != now;
      m_timeshiftOffset = timestamp - m_channel.etEpgTag.startTime;
      if (m_isTimeshifting)
        Create(CatchupUrlFormatter::FormatCatchupUrl(m_channel));
      else
        Create(m_channel.strStreamURL);
    }
    else
    {
      Close();
    }
  }
  else
  {
    m_channel.etEpgTag = {0};
    m_timeshiftOffset = 0;
    m_timeshiftOffset = 0;
    m_isTimeshifting = false;
    Create(m_channel.strStreamURL);
  }

  return m_streamHandle ? XBMC->CURLOpen(m_streamHandle, XFILE::READ_NO_CACHE) : false;
}

ssize_t ChannelStreamReader::ReadData(unsigned char *buffer, unsigned int size)
{
  return XBMC->ReadFile(m_streamHandle, buffer, size);
}

int64_t ChannelStreamReader::Seek(long long position, int whence)
{
  if (whence == 2)
  {
    return 1;
  }
  else if (m_firstSeek && position == 0)
  {
    m_firstSeek = false;
    return m_timeshiftOffset;
  }
  else
  {
    const time_t now = time(nullptr);
    position += 500;
    position /= 1000;
    switch (whence)
    {
      case SEEK_SET:
        if (m_channel.etEpgTag.startTime + position < now - 10)
        {
          m_timeshiftOffset = position;
          m_isTimeshifting = true;
        }
        else
        {
          m_timeshiftOffset = now - m_channel.etEpgTag.startTime;
          m_isTimeshifting = false;
        }
        break;
      case SEEK_CUR:
      case SEEK_END:
      default:
        XBMC->Log(LOG_NOTICE, "ChannelStreamReader::Seek - Unsupported SEEK command (%d)", whence);
        return -1;
        break;
    }
    return m_timeshiftOffset * DVD_TIME_BASE;
  }
}

int64_t ChannelStreamReader::Position()
{
  return XBMC->GetFilePosition(m_streamHandle) + m_timeshiftOffset;
}

int64_t ChannelStreamReader::Length()
{
  return IsTimeshiftSupported() ? m_channel.etEpgTag.endTime - m_channel.etEpgTag.startTime : -1;
}

std::time_t ChannelStreamReader::TimeStart()
{
  return m_channel.etEpgTag.startTime;
}

std::time_t ChannelStreamReader::TimeEnd()
{
  m_channel.etEpgTag.endTime = time(nullptr);
  return m_channel.etEpgTag.endTime;
}

bool ChannelStreamReader::IsRealTime()
{
  return true;
}

bool ChannelStreamReader::IsTimeshifting()
{
  return m_isTimeshifting;
}

bool ChannelStreamReader::IsArchiveSupportedOnChannel(const PVRIptvChannel &channel)
{
  return !channel.strCatchupSource.empty();
}
