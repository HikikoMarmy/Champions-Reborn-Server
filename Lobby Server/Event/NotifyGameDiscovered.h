#pragma once

class NotifyGameDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyGameDiscovered( std::string clientIp, int32_t clientPort );
	ByteStream &Serialize() override;
};