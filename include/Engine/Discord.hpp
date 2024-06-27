#pragma once
#include "uuid.hpp"
#include <cstring>
#include <discord-rpc/discord_rpc.h>
#include <iostream>
#include <memory>

namespace Engine {
    class DiscordRPC {
    public:
        static DiscordRPC *instance;
        std::string m_ID;

        time_t timestamp = time(0);

        std::unique_ptr<DiscordEventHandlers> handlers;

        DiscordRPC(const std::string &id);
        ~DiscordRPC();
        void UpdatePresence(const std::string &details = "", const std::string &state = "", const std::string &largeImageKey = "logo", const std::string &largeImageText = "", const std::string &smallImageKey = "", const std::string &smallImageText = "");
    };
} // namespace Engine