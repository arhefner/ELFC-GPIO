#!/usr/bin/env python3
"""Makes movies/walk.mov, a movie for the play example: a stick figure that
walks on the spot while the ground moves under it.

The movie is one walk cycle of two steps, 32 images, so it is a second
long at the 32 images in a second of play and loops without a jump.
An image is 1024 bytes for a 128 by 64 display: eight pages of eight
pixel rows from the top down, a byte for each column from left to
right, with the top pixel of the eight in bit 0.

    python3 mkwalk.py [movies/walk.mov]
"""
import math
import sys

WIDTH, HEIGHT = 128, 64
FRAMES = 32                 # one walk cycle

GROUND = 58                 # row of the ground line
HIP_X = 64
LEG = 13                    # length of thigh and of shin
HIP_HEIGHT = 24             # less than two legs, so the knees stay bent
STRIDE = 20                 # how far a foot moves back on the ground
LIFT = 5                    # how high a foot is lifted
TORSO = 15
ARM = 8                     # length of upper arm and of forearm
HEAD = 5                    # radius
MARK = 20                   # distance between the marks on the ground


class Image:
    def __init__(self):
        self.rows = [[0] * WIDTH for _ in range(HEIGHT)]

    def dot(self, x, y, size=2):
        """A square of size pixels with its top left at x, y."""
        x, y = int(round(x)), int(round(y))
        for dy in range(size):
            for dx in range(size):
                if 0 <= x + dx < WIDTH and 0 <= y + dy < HEIGHT:
                    self.rows[y + dy][x + dx] = 1

    def line(self, a, b, size=2):
        steps = int(max(abs(b[0] - a[0]), abs(b[1] - a[1]))) + 1
        for i in range(steps + 1):
            t = i / steps
            self.dot(a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t, size)

    def circle(self, c, radius, size=2):
        steps = 8 * radius
        for i in range(steps):
            angle = 2 * math.pi * i / steps
            self.dot(c[0] + radius * math.cos(angle),
                     c[1] + radius * math.sin(angle), size)

    def bytes(self):
        out = bytearray()
        for page in range(HEIGHT // 8):
            for x in range(WIDTH):
                out.append(sum(self.rows[page * 8 + bit][x] << bit
                               for bit in range(8)))
        return bytes(out)


def joint(a, b, len1, len2, bend):
    """The joint between two limbs of len1 and len2 that go from a to b.
    bend is 1 or -1 for the side the joint is on."""
    dx, dy = b[0] - a[0], b[1] - a[1]
    dist = min(math.hypot(dx, dy), len1 + len2 - 0.01)
    along = (len1 * len1 - len2 * len2 + dist * dist) / (2 * dist)
    off = math.sqrt(max(len1 * len1 - along * along, 0))
    ux, uy = dx / dist, dy / dist
    return (a[0] + ux * along - bend * uy * off,
            a[1] + uy * along + bend * ux * off)


def foot(phase):
    """Where a foot is, from the point on the ground below the hip.
    The figure faces right.  In the first half of the cycle the foot
    is on the ground and moves back with it, in the second half it is
    lifted and swings forward."""
    phase %= 1.0
    if phase < 0.5:
        return STRIDE / 2 - STRIDE * phase / 0.5, 0.0
    swing = (phase - 0.5) / 0.5
    ease = (1 - math.cos(math.pi * swing)) / 2
    return -STRIDE / 2 + STRIDE * ease, -LIFT * math.sin(math.pi * swing)


def frame(n):
    phase = n / FRAMES
    img = Image()

    # the ground, with marks that move back as fast as a foot on it
    img.line((0, GROUND + 2), (WIDTH - 1, GROUND + 2), 1)
    shift = 2 * STRIDE * phase
    for k in range(-1, WIDTH // MARK + 2):
        x = k * MARK - shift % MARK
        img.line((x, GROUND + 3), (x - 3, GROUND + 5), 1)

    # the body goes down a little twice in a cycle, when the feet are
    # furthest apart
    bob = 1.2 * math.cos(4 * math.pi * phase)
    hip = (HIP_X, GROUND - HIP_HEIGHT + bob)
    neck = (HIP_X + 2, hip[1] - TORSO)
    head = (neck[0] + 1, neck[1] - HEAD - 1)

    def leg(ph):
        fx, fy = foot(ph)
        ankle = (HIP_X + fx, GROUND + fy)
        knee = joint(hip, ankle, LEG, LEG, -1)
        img.line(hip, knee)
        img.line(knee, ankle)
        img.line(ankle, (ankle[0] + 3, ankle[1]))

    def arm(ph):
        swing = 0.6 * math.sin(2 * math.pi * ph)
        elbow = (neck[0] + ARM * math.sin(swing),
                 neck[1] + 2 + ARM * math.cos(swing))
        fore = swing + 0.5
        hand = (elbow[0] + ARM * math.sin(fore),
                elbow[1] + ARM * math.cos(fore))
        img.line((neck[0], neck[1] + 2), elbow)
        img.line(elbow, hand)

    # an arm swings forward when the leg of that side swings back
    leg(phase)
    leg(phase + 0.5)
    arm(phase + 0.25)
    arm(phase + 0.75)
    img.line(hip, neck)
    img.circle(head, HEAD)

    return img


def main():
    name = sys.argv[1] if len(sys.argv) > 1 else 'movies/walk.mov'
    with open(name, 'wb') as out:
        for n in range(FRAMES):
            out.write(frame(n).bytes())
    print('%s: %d images, %d bytes' % (name, FRAMES, FRAMES * WIDTH * HEIGHT // 8))


if __name__ == '__main__':
    main()
