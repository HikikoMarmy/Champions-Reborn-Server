#pragma once

class RequestDoClientDiscovery : public GenericRequest
{
private:
	enum DISCOVERY_REPLY {
		SUCCESS = 0,
		FATAL_ERROR = 1,
		GAME_FULL = 2,
	};

	std::wstring m_sessionId;
	int32_t m_gameId;

public:
	static std::unique_ptr< RequestDoClientDiscovery > Create()
	{
		return std::make_unique< RequestDoClientDiscovery >();
	}

	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultDoClientDiscovery : public GenericResponse {
private:
	int32_t m_reply;
	std::string m_discoveryIP;
	int32_t m_discoveryPort;

public:
	ResultDoClientDiscovery( GenericRequest *request, int32_t reply, std::string ip = "", int32_t port = 0);
	ByteStream &Serialize();
};