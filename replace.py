import os
import win32api
import collections
import re

scene_map = {
"ydan":"Ydan",
"ddan":"Ddan",
"bdan":"Bdan",
"Bmori1":"Bmori1",
"HIDAN":"Hidan",
"MIZUsin":"Mizusin",
"jyasinzou":"Jyasinzou",
"HAKAdan":"Hakadan",
"HAKAdanCH":"Hakadan_ch",
"ice_doukutu":"Ice_doukutu",
"men":"Men",
"ganontika":"Ganontika",
#"ydan":"YdanDD",
#"ddan":"DdanDD",
#"bdan":"BdanDD",
#"Bmori1":"Bmori1DD",
#"HIDAN":"HidanDD",
#"MIZUsin":"MizusinDD",
#"jyasinzou":"JyasinzouDD",
#"HAKAdan":"HakadanDD",
#"HAKAdanCH":"Hakadan_chDD",
#"ice_doukutu":"Ice_doukutuDD",
#"men":"MenDD",
#"ganontika":"GanontikaDD",
"syotes":"Syotes",
"syotes2":"Syotes2",
"depth_test":"P_depth_test",
"spot00":"Spot00",
"spot01":"Spot01",
"spot02":"Spot02",
"spot03":"Spot03",
"spot04":"Spot04",
"spot05":"Spot05",
"spot06":"Spot06",
"spot07":"Spot07",
"spot08":"Spot08",
"spot09":"Spot09",
"spot10":"Spot10",
"spot11":"Spot11",
"spot12":"Spot12",
"spot13":"Spot13",
"spot15":"Spot15",
"spot16":"Spot16",
"spot17":"Spot17",
"spot18":"Spot18",
"market_day":"Market_day",
"market_night":"Market_night",
"testroom":"Testroom",
"kenjyanoma":"Kenjyanoma",
"tokinoma":"Tokinoma",
"sutaru":"Sutaru",
"link_home":"Link",
"kokiri_shop":"Kokiri",
"kokiri_home":"K_Home",
"kakusiana":"Kakusiana",
"entra":"Entra_day",
"moribossroom":"Moriboss",
"syatekijyou":"Syatekijyou",
"shop1":"Shop",
"hairal_niwa":"Hairal_niwa",
"ganon_tou":"Ganon_tou",
"sasatest":"Sasatest",
"market_alley":"Market_alley",
"spot20":"Spot20",
"market_ruins":"Market_ruins",
"entra_n":"Entra_night",
"enrui":"Entra_ruins",
"market_alley_n":"Market_alley_n",
"hiral_demo":"Hiral_demo",
"kokiri_home2":"K_Home2",
"kokiri_home3":"K_Home3",
"malon_stable":"Stable",
"kakariko":"Kakariko",
"bdan_boss":"Bdan_boss",
"FIRE_bs":"Fire_bs",
"hut":"HUT",
"daiyousei_izumi":"Daiyousei_izumi",
"hakaana":"Hakaana",
"yousei_izumi_tate":"Yousei_izumi_tate",
"yousei_izumi_yoko":"Yousei_izumi_yoko",
"golon":"Shop_golon",
"zoora":"Zoora",
"drag":"Shop_drag",
"alley_shop":"Shop_alley",
"night_shop":"Shop_night",
"impa":"Kakariko_impa",
"labo":"Labo",
"tent":"Tent",
"nakaniwa":"Nakaniwa",
"ddan_boss":"Ddan_boss",
"ydan_boss":"Ydan_boss",
"HAKAdan_bs":"Hakadan_boss",
"MIZUsin_bs":"Mizusin_boss",
"ganon":"Ganon",
"ganon_boss":"Ganon_boss",
"jyasinboss":"Jyasinzou_boss",
"kokiri_home4":"K_Home4",
"kokiri_home5":"K_Home5",
"ganon_final":"Ganon_final",
"kakariko3":"Kakariko_home3",
"hairal_niwa2":"Hairal_niwa2",
"hakasitarelay":"Hakasitarelay",
"shrine":"Shrine",
"turibori":"Turibori",
"shrine_n":"Shrine_n",
"shrine_r":"Shrine_r",
"hakaana2":"Hakaana2",
"gerudoway":"Gerudoway",
"hairal_niwa_n":"Hairal_niwa_n",
"bowling":"Bowling",
"hakaana_ouke":"Hakaana_ouke",
"hylia_labo":"Hylia_labo",
"souko":"Souko",
"miharigoya":"Miharigoya",
"mahouya":"Mahouya",
"takaraya":"Takaraya",
"ganon_sonogo":"Ganon_sonogo",
"ganon_demo":"Ganon_demo",
"besitu":"Besitu",
"face_shop":"Shop_face",
"kinsuta":"Kinsuta",
"ganontikasonogo":"Ganontikasonogo",
"test01":"Test01",
}

def getType(file,fullname):
    fileType = None
    #print(file)
    if '_info_' in file or file.endswith('_info.o'):
        fileType = 'info'
    if re.match(r'.*_demo(uma)?(0\d)?.o', file) or file in (
        'spot00_opening.o',
        'spot00_demooca.o',
        'spot02_kodomo_inazuma.o',
        'spot04_yahoo.o',
        'spot04_jr.o',
        'spot04_jr2.o',
        'spot06_itemfall.o',
        'spot06_tori.o',
        'spot09_jump.o',
        'spot09_2ndjump.o',
        'spot16_tori.o',
        'spot18_daiman.o',
        'spot18_shoman.o',
        'okarina_epona.o',
        'okarina_storm.o',
        'ganontika_hikari.o',
        'ganontika_honoo.o',
        'ganontika_kaze.o',
        'ganontika_kori.o',
        'ganontika_tamasi.o',
        'ganontika_yami.o',
        'tokinoma_after_iamseek.o',
        'rinkie_demo.o',
        'ganon_data.o',
        'link_data.o',
        'navy_data.o',
        'hakaana_ouke_sunA.o',
        'hakaana_ouke_sunB.o'
        ) or file.endswith('_show.o') or '_dm_' in file:
        fileType = 'demo'
    if file.endswith('_texture.o') or file.endswith('_txt.o'):
        fileType = 'texture'
    if file.endswith('_bgc.o'):
        fileType = 'bgc'
    if file.endswith('_shape.o'):
        fileType = 'shape'
    if '_grp_' in file or file in (
        'face_shop.o',
        'kokiri_shop.o',
        'kokiri_home.o',
        'kokiri_home3.o',
        'kokiri_home5.o',
        'shop1.o',
        'market_alley.o',
        'market_alley_n.o',
        'enrui.o',
        'kakariko.o',
        'kakariko3.o',
        'impa.o',
        'labo.o',
        'tent.o',
        'shrine.o',
        'shrine_n.o',
        'shrine_r.o'
        ):
        fileType = 'grp'
    if file in (
        'depth_test.o',
        'link_home.o',
        'entra.o',
        'entra_n.o',
        'shop1_d.o',
        'malon_stable.o',
        'hut.o',
        'golon.o',
        'zoora.o',
        'drag.o',
        'alley_shop.o',
        'night_shop.o'
        ):
        fileType = 'obj'
    if fileType is None:
        print('mips-linux-gnu-objdump -D %s | head -n 25'%fullname)
    assert fileType is not None
    return fileType

fileTypeMap = { # True = load from zelda_tool_scene instead of zelda_scene
    'info': False, # must be false - all zeroes in zelda_tool_scene
    'bgc': False, # must be false - not present in zelda_tool_scene
    
    'demo': True,
    'texture': False,
    'shape': False,
    'grp': False,
    'obj': False
}

lateAdditionCutscenes = [
'spot01_demo03.o',
'spot01_demo04.o',
'spot02_demo00.o',
'spot02_demo01.o',
'spot04_demo05.o',
'spot04_demo06.o',
'spot04_demo07.o',
'spot04_jr.o',
'spot04_jr2.o',
'spot06_demo01.o',
'spot08_demo02.o',
'spot09_demo02.o',
'spot10_demo00.o',
'spot12_demo01.o',
'spot12_demo02.o',
'spot16_demo03.o',
'spot16_demo04.o',
'spot17_demo01.o',
'spot18_demo00.o',
'spot18_demo01.o',
'spot20_demo03.o',
'spot20_demo04.o',
'spot20_demo05.o',
'spot20_demo06.o',
'spot20_demo07.o',
'hiral_demo05.o',
'hiral_demo06.o',
'hiral_demo07.o',
'hiral_demo08.o',
'Daiyousei_demo00.o',
'Daiyousei_demo01.o',
'Daiyousei_demo02.o',
'Yousei_izumi_yoko_demo00.o',
'Yousei_izumi_yoko_demo01.o',
'Yousei_izumi_yoko_demo02.o',
'jyasinboss_demo02.o',
'hakaana_ouke_sunA.o',
'hakaana_ouke_sunB.o',
'ganon_demo00.o',
]

f=open('spec.org','rb')
f2=open('spec','wb')
for line in f.read().decode('ascii').split('\n')[:-1]:
    if 'build/scenes' in line and 'kokiri_home4' not in line:
        filename = line.split('"')[1]
        _,_,_,scene,obj = filename.split('/')
        newname = scene_map[scene]
        includes = []
        if '_scene.o' in obj:
            includes.append('zelda_scene/%s/%s_main_info.o'%(newname,scene))
            
            dirname = 'zelda_tool_scene/%s/'%newname
            for file in sorted(os.listdir(dirname)):
                if file.endswith('.o') and file!=newname+'.o':
                    fileType = getType(file,dirname+file)
                    if fileTypeMap[fileType]:
                        includes.append(dirname+file)
            
            dirname = 'zelda_scene/%s/'%newname
            for file in sorted(os.listdir(dirname)):
                if file.endswith('.o') and file!=newname+'.o':
                    fileType = getType(file,dirname+file)
                    if not fileTypeMap[fileType] or file in lateAdditionCutscenes:
                        includes.append(dirname+file)

        elif '_room_' in obj:
            room = obj.split('.')[0].split('_')[-1]
            includes.append('zelda_scene/%s/ROOM%s/%s_%s_info.o'%(newname,room,scene,room))
            
            dirname = 'zelda_tool_scene/%s/ROOM%s/'%(newname,room)
            for file in sorted(os.listdir(dirname)):
                if file.endswith('.o') and not file.startswith('ROOM'):
                    fileType = getType(file,dirname+file)
                    if fileTypeMap[fileType]:
                        includes.append(dirname+file)
                    
            dirname = 'zelda_scene/%s/ROOM%s/'%(newname,room)
            for file in sorted(os.listdir(dirname)):
                if file.endswith('.o') and not file.startswith('ROOM'):
                    fileType = getType(file,dirname+file)
                    if not fileTypeMap[fileType]:
                        includes.append(dirname+file)
        else:
            1/0

        for include in list(collections.OrderedDict.fromkeys(includes)):
            if os.path.isfile(include):
                f2.write(line.replace(filename,include).encode('ascii')+b'\n')
    else:
        pass
        f2.write(line.encode('ascii')+b'\n')
f2.close()
