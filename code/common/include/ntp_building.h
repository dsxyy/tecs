#ifndef NTP_BUILDING_H
#define NTP_BUILDING_H

namespace zte_tecs{


STATUS SetNtpServerConfig(const string& reg,
                          const string& ip,
                          bool check_local_ip);
STATUS SetNtpClientConfig(const string& reg,
                          const string& ip,
                          bool check_local_ip);
STATUS NtpServiceRestart();
}
#endif
