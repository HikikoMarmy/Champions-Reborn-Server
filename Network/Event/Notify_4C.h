#pragma once

class Notify_4C : public GenericMessage {
private:

public:
	Notify_4C();
	ByteStream &Serialize() override;
};