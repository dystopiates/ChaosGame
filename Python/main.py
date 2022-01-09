## This is the main file, it just calls the generate() function. All the
## parameters of the fractal are specified in the generate() impl.
##
## There is also another "mode" I ended up writing, where I generate
## the fractal in C and only use Python to display the final image.
## I've commented out that mode, assuming you just want to run the
## python bit and see pretty pictures.

import time

from PIL import Image

import chaos
from tkdisplay import TkDisplay


def main():
    now = time.time()

    ## This uses the alternate generate() impl, that reads pre-rendered data from the C program
    # image = chaos.generate('/dev/shm/out.render', 40000)

    image = Image.new(size=(1000, 1000), mode='L')
    image = chaos.generate(image)

    image.save('output.png', format='PNG')

    ## This TkDisplay class was actually copied from an entirely different project,
    ## but it was useful and I just copied it here.
    display = TkDisplay(image, 100)
    print(f'{time.time() - now} seconds to display')
    display.block()


if __name__ == '__main__':
    main()
