#include "RealmSocket.h"
#include "../Common/Utility.h"

RealmSocket::RealmSocket()
{
	fd = INVALID_SOCKET;

	std::memset( &local_addr, 0, sizeof( local_addr ) );
	std::memset( &remote_addr, 0, sizeof( remote_addr ) );

	remote_ip = "";
	remote_port = 0;

	flag.disconnected_wait = 0;
	flag.disconnected_forced = 0;
	flag.is_listener = 0;
	flag.want_more_read_data = 0;
	flag.want_more_write_data = 0;

	last_write_position = 0;

	last_recv_time = std::chrono::steady_clock::now();
	last_send_time = std::chrono::steady_clock::now();

	latency = 0;
}

RealmSocket::~RealmSocket()
{
	if( INVALID_SOCKET != fd )
	{
		closesocket( fd );
	}

	fd = INVALID_SOCKET;

	std::memset( &local_addr, 0, sizeof( local_addr ) );
	std::memset( &remote_addr, 0, sizeof( remote_addr ) );

	remote_ip = "";
	remote_port = 0;

	flag.disconnected_wait = 0;
	flag.disconnected_forced = 0;
	flag.is_listener = 0;
	flag.want_more_read_data = 0;
	flag.want_more_write_data = 0;

	last_write_position = 0;

	latency = 0;

	m_pendingWriteBuffer.reserve( WRITE_BUFFER_SIZE );
}

void RealmSocket::send( const sptr_generic_response response )
{
	ByteBuffer stream;
	response->Serialize( stream );
	const auto netSize = Util::ByteSwap( static_cast< uint32_t >( stream.get_position() ) + 4 );

	std::lock_guard< std::mutex > lock( write_mutex );

	m_pendingWriteBuffer.insert( m_pendingWriteBuffer.end(), ( uint8_t * )&netSize, ( uint8_t * )&netSize + 4 );
	m_pendingWriteBuffer.insert( m_pendingWriteBuffer.end(), stream.m_buffer.begin(), stream.m_buffer.end() );
}

void RealmSocket::send( const GenericMessage &message )
{
	ByteBuffer stream;
	message.Serialize( stream );
	const auto netSize = Util::ByteSwap( static_cast< uint32_t >( stream.get_position() ) + 4 );

	std::lock_guard< std::mutex > lock( write_mutex );

	m_pendingWriteBuffer.insert( m_pendingWriteBuffer.end(), ( uint8_t * )&netSize, ( uint8_t * )&netSize + 4 );
	m_pendingWriteBuffer.insert( m_pendingWriteBuffer.end(), stream.m_buffer.begin(), stream.m_buffer.end() );
}