#pragma once

#include "../misc/Encryptor.h"

class CClient {
public:
	CClient();
	~CClient();

	sptr_socket	socket;

	Encryptor encryptor;
};

typedef std::shared_ptr< CClient >	sptr_client;
