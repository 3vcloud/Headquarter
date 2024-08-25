import os
import struct
import argparse

if os.name == 'nt':
  from process import *
  


from scanner import FileScanner

region_type_to_str_table = {
    0:  'RegionType_AllianceBattle',
    1:  'RegionType_Arena',
    2:  'RegionType_ExplorableZone',
    3:  'RegionType_GuildBattleArea',
    4:  'RegionType_GuildHall',
    5:  'RegionType_MissionOutpost',
    6:  'RegionType_CooperativeMission',
    7:  'RegionType_CompetitiveMission',
    8:  'RegionType_EliteMission',
    9:  'RegionType_Challenge',
    10: 'RegionType_Outpost',
    11: 'RegionType_ZaishenBattle',
    12: 'RegionType_HeroesAscent',
    13: 'RegionType_City',
    14: 'RegionType_MissionArea',
    15: 'RegionType_HeroBattleOutpost',
    16: 'RegionType_HeroBattleArea',
    17: 'RegionType_EotnMission',
    18: 'RegionType_Dungeon',
    19: 'RegionType_Marketplace',
    20: 'RegionType_Unknown',
    21: 'RegionType_DevRegion',
}

region_to_str_table = {
    0: 'Region_Kryta',
    1: 'Region_Maguuma',
    2: 'Region_Ascalon',
    3: 'Region_NorthernShiverpeaks',
    4: 'Region_HeroesAscent',
    5: 'Region_CrystalDesert',
    6: 'Region_FissureOfWoe',
    7: 'Region_Presearing',
    8: 'Region_Kaineng',
    9: 'Region_Kurzick',
    10: 'Region_Luxon',
    11: 'Region_ShingJea',
    12: 'Region_Kourna',
    13: 'Region_Vaabi',
    14: 'Region_Desolation',
    15: 'Region_Istan',
    16: 'Region_DomainOfAnguish',
    17: 'Region_TarnishedCoast',
    18: 'Region_DepthsOfTyria',
    19: 'Region_FarShiverpeaks',
    20: 'Region_CharrHomelands',
    21: 'Region_BattleIslands',
    22: 'Region_TheBattleOfJahai',
    23: 'Region_TheFlightNorth',
    24: 'Region_TheTenguAccords',
    25: 'Region_TheRiseOfTheWhiteMantle',
    26: 'Region_Swat',
    27: 'Region_DevRegion',
}

def region_type_to_str(region_type):
    return region_type_to_str_table[region_type]

def region_to_str(region):
    return region_to_str_table[region]

def get_area_info_from_scanner(scanner):
    scan_addr = scanner.find(b'\x6B\xC6\x7C\x5E\x05', + 0x5)
    table_addr = scanner.read(scan_addr, 'I')[0]
    table_size = scanner.read(scan_addr - 0x1F, 'I')[0]
    ENTRY_SIZE = 0x7C
    if type(scanner) is FileScanner:
        table_addr -= scanner.parsed.OPTIONAL_HEADER.ImageBase

    size_in_bytes = table_size * ENTRY_SIZE
    data = scanner.read(table_addr, f'{size_in_bytes}s')[0]
    row = ''
    for i in range(table_size):
        campaign, continent, region, region_type, flags = struct.unpack_from('<IIIII', data, (i * ENTRY_SIZE))
        x,y,icon_start_x,icon_start_y,icon_end_x,icon_end_y,icon_start_x_dupe,icon_start_y_dupe,icon_end_x_dupe,icon_end_y_dupe = struct.unpack_from('<IIIIIIIIII', data, (i * ENTRY_SIZE + 0x40))
        region = region_to_str(region)
        region_type = region_type_to_str(region_type)
        if len(row) > 0:
          row += ',\n'
        row += '{'
        row += f'.campaign = {campaign}, .continent = {continent}, .region = {region}, .region_type = {region_type}, .flags = 0x{flags:08X}'
        if icon_start_x == 0:
          start_x = icon_start_x_dupe
          start_y = icon_start_y_dupe
          end_x = icon_end_x_dupe
          end_y = icon_end_y_dupe
        else:
          start_x = icon_start_x
          start_y = icon_start_y
          end_x = icon_end_x
          end_y = icon_end_y
        row += f', .x = {x}, .y = {y}, .start_x = {start_x}, .start_y = {start_y}, .end_x = {end_x}, .end_y = {end_y}'
        row += '}';
    return row;

    addr = scanner.find(b'\x8B\x45\x08\xC7\x00\x88\x00\x00\x00\xB8', +0xA)
    keys, = scanner.read(addr)

    if type(scanner) is FileScanner:
        # It would be nice to not do that explicitly, but overall, this is a reloc,
        # so when reading from a file, we need to remove the `ImageBase` to get an
        # RVA based on 0.
        keys -= scanner.parsed.OPTIONAL_HEADER.ImageBase

    pr, = scanner.read(keys + 4, '4s')
    pm, = scanner.read(keys + 8, '64s')
    pk, = scanner.read(keys + 72, '64s')

    pr = int.from_bytes(pr, byteorder='little')
    pm = int.from_bytes(pm, byteorder='little')
    pk = int.from_bytes(pk, byteorder='little')

    return (pr, pm, pk)

def main(args):
    if args.pid:
        proc = Process(args.pid)
    if args.proc:
        proc = Process.from_name(args.proc)
    scanner = ProcessScanner(proc)

    print(get_area_info_from_scanner(scanner))
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int, required=False,
        help="Process id of the target Guild Wars instance.")
    parser.add_argument("--proc", type=str, default='Gw.exe',
        help="Process name of the target Guild Wars instance.")
    args = parser.parse_args()

    main(args)
