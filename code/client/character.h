#ifdef CORE_CHARACTER_H
#error "character.h included more than once"
#endif
#define CORE_CHARACTER_H

typedef struct Character {
    struct kstr_hdr name;
    uint16_t    name_buffer[20];

    struct uuid uuid;

    Profession  prof1;
    Profession  prof2;
    uint16_t    map;
} Character;
typedef array(Character) CharacterArray;

void init_character(Character *character)
{
    kstr_hdr_init(&character->name, character->name_buffer, ARRAY_SIZE(character->name_buffer));

    character->prof1 = Profession_None;
    character->prof2 = Profession_None;
    character->map = 0;
}
