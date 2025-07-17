#pragma once

#include "../GenericNetMessage.h"

class NotifyInstantMessage : public GenericMessage {
private:
	std::wstring m_message;
	std::wstring m_chatHandle;

public:
	NotifyInstantMessage( std::wstring chatHandle, std::wstring message );
	void Serialize(ByteBuffer &out) const override;
};