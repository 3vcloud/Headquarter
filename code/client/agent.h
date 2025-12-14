#ifdef CORE_AGENT_H
#error "agent.h included more than once"
#endif
#define CORE_AGENT_H

typedef struct Agent {
    AgentId         agent_id;
    AgentType       type;
    bool            spawned;

    uint32_t        attacking_agent_id;
    uint32_t        party_id;
    uint32_t        npc_id;
    uint32_t        item_id;
    uint32_t        gadget_id;
    uint32_t        player_id;

    uint32_t        effects;

    float           health;
    float           energy;
    int32_t         health_max;
    int32_t         energy_max;

    float           health_per_sec;
    float           energy_per_sec;

    Vec2f           velocity; // api
    float           rotation; // api
    float           speed;    // api

    int32_t         model_state;
    Profession      prof1;
    Profession      prof2;

    bool            moving;
    bool            maybe_moving;

    float           speed_base;
    float           speed_modifier;

    Vec2f           position;
    Vec2f           destination;
    Vec2f           direction;

    int32_t         level;
    AgentMarker     marker;
    uint32_t        casting_skill_id;
} Agent;
typedef array(Agent *) ArrayAgent;

void agent_set_casting(Agent *agent, uint32_t casting_skill_id)
{
    assert(agent);
    agent->casting_skill_id = casting_skill_id;
}

AgentId agent_get_id(Agent *agent)
{
    assert(agent);
    return agent->agent_id;
}

void agent_stop_moving(Agent *agent)
{
    agent->speed = 0.f;
    agent->moving = false;
    agent->maybe_moving = false;
    agent->velocity.x = 0.f;
    agent->velocity.y = 0.f;
    agent->destination.x = 0.f;
    agent->destination.y = 0.f;
}

void agent_update_api(Agent *agent)
{
    float speed = agent->speed_base * agent->speed_modifier;
    agent->speed = speed;
    agent->velocity.x = agent->direction.x * speed;
    agent->velocity.y = agent->direction.y * speed;
}

bool agent_is_dead(Agent *a) { return (a->effects & AgentEffect_Dead) != 0; }

void api_make_agent(ApiAgent *dest, Agent *src)
{
    dest->agent_id = src->agent_id;
    dest->type = src->type;
    dest->spawned = src->spawned;

    dest->attacking_agent_id = src->attacking_agent_id;
    dest->party_id = src->party_id;
    dest->npc_id = src->npc_id;
    dest->item_id = src->item_id;
    dest->gadget_id = src->gadget_id;
    dest->player_id = src->player_id;

    dest->effects = src->effects;

    dest->health = src->health;
    dest->energy = src->energy;
    dest->health_max = src->health_max;
    dest->energy_max = src->energy_max;

    dest->health_per_sec = src->health_per_sec;
    dest->energy_per_sec = src->energy_per_sec;

    dest->velocity = src->velocity;
    dest->rotation = src->rotation;
    dest->speed = src->speed;

    dest->model_state = src->model_state;
    dest->prof1 = src->prof1;
    dest->prof2 = src->prof2;

    dest->moving = src->moving;
    dest->maybe_moving = src->maybe_moving;

    dest->speed_base = src->speed_base;
    dest->speed_modifier = src->speed_modifier;

    dest->position = src->position;
    dest->destination = src->destination;
    dest->direction = src->direction;

    dest->level = src->level;
    dest->marker = src->marker;
    dest->casting_skill_id = src->casting_skill_id;
}

Agent *get_agent_safe(World *world, AgentId id);
