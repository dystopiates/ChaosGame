## This file was copied from an entirely unrelated project.
## It works, that's really all I cared about. It has some
## features that don't make sense here, oh well

from threading import Thread
from tkinter import Canvas, Tk

from PIL.Image import Image
from PIL.ImageTk import PhotoImage as PilPhotoImage


class TkDisplay:
    def __init__(self, image: Image, update_interval: float):
        self._update_interval = update_interval

        self.image = image

        self.master: Tk = None
        self.canvas: Canvas = None
        self.__refresh = False

        def thread_init():
            self.master = Tk()

            self.canvas = Canvas(self.master, bg='black', width=self.image.width, height=self.image.height)
            self.canvas.pack()

            self.__redraw()

            self.master.after(round(update_interval * 1000), self.__update_thread)

            self.master.mainloop()

        self._thread = Thread(target=thread_init, daemon=True)
        self._thread.start()

    def __update_thread(self):
        update_sleep = 0.05

        # noinspection PyBroadException
        try:
            if self.__refresh:
                self.__refresh = False

                self.__redraw()
                update_sleep = self._update_interval
        except Exception:
            pass

        self.master.after(round(update_sleep * 1000), self.__update_thread)

    def __redraw(self):
        self.canvas.delete('all')
        self.__image_tk = PilPhotoImage(self.image)
        self.canvas.create_image((0, 0), anchor='nw', image=self.__image_tk, state='normal')

    def draw_image(self, image: Image, xywh=None):
        x, y = 0, 0

        if xywh is not None:
            x, y, width, height = xywh
            image = image.crop((x, y, x + width, y + height))

        self.image.paste(image, (x, y))

        self.__redraw()

    def draw_screen(self):
        pass

    def update(self):
        self.__refresh = True

    def block(self):
        self._thread.join()
