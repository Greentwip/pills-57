#!/usr/bin/python

import sys, struct, string, os, getopt

def rgb2s(a):
  return ((ord(a[2]) >> 3) << 11) | (ord(a[1])>>3) << 6 | (ord(a[0]) >>3)<< 1
  
def usage():
  print "Usage: %s [options] <files...>" % os.path.basename(sys.argv[0])
  print "  --TI <index> Set <index> to the transparent color"
  print "  --TC <r,g,b> Set <r,g,b> to the transparent color"
  print "  --RV         Flip the image vertically"
  
reverse = 1
tindex = -1
tcolor = 0x10000

try:
  optlist, args = getopt.getopt(sys.argv[1:], "ri:c:", ["RV", "TI=", "TC="])
except getopt.GetoptError:
  usage()
  sys.exit(-1)

for o, a in optlist:
  if o in ("-r", "--RV"):
    print "reversing"
    reverse = 0
  elif o in ("-i", "--TI"):
    tindex = int(a)
  elif o in ("-c", "--TC"):
    a = string.join(map(chr, map(int, a.split(','))), '')
    tcolor = rgb2s(a)

for f in args:
  try:
    i = open(f)
  except IOError:
    sys.stderr.write("Unable to open: %s\n" % f)
    continue
  header = i.read(54)
  (sig, filesize, d, offset, size, width, height, planes, bpp, comp, csize, xpix, ypix, colors, important) = struct.unpack("<2s6L2H6L", header)
  if sig != "BM":
    sys.stderr.write("Invalid BMP file: %s\n" % f)
    continue
  if f[-4:] == ".bmp":
    base = f[:-4]
  else:
    base = f

  if colors == 0:
    colors = 1 << bpp

  print "%s (%d x %d x %d)" % (f, width, height, bpp)

  lut = i.read(4*(1<<bpp))
  i.seek(offset)
  image = i.read()

  try:
    out = open("%s.txt" % base, "w")
  except IOError, reason:
    sys.stderr.write("Unable to open %s.txt: %s\n" % (base, reason))
    continue
  base = os.path.basename(base)

  out.write("#if defined(USE_TEX_ADDR)\n")
  if bpp <= 8:
    out.write("extern unsigned short %s_lut[];\n" % base)
    out.write("extern unsigned char %s_tex[];\n" % base)
  else:
    out.write("extern unsigned short %s_tex[];\n" % base)

  out.write("void *%s_tex_addr[] = {\n" % base)
  out.write("#if defined(NON_PALETTE)\n")
  out.write("        0,\n")
  out.write("#else\n")
  if bpp <= 8:
    out.write("        %s_lut,\n" % base)
  else:
    out.write("        0,\n")
  out.write("#endif\n")
  out.write("#if defined(NON_TEXTURE)\n")
  out.write("        0,\n")
  out.write("#else\n")
  out.write("        %s_tex,\n" % base)
  out.write("#endif\n")
  out.write("};\n")
  out.write("#endif\n")
  out.write("\n")
  out.write("#if defined(USE_TEX_SIZE)\n")
  if bpp > 8:
    if width * height * 2 > 4096:
      untiled = 0
    else:
      untiled = 1
    out.write("unsigned short %s_tex_size[] = { %d, %d, %d, %d, };\n" % (base, width, height, 16, untiled))
  else:
    if width * height * bpp/8 > 2048:
      untiled = 0
    else:
      untiled = 1
    out.write("unsigned short %s_tex_size[] = { %d, %d, %d, %d, };\n" % (base, width, height, bpp, untiled))
  out.write("#endif\n")
  out.write("\n")

  if bpp <= 8:
    out.write("#if !defined(NON_PALETTE)\n")
    out.write("unsigned short %s_lut[] = {\n" % base)
    for x in xrange(0, (1 << bpp)):
      if (x > colors):
        out.write("0x0000,")
      else:
        s = rgb2s(lut[x*4:x*4+4])
        if x != tindex:
          s = s | 1
        out.write("0x%04x," % s)
      if (x+1) % 16 == 0:
        out.write("\n")
    out.write("};\n")
    out.write("#endif\n")
    out.write("\n")

  out.write("#if !defined(NON_TEXTURE)\n")
  if bpp > 8:
    out.write("unsigned short %s_tex[] = {\n" % base)
  else:
    out.write("unsigned char %s_tex[] = {\n" % base)
  if reverse:
    start = height-1
    end = -1
    step = -1
  else:
    start = 0
    end = height
    step = 1
  if bpp == 4:
    for y in xrange(start, end, step):
      for x in xrange(0, width/2):
        out.write("0x%02x," % ord(image[y*width/2+x]))
      out.write("\n")
  elif bpp == 8:
    for y in xrange(start, end, step):
      for x in xrange(0, width):
        out.write("0x%02x," % ord(image[y*width+x]))
      out.write("\n")
  elif bpp == 16:
    for y in xrange(start, end, step):
      for x in xrange(0, width):
        s = (ord(image[y*width*2+x*2]) << 8) | ord(image[y*width*2+x*2+1])
        if s != tcolor:
          s = s | 1
        out.write("0x%04x," % s)
      out.write("\n")
  elif bpp == 24:
    for y in xrange(start, end, step):
      for x in xrange(0, width):
        s = rgb2s(image[y*width*3+x*3:y*width*3+x*3+3])
        if s != tcolor:
          s = s | 1
        out.write("0x%04x," % s)
      out.write("\n")
    
  out.write("};\n")
  out.write("#endif\n")
  out.write("\n")

  out.close()
