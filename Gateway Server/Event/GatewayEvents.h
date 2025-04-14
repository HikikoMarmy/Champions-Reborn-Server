#pragma once

#include <map>
#include <functional>
#include <memory>

#include "../../Network/GenericNetRequest.hpp"
#include "GetServerAddressEvent.h"

const std::map< int16_t, std::function< std::unique_ptr< GenericRequest >() > > LOBBY_EVENT_LOOKUP =
{
	{ 0x43, []() -> std::unique_ptr< GenericRequest >
		{
		return std::make_unique< RequestGetServerAddress >();
		}
	}
};