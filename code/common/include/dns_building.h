#ifndef DNS_BUILDING_H
#define DNS_BUILDING_H

namespace zte_tecs{

/***************************************************
 * �������ɹ���: [m/s].component.subsystem.system
 * m/s ֧������������
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
