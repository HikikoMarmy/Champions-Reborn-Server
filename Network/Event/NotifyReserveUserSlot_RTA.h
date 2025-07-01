#pragma once

#include "../GenericNetMessage.h"

class NotifyReserveUserSlot_RTA : public GenericMessage {
private:
	std::string m_discoveryAddr;
	int32_t m_port;
	int32_t m_memberId;

public:
	NotifyReserveUserSlot_RTA( int32_t memberId, std::string discoveryAddr, int32_t discoveryPort );
	void Serialize(ByteBuffer &out) const override;
};