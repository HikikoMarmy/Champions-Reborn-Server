#pragma once

#include "../GenericNetMessage.h"

class NotifyClientRequestsConnect2 : public GenericMessage {
private:
	std::string m_discoveryAddr;
	std::string m_localAddr;
	int32_t m_port;

public:
	NotifyClientRequestsConnect2( std::string discoveryAddr, std::string localAddr, int32_t port );
	ByteBuffer &Serialize() override;
};