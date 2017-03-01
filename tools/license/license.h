#ifndef LICENSE_H        
#define LICENSE_H
#include <iostream>
#include <sstream>
#include <string>
typedef long long int64;

#define LICENSE_OK                      0     //license正常
#define LICENSE_FAIL                    1     //license失败，错误原因未知
#define LICENSE_NOT_READY       2      //license失败，还未初始化
#define LICENSE_EXPIRED             3      //license过期
#define LICENSE_OVERLIMIT         4      //资源数量超过限制
#define LICENSE_UNMATCHED     5      //硬件指纹不匹配
#define LICENSE_ILLEGAL              6      //license的数字签名校验错误，可能被非法篡改

class License
{
public:
    License();
    void Unlimit();
    int Parse(const char* file);
    void Print();

    string customer_name;
    string customer_company;
    string customer_address;
    string customer_phone;
    string customer_email;
    string customer_description;
    int64 max_created_vms;
    int64 max_deployed_vms;
    int64 max_images;
    int64 max_users;
    int64 max_clusters;
    int effective_days;
    time_t expired_date;
    vector<string> hardware_fingerprints;
    vector<string> special_cpus;
    string features;
    string software_name;
    string software_version;
    string software_release;
    string software_description;
    string software_vendor;
    bool init;
};

class RunningStat
{
public:
    RunningStat()
    {
        created_vms = 0;
        deployed_vms = 0;
        images = 0;
        users = 0;
        clusters = 0;
    };
    
    int64 created_vms;
    int64 deployed_vms;
    int64 images;
    int64 users;
    int64 clusters;
    std::string version;
    std::string release;
};

int license_init(const char *lic,const char *sig,std::ostringstream& error);
int license_valid(const RunningStat& s, std::ostringstream& error);
int license_valid(std::ostringstream& error);
const License& license();
void license_print();
int license_days_remained();
string get_hardware_fingerprint();
#endif

