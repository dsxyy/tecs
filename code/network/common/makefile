#include ./makeconfig

include $(_NETWORK_COMMON_EVENT_MAKEFILE)
include $(_NETWORK_COMMON_ALARM_MAKEFILE)
include $(_NETWORK_COMMON_VNET_MAKEFILE)
include $(_NETWORK_COMMON_LOG_MAKEFILE)

#$(warning $(_NETWORK_COMMON_EVENT_MAKEFILE))
#$(warning $(_NETWORK_COMMON_ALARM_MAKEFILE))
#$(warning $(_NETWORK_COMMON_LOG_MAKEFILE))
#$(warning $(_NETWORK_COMMON_OBJS))
#$(warning $(_NETWORK_COMMON_INCLUDE_PATH))

networkcommonobj: $(_NETWORK_COMMON_OBJS)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) network common  has been done!
	$(ECHO) -----------------------------------------------------------------
