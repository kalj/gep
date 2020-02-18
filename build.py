#!/usr/bin/env python3
#

import argparse
import subprocess
import os

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('-v','--verbose', help='Enable verbose output.', action='store_true')
    parser.add_argument('-d','--device', help='Specify serial device.', default='/dev/ttyUSB0')
    parser.add_argument('-b','--board', help='Specify board type.', choices=['nano', 'mega'], default='mega')
    parser.add_argument("command", help="Specify command.", choices=["build", "upload"])

    pargs = parser.parse_args()

    sketch="gep.ino"

    if pargs.board == 'nano':
        board_id="arduino:avr:nano"
    else:
        board_id="arduino:avr:mega"

    build_dir=os.path.abspath("build-"+pargs.board)

    if pargs.command == "build":
        args = ["compile", "--fqbn", board_id, "--build-path", build_dir, "--build-cache-path", build_dir, "--warnings", "default"]

        if pargs.board == "mega":
            args.extend(["--build-properties", "build.extra_flags=-DMEGA_SHIELD"])

        args.append(sketch)

    elif pargs.command == "upload":
        hexfile = build_dir+"/"+sketch+".hex"
        args = ["upload", "--fqbn", board_id, "--port", pargs.device, "--input", hexfile]

    if pargs.verbose:
        args.insert(0,"-v")

    subprocess.run(["arduino-cli"]+args)
