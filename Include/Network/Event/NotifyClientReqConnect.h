#pragma once

#include <string>

#include "../GenericNetMessage.h"

class NotifyClientRequestConnect : public GenericMessage
{
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientRequestConnect( std::string clientIp, int32_t clientPort );
	ByteBuffer &Serialize() override;
};