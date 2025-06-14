#include "RequestGetServerAddress.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Common/Constant.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestGetServerAddress::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetServerAddress::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultGetServerAddress >( this, "", 0, RealmGameType::RETURN_TO_ARMS );
	}

	if (user->m_gameType == RealmGameType::RETURN_TO_ARMS)
	{
		return std::make_shared< ResultGetServerAddress >(this, Config::service_ip, Config::rta_lobby_port, user->m_gameType);
	}
	else
	{
		return std::make_shared< ResultGetServerAddress >(this, Config::service_ip, Config::con_lobby_port, user->m_gameType);
	}
}

ResultGetServerAddress::ResultGetServerAddress( GenericRequest *request, std::string ip, int32_t port, RealmGameType clientType ) : GenericResponse( *request )
{
	m_ip = ip;
	m_port = port;
	m_clientType = clientType;
}

ByteBuffer &ResultGetServerAddress::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	if( m_clientType == RealmGameType::RETURN_TO_ARMS )
		m_stream.write_utf8( m_ip );
	else
		m_stream.write_sz_utf8( m_ip );

	m_stream.write( m_port );

	return m_stream;
}
