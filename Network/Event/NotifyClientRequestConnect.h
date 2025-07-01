#pragma once

#include <string>

#include "../GenericNetMessage.h"
#include "../../Common/ForwardDecl.h"

class NotifyClientRequestConnect : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientRequestConnect( sptr_user user );
	void Serialize( ByteBuffer &out ) const override;
};