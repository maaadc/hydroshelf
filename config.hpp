
#ifndef HYDROSHELF_CONFIG_HPP
#define HYDROSHELF_CONFIG_HPP

namespace config
{
constexpr char wifiHostname[] = "hydroshelf";
constexpr char wifiNetwork[]  = "abc";
constexpr char wifiPassword[] = "def";

// Europe/Berlin, see
// https://remotemonitoringsystems.ca/time-zone-abbreviations.php
constexpr char timeZone[]          = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";
constexpr char ntpServer[]         = "pool.ntp.org";
constexpr int  ntpUpdateInterval_s = 6 * 3600;

}  // namespace config

#endif  // HYDROSHELF_CONFIG_HPP