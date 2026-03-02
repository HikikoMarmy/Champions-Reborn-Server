#pragma once

#include <string>

#include "../GenericNetMessage.hpp"
#include "Common/ForwardDecl.hpp"

class NotifyClientDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientDiscovered( sptr_user user );
	void Serialize( ByteBuffer &out ) const override;
};