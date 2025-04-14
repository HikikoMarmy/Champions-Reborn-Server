#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define FD_SETSIZE 512
#include <WinSock2.h>

#include <cstdint>
#include <memory>

#include "misc/Utility.h"
#include "misc/ByteStream.h"

#include "Crypto/RealmCrypt.h"

#include "Network/RealmSocket.h"

#include "Game/RealmUserManager.h"
#include "Game/GameSessionManager.h"

#include "Network/GenericNetRequest.hpp"
#include "Network/GenericNetResponse.hpp"
#include "Network/GenericNetMessage.hpp"

#include "Gateway Server/GatewayServer.h"
#include "Lobby Server/LobbyServer.h"
#include "Discovery Server/DiscoveryServer.h"

#include "logging.h"
#include "configuration.h"