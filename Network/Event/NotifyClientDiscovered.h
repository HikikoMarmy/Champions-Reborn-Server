#pragma once

#include <string>

#include "../GenericNetMessage.h"
#include "../../Common/Constant.h"

class NotifyClientDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientDiscovered( std::string clientIp, int32_t clientPort );
	ByteBuffer &Serialize() override;
};