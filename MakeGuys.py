#!/usr/bin/env python3
import numpy

width = 20
height = 20
name = 'player'


border = 0
black = 0x0000
white = 0xffff
green = 15<<5 # 0x0710
blue = 0x0000FF
red= 0xFF0000

primCol = 0x0000
secCol = 0xffff

arrColors = [ 
[primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol],
[primCol, primCol, primCol, primCol, primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol],
[primCol, primCol, primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol],
[primCol, primCol, primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol],
[primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol],
[primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol],
[secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol],
[primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol, primCol],
[primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, primCol, primCol, primCol, primCol, primCol, primCol],
[primCol, primCol, primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol],
[primCol, primCol, primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol],
[primCol, primCol, primCol, primCol, primCol, primCol, primCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol, secCol],
[primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol, primCol]]

for x in range(0, width):
  a = arrColors[x]
  a.reverse()
  arrColors[x] = a

# Initialize to zero
arr = [ [0 for x in range(0,height)] for y in range(0,width) ]


for x in range(0, width):
  for y in range(0, height):

    if x < border or x >= width-border:
      arr[x][y] = white
      continue
    if y < border or y >= height-border:
      arr[x][y] = white
      continue
    if (x // 20) % 2 == 0:
      if (y // 20) % 2 == 0:
        arr[x][y] = arrColors[x][y]
      else:
        arr[x][y] = arrColors[x][y]
    else:
      if (y // 20) % 2 == 0:
        arr[x][y] = arrColors[x][y]
      else:
        arr[x][y] = arrColors[x][y]

out = open(name + '.c','w')

# Print out the header
out.write("const struct {\n")
out.write("  unsigned int width;\n")
out.write("  unsigned int height;\n")
out.write("  unsigned int bytes_per_pixel;\n")
out.write("  unsigned char pixel_data[%d * %d * 2 + 1];\n" % (width,height))
out.write("} %s = {\n" % name)
out.write("  %d, %d, 2,\n" % (width,height))

# Print out one row at a time from top to bottom
for y in range(0, height):
  out.write('  "')
  for x in range(0, width):
    # Print each 16-bit value in little-endian format
    out.write('\\%03o\\%03o' % (arr[x][y]&0xff, arr[x][y]>>8))
  out.write('"\n')

out.write("};\n")

