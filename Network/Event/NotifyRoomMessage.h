#pragma once

#include "../GenericNetMessage.h"

class NotifyRoomMessage : public GenericMessage {
private:
	std::wstring m_message;
	std::wstring m_chatHandle;
	std::wstring m_roomName;

public:
	NotifyRoomMessage( std::wstring roomName, std::wstring chatHandle, std::wstring message );
	void Serialize(ByteBuffer &out) const override;
};