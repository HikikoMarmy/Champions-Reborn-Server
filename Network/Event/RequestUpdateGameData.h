#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

#include "../../Game/GameSession.h"

class RequestUpdateGameData : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::string m_gameData;

public:
	static std::unique_ptr< RequestUpdateGameData > Create()
	{
		return std::make_unique< RequestUpdateGameData >();
	}
	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
	bool ParseGameData( sptr_game_session session );
};

class ResultUpdateGameData : public GenericResponse {
public:
	ResultUpdateGameData( GenericRequest *request );
	void Serialize( ByteBuffer &out ) const;
};