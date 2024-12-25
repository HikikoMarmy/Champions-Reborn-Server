#pragma once

class RequestCreatePublicGame : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_gameName;

	int32_t m_minimumLevel;
	int32_t m_maximumLevel;

	enum CREATE_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};
public:
	static std::unique_ptr< RequestCreatePublicGame > Create()
	{
		return std::make_unique< RequestCreatePublicGame >();
	}
	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultCreatePublicGame : public GenericResponse {
private:
	int32_t m_reply;
	std::string m_discoveryIp;
	int32_t m_discoveryPort;

public:
	ResultCreatePublicGame( GenericRequest *request, int32_t reply, std::string discoveryIp = "", int32_t discoveryPort = 0 );
	ByteStream &Serialize();
};