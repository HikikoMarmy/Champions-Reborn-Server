#pragma once

#include "../GenericNetMessage.h"

class NotifyFriendStatus : public GenericMessage {
private:
	std::wstring m_handle;
	bool m_status;

public:
	NotifyFriendStatus( std::wstring handle, bool status );
	void Serialize( ByteBuffer &out ) const override;
};