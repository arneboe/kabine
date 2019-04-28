import io

import gphoto2 as gp
import threading

from PIL import Image


class Camera:

    def __init__(self, preview_callback):
        '''
        :raises: Exception if camera init fails
        '''
        gp.check_result(gp.use_python_logging())
        self.camera = gp.check_result(gp.gp_camera_new())
        self.run_thread = False
        self.user_preview_callback = preview_callback
        gp.check_result(gp.gp_camera_init(self.camera))

    def __del__(self):
        gp.check_result(gp.gp_camera_exit(self.camera))

    def capture(self):
        '''
        Captures and returns an image. Preview Streaming has to be disabled for this to work
        :return: The captured images
        :raises: Exception in case of error.
        :raises: Exception if preview streaming is enabled.
        '''
        pass

    def _get_preview(self):
        while self.run_thread:
            camera_file = gp.check_result(gp.gp_camera_capture_preview(self.camera))
            file_data = gp.check_result(gp.gp_file_get_data_and_size(camera_file))
            self.user_preview_callback(Image.open(io.BytesIO(file_data)))

    def start_preview(self):
        '''
        Start preview streaming
        :return:
        '''
        self.run_thread = True
        self.preview_thread = threading.Thread(target=self._get_preview)
        self.preview_thread.start()

    def stop_preview(self):
        '''
        Stop the preview streaming
        :return:
        '''
        if self.run_thread:
            self.run_thread = False
            self.preview_thread.join()

    def take_picture(self):
        self.stop_preview()
        file_path = gp.check_result(gp.gp_camera_capture(self.camera, gp.GP_CAPTURE_IMAGE))
        print('Camera file path: {0}/{1}'.format(file_path.folder, file_path.name))
        camera_file = gp.check_result(gp.gp_camera_file_get(self.camera, file_path.folder, file_path.name, gp.GP_FILE_TYPE_NORMAL))
        file_data = gp.check_result(gp.gp_file_get_data_and_size(camera_file))
        gp.check_result(gp.gp_camera_file_delete(self.camera, file_path.folder, file_path.name))

        self.user_preview_callback(Image.open(io.BytesIO(file_data)))


