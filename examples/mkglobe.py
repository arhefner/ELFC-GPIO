#!/usr/bin/env python3
"""Makes movies/globe.mov, a movie for the play example: the earth, turning.

The movie is one turn of the globe in 96 images, so it is three
seconds long at the 32 images in a second of play and loops without a
jump.  An image is 1024 bytes for a 128 by 64 display: eight pages of
eight pixel rows from the top down, a byte for each column from left
to right, with the top pixel of the eight in bit 0.

    python3 mkglobe.py [movies/globe.mov]

The map of the land is made from the 1:110m land polygons of Natural
Earth, naturalearthdata.com, which are in the public domain.
"""
import base64
import math
import sys
import zlib

WIDTH, HEIGHT = 128, 64
FRAMES = 96                 # one turn of the globe

CENTER_X, CENTER_Y = 63.5, 31.5
RADIUS = 30.5
TILT = math.radians(18)     # the north pole leans towards the viewer
SAMPLES = 3                 # points looked at in a pixel, each way

# The land, a bit for each 1.5 degrees of longitude and of latitude,
# in rows from the north pole down that start at 180 degrees west.
MAP_WIDTH, MAP_HEIGHT = 240, 120
LAND = zlib.decompress(base64.b64decode(
    'eNrVlr2PG0UUwN94Ha8lzO1JSMQSVtYSUkq4LidBbl3ShTKlO1JSgjg8c6K4Bt1R0vn/'
    'oMkekSKKiGtTwUSOlIZilSuycJt9zPe+sX2WqCJeYY/35/fmfc3bAXjnkrQ3kRyvEPHs'
    'BtrwpaIVQFYyAZCWhPU/+67mikqA2j6ZEFpCKgtLv7ZPRoTePzsQYKkSJiFRm2QOpuZT'
    'KdvdBnotgmr6y++XpXLbPRonibLTDPSWDKBXlJcIqaeTVHvYYA1MqZSvi38uMpkhajsN'
    'wKmmSsrjUq+qJ9+eyFSHq3Z8eu/6MTr5K7PfJ21mDHNcIXJPx3b1NnuZ1rBQSVvk2Il2'
    'RsnVvE5qOIDkb8Kw1WFqB84OmN42uyLwuoHiWi9EVsJc14pSVYtM5IY+M7FmVQfvjlVW'
    'Z0xbTk3phyADrHVF+qDz31g6GqwCrXQ1pzpGxKUpwYe924GWUJmKFYbq8t3LaTjSNRXi'
    'n0Y3XVIqAsUrsy/v6FtXX5Pq57pnbpFoa19982OoW/omisZBxNc89xER+pulhTyymccV'
    'pcbB4uJcZk43pdT2pfjq1fLcwAYItSesgLsFWvpw4A9f+GsBHzvaTvqOsuD/3ieuGxB/'
    'nGzQ91v41dFR2Jj76PLW1NN4KT0NR0h1M3P0JByk2x194xMpCP1p5rdALNboRy3MvHsN'
    'LtforRYe+cBbPA1FcXOj7dLS8mdd7qwsSMKL1uSuswxHsB/oY0zwFUqiezTaC3TZAn8S'
    'jb18AmWgDXDW0nGW1t2+l+ocsxYIZgWhEo4ZB0FNi0DfCDgELihNOl1VuUPVQlNCGZv5'
    'dq5LGKvx9yX1+oeFy7Mu1xh68YB+gIEKdabnasIQsdMq0H2bvI7qkIvcVefnQWQ5tT2b'
    'OPp8BLGupybFj47WXiqtoyaph4uXlBa2VxLXu+Pjvohpaamp7OR78TCmjY3Y0L3hFzym'
    'xlnX2SqgTMRUdDSZwoNSEJ/RzXZb9kPI5lMSr7XMfb9NaaYzlwYedSOZDjFlL2LaWJ+l'
    'dea9FW2dQF9Yt/IVTVagp/ZpcU0p9zQV7ncZUVMc3iZ+0sj+JnUTUHlZ9SiVNqOpj7+O'
    'LFtn2oGnzRbawH+nblLkdZw7Txv3ItxOrVIm/XnnEbVKvl9S+SmNKEqc2yOcJbZB0wqG'
    'HRXrl7EZ7AW67WIWjhqrN9icWN5GQ1cm1RbLwdFkS6Oycqcu7NIltNxJL8QOynZahhr+'
    'h7IzHf2dquOw+lwNnDvHPErPPpnU9uJDcUPGS+HvbiHPf+jPD8yrvVl/9w/hTninILn4'
    'afnGzbIkhuSRdIM2vp31LsOFKcMNOvb3NCbXILHcsKxM16j09xONz9d1q01XqOhDzfEd'
    'yL/mV5dH'
))

# stars, where the globe is not
STARS = [(5, 9), (17, 50), (9, 30), (24, 20), (20, 4), (30, 58),
         (99, 6), (108, 41), (121, 15), (114, 57), (103, 25), (125, 36),
         (2, 60), (95, 52), (36, 12)]


def land(lon, lat):
    """True if there is land at a longitude and latitude in radians."""
    col = int((lon / (2 * math.pi) + 0.5) * MAP_WIDTH) % MAP_WIDTH
    row = min(int((0.5 - lat / math.pi) * MAP_HEIGHT), MAP_HEIGHT - 1)
    return (LAND[row * (MAP_WIDTH // 8) + col // 8] >> (7 - col % 8)) & 1


def frame(n):
    # the earth turns to the east, so the land moves to the right and
    # the longitude in the middle of the globe goes west
    turn = -2 * math.pi * n / FRAMES

    rows = [[0] * WIDTH for _ in range(HEIGHT)]
    for x, y in STARS:
        rows[y][x] = 1

    for y in range(HEIGHT):
        for x in range(WIDTH):
            dist = math.hypot(x - CENTER_X, y - CENTER_Y)
            if dist > RADIUS + 0.5:
                continue
            if dist > RADIUS - 0.6:
                rows[y][x] = 1          # the edge of the globe
                continue

            # the part of the pixel that is land
            count = 0
            for sy in range(SAMPLES):
                for sx in range(SAMPLES):
                    px = (x + (sx + 0.5) / SAMPLES - 0.5 - CENTER_X) / RADIUS
                    py = (CENTER_Y - (y + (sy + 0.5) / SAMPLES - 0.5)) / RADIUS
                    pz = math.sqrt(max(1 - px * px - py * py, 0))
                    # lean the globe back to where it stands upright
                    uy = py * math.cos(TILT) + pz * math.sin(TILT)
                    uz = pz * math.cos(TILT) - py * math.sin(TILT)
                    count += land(math.atan2(px, uz) + turn,
                                  math.asin(max(-1, min(1, uy))))
            rows[y][x] = 1 if 2 * count > SAMPLES * SAMPLES else 0

    out = bytearray()
    for page in range(HEIGHT // 8):
        for x in range(WIDTH):
            out.append(sum(rows[page * 8 + bit][x] << bit for bit in range(8)))
    return bytes(out)


def main():
    name = sys.argv[1] if len(sys.argv) > 1 else 'movies/globe.mov'
    with open(name, 'wb') as out:
        for n in range(FRAMES):
            out.write(frame(n))
    print('%s: %d images, %d bytes' % (name, FRAMES, FRAMES * WIDTH * HEIGHT // 8))


if __name__ == '__main__':
    main()
