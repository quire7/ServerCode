#ifndef _WH_BASE64_H_
#define _WH_BASE64_H_

#include <string>

#include "ServiceCoreHead.h"



	std::string SERVICE_CORE_CLASS base64_encode(unsigned char const*, unsigned int len);
	std::string SERVICE_CORE_CLASS base64_decode(std::string const& s);



#endif

