#pragma once

class NotifyForcedLogout : public GenericMessage {
private:

public:
	NotifyForcedLogout();
	ByteStream &Serialize() override;
};