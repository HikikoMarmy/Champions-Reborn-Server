#pragma once

#include <string>

#include "../GenericNetMessage.h"
#include "../../Common/ForwardDecl.h"

class NotifyClientDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientDiscovered( sptr_user user );
	void Serialize( ByteBuffer &out ) const override;
};