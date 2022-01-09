import copy
import itertools
import random
from typing import List, Callable, Tuple


def explore_paths(history_len: int, option_len: int, get_path: Callable[[List[int]], List[int]]) -> List:
    histories = [list(range(-1, option_len)) for _ in range(history_len)]

    paths = [None] * option_len
    for depth in range(history_len - 1):
        paths = [copy.deepcopy(paths) for _ in range(option_len)]

    for history in itertools.product(*histories):
        path = paths
        for h in history[:-1]:
            path = path[h]

        path[history[-1]] = get_path(history)

    return paths


def get_fancy(n) -> Callable[[List[int]], List[int]]:
    def fancy(history: List[int]) -> List[int]:
        if history[-1] == history[-2]:
            p = history[-1]

            return list(
                set(range(n)) - {(p - 1) % n, (p + 1) % n}
            )
        else:
            return list(range(n))

    return fancy


def fancy_by_history(history: Tuple[int, int]) -> List[int]:
    if history[0] != history[1]:
        return [0, 1, 2, 3, 4]
    else:
        p = history[1]
        return list(set(range(5)) - {(p - 1) % 5, (p + 1) % 5})


def get_bounce(n: int) -> Callable[[List[int]], List[int]]:
    def bounce(history: List[int]) -> List[int]:
        if history[-1] != history[-2]:
            return list(range(n))

        options = set(range(n))
        for i, (p_, p) in enumerate(zip(history[::-1][1:], history[::-1])):
            # if i == 0:
            #     continue
            i += 1

            if p_ != p:
                continue

            options -= {(history[-1] + i) % n}
            options -= {(history[-1] - i) % n}

        return list(options)

    return bounce


# def get_index_paths(npoints: int) -> List[List[List[int]]]:
#     all_indices = list(range(npoints))
#     non_neighbors_for_index = [list(set(range(npoints)) - {(i - 1) % npoints, (i + 1) % npoints}) for i in
#                                range(npoints)]
#
#     paths = []
#
#     for p__ in range(npoints):
#         paths.append([])
#         for p_ in range(npoints):
#             # paths[p__].append([(p_ - 4) % npoints, (p_ - 2) % npoints, (p_ + 2) % npoints, (p_ + 4) % npoints])
#             if p_ != p__:
#                 paths[p__].append(all_indices)
#             else:
#                 paths[p__].append(non_neighbors_for_index[p_])
#
#     return paths


# def get_possible_fancy_indices(
#         p__: int, p_: int, all_indices: List[int], non_neighbors_for_index: List[List[int]]
# ) -> List[int]:
#     if p_ != p__:
#         return all_indices
#     else:
#         return non_neighbors_for_index[p_]
#
#
# def fancy(n, m):
#     options = list(range(m))
#     nneighbor = [list(set(range(m)) - {(i - 1) % m, (i + 1) % m}) for i in range(m)]
#
#     p__ = random.choice(options)
#     p_ = random.choice(options)
#
#     for _ in range(n):
#         op = options if p_ != p__ else nneighbor[p_]
#
#         p = random.choice(op)
#
#         yield p
#
#         p__ = p_
#         p_ = p
#
#
# def nodupes(n, m):
#     options = list(list(set(range(m)) - {i}) for i in range(m))
#
#     p_ = random.randint(0, m - 1)
#
#     for _ in range(n):
#         p = random.choice(options[p_])
#
#         yield p
#
#         p_ = p
#
#
# def star_of_david(n, m):
#     assert m == 6
#
#     options = [0, 2, 4]
#     options_ = [1, 3, 5]
#
#     for _ in range(n):
#         yield random.choice(options)
#
#         options, options_ = options_, options
