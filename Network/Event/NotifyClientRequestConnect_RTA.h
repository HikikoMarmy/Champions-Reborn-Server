#pragma once

#include <memory>
#include <string>

#include "../GenericNetMessage.h"
#include "../../Common/ForwardDecl.h"

class NotifyClientRequestConnect_RTA : public GenericMessage {
private:
	std::string m_localAddr;
	std::string m_remoteAddr;
	int32_t m_localPort;
	int32_t m_remotePort;

public:
	NotifyClientRequestConnect_RTA( sptr_user user );
	void Serialize( ByteBuffer &out ) const override;
};