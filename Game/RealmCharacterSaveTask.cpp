#include "CharacterSaveTask.h"
#include "RealmCharacter.h"

#include "../Common/ByteBufferReader.hpp"
#include "../../logging.h"

CharacterSaveTask::CharacterSaveTask( CharacterSaveType Type, uint32_t characterId )
{
	m_saveType = Type;
	m_ownerUser.reset();
	m_targetUser.reset();

	m_characterId = characterId;
	m_writePosition = 0;

	m_data.reserve(4096);
}

CharacterSaveTask::~CharacterSaveTask()
{
	m_ownerUser.reset();
	m_targetUser.reset();
	m_data.clear();
}

void CharacterSaveTask::SetMetaData( const CharacterSlotData &metaData )
{
	this->m_meta = metaData;
}

bool CharacterSaveTask::AppendData( const std::vector< uint8_t > &data )
{
	if( data.empty() )
	{
		Log::Error( "AppendTailData called with empty data!" );
		return false;
	}

	m_data.insert( m_data.end(), data.begin(), data.end() );

	return true;
}

bool CharacterSaveTask::Validate()
{
	if( m_data.empty() )
	{
		Log::Error( "EndUpdate called with empty pending data!" );
		return false;
	}

	if( m_meta.empty() )
	{
		Log::Error( "EndUpdate called with empty metadata!" );
		return false;
	}

	// Verify the size of the decompressed buffer
	size_t read = 4;
	size_t decompressedSize = 0;
	size_t lastUsedByte = read;

	while( read < m_data.size() )
	{
		uint8_t byte = m_data[ read++ ];
		decompressedSize += 1;
		lastUsedByte = read;

		if( byte == 0x00 )
		{
			if( read >= m_data.size() )
			{
				Log::Error( "Unexpected end of data during decompression" );
				return false;
			}

			uint8_t count = m_data[ read++ ];
			decompressedSize += count;
			lastUsedByte = read;
		}

		if( decompressedSize >= CHARACTER_DATA_SIZE )
		{
			break;
		}
	}

	if( decompressedSize < CHARACTER_DATA_SIZE )
	{
		Log::Error( "Failed to decompress pending character data. More data needed!" );
		return false;
	}

	// TODO: Better error checking.

	// Trim Garbage from the compressed buffer.
	m_data.resize( lastUsedByte );

	return true;
}

const std::vector<uint8_t> &CharacterSaveTask::GetData() const
{
	return m_data;
}