#pragma once

#include <memory>
#include <string>

#include "../GenericNetMessage.h"
#include "../../Common/Constant.h"

class NotifyGameDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;
	RealmGameType m_clientType;

public:
	NotifyGameDiscovered( std::string clientIp, int32_t clientPort, RealmGameType clientType );
	void Serialize(ByteBuffer &out) const override;
};