#pragma once

#include "../GenericNetMessage.h"

class NotifyForcedLogout : public GenericMessage {
private:

public:
	NotifyForcedLogout();
	ByteBuffer &Serialize() override;
};