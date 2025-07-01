#pragma once

#include "../GenericNetMessage.h"

class NotifyForcedLogout : public GenericMessage {
private:

public:
	NotifyForcedLogout();
	void Serialize( ByteBuffer &out ) const override;
};