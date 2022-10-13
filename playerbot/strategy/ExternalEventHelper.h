#pragma once

#include "Trigger.h"

namespace ai
{
    class ExternalEventHelper {
    public:
        ExternalEventHelper(AiObjectContext* aiObjectContext) : aiObjectContext(aiObjectContext) {}

        bool ParseChatCommand(std::string command, Player* owner = NULL)
        {
            if (HandleCommand(command, "", owner))
                return true;

            size_t i = string::npos;
            while (true)
            {
                size_t found = command.rfind(" ", i);
                if (found == string::npos || !found)
                    break;

                std::string name = command.substr(0, found);
                std::string param = command.substr(found + 1);

                i = found - 1;

                if (HandleCommand(name, param, owner))
                    return true;
            }

            if (!ChatHelper::parseable(command))
                return false;

            HandleCommand("c", command, owner);
            HandleCommand("t", command, owner);
            return true;
        }

        void HandlePacket(map<uint16, string> &handlers, const WorldPacket &packet, Player* owner = NULL)
        {
            uint16 opcode = packet.GetOpcode();
            std::string name = handlers[opcode];
            if (name.empty())
                return;

            Trigger* trigger = aiObjectContext->GetTrigger(name);
            if (!trigger)
                return;

            WorldPacket p(packet);
            trigger->ExternalEvent(p, owner);
        }

        bool HandleCommand(std::string name, std::string param, Player* owner = NULL)
        {
            Trigger* trigger = aiObjectContext->GetTrigger(name);
            if (!trigger)
                return false;

            trigger->ExternalEvent(param, owner);
            return true;
        }

    private:
        AiObjectContext* aiObjectContext;
    };
}
