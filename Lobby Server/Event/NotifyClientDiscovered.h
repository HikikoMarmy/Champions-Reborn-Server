#pragma once

class NotifyClientDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientDiscovered( std::string clientIp, int32_t clientPort );
	ByteStream &Serialize() override;
};