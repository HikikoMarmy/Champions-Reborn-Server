#include "../../global_define.h"
#include "Notify_4C.h"

Notify_4C::Notify_4C() : GenericMessage( 0x4C )
{
}

ByteStream &Notify_4C::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	m_stream.write_u32( 0 ); // Unknown

	m_stream.write_u32( 1 ); // Unknown
	{
		m_stream.write_utf16( L"Dummy 1" );
	}

	m_stream.write_u32( 1 ); // Unknown
	{
		m_stream.write_u32( 0 );
		// Blob
	}

	m_stream.write_u32( 1 ); // Unknown
	{
		m_stream.write_u32( 0 );
	}

	m_stream.write_u16( 0 ); // Unknown

	m_stream.write_u32( 1 ); // Unknown
	{
		m_stream.write_u16( 0 );
		m_stream.write_u32( 0 );
		m_stream.write_utf16( L"Dummy 2" );
		m_stream.write_u32( 0 );
		m_stream.write_u32( 0 );
		m_stream.write_u32( 0 );
		m_stream.write_u16( 1 );
	}

	m_stream.write_u32( 0 ); // Unknown

	m_stream.write_utf8( "127.0.0.1" );

	m_stream.write_u32( 0 ); // Unknown
	m_stream.write_u32( 0 ); // Unknown

	return m_stream;
}
