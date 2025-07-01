#pragma once

#include <memory>
#include <string>

#include "../GenericNetMessage.h"
#include "../../Common/Constant.h"
#include "../../Common/ForwardDecl.h"

class NotifyGameDiscovered : public GenericMessage {
private:
	std::string m_clientIp;
	int32_t m_clientPort;
	RealmGameType m_clientType;

public:
	NotifyGameDiscovered( sptr_user user );
	void Serialize(ByteBuffer &out) const override;
};