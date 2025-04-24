#pragma once

class NotifyClientRequestConnect_RTA : public GenericMessage {
private:
	std::string m_localAddr;
	std::string m_remoteAddr;
	int32_t m_port;

public:
	NotifyClientRequestConnect_RTA( std::string localAddr, std::string remoteAddr, int32_t port );
	ByteStream &Serialize() override;
};