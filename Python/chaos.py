## This is the file that "plays" the game and renders the final image.

import math
import random
import time
from datetime import datetime
from queue import SimpleQueue, LifoQueue
from threading import Thread
from typing import List, Tuple, Union, Iterable

from PIL.Image import Image

import index_generator


def get_corners(npoints: int) -> List[Tuple[float, float]]:
    rad = math.tau / npoints
    # rad_off = rad / 2
    rad_off = 0
    points = [(math.cos(i * rad + rad_off), math.sin(i * rad + rad_off)) for i in range(npoints)]

    return points


def get_scale(size: int, points: List[Tuple[float, float]]) -> Tuple[float, int, int]:
    min_x = min(map(lambda p: p[0], points))
    max_x = max(map(lambda p: p[0], points))
    range_x = max_x - min_x
    min_y = min(map(lambda p: p[1], points))
    max_y = max(map(lambda p: p[1], points))
    range_y = max_y - min_y

    scale = math.floor(min(size / range_x, size / range_y))
    dx = int(-scale * ((max_x + min_x) / 2)) + size // 2
    dy = int(scale * ((max_y + min_y) / 2)) + size // 2

    return scale, dx, dy


def average_points(p0: Tuple[float, float], p1: Tuple[float, float]) -> Tuple[float, float]:
    x0, y0 = p0
    x1, y1 = p1

    return (x0 + x1) * 0.5, (y0 + y1) * 0.5


def draw_counts(image: Image, counts: List[List[int]]):
    ##
    ## This is the function that takes raw pixel counts and turns it into an image.
    ## Several fractals look better with log hits, and different exponents can be used
    ## to increase contrast and make it look more dramatic.
    ##
    most = max(max(row) for row in counts)
    pixels = image.load()
    print(f'Max visitations: {most}')
    # most = math.log(most)

    for y, row in enumerate(counts):
        for x, pixel_count in enumerate(row):
            if pixel_count > 0:
                # whiteness = int(255 * (math.log(pixel_count) / most) ** 3.5)
                whiteness = int(255 * (pixel_count / most) ** 1.3)
            else:
                whiteness = 0

            # pixels[x, y] = (whiteness << 16) | (whiteness << 8) | whiteness
            pixels[x, y] = whiteness


non_neighbors_for_index = [list(set(range(5)) - {(i - 1) % 5, (i + 1) % 5}) for i in range(5)]


def iterate(
        history: List[int], point: Tuple[float, float], depth: int, corners: List[Tuple[float, float]], paths
):
    if depth < 0:
        return

    for i in paths[history[-2]][history[-1]]:
        p = average_points(point, corners[i])
        yield p
        # yield i

        history.append(i)
        yield from iterate(history, p, depth - 1, corners, paths)
        history.pop()


def iterate_scaled(
        corners: List[Union[Tuple[float, float], Tuple[int, int]]], parent_point: Tuple[float, float], depth: int,
        dfs_path: Iterable[int]
) -> Iterable[Tuple[float, float]]:
    if depth == 0:
        for p in dfs_path:
            if p == 0xff:
                break

            yield average_points(parent_point, corners[p])
    else:
        for p in dfs_path:
            if p == 0xff:
                break

            point = average_points(parent_point, corners[p])
            yield point

            yield from iterate_scaled(corners, point, depth - 1, dfs_path)


def re_render(filename: str) -> List[List[int]]:
    with open(filename, 'rb') as src:
        size = int.from_bytes(src.read(4), 'big')

        return [
            [int.from_bytes(src.read(4), 'big') for _ in range(size)]
            for _ in range(size)
        ]


def bytes_from_file(fp, chunksize=8192):
    while chunk := fp.read(chunksize):
        yield from chunk


# def _generate(data_file: str, size: int, image: Image):
#     print('Re-rendering file')
#     now = time.time()
#     counts = re_render(data_file)
#     print(f'Re-rendered {len(counts)}x{len(counts)} in {time.time() - now} seconds')
#
#     print(f'Drawing {size}x{size} image')
#     now = time.time()
#     draw_counts(image, counts)
#     print(f'Drew image in {time.time() - now} seconds')
#
#     return image
#
#
# def generate(data_file: str, size: int, threaded=False) -> Image:
#     ##
#     ## I tried multiprocessing this before switching to C generation, it didn't go well, but this was left behind.
#     ##
#     from PIL import Image as ImageClass
#     image = ImageClass.new(size=(size, size), mode='L')
#
#     if threaded:
#         thread = Thread(target=_generate, args=(data_file, size, image))
#         thread.start()
#     else:
#         _generate(data_file, size, image)
#
#     return image


##
## This code was from when I was actually generating the fractals in Python.
##
def generate(image: Image):
    size = min(image.width, image.height)

    ## This determines the regular polygon to play in, and the depth to explore
    ## the fractal to.
    npoints = 5
    depth = 5

    corners = get_corners(npoints)
    scale, dx, dy = get_scale(size, corners)
    scaled_corners = [(scale * x + dx, scale * y + dy) for (x, y) in corners]

    counts = [[0] * image.width for _ in range(image.height)]

    ## This determines the ruleset to use. In this case, I'm using a ruleset
    ## called "fancy": if the same point is chosen 2x in a row, then the next
    ## point can't be a neighbor of that point.
    paths = index_generator.explore_paths(2, npoints, index_generator.get_fancy(npoints))
    # paths = index_generator.explore_paths(5, npoints, index_generator.get_bounce(npoints))

    now = time.time()

    visited = 0

    for (x, y) in iterate([0, 0, 0, 0, 1], (float(dx), float(dy)), 8, scaled_corners, paths):
        counts[int(y)][int(x)] += 1
        visited += 1

    print(f'Visited {visited:,} pixels in {time.time() - now} seconds')

    draw_counts(image, counts)

    print(f'Generated fractal in {time.time() - now} s')

    return image
