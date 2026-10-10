#!/usr/bin/env python3
"""Makes an image for the show example from a picture, dithered to
black and white.  It needs the Pillow package.

    python3 mkimage.py picture image [--portrait] [--crop l,t,r,b]
                       [--gamma g] [--contrast c]

The image is 1024 bytes for a 128 by 64 display: eight pages of eight
pixel rows from the top down, a byte for each column from left to
right, with the top pixel of the eight in bit 0.

With --portrait the picture is 64 wide and 128 high, and is upright
when the display is turned so that its left edge is up.

The picture is cut to the shape of the display around its middle, or
give the part to use with --crop, in pixels of the picture.  A gamma
below 1 makes the picture lighter and above 1 darker.

images/mona.img was made from the picture of the Mona Lisa at Wikimedia
Commons, which is in the public domain, 500 pixels wide, with

    python3 mkimage.py mona.jpg images/mona.img --portrait --crop 95,40,415,680
"""
import argparse

from PIL import Image, ImageEnhance, ImageFilter, ImageOps

WIDTH, HEIGHT = 128, 64


def main():
    parser = argparse.ArgumentParser(description='make an image for show')
    parser.add_argument('picture')
    parser.add_argument('image')
    parser.add_argument('--portrait', action='store_true')
    parser.add_argument('--crop')
    parser.add_argument('--gamma', type=float, default=1.0)
    parser.add_argument('--contrast', type=float, default=0.95)
    args = parser.parse_args()

    width, height = (HEIGHT, WIDTH) if args.portrait else (WIDTH, HEIGHT)

    pic = Image.open(args.picture).convert('L')
    if args.crop:
        pic = pic.crop(tuple(int(v) for v in args.crop.split(',')))
    pic = ImageOps.fit(pic, (width, height), Image.LANCZOS)

    # use all of black to white, then the changes that were asked for
    pic = ImageOps.autocontrast(pic, cutoff=1)
    pic = pic.point(lambda v: int(255 * (v / 255.0) ** args.gamma))
    pic = ImageEnhance.Contrast(pic).enhance(args.contrast)
    pic = pic.filter(ImageFilter.UnsharpMask(radius=1, percent=120,
                                             threshold=0))

    # to black and white, with the error of a pixel spread over the
    # ones next to it
    pixels = pic.convert('1').load()

    def lit(x, y):
        if args.portrait:
            # the top of the picture is the left edge of the display
            return pixels[HEIGHT - 1 - y, x]
        return pixels[x, y]

    out = bytearray()
    for page in range(HEIGHT // 8):
        for x in range(WIDTH):
            out.append(sum((1 if lit(x, page * 8 + bit) else 0) << bit
                           for bit in range(8)))
    with open(args.image, 'wb') as f:
        f.write(out)


if __name__ == '__main__':
    main()
