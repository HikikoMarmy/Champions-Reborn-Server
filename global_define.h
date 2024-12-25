#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define FD_SETSIZE 1024
#include <WinSock2.h>

#include <cstdint>
#include <memory>

#include "misc/math.h"
#include "misc/Timer.h"
#include "misc/ByteStream.h"

#include "misc/threadsafe_queue.hpp"
#include "misc/RealmCrypt.h"
#include "misc/ByteStream.h"

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