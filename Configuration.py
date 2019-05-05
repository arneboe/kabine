import gphoto2 as gp

from enum import Enum


class ISO(Enum):
    ISO_AUTO = "Auto"
    ISO_80 = "80"
    ISO_100 = "100"
    ISO_200 = "200"
    ISO_400 = "400"
    ISO_800 = "800"
    ISO_1600 = "1600"

class WHITE_BALANCE(Enum):
    WB_AUTO = "0"
    WB_DAYLIGHT = "1"
    WB_CLOUDY = "2"
    WB_ARTIFICIAL = "3"
    WB_NEON = "4"
    WB_NEON_2 = "5"
    WB_FLASH = "6"
    WB_MANUAL = "7" #TODO triggers manual wb, how does that work.




class Configuration:

    def __init__(self, camera):
        self.camera = camera
        self.config = gp.check_result(gp.gp_camera_get_config(self.camera))

        #self.set_zoom(255)
        #self.print_config()
        #self.set_iso(ISO.ISO_1600)
        self.set_white_balance(WHITE_BALANCE.WB_DAYLIGHT)

    def set_zoom(self, zoom):
        zoom_widget = self.find_config_widget_by_name("zoom")
        lo, hi, inc = zoom_widget.get_range()
        zoom_widget.set_value(self.__clamp(zoom, lo, hi))
        self.camera.set_config(self.config)

    def set_iso(self, iso: ISO):
        iso_widget = self.find_config_widget_by_name("iso")
        iso_widget.set_value(iso.value)
        self.camera.set_config(self.config)

    def set_white_balance(self, wb: WHITE_BALANCE):
        wb_widget = self.find_config_widget_by_name("d013")
        wb_widget.set_value(wb.value)
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