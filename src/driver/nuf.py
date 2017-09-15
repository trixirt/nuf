#!/usr/bin/env python

import argparse
import os
import signal
import subprocess
import sys
import tempfile

nuf_version = '0.9.2'

tmp_ll = None
tmp_s = None
tmp_o = None
aout = None

def cleanup():
    global tmp_ll
    global tmp_s
    global tmp_o
    if tmp_ll:
        os.remove(tmp_ll)
    if tmp_s:
        os.remove(tmp_s)
#    if tmp_o:
#        os.remove(tmp_o)

def handler(sig, frame):
    cleanup()
    sys.exit(1)

def print_version():
    global nuf_version
    print 'nuf version: ' + nuf_version
    
def main():
    global tmp_ll
    global aout
    cc = '/volume/hab/Linux/Ubuntu-16.04/x86_64/llvm/5.0/current/bin/clang'
    llc = '/volume/hab/Linux/Ubuntu-16.04/x86_64/llvm/5.0/current/bin/llc'
#    ld = '/volume/hab/Linux/Ubuntu-16.04/x86_64/llvm/4.0/current/bin/ld'
    ld = '/usr/bin/gcc'
    nuf1 = os.path.normpath(os.path.dirname(os.path.realpath(__file__)) + '../../../obj/nuf1')
    rt = os.path.normpath(os.path.dirname(os.path.realpath(__file__)) + '../../../obj/runtime/libnufrt.debug.a')
    
    if not os.path.exists(cc):
        print 'Fatal Error : cc ' + cc + ' does not exist'
        sys.exit(1)

    if not os.path.exists(llc):
        print 'Fatal Error : llc ' + llc + ' does not exist'
        sys.exit(1)

    if not os.path.exists(nuf1):
        print 'Fatal Error : nuf1 ' + nuf1 + ' does not exist'
        sys.exit(1)

    if not os.path.exists(rt):
        print 'Fatal Error : libnufrt ' + rt + ' does not exist'
        sys.exit(1)

    if not os.path.exists(ld):
        print 'Fatal Error : ld ' + ld + ' does not exist'
        sys.exit(1)

    cmdline = argparse.ArgumentParser()
    cmdline.add_argument('--version', help='print version and exit', action="store_true")
    cmdline.add_argument('-v', help='verbose output', action="store_true")
    cmdline.add_argument('-vv', help='very verbose output', action="store_true")
    cmdline.add_argument('-o', help='output file')
    cmdline.add_argument('-ll', help='output llvm ir', action="store_true")
    cmdline.add_argument('file', help='a forth source file', nargs='?')

    args, unknownargs = cmdline.parse_known_args()
    if unknownargs:
        a = ', '.join(unknownargs)
        print sys.argv[0] + ': unknown option ' + a + '\n'
        cmdline.print_usage()
        sys.exit(1)

    if args.version:
        print_version()
        sys.exit(0)

    # Past where not having file is ok
    if not args.file:
        print sys.argv[0] + ' must have an input file'
        cmdline.print_usage()
        sys.exit(1)

    if not os.path.exists(args.file):
        print 'Fatal Error : the input file ' + args.file + ' does not exit'
        sys.exit(1)
    else:
        args.file = os.path.abspath(args.file)

    base = os.path.splitext(args.file)[0]
    
    if args.o:
        aout = args.o
    else:
        if args.ll:
            aout = base + '.ll'
        else:
            aout = os.path.dirname(args.file) + os.sep + 'a.out'

    if args.ll:
        tmp_ll = aout
    else:
        fd, tmp_ll = tempfile.mkstemp(suffix='.ll')

    try:
        r = subprocess.check_call([nuf1, '-o', tmp_ll, args.file])
    except subprocess.CalledProcessError as e:
        cleanup()
        sys.exit (1)

    if args.v or args.vv:
        subprocess.call(['cat', tmp_ll])

    if args.ll:
        sys.exit(0)

    fd, tmp_s = tempfile.mkstemp(suffix='.s')
    try:
        r = subprocess.check_call([llc, '-o', tmp_s, tmp_ll])
    except subprocess.CalledProcessError as e:
        cleanup()
        sys.exit (1)

    if args.vv:
        subprocess.call(['cat', tmp_s])

    fd, tmp_o = tempfile.mkstemp(suffix='.o')

    try:
        r = subprocess.check_call([cc, '-c', '-o', tmp_o, tmp_s])
    except subprocess.CalledProcessError as e:
        cleanup()
        sys.exit (1)

    try:
        # r = subprocess.check_call([ld, '-verbose', '-L/lib', '-L/usr/lib', '-L/usr/lib/x86_64-linux-gnu', '-o', aout,
        #                           '/usr/lib/x86_64-linux-gnu/Scrt1.o',
        #                           '/usr/lib/x86_64-linux-gnu/crti.o', '/usr/lib/gcc/x86_64-linux-gnu/5.4.0/crtbeginS.o',
        #                           tmp_o, rt, '-lc',
        #                           '/usr/lib/gcc/x86_64-linux-gnu/5.4.0/crtendS.o', '/usr/lib/x86_64-linux-gnu/crtn.o'
        #])
        r = subprocess.check_call([ld, '-o', aout, tmp_o, rt ])
    except subprocess.CalledProcessError as e:
        cleanup()
        sys.exit (1)
        
    cleanup()
    sys.exit(0)
    
if __name__ == "__main__":
    main()
