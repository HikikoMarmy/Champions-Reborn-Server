#include "Notify_4C.h"

Notify_4C::Notify_4C() : GenericMessage( 0x4C )
{
}

void Notify_4C::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	out.write_u32( 0 ); // Unknown

	out.write_u32( 1 ); // Unknown
	{
		out.write_utf16( L"Dummy 1" );
	}

	out.write_u32( 1 ); // Unknown
	{
		out.write_u32( 0 );
		// Blob
	}

	out.write_u32( 1 ); // Unknown
	{
		out.write_u32( 0 );
	}

	out.write_u16( 0 ); // Unknown

	out.write_u32( 1 ); // Unknown
	{
		out.write_u16( 0 );
		out.write_u32( 0 );
		out.write_utf16( L"Dummy 2" );
		out.write_u32( 0 );
		out.write_u32( 0 );
		out.write_u32( 0 );
		out.write_u16( 1 );
	}

	out.write_u32( 0 ); // Unknown

	out.write_utf8( "127.0.0.1" );

	out.write_u32( 0 ); // Unknown
	out.write_u32( 0 ); // Unknown
}
