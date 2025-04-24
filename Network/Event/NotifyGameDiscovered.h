#pragma once

class NotifyGameDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;
	RealmClientType m_clientType;

public:
	NotifyGameDiscovered( std::string clientIp, int32_t clientPort, RealmClientType clientType );
	ByteStream &Serialize() override;
};