#ifdef CORE_ITEM_C
#error "item.c included more than once"
#endif
#define CORE_ITEM_C

void remove_item_from_bag(Item *item)
{
    if (!item->bag) return;

    Bag *bag = item->bag;
    assert(array_inside(bag->items, item->slot));
    assert(array_at(bag->items, item->slot));

    array_set(bag->items, item->slot, NULL);
    item->bag = NULL;
    item->slot = 0;
    bag->item_count--;
}

Item *get_item_safe(GwClient *client, int32_t id)
{
    if (!(client->state.ingame && client->world.hash))
        return NULL;
    ArrayItem items = client->world.items;
    if (!array_inside(items, id))
        return NULL;
    return array_at(items, id);
}

Bag *get_bag_safe(GwClient *client, BagEnum bag_id)
{
    if (!client->state.ingame)
        return NULL;
    return client->inventory.bags[bag_id];
}

void HandleItemStreamCreate(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header   header;
        uint16_t stream_id;
        uint8_t  is_hero;
    } StreamCreate;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_STREAM_CREATE);
    assert(sizeof(StreamCreate) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    StreamCreate *pack = cast(StreamCreate *)packet;
    assert(client && client->game_srv.secured);
}

void HandleItemStreamDestroy(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        int16_t stream_id;
    } StreamDestroy;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_STREAM_DESTROY);
    assert(sizeof(StreamDestroy) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    StreamDestroy *pack = cast(StreamDestroy *)packet;
    assert(client && client->game_srv.secured);
}

void HandleItemGeneralInfo(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        /* +h0000 */ Header header;
        /* +h0002 */ int32_t item_id;
        /* +h0006 */ int32_t file_id;
        /* +h000A */ int8_t type;
        /* +h000B */ int8_t unk0;
        /* +h000C */ int16_t dye_color;
        /* +h000E */ int16_t meterials;
        /* +h0010 */ int8_t unk1;
        /* +h0011 */ int32_t flags;
        /* +h0015 */ int32_t value;
        /* +h0019 */ int32_t model;
        /* +h001D */ int32_t quantity;
        /* +h0021 */ uint16_t name[64];
        /* +h009D */ size_t n_unk2;
        /* +h00A1 */ uint32_t unk2[64];
    } ItemInfo;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_GENERAL_INFO);
    assert(sizeof(ItemInfo) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    ItemInfo *pack = cast(ItemInfo *)packet;
    assert(client && client->game_srv.secured);
    // LogInfo("ItemGeneralInfo {id: %d, modele: %d}", pack->item_id, pack->model);

    ArrayItem *items = &client->world.items;
    if (!array_inside(*items, pack->item_id)) {
        array_grow_to(*items, pack->item_id + 1);
        items->size = items->capacity;
    }

    Item *new_item = cast(Item *)game_object_alloc(&client->object_mgr, ObjectType_Item);
    new_item->item_id = pack->item_id;
    new_item->flags = pack->flags;
    new_item->model_id = pack->model;
    new_item->quantity = pack->quantity;
    new_item->type = pack->type;

    array_set(*items, pack->item_id, new_item);
}

void HandleItemRemove(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        int16_t stream_id;
        int32_t item_id;
    } ItemRemove;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_REMOVE);
    assert(sizeof(ItemRemove) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    ItemRemove *pack = cast(ItemRemove *)packet;
    assert(client && client->game_srv.secured);

    ArrayItem *items = &client->world.items;
    Item *item = get_item_safe(client, pack->item_id);
    if (!item) return;

    array_set(*items, pack->item_id, NULL);
    remove_item_from_bag(item);
    free(item);
}

void HandleItemWeaponSet(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        int16_t stream_id;
        int8_t slot;
        int32_t leadhand;
        int32_t offhand;
    } WeaponSet;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_WEAPON_SET);
    assert(sizeof(WeaponSet) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    WeaponSet *pack = cast(WeaponSet *)packet;
    assert(client && client->game_srv.secured);

    // LogInfo("New Weapon set !");
}

void HandleInventoryItemQuantity(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        int32_t item_id;
        int32_t quantity;
    } ItemQuantity;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_INVENTORY_ITEM_QUANTITY);
    assert(sizeof(ItemQuantity) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    ItemQuantity *pack = cast(ItemQuantity *)packet;
    assert(client && client->game_srv.secured);

    Item *item = get_item_safe(client, pack->item_id);
    if (!item) {
        LogInfo("Updated item quantity of item %d, but the item doesn't exist.", pack->item_id);
        return;
    }

    item->quantity = pack->quantity;
}

void HandleInventoryItemLocation(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        int16_t stream_id;
        int32_t item_id;
        int16_t bag_id;
        int8_t slot;
    } ItemLocation;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_INVENTORY_ITEM_LOCATION);
    assert(sizeof(ItemLocation) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    ItemLocation *pack = cast(ItemLocation *)packet;
    assert(client && client->game_srv.secured);

#if 0
    Item *item = get_item_safe(client, pack->item_id);
    if (!item) return;
#else
    if (!client->world.hash)
        return;
    ArrayItem items = client->world.items;
    if (!(array_inside(items, pack->item_id) && array_at(items, pack->item_id)))
        return;
    Item *item = array_at(items, pack->item_id);
#endif

    BagArray *bags = &client->world.bags;
    uint32_t hash = hash_int32(pack->bag_id);
    uint32_t index = hash % bags->size;
    Bag *bag = &array_at(*bags, index);

    while (bag->bag_id != pack->bag_id) {
        assert(bag->bag_id != 0);
        hash = hash_int32(hash);
        index = hash % bags->size;
        bag = &array_at(*bags, index);
    }

    assert(bag->bag_id == pack->bag_id);
    assert(array_inside(bag->items, pack->slot));
    assert(array_at(bag->items, pack->slot) == NULL);

    item->bag = bag;
    item->slot = pack->slot;
    array_set(bag->items, pack->slot, item);
    bag->item_count++;
}

void HandleInventoryCreateBag(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        int16_t stream_id;
        int8_t bag_type;
        int8_t bag_model_id;
        int16_t bag_id;
        int8_t slot_count;
        int32_t assoc_item_id;
    } BagInfo;
#pragma pack(pop)
    
    assert(packet->header == GAME_SMSG_INVENTORY_CREATE_BAG);
    assert(sizeof(BagInfo) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    BagInfo *pack = cast(BagInfo *)packet;

    BagArray *bags = &client->world.bags;
    uint32_t hash = hash_int32(pack->bag_id);
    uint32_t index = hash % bags->size;
    Bag *bag = &array_at(*bags, index);

    while (bag->bag_id != 0) {
        hash = hash_int32(hash);
        index = hash % bags->size;
        bag = &array_at(*bags, index);
    }

    assert(bag->bag_id == 0);
    bag->bag_id = pack->bag_id;
    bag->type = (BagType)pack->bag_type;
    bag->model = (BagEnum)pack->bag_model_id;
    array_init2(bag->items, pack->slot_count);

    assert(pack->bag_model_id < BagEnum_Count);
    client->inventory.bags[pack->bag_model_id] = bag;
}

void HandleWindowOwner(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        AgentId agent_id;
    } WindowOwner;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_WINDOW_OWNER);
    assert(sizeof(WindowOwner) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    WindowOwner *pack = cast(WindowOwner *)packet;
    assert(client && client->game_srv.secured);

    array_clear(client->merchant_items);
    client->merchant_agent_id = pack->agent_id;
    client->interact_with = pack->agent_id;
}

void HandleWindowAddItems(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        size_t  n_items;
        int32_t items[16];
    } AddItems;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_WINDOW_ADD_ITEMS);
    assert(sizeof(AddItems) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    AddItems *pack = cast(AddItems *)packet;
    assert(client && client->game_srv.secured);

    ArrayItem *items = &client->world.items;
    ArrayItem *merchant_items = &client->merchant_items;

    for (size_t i = 0; i < pack->n_items; i++) {
        int32_t item_id = pack->items[i];
        if (!array_inside(*items, item_id)) {
            LogError("Expected item '%d' but was not found", item_id);
            continue;
        }

        Item *item = array_at(*items, item_id);
        if (!item) {
            LogError("Expected item '%d' but was not found", item_id);
            continue;
        }
        array_add(*merchant_items, item);
    }
}

void HandleItemPriceQuote(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        int32_t item_id;
        int32_t price;
    } PriceQuote;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_PRICE_QUOTE);
    assert(sizeof(PriceQuote) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    PriceQuote *pack = cast(PriceQuote *)packet;
    assert(client && client->game_srv.secured);

    ArrayItem *items = &client->world.items;
    assert(array_inside(*items, pack->item_id));
    assert(array_at(*items, pack->item_id));
    Item *item = array_at(*items, pack->item_id);

    item->quote_price = pack->price;
}

void HandleItemChangeLocation(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        int16_t unk1;
        int32_t item_id;
        int16_t bag_id;
        int8_t  slot;
    } ChangeLocation;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_CHANGE_LOCATION);
    assert(sizeof(ChangeLocation) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    ChangeLocation *pack = cast(ChangeLocation *)packet;
    assert(client && client->game_srv.secured);

    Item *item = get_item_safe(client, pack->item_id);
    if (!item) {
        LogError("HandleItemChangeLocation: Couldn't find item %d", pack->item_id);
        return;
    }

    BagArray *bags = &client->world.bags;
    uint32_t hash = hash_int32(pack->bag_id);
    uint32_t index = hash % bags->size;
    Bag *bag = &array_at(*bags, index);

    while (bag->bag_id != pack->bag_id) {
        assert(bag->bag_id != 0);
        hash = hash_int32(hash);
        index = hash % bags->size;
        bag = &array_at(*bags, index);
    }

    // @Cleanup
    remove_item_from_bag(item);
    item->bag  = bag;
    item->slot = pack->slot;
    bag->items.data[pack->slot] = item;
    bag->item_count++;
}

void GameSrv_UseItem(GwClient *client, Item *item)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        int32_t item_id;
    } ItemUse;
#pragma pack(pop)

    assert(client && client->game_srv.secured);
    ItemUse packet = NewPacket(GAME_CMSG_ITEM_USE);
    packet.item_id = item->item_id;

    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void GameSrv_InteractItem(GwClient *client, uint32_t agent_id)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        AgentId agent_id;
        uint8_t unk0;
    } ItemUse;
#pragma pack(pop)

    assert(client && client->game_srv.secured);
    ItemUse packet = NewPacket(GAME_CMSG_INTERACT_ITEM);
    packet.agent_id = agent_id;
    packet.unk0 = 0;

    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void GameSrv_MoveItem(GwClient *client, Item *item, Bag *bag, int slot)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        int32_t item_id;
        int16_t bag_id;
        uint8_t slot;
    } ItemMove;
#pragma pack(pop)

    assert(client && client->game_srv.secured);
    ItemMove packet = NewPacket(GAME_CMSG_ITEM_MOVE);
    packet.item_id = item->item_id;
    packet.bag_id = bag->bag_id;
    packet.slot = slot;

    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void GameSrv_StartSalvage(GwClient *client, Item *kit, Item *item)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        int16_t session_id;
        int32_t kit_item_id;
        int32_t item_id;
    } SalvagePacket;
#pragma pack(pop)

    assert(client && client->game_srv.secured);
    SalvageSession *session = &client->salvage_session;

    SalvagePacket packet = NewPacket(GAME_CMSG_ITEM_SALVAGE_SESSION_OPEN);
    packet.session_id = session->salvage_session_id++;
    packet.kit_item_id = kit->item_id;
    packet.item_id = item->item_id;

    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void GameSrv_CancelSalvage(GwClient *client)
{
    assert(client && client->game_srv.secured);
    SalvageSession *session = &client->salvage_session;
    if (!session->is_open) return;

    Packet packet = NewPacket(GAME_CMSG_ITEM_SALVAGE_SESSION_CANCEL);
    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void GameSrv_SalvageDone(GwClient *client)
{
    assert(client && client->game_srv.secured);
    Packet packet = NewPacket(GAME_CMSG_ITEM_SALVAGE_SESSION_DONE);
    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void GameSrv_SalvageMaterials(GwClient *client)
{
    assert(client && client->game_srv.secured);
    Packet packet = NewPacket(GAME_CMSG_ITEM_SALVAGE_MATERIALS);
    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void GameSrv_SalvageUpgrade(GwClient *client, size_t index)
{
#pragma pack(push, 1)
    typedef struct {
        Header header;
        uint8_t upgrade_index;
    } SalvagePacket;
#pragma pack(pop)

    assert(client && client->game_srv.secured);
    SalvageSession *session = &client->salvage_session;
    if (index >= session->n_upgrades) {
        LogError("The maximum upgrade index is %u, but you entered %u", session->n_upgrades, index);
        return;
    }

    SalvagePacket packet = NewPacket(GAME_CMSG_ITEM_SALVAGE_UPGRADE);
    packet.upgrade_index = cast(uint8_t)index;
    SendPacket(&client->game_srv, sizeof(packet), &packet);
}

void HandleSalvageSessionStart(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        int16_t salvage_session_id;
        int32_t item_id; // representing the trade
        size_t  n_upgrades;
        int32_t upgrades[3];
    } SalvagePacket;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_SALVAGE_SESSION_START);
    assert(sizeof(SalvagePacket) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    SalvagePacket *pack = cast(SalvagePacket *)packet;
    assert(client && client->game_srv.secured);
    SalvageSession *session = &client->salvage_session;

    if (pack->salvage_session_id != session->salvage_session_id) {
        LogError("salvage_session_id mismatch {received: %d, expected: %d}",
            pack->salvage_session_id, session->salvage_session_id);
        return;
    }

    session->n_upgrades = pack->n_upgrades;
    for (size_t i = 0; i < session->n_upgrades; i++) {
        uint32_t item_id = pack->upgrades[i];
        session->upgrades[i] = get_item_safe(client, item_id);
    }
    session->is_open = true;
}

void HandleSalvageSessionCancel(Connection *conn, size_t psize, Packet *packet)
{
    assert(packet->header == GAME_SMSG_ITEM_SALVAGE_SESSION_CANCEL);
    assert(sizeof(Packet) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    assert(client && client->game_srv.secured);

    SalvageSession *session = &client->salvage_session;
    session->is_open = false;
    session->n_upgrades = 0;
}

void HandleSalvageSessionDone(Connection *conn, size_t psize, Packet *packet)
{
    assert(packet->header == GAME_SMSG_ITEM_SALVAGE_SESSION_DONE);
    assert(sizeof(Packet) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    assert(client && client->game_srv.secured);
    SalvageSession *session = &client->salvage_session;

    session->is_open = false;
    session->n_upgrades = 0;
}

void HandleSalvageSessionItemKept(Connection *conn, size_t psize, Packet *packet)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        uint8_t item_kept;
    } SalvagePacket;
#pragma pack(pop)

    assert(packet->header == GAME_SMSG_ITEM_SALVAGE_SESSION_ITEM_KEPT);
    assert(sizeof(SalvagePacket) == psize);

    GwClient *client = cast(GwClient *)conn->data;
    SalvagePacket *pack = cast(SalvagePacket *)packet;
    assert(client && client->game_srv.secured);
    SalvageSession *session = &client->salvage_session;
}

void GameSrv_UnequipItem(GwClient *client, EquipedItemSlot equip_slot, Bag *bag, int slot)
{
#pragma pack(push, 1)
    typedef struct {
        Header  header;
        uint8_t equip_slot;
        int16_t bag_id;
        uint8_t bag_slot;
    } UnequipItemPacket;
#pragma pack(pop)

    assert(client && client->game_srv.secured);

    UnequipItemPacket packet = NewPacket(GAME_CMSG_UNEQUIP_ITEM);
    packet.equip_slot = equip_slot;
    packet.bag_id = bag->bag_id;
    packet.bag_slot = slot;

    SendPacket(&client->game_srv, sizeof(packet), &packet);    
}
