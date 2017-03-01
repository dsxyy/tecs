#ifndef _SSH_SIGNKEY_H
#define _SSH_SIGNKEY_H
#include "ssh_session.h"

void buf_put_pub_key(buffer* buf, sign_key *key);
void buf_put_sign(buffer* buf, sign_key *key, const unsigned char *data, unsigned int len);
#endif

