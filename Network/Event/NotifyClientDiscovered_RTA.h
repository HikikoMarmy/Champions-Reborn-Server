#pragma once

#include <string>

#include "../GenericNetMessage.h"
#include "../../Common/Constant.h"

class NotifyClientDiscovered_RTA : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientDiscovered_RTA( std::string clientIp, int32_t clientPort );
	ByteBuffer &Serialize() override;
};