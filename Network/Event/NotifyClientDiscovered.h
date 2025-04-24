#pragma once

class NotifyClientDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;
	RealmClientType m_clientType;

public:
	NotifyClientDiscovered( std::string clientIp, int32_t clientPort, RealmClientType clientType );
	ByteStream &Serialize() override;
};