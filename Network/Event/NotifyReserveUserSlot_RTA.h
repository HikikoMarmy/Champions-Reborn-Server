#pragma once

#include "../GenericNetMessage.h"

class NotifyReserveUserSlot_RTA : public GenericMessage {
private:
	std::string m_discoveryAddr;
	int32_t m_port;

public:
	NotifyReserveUserSlot_RTA( std::string discoveryAddr, int32_t discoveryPort );
	ByteBuffer &Serialize() override;
};