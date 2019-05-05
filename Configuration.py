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


class WhiteBalance(Enum):
    WB_AUTO = "0"
    WB_DAYLIGHT = "1"
    WB_CLOUDY = "2"
    WB_ARTIFICIAL = "3"
    WB_NEON = "4"
    WB_NEON_2 = "5"
    WB_FLASH = "6"
    WB_MANUAL = "7" #TODO triggers manual wb, how does that work.

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


class ShootingMode(Enum):
    AUTO = "Auto"
    TV = "TV"
    AV = "AV"
    MANUAL = "Manual"


class Configuration:

    def __init__(self, camera):
        self.camera = camera
        self.config = gp.check_result(gp.gp_camera_get_config(self.camera))

        self.set_shootingmode(ShootingMode.AV) #needs to be done first, otherwise some settings are read only
        self.set_zoom(0)
        #self.print_config()
        self.set_iso(Iso.ISO_100)
        self.set_white_balance(WhiteBalance.WB_AUTO)
        self.set_aperture(Aperature.AP_7_1)

    def set_shootingmode(self, mode: ShootingMode):
        mode_widget = self.find_config_widget_by_name("shootingmode")
        mode_widget.set_value(mode.value)
        self.camera.set_config(self.config)


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
            print("READ ONLY")
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