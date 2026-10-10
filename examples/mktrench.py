#!/usr/bin/env python3
"""Makes movies/trench.mov, a movie for the play example: a wire frame attack
on a battle station, after the briefing room display of Star Wars.

The movie is three scenes and loops:

    the station, a wire frame globe with a dish and a trench around
    its middle, which turns and comes closer until the trench fills
    the display

    the flight along the trench to the port at its end

    a plan of the shaft from the port to the reactor, which a torpedo
    follows, and what happens then

It is 224 images, seven seconds at the 32 images in a second of play.
An image is 1024 bytes for a 128 by 64 display: eight pages of eight
pixel rows from the top down, a byte for each column from left to
right, with the top pixel of the eight in bit 0.

    python3 mktrench.py [movies/trench.mov]
"""
import math
import sys

WIDTH, HEIGHT = 128, 64

GLOBE_FRAMES = 72
TRENCH_FRAMES = 96
PLAN_FRAMES = 56


class Image:
    def __init__(self, fill=0):
        self.rows = [[fill] * WIDTH for _ in range(HEIGHT)]

    def dot(self, x, y):
        x, y = int(round(x)), int(round(y))
        if 0 <= x < WIDTH and 0 <= y < HEIGHT:
            self.rows[y][x] = 1

    def line(self, a, b):
        # keep the number of steps sane for lines that go far off the image
        steps = int(min(max(abs(b[0] - a[0]), abs(b[1] - a[1])), 2000)) + 1
        for i in range(steps + 1):
            t = i / steps
            self.dot(a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t)

    def box(self, x1, y1, x2, y2):
        self.line((x1, y1), (x2, y1))
        self.line((x2, y1), (x2, y2))
        self.line((x2, y2), (x1, y2))
        self.line((x1, y2), (x1, y1))

    def circle(self, c, radius):
        steps = max(int(8 * radius), 8)
        for i in range(steps):
            angle = 2 * math.pi * i / steps
            self.dot(c[0] + radius * math.cos(angle),
                     c[1] + radius * math.sin(angle))

    def bytes(self):
        out = bytearray()
        for page in range(HEIGHT // 8):
            for x in range(WIDTH):
                out.append(sum(self.rows[page * 8 + bit][x] << bit
                               for bit in range(8)))
        return bytes(out)


def ease(t):
    """From 0 to 1, slow at both ends."""
    return (1 - math.cos(math.pi * max(0.0, min(1.0, t)))) / 2


# The station: a globe seen from far away, so things do not get smaller
# with distance.  Only the half that faces the viewer is drawn.

GLOBE_TILT = math.radians(16)       # the top leans towards the viewer
DISH_LAT = math.radians(38)
DISH_LON = math.radians(-35)


def globe_point(lon, lat, turn):
    """A point of the globe, as x to the right, y up and z to the viewer."""
    x = math.cos(lat) * math.sin(lon + turn)
    y = math.sin(lat)
    z = math.cos(lat) * math.cos(lon + turn)
    return (x,
            y * math.cos(GLOBE_TILT) - z * math.sin(GLOBE_TILT),
            z * math.cos(GLOBE_TILT) + y * math.sin(GLOBE_TILT))


def globe_frame(n):
    t = n / GLOBE_FRAMES
    img = Image()

    # turn all the time, and come closer after a while, keeping the
    # trench where it is on the display
    turn = math.radians(-50 + 75 * t)
    radius = 26 * math.exp(3.0 * ease((t - 0.3) / 0.7))
    trench_y = 32 + 26 * math.sin(GLOBE_TILT)
    cx, cy = 64, trench_y - radius * math.sin(GLOBE_TILT)

    def curve(points):
        last = None
        for lon, lat in points:
            x, y, z = globe_point(lon, lat, turn)
            here = (cx + radius * x, cy - radius * y) if z > 0 else None
            if here and last:
                img.line(last, here)
            last = here

    steps = 90
    # the trench is two lines close together around the middle
    for lat in (-60, -30, -2.5, 2.5, 30, 60):
        curve([(2 * math.pi * i / steps, math.radians(lat))
               for i in range(steps + 1)])
    for lon in range(0, 360, 20):
        curve([(math.radians(lon), math.pi * (i / steps - 0.5))
               for i in range(steps + 1)])

    # the dish, circles around a point of the globe
    for size in (17, 9, 2):
        ring = []
        for i in range(steps + 1):
            a = 2 * math.pi * i / steps
            d = math.radians(size)
            lat = math.asin(math.sin(DISH_LAT) * math.cos(d)
                            + math.cos(DISH_LAT) * math.sin(d) * math.cos(a))
            lon = DISH_LON + math.atan2(
                math.sin(a) * math.sin(d) * math.cos(DISH_LAT),
                math.cos(d) - math.sin(DISH_LAT) * math.sin(lat))
            ring.append((lon, lat))
        curve(ring)

    img.circle((cx, cy), radius)
    return img


# The trench: seen by a pilot who flies along it.  x is to the right,
# y is up and z is ahead.

FOCAL = 40                  # pixels for a slope of one
EYE_Y = 0.55                # the pilot, above the floor of the trench
HALF_WIDTH = 1.0
WALL = 1.2                  # height of the walls
RIB = 2.0                   # distance between the ribs
NEAR = 0.25
FAR = 16.0                  # ribs further away than this are left out
SPEED = 0.5                 # distance flown in an image
PORT_Z = TRENCH_FRAMES * SPEED + 1.2    # where the port is


def trench_frame(n):
    img = Image()
    eye_z = n * SPEED
    # the pilot weaves a little
    eye_x = 0.25 * math.sin(2 * math.pi * n / 48)
    horizon = 28

    def project(p):
        x, y, z = p[0] - eye_x, p[1] - EYE_Y, p[2] - eye_z
        return (64 + FOCAL * x / z, horizon - FOCAL * y / z)

    def line3(a, b):
        """A line between two points, the part of it that is ahead."""
        za, zb = a[2] - eye_z, b[2] - eye_z
        if za < NEAR and zb < NEAR:
            return
        if za < NEAR or zb < NEAR:
            t = (NEAR - za) / (zb - za)
            cut = tuple(a[i] + (b[i] - a[i]) * t for i in range(3))
            if za < NEAR:
                a = cut
            else:
                b = cut
        img.line(project(a), project(b))

    end = PORT_Z + 1.0      # the trench ends in a wall behind the port

    # the edges of the floor and of the top of the walls
    for x in (-HALF_WIDTH, HALF_WIDTH):
        line3((x, 0, eye_z), (x, 0, end))
        line3((x, WALL, eye_z), (x, WALL, end))

    # the ribs, and panels on the walls between some of them
    first = int(eye_z / RIB)
    for k in range(first, first + int(FAR / RIB) + 1):
        z = k * RIB
        if z > end:
            break
        line3((-HALF_WIDTH, 0, z), (HALF_WIDTH, 0, z))
        for side in (-1, 1):
            x = side * HALF_WIDTH
            line3((x, 0, z), (x, WALL, z))
            line3((x, WALL, z), (side * 1.6, WALL, z))
            if (k + (side > 0)) % 3 == 0 and z + RIB <= end:
                line3((x, 0.3, z + 0.4), (x, 0.3, z + 1.6))
                line3((x, 0.8, z + 0.4), (x, 0.8, z + 1.6))
                line3((x, 0.3, z + 0.4), (x, 0.8, z + 0.4))
                line3((x, 0.3, z + 1.6), (x, 0.8, z + 1.6))

    # the wall at the end, and the port in the floor before it
    line3((-HALF_WIDTH, 0, end), (HALF_WIDTH, 0, end))
    line3((-HALF_WIDTH, WALL, end), (HALF_WIDTH, WALL, end))
    line3((-HALF_WIDTH, 0, end), (-HALF_WIDTH, WALL, end))
    line3((HALF_WIDTH, 0, end), (HALF_WIDTH, WALL, end))
    size = 0.3
    corners = [(-size, 0, PORT_Z - size), (size, 0, PORT_Z - size),
               (size, 0, PORT_Z + size), (-size, 0, PORT_Z + size)]
    for i in range(4):
        line3(corners[i], corners[(i + 1) % 4])

    # the sight of the targeting computer closes on the port
    left = PORT_Z - eye_z
    if left > NEAR:
        px, py = project((0, 0, PORT_Z))
        gap = 4 + 26 * min(left / 30.0, 1.0)
        if left > 6 or n % 4 < 2:       # it blinks when it is close
            for side in (-1, 1):
                x = px + side * gap
                img.line((x, py - 5), (x, py + 5))
                img.line((x, py - 5), (x - side * 2, py - 5))
                img.line((x, py + 5), (x - side * 2, py + 5))

    # the distance that is left, as a bar that gets shorter
    bar = int(120 * max(left, 0) / PORT_Z)
    img.box(3, 59, 124, 62)
    for x in range(4, 4 + bar):
        img.dot(x, 60)
        img.dot(x, 61)

    return img


# The plan: the station cut open, with the surface at the top, the
# trench in it, and the shaft that goes from the port down to the
# reactor.

SURFACE = 12
PORT_X = 40
CORE = (86, 42)
CORE_RADIUS = 13
BEND = (PORT_X, 42)         # where the shaft turns to the reactor

# where the torpedo goes: along the trench, down the shaft, to the core
PATH = [(4, SURFACE + 4), (PORT_X, SURFACE + 4), BEND, CORE]


def along(points, t):
    """The point at part t of the way along a path."""
    lengths = [math.hypot(b[0] - a[0], b[1] - a[1])
               for a, b in zip(points, points[1:])]
    left = t * sum(lengths)
    for (a, b), length in zip(zip(points, points[1:]), lengths):
        if left <= length:
            f = left / length
            return (a[0] + (b[0] - a[0]) * f, a[1] + (b[1] - a[1]) * f)
        left -= length
    return points[-1]


def plan_frame(n):
    fly = 30                # images for the torpedo to get to the core
    burst = n - fly

    if 14 <= burst < 18:
        return Image(1)     # the flash
    if burst >= 18:
        return Image()      # and nothing is left

    img = Image()
    if burst < 8 or n % 2:
        # the surface, with the trench as a step in it
        img.line((0, SURFACE), (PORT_X - 30, SURFACE))
        img.line((PORT_X - 30, SURFACE), (PORT_X - 30, SURFACE + 6))
        img.line((PORT_X - 30, SURFACE + 6), (PORT_X - 3, SURFACE + 6))
        img.line((PORT_X + 3, SURFACE + 6), (PORT_X + 20, SURFACE + 6))
        img.line((PORT_X + 20, SURFACE + 6), (PORT_X + 20, SURFACE))
        img.line((PORT_X + 20, SURFACE), (127, SURFACE))

        # the shaft, two lines down from the port and across to the core
        for side in (-3, 3):
            img.line((PORT_X + side, SURFACE + 6), (PORT_X + side, BEND[1] - side))
            img.line((PORT_X + side, BEND[1] - side),
                     (CORE[0] - CORE_RADIUS, BEND[1] - side))

        # the reactor
        img.circle(CORE, CORE_RADIUS)
        img.circle(CORE, 6)
        for i in range(8):
            a = math.pi * i / 4
            img.line((CORE[0] + 6 * math.cos(a), CORE[1] + 6 * math.sin(a)),
                     (CORE[0] + CORE_RADIUS * math.cos(a),
                      CORE[1] + CORE_RADIUS * math.sin(a)))

    if burst < 0:
        # the torpedo, with a short tail
        for back in range(4):
            x, y = along(PATH, max(n - back * 0.5, 0) / fly)
            img.dot(x, y)
            if back == 0:
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    img.dot(x + dx, y + dy)
    else:
        # rings that grow from the core
        for k in range(4):
            radius = (burst - k * 2) * 6
            if radius > 0:
                img.circle(CORE, radius)

    return img


def main():
    name = sys.argv[1] if len(sys.argv) > 1 else 'movies/trench.mov'
    count = 0
    with open(name, 'wb') as out:
        for scene, frames in ((globe_frame, GLOBE_FRAMES),
                              (trench_frame, TRENCH_FRAMES),
                              (plan_frame, PLAN_FRAMES)):
            for n in range(frames):
                out.write(scene(n).bytes())
                count += 1
    print('%s: %d images, %d bytes' % (name, count, count * WIDTH * HEIGHT // 8))


if __name__ == '__main__':
    main()
