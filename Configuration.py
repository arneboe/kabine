import gphoto2 as gp

from enum import Enum


class Iso(Enum):
    ISO_AUTO = "Auto"
    ISO_80 = "80"
    ISO_100 = "100"
    ISO_200 = "200"
    ISO_400 = "400"
    ISO_800 = "800"
    ISO_1600 = "1600"

    def __str__(self):
        return '%s' % self.name


class WhiteBalance(Enum):
    WB_AUTO = "0"
    WB_DAYLIGHT = "1"
    WB_CLOUDY = "2"
    WB_ARTIFICIAL = "3"
    WB_NEON = "4"
    WB_NEON_2 = "5"
    WB_FLASH = "6"
    WB_MANUAL = "7" #TODO triggers manual wb, how does that work.

    def __str__(self):
        return '%s' % self.name

class Aperature(Enum):
    AP_IMPLICIT_AUTO = "implicit auto"
    AP_AUTO = "auto"
    AP_1 = "1"
    AP_1_1 = "1.1"
    AP_1_2 = "1.2"
    AP_1_4 = "1.4"
    AP_1_6 = "1.6"
    AP_1_8 = "1.8"
    AP_2 = "2"
    AP_2_2 = "2.2"
    AP_2_5 = "2.5"
    AP_2_8 = "2.8"
    AP_3_2 = "3.2"
    AP_3_5 = "3.5"
    AP_4 = "4"
    AP_4_5 = "4.5"
    AP_5 = "5"
    AP_5_6 = "5.6"
    AP_6_3 = "6.3"
    AP_6_7 = "6.7"
    AP_7_1 = "7.1"
    AP_8 = "8"
    AP_9 = "9"
    AP_9_5 = "9.5"
    AP_10 = "10"
    AP_11 = "11"
    AP_13 = "13"
    AP_14 = "14"
    AP_16 = "16"
    AP_18 = "18"
    AP_19 = "19"
    AP_20 = "20"
    AP_22 = "22"
    AP_25 = "25"
    AP_27 = "27"
    AP_29 = "29"
    AP_32 = "32"
    AP_36 = "36"
    AP_38 = "38"
    AP_40 = "40"
    AP_45 = "45"
    AP_51 = "51"
    AP_54 = "54"
    AP_57 = "57"
    AP_64 = "64"
    AP_72 = "72"
    AP_76 = "76"
    AP_81 = "81"
    AP_91 = "91"

    def __str__(self):
        return '%s' % self.name


class ShootingMode(Enum):
    AUTO = "Auto"
    TV = "TV"
    AV = "AV"
    MANUAL = "Manual"


class ShutterSpeed(Enum):
    SS_auto = "auto"
    SS_bulb = "bulb"
    SS_30 = "30"
    SS_25 = "25"
    SS_20 = "20"
    SS_15 = "15"
    SS_13 = "13"
    SS_10 = "10"
    SS_8 = "8"
    SS_6 = "6"
    SS_5 = "5"
    SS_4 = "4"
    SS_3_2 = "3.2"
    SS_3 = "3"
    SS_2_5 = "2.5"
    SS_2 = "2"
    SS_1_6 = "1.6"
    SS_1_5 = "1.5"
    SS_1_3 = "1.3"
    SS_1 = "1"
    SS_0_8 = "0.8"
    SS_0_7 = "0.7"
    SS_0_6 = "0.6"
    SS_0_5 = "0.5"
    SS_0_4 = "0.4"
    SS_0_3 = "0.3"
    SS_1__4 = "1/4"
    SS_1__5 = "1/5"
    SS_1__6 = "1/6"
    SS_1__8 = "1/8"
    SS_1__10 = "1/10"
    SS_1__13 = "1/13"
    SS_1__15 = "1/15"
    SS_1__20 = "1/20"
    SS_1__25 = "1/25"
    SS_1__30 = "1/30"
    SS_1__40 = "1/40"
    SS_1__45 = "1/45"
    SS_1__50 = "1/50"
    SS_1__60 = "1/60"
    SS_1__80 = "1/80"
    SS_1__90 = "1/90"
    SS_1__100 = "1/100"
    SS_1__125 = "1/125"
    SS_1__160 = "1/160"
    SS_1__180 = "1/180"
    SS_1__200 = "1/200"
    SS_1__250 = "1/250"
    SS_1__320 = "1/320"
    SS_1__350 = "1/350"
    SS_1__400 = "1/400"
    SS_1__500 = "1/500"
    SS_1__640 = "1/640"
    SS_1__750 = "1/750"
    SS_1__800 = "1/800"
    SS_1__1000 = "1/1000"
    SS_1__1250 = "1/1250"
    SS_1__1500 = "1/1500"
    SS_1__1600 = "1/1600"
    SS_1__2000 = "1/2000"
    SS_1__2500 = "1/2500"
    SS_1__3000 = "1/3000"
    SS_1__3200 = "1/3200"
    SS_1__4000 = "1/4000"
    SS_1__5000 = "1/5000"
    SS_1__6000 = "1/6000"
    SS_1__6400 = "1/6400"
    SS_1__8000 = "1/8000"

    def __str__(self):
        return '%s' % self.name

class Configuration:

    def __init__(self, camera):
        self.camera = camera
        self.config = gp.check_result(gp.gp_camera_get_config(self.camera))

        #note in av mode shutter speed is ignored!
        self.set_shootingmode(ShootingMode.AV) #needs to be done first, otherwise some settings are read only
        self.set_zoom(0)
        #self.print_config()
        self.set_iso(Iso.ISO_100)
        self.set_white_balance(WhiteBalance.WB_AUTO)
        self.set_aperture(Aperature.AP_AUTO)
        self.set_shutter_speed(ShutterSpeed.SS_auto)#
        self.set_flashmode()
        self.set_shootmode()
        
    def set_shootmode(self):
        flash_widget = self.find_config_widget_by_name("afdistance")
        if flash_widget.get_readonly():
            print("afdistance READ ONLY")
            return
        flash_widget.set_value("Zone Focus (Close-up)")
        self.camera.set_config(self.config)
        
    def set_flashmode(self):
        flash_widget = self.find_config_widget_by_name("flashmode")
        if flash_widget.get_readonly():
            print("flash mode READ ONLY")
            return

        for choice in flash_widget.get_choices():
            if choice:
                print(choice)


    def set_shutter_speed(self, ss : ShutterSpeed):
        speed_widget = self.find_config_widget_by_name("shutterspeed")
        if speed_widget.get_readonly():
            print("shutter speed READ ONLY")
            return
        speed_widget.set_value(ss.value)
        self.camera.set_config(self.config)

    def set_shootingmode(self, mode: ShootingMode):
        mode_widget = self.find_config_widget_by_name("shootingmode")
        mode_widget.set_value(mode.value)
        self.camera.set_config(self.config)

    def get_zoom_range(self):
        zoom_widget = self.find_config_widget_by_name("zoom")
        lo, hi, inc = zoom_widget.get_range()
        return lo, hi

    def set_zoom(self, zoom):
        zoom_widget = self.find_config_widget_by_name("zoom")
        lo, hi, inc = zoom_widget.get_range()

        zoom_widget.set_value(self.__clamp(zoom, lo, hi))
        self.camera.set_config(self.config)

    def set_iso(self, iso: Iso):
        iso_widget = self.find_config_widget_by_name("iso")
        iso_widget.set_value(iso.value)
        self.camera.set_config(self.config)

    def set_white_balance(self, wb: WhiteBalance):
        wb_widget = self.find_config_widget_by_name("d013")
        wb_widget.set_value(wb.value)
        self.camera.set_config(self.config)

    def set_aperture(self, aperture: Aperature):
        #FIXME aperature seems to be read only for this camera model
        ap_widget = self.find_config_widget_by_name("aperture")
        if ap_widget.get_readonly():
            print("Aperature READ ONLY")
            return
        ap_widget.set_value(aperture.value)
        self.camera.set_config(self.config)


    def find_config_widget_by_name(self, name):
        widget = gp.check_result(gp.gp_widget_get_child_by_name(self.config, name))
        return widget
        #self.print_widget_info(widget)


    def print_widget_info(self, widget):
        label = gp.check_result(gp.gp_widget_get_label(widget))
        name = gp.check_result(gp.gp_widget_get_name(widget))
        child_type = self.__widget_type_to_string(gp.check_result(gp.gp_widget_get_type(widget)))
        id = gp.check_result(gp.gp_widget_get_id(widget))
        text = 'type:{}, label:{}, name:{}, id:{}'.format(child_type, label, name, id)
        print(text)

    def print_config(self):
        self.print_config_recursive(self.config, 0)

    def print_config_recursive(self, config, level):

        for child in gp.check_result(gp.gp_widget_get_children(config)):
            label = gp.check_result(gp.gp_widget_get_label(child))
            name = gp.check_result(gp.gp_widget_get_name(child))
            child_type = gp.check_result(gp.gp_widget_get_type(child))
            child_type_str = self.__widget_type_to_string(child_type)
            id = gp.check_result(gp.gp_widget_get_id(child))

            text = 'type:{}, label:{}, name:{}, id:{}'.format(child_type_str, label, name, id)

            if child_type == gp.GP_WIDGET_RANGE \
               or child_type == gp.GP_WIDGET_TOGGLE \
               or child_type == gp.GP_WIDGET_RADIO:
                value = child.get_value()
                text += " value:{}".format(value)
                
            if child_type == gp.GP_WIDGET_RADIO:
                choice_count = child.count_choices()
                if choice_count > 3:
                    for n in range(choice_count):
                        choice = child.get_choice(n)
                        text += "choice:{}".format(choice)
                else:
                    for choice in child.get_choices():
                        if choice:
                            text += "choice:{}".format(choice)       
                    

            for i in range(level):
                text = "  " + text

            print(text)
            self.print_config_recursive(child, level + 1)

    def __widget_type_to_string(self, widget_type):
        switcher = {
            0: "GP_WIDGET_WINDOW",
            1: "GP_WIDGET_SECTION",
            2: "GP_WIDGET_TEXT",
            3: "GP_WIDGET_RANGE",
            4: "GP_WIDGET_TOGGLE",
            5: "GP_WIDGET_RADIO",
            6: "GP_WIDGET_MENU",
            7: "GP_WIDGET_BUTTON",
            8: "GP_WIDGET_DATE"
        }
        return switcher.get(widget_type, "Invalid camwidget type")

    def __clamp(self, n, min_n, max_n):
        return max(min(max_n, n), min_n)