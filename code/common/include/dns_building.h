#ifndef DNS_BUILDING_H
#define DNS_BUILDING_H

namespace zte_tecs{

/***************************************************
 * 域名生成规则: [m/s].component.subsystem.system
 * m/s 支持主备后增加
 ***************************************************/
#define DNS_IM_IMAGE_TECS "im.image.tecs"
#define DNS_IA_IMAGE_TECS "ia.image.tecs"

STATUS SetDnsServerConfig(const string& reg,
                          const string& ip,
                          const string& domain_name);
STATUS SetDnsClientConfig(const string& reg,
                          string& ip,
                          const string& character);
STATUS DnsServiceRestart();
}
#endif
