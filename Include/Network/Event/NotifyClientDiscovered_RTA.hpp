#pragma once

#include <string>

#include "../GenericNetMessage.hpp"
#include "Common/Constant.hpp"
#include "Common/ForwardDecl.hpp"

class NotifyClientDiscovered_RTA : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;

public:
	NotifyClientDiscovered_RTA( sptr_user user );
	void Serialize( ByteBuffer &out ) const override;
};