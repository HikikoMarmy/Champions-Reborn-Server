#pragma once

#include "../GenericNetMessage.hpp"

class NotifyForcedLogout : public GenericMessage {
private:

public:
	NotifyForcedLogout();
	void Serialize( ByteBuffer &out ) const override;
};