#pragma once

#include "../GenericNetMessage.h"

class Notify_4C : public GenericMessage {
private:

public:
	Notify_4C();
	ByteBuffer &Serialize() override;
};