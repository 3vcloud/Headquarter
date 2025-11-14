#ifndef EVENT_H
#define EVENT_H

#include <common/list.h>
#include <stdbool.h>

typedef enum ObserverMatchType {
    ObserverMatchType_SpecialEvent = 0,
    ObserverMatchType_HallOfHeroe = 1,
    ObserverMatchType_MyGuildsBattle = 2,
    ObserverMatchType_MyGuildsHeroesAscentGame = 3,
    ObserverMatchType_TopGuildBattle = 4,
    ObserverMatchType_TopGuildHeroesAscentGame = 5,
    ObserverMatchType_UploadedGame = 6,
    ObserverMatchType_Top1v1Battle = 7,
    ObserverMatchType_Top1v1TournamentBattle = 8
} ObserverMatchType;

typedef enum EventType {
    EventType_AgentFocus,
    EventType_ChatMessage,
    EventType_CinematicPlay,
    EventType_CinematicStop,
    EventType_DialogOpen,
    EventType_DialogButton,
    EventType_AuthError,
    EventType_FriendStatus,
    EventType_GuildMemberUpdated,
    EventType_ItemQuotePrice,
    EventType_PartyInviteRequest,
    EventType_PartyLeaderChanged,
    EventType_PartyMembersChanged,
    EventType_PartySearchAdvertisement,
    EventType_PartySearchRemoved,
    EventType_PartySearchSize,
    EventType_PartySearchType,
    EventType_SalvageSessionStart,
    EventType_WorldCantTravel,
    EventType_WorldMapEnter,
    EventType_WorldMapLeave,
    EventType_InstanceLoadInfo,
    EventType_PlayerPartySize,
    EventType_AgentDespawned,
    EventType_ObserverMatchReceive,
    EventType_AgentMoveToPoint,
    EventType_GenericValue,
    EventType_GenericValueTarget,
    EventType_GenericModifier,
    EventType_GenericFloat,
    EventType_JumboMessage,

    EventType_Count
} EventType;

typedef struct Event {
    EventType type;
    union {
        struct {
            uint32_t player_id; // player that issued the focus
            uint32_t agent_id;  // id of the focused mob
        } AgentFocus;
        struct {
            Channel             channel;
            uint16_t            extra_id;
            struct {
                size_t          length;
                const uint16_t *buffer;
            } message;

            struct {
                size_t          length;
                const uint16_t *buffer;
            } sender;
        } ChatMessage;
        struct {
            uint32_t sender_agent_id;
        } DialogOpen;
        struct {
            int button_id;
            int icon_id;
        } DialogButton;
        struct {
            uint32_t type;
            uint32_t code;
        } AuthError;
        struct {
            ApiFriend friend;
        } FriendStatus;
        struct {
            ApiGuildMember member;
        } GuildMemberUpdated;
        struct {
            int32_t     item_id;
            uint32_t    quote_price;
        } ItemQuotePrice;
        struct {
            uint32_t party_id;
        } PartyInviteRequest;
        struct {
            uint32_t party_id;
        } PartyMembersChanged;
        struct {
            uint16_t            party_id;
            uint8_t             party_size;
            uint8_t             hero_count;
            uint8_t             search_type; // 0=hunting, 1=mission, 2=quest, 3=trade, 4=guild
            uint8_t             hardmode;
            uint16_t            district_number;
            uint8_t             language;
            uint8_t             primary;
            uint8_t             secondary;
            uint8_t             level;
            struct {
                size_t          length;
                const uint16_t* buffer;
            } message;
            struct {
                size_t          length;
                const uint16_t* buffer;
            } sender;
        } PartySearchAdvertisement;
        struct {
            uint32_t item_id;
        } SalvageSessionStart;
        struct {
            int8_t value;
        } WorldCantTravel;
        struct {
            AgentId     agent;
            int16_t     map_id;
            int8_t      is_explorable;
            int32_t     district;
            int8_t      language;
            int8_t      is_observer;
        } InstanceLoadInfo;
        struct {
            uint16_t player_id;
            uint8_t size;
        } PlayerPartySize;
        struct {
            AgentId agent_id;
        } AgentDespawned;
        struct {
            #pragma pack(push, 1)
            struct PacketObserverMatchReceive {
                uint16_t header;
                uint32_t match_id;
                uint16_t map_id;
                uint16_t mins_ago;
                uint8_t  type;  // ObserverMatchType
                uint8_t  unknown_byte1;
                uint8_t  unknown_byte2;
                uint32_t nested_count;
                struct {
                    uint8_t  b0;
                    uint8_t  b1;
                    uint8_t  b2;
                    uint8_t  b3;
                    uint8_t  b4;
                    uint8_t  b5;
                    uint8_t  b6;
                    uint16_t unknown_word0;
                    uint8_t  unknown_byte7;
                    uint16_t string[48];
                } entries[2];
            };
            #pragma pack(pop)
            const struct PacketObserverMatchReceive* packet;
            size_t packet_size;
        } ObserverMatchReceive;
        struct {
            uint32_t agent_id;
            float x;
            float y;
            int16_t plane;
            int16_t current_plane;
        } AgentMoveToPoint;
        struct {
            uint32_t value_id;
            uint32_t agent_id;
            uint32_t value;
        } GenericValue;
        struct {
            uint32_t value_id;
            uint32_t target;
            uint32_t caster;
            uint32_t value;
        } GenericValueTarget;
        struct {
            uint32_t type;
            uint32_t target_id;
            uint32_t cause_id;
            float value;
        } GenericModifier;
        struct {
            uint32_t type;
            uint32_t agent_id;
            uint32_t target_id;
            float value;
        } GenericFloat;
        struct {
            uint8_t type;
            uint32_t value;
        } JumboMessage;
    };
} Event;

static inline void Event_Init(Event *event, EventType type)
{
    memset(event, 0, sizeof(*event));
    event->type = type;
}

typedef void (*Callback)(Event *event, void *param);
typedef struct CallbackEntry {
    struct list node;
    bool        registered;

    Callback    callback;
    void       *param;
} CallbackEntry;

static inline void
CallbackEntry_Init(CallbackEntry *entry, Callback cb, void *param)
{
    entry->registered = false;
    entry->callback = cb;
    entry->param = param;
}

#endif // EVENT_H
