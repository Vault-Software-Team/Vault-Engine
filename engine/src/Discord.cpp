#include "discord-rpc/discord_rpc.h"
#include <Engine/Discord.hpp>

using namespace Engine;

DiscordRPC *DiscordRPC::instance = nullptr;

DiscordRPC::DiscordRPC(const std::string &id) : m_ID(id) {
    if (instance) {
        std::cout << "ERROR: Cannot have more than one Discord RPC client!\n";
        return;
    }
    instance = this;

    handlers = std::make_unique<DiscordEventHandlers>();
    memset(handlers.get(), 0, sizeof(*handlers));

    handlers->ready = [](const DiscordUser *request) {
        std::cout << "Discord: Ready" << std::endl;
    };

    handlers->errored = [](int errorCode, const char *message) {
        std::cout << "Discord: Error " << errorCode << ": " << message
                  << std::endl;
    };

    handlers->disconnected = [](int errorCode, const char *message) {
        std::cout << "Discord: Disconnected " << errorCode << ": " << message
                  << std::endl;
    };

    handlers->joinGame = [](const char *joinSecret) {
        std::cout << "Discord: Join Game " << joinSecret << std::endl;
    };

    handlers->spectateGame = [](const char *spectateSecret) {
        std::cout << "Discord: Spectate Game " << spectateSecret << std::endl;
    };

    std::cout << "Discord Initialized\n";
    Discord_Initialize(id.c_str(), handlers.get(), 1, nullptr);
}

DiscordRPC::~DiscordRPC() {
    Discord_Shutdown();
}

void DiscordRPC::UpdatePresence(const std::string &details, const std::string &state, const std::string &largeImageKey, const std::string &largeImageText, const std::string &smallImageKey, const std::string &smallImageText) {
#ifndef GAME_BUILD
    char buffer[256];
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.largeImageKey = largeImageKey.c_str();
    discordPresence.largeImageText = largeImageText.c_str();
    discordPresence.smallImageKey = smallImageKey.c_str();
    discordPresence.smallImageText = smallImageText.c_str();
    discordPresence.state = state.c_str();
    discordPresence.details = details.c_str();
    discordPresence.joinSecret = "asdfasdf83784387fgd8";
    discordPresence.matchSecret = "adfkjfdhjk394874567";
    discordPresence.partyId = uuid::generate_uuid_v4().c_str();
    discordPresence.partySize = 1;
    discordPresence.partyMax = 5;
    discordPresence.startTimestamp = timestamp;
    Discord_UpdatePresence(&discordPresence);
#endif
}