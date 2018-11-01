/*
 *      Copyright (C) 2018-now Arthur Liberman
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <string>
#include <ctime>

#include "ArchiveConfig.h"
#include "iptv/utilities/math.h"

using namespace ADDON;

void CArchiveConfig::ReadSettings(CHelper_libXBMC_addon *XBMC)
{
  XBMC->Log(LOG_DEBUG, "%s", __FUNCTION__);
    m_XBMC = XBMC;
    if (!XBMC->GetSetting("archEnable", &m_bIsEnabled))
    {
        m_bIsEnabled = false;
    }
    char buffer[1024];
    if (XBMC->GetSetting("archUrlFormat", &buffer))
    {
        m_sUrlFormat = std::string(buffer);
    }
    else
    {
        m_bIsEnabled = false;
    }
    int temp = 0;
    if (XBMC->GetSetting("archTimeshiftBuffer", &temp))
    {
        m_tTimeshiftBuffer = iptv::days_to_seconds(++temp);
    }
    else
    {
        m_tTimeshiftBuffer = iptv::days_to_seconds(1);
    }
    if (XBMC->GetSetting("archBeginBuffer", &temp))
    {
        m_tEpgBeginBuffer = GetEpgBufferFromSettings(temp);
    }
    else
    {
        m_tEpgBeginBuffer = iptv::minutes_to_seconds(5);
    }
    if (XBMC->GetSetting("archEndBuffer", &temp))
    {
        m_tEpgEndBuffer = GetEpgBufferFromSettings(temp);
    }
    else
    {
        m_tEpgEndBuffer = iptv::minutes_to_seconds(15);
    }
    if (!XBMC->GetSetting("archPlayEpgAsLive", &m_bPlayEpgAsLive))
    {
        m_bPlayEpgAsLive = false;
    }
}

time_t CArchiveConfig::GetEpgBufferFromSettings(int setting) const
{
    time_t minutes = 0;
    switch (setting)
    {
        case 1:
            minutes = 2;
        break;
        case 2:
            minutes = 5;
        break;
        case 3:
            minutes = 10;
        break;
        case 4:
            minutes = 15;
        break;
        case 5:
            minutes = 30;
        break;
        case 6:
            minutes = 60;
        break;
    }
    return minutes * 60;
}
