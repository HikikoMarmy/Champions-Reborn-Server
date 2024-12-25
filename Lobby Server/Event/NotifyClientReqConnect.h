#pragma once

class NotifyClientRequestConnect : public GenericMessage
{
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientRequestConnect( std::string clientIp, int32_t clientPort );
	ByteStream &Serialize() override;
};