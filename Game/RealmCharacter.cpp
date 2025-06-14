#include "RealmCharacter.h"

#include "../../logging.h"

RealmCharacter::RealmCharacter( std::vector< uint8_t > &data )
{
	ByteBuffer stream( data );

	m_metaData.Deserialize( stream );

	auto characterDataSize = stream.read_u32();
	if( characterDataSize > 0 && characterDataSize < 1024 )
	{
		m_characterData = stream.read_bytes( characterDataSize );
	}
}

RealmCharacter::RealmCharacter(sptr_byte_stream stream)
{
    m_metaData.Deserialize(*stream);

    auto characterDataSize = stream->read_u32();
    if (characterDataSize > 0 && characterDataSize < 1024)
	{
		m_characterData = stream->read_bytes(characterDataSize);
	}
}

const std::vector< uint8_t > RealmCharacter::GetCharacterData() const
{
	return m_characterData;
}

void RealmCharacter::SetCharacterData( const std::vector<uint8_t> &data )
{
	m_characterData = data;
}

const std::vector< uint8_t > RealmCharacter::GetInventoryData() const
{
	return m_inventoryData;
}

void RealmCharacter::SetInventoryData( const std::vector<uint8_t> &data )
{
	m_inventoryData = data;
}

RealmCharacterMetaData RealmCharacter::GetMetaData() const
{
	return m_metaData;
}

void RealmCharacter::SetMetaData( const std::vector<uint8_t> &data )
{
	ByteBuffer stream( data );
	m_metaData.Deserialize( stream );
}

const std::vector<uint8_t> RealmCharacter::Unpack()
{
    std::vector<uint8_t> output;
    const int expectedOutputSize = 19504;
    const size_t inputSize = m_characterData.size();
    const uint8_t* input = m_characterData.data();
    output.reserve(expectedOutputSize);

    try
    {
        size_t i = 0;
        while (i < inputSize && output.size() < expectedOutputSize) {
            uint8_t byte = input[i++];
            if (byte != 0x00) {
                output.push_back(byte);
            }
            else {
                if (i >= inputSize) {
                    throw std::runtime_error("Invalid blob format: zero-byte with no length");
                }
                uint8_t next = input[i++];
                if (next == 0x00) {
                    // Escaped literal 0x00 byte
                    output.push_back(0x00);
                }
                else {
                    // Insert `next` zero bytes
                    output.insert(output.end(), next, 0x00);
                }
            }
        }
    }
    catch (const std::exception& e)
	{
		Log::Error("Error unpacking character data: " + std::string(e.what()));
	}

	// Ensure we have exactly 19504 bytes
	if (output.size() != expectedOutputSize)
	{
		Log::Error("Unpacked character data size mismatch: expected " + std::to_string(expectedOutputSize) + ", got " + std::to_string(output.size()));
	}

    return output;
}

std::vector<uint8_t> RealmCharacter::Pack(const std::vector<uint8_t>& input)
{
    constexpr size_t ExpectedInputSize = 19504;
    if (input.size() != ExpectedInputSize)
    {
        throw std::runtime_error("Input to compressor must be exactly 19504 bytes");
    }

    std::vector<uint8_t> output;

    // Optional: add 4-byte header if game expects it (match decompress behavior)
    output.insert(output.end(), { 0x00, 0x00, 0x00, 0x00 }); // Placeholder header

    size_t i = 0;
    while (i < input.size())
    {
        uint8_t byte = input[i];
        if (byte != 0x00)
        {
            // Literal non-zero byte, just write it
            output.push_back(byte);
            ++i;
        }
        else
        {
            // Zero run: count how many consecutive 0x00s (max 255)
            size_t zeroCount = 0;
            while (i < input.size() && input[i] == 0x00 && zeroCount < 255)
            {
                ++zeroCount;
                ++i;
            }

            output.push_back(0x00);               // Zero marker
            output.push_back(static_cast<uint8_t>(zeroCount)); // Count
        }
    }

    return output;
}