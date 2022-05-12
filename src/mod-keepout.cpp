#include "Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"
#include "Chat.h"

uint32 maxwarnings;
bool KeepoutEnabled;

class Playerwarning : public DataMap::Base
{
public:
    Playerwarning() {}
    Playerwarning(uint32 warning) : warning(warning) {}
    uint32 warning = 1;
};

class Keepout : public PlayerScript
{
public:
    Keepout() : PlayerScript("Keepout") { }

    std::string playername;
    uint32 mapId;
    std::string maparea;

    void OnLogin(Player* player)
{
            if (sConfigMgr->GetOption<bool>("Announcer.Enable", true))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("This server is running the |cff4CFF00Keepout |rmodule.");
    }
}
    void OnMapChanged(Player* player)
    {
        if (KeepoutEnabled)
        {
            if (player->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
                return;

            QueryResult result = WorldDatabase.Query("SELECT `mapId` FROM `map_lock` WHERE `mapId` = '{}'", player->GetMapId());

            playername = player->GetName();
            mapId =  player->GetMap()->GetId();
            maparea = player->GetZoneId();

            if (!result)
                return;

            do
            {
                CharacterDatabase.Query("INSERT INTO `map_exploit` (`player`, `map`, `area`) VALUES ('{}', '{}', '{}')", playername.c_str(), mapId, player->GetAreaId());
                ChatHandler(player->GetSession()).PSendSysMessage("Has ido a un lugar prohibido tus acciones han sido registradas.");

                /*uint32& warninggiven = player->CustomData.GetDefault<Playerwarning>("warning")->warning;

                if (warninggiven == maxwarnings)
                    player->GetSession()->KickPlayer();
                else
                    warninggiven++;*/
				
				std::string tag_colour = "7bbef7";
				std::string plr_colour = "7bbef7";
				std::ostringstream stream;
				
				stream << "|CFF" << tag_colour << "|r|cff" << plr_colour << " " << playername << "|r fue expulsado de una -> Zona Prohibida <-";
                sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());

                if (player->GetTeamId() == TEAM_HORDE)
                    player->TeleportTo(1, 1484.36f, -4417.93f, 24.4709f, 0.00f);
                else
                    player->TeleportTo(0, -9075.6650f, 425.8427f, 93.0560f, 0.00f);

            } while (result->NextRow());
        }
    }

    void OnUpdateZone(Player* player, uint32 /*newZone */,  uint32 /*newArea*/)
    {
        if (KeepoutEnabled)
        {
            if (player->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
                return;

            QueryResult result = WorldDatabase.Query("SELECT `zoneID` FROM `map_lock` WHERE `zoneID` = '{}'", player->GetZoneId());

            if (!result)
                return;

            do
            {
                CharacterDatabase.Query("INSERT INTO `map_exploit` (`player`, `map`, `area`) VALUES ('{}', '{}', '{}')", playername.c_str(), mapId, player->GetAreaId());

                ChatHandler(player->GetSession()).PSendSysMessage("Has ido a un lugar prohibido tus acciones han sido registradas.");
				
				std::string tag_colour = "7bbef7";
				std::string plr_colour = "7bbef7";
				std::ostringstream stream;
				
				stream << "|CFF" << tag_colour << "|r|cff" << plr_colour << " " << playername << "|r fue expulsado de una -> Zona Prohibida <-";
                sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());

                /*uint32& warninggiven = player->CustomData.GetDefault<Playerwarning>("warning")->warning;

                if (warninggiven == maxwarnings)
                    player->GetSession()->KickPlayer();
                else
                    warninggiven++;*/

                if (player->GetTeamId() == TEAM_HORDE)
                    player->TeleportTo(1, 1484.36f, -4417.93f, 24.4709f, 0.00f);
                else
                    player->TeleportTo(0, -9075.6650f, 425.8427f, 93.0560f, 0.00f);

            } while (result->NextRow());

        }
    }
};

class Keepout_conf : public WorldScript
{
public:
    Keepout_conf() : WorldScript("Keepout_conf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            maxwarnings = sConfigMgr->GetOption<int32>("MaxWarnings", 3);
            KeepoutEnabled = sConfigMgr->GetOption<bool>("KeepOutEnabled", true);

        }
    }
};

void AddKeepOutScripts()
{
    new Keepout_conf();
    new Keepout();
}
