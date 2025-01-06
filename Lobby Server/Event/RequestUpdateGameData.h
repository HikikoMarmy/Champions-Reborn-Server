#pragma once

class RequestUpdateGameData : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::string m_gameData;
	std::string m_hardwareIp;

public:
	static std::unique_ptr< RequestUpdateGameData > Create()
	{
		return std::make_unique< RequestUpdateGameData >();
	}
	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultUpdateGameData : public GenericResponse {
public:
	ResultUpdateGameData( GenericRequest *request );
	ByteStream &Serialize();
};