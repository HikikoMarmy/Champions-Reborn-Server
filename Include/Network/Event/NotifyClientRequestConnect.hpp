#pragma once

#include <string>

#include "../GenericNetMessage.hpp"
#include "Common/ForwardDecl.hpp"

class NotifyClientRequestConnect : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientRequestConnect( sptr_user user );
	void Serialize( ByteBuffer &out ) const override;
};