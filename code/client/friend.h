#ifdef CORE_FRIEND_H
#error "friend.h included more than once"
#endif
#define CORE_FRIEND_H

typedef struct Friend {
    PlayerStatus    status;
    FriendType      type;
    struct kstr     name;
    uint16_t        name_buffer[20];
    struct kstr     account;
    uint16_t        account_buffer[20];
    uuid_t          uuid;
    uint32_t        zone;
} Friend;
typedef array(Friend) FriendArray;

Friend* get_friend(uint8_t* uuid, uint16_t* name);

void init_friend(Friend* friend)
{
    kstr_init(&friend->name, friend->name_buffer, 0, ARRAY_SIZE(friend->name_buffer));
    kstr_init(&friend->account, friend->account_buffer, 0, ARRAY_SIZE(friend->account_buffer));
    friend->status = PlayerStatus_Offline;
    friend->type = FriendType_Unknow;
    friend->zone = 0;
}

static void api_make_friend(ApiFriend* dest, Friend* src)
{
    dest->type = src->type;
    dest->status = src->status;
    dest->map_id = src->zone;
    uuid_copy(dest->uuid, src->uuid);
    kstr_write(&src->account, dest->account, ARRAY_SIZE(dest->account));
    kstr_write(&src->name, dest->playing, ARRAY_SIZE(dest->playing));
}