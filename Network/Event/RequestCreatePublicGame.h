#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestCreatePublicGame : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_gameInfo;
	std::wstring m_gameName;
	std::wstring m_stageName;

	int32_t m_minimumLevel;
	int32_t m_maximumLevel;

	enum ERROR_CODE {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
		GAME_NAME_IN_USE = 38,
		GAME_PENDING = 40,
	};

public:
	static std::unique_ptr< RequestCreatePublicGame > Create()
	{
		return std::make_unique< RequestCreatePublicGame >();
	}
	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;

	std::tuple< std::wstring, std::wstring > ParseNameAndStage( const std::wstring &gameInfo );
};

class ResultCreatePublicGame : public GenericResponse {
private:
	int32_t m_reply;
	std::string m_discoveryIp;
	int32_t m_discoveryPort;

public:
	ResultCreatePublicGame( GenericRequest *request, int32_t reply, std::string discoveryIp = "", int32_t discoveryPort = 0 );
	void Serialize( ByteBuffer &out ) const;
};