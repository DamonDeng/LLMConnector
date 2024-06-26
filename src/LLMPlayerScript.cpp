/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "Config.h"
#include "Chat.h"

#include "LLMInterface.h"

using namespace DDLLM;


// Add player scripts
class LLMPlayerScript : public PlayerScript
{

private:
    LLMInterface llm_interface;

public:
    LLMPlayerScript() : PlayerScript("LLMPlayerScript") { 

        

    }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("MyModule.Enable", false))
        {
            ChatHandler(player->GetSession()).PSendSysMessage("LLMConnector script loaded!");
        }

        
    }

    //  virtual functions from PlayerScript
    // virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/) { }

    // virtual void OnBeforeSendChatMessage(Player* /*player*/, uint32& /*type*/, uint32& /*lang*/, std::string& /*msg*/) { }

    // virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Player* /*receiver*/) { }

    // virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Group* /*group*/) { }

    // virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Guild* /*guild*/) { }

    // virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Channel* /*channel*/) { }


    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg) override
    {
        // ChatHandler(player->GetSession()).PSendSysMessage("you have sent message: %s", msg.c_str());

        Creature* target = ChatHandler(player->GetSession()).getSelectedCreature();

        // Check whether there is any target selected:
        if (target == nullptr)
        {
            // if no target selected, conduct normal Say command.
            // sender->Say(msg, Language(lang));
        }
        else
        {
            // if there is target selected:
            

            // sender->Say(msg, Language(lang));

            // check target's name and detect whether it is a NPC in our list:

            // std::string target_name = target->GetName();

            // target->Say("LLMConnector 让我想想。。。, and let me think again.", Language(lang));
            // target->Say("LLMConnector And, my name is: " + target_name + ".", Language(lang));
            // player->Say(msg, Language(lang));

            // auto return_str = LLMInterface::player_say(msg);

            // target->Say(return_str, Language(lang));

            

            llm_interface.player_say_async(msg, player, target, Language(lang));


        }
    }

    

};

// Add all scripts in one
void AddAllScripts()
{
    new LLMPlayerScript();
}
