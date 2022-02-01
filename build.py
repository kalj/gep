#!/usr/bin/env python3
#

import argparse
import subprocess
import pathlib
import sys

ARDUINO_CLI='arduino-cli'

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('-v','--verbose', help='Enable verbose output.', action='store_true')
    parser.add_argument('-d','--device', help='Specify serial device.', default='/dev/ttyUSB0')
    parser.add_argument('-b','--board', help='Specify board type.', choices=['nano', 'mega'], default='mega')
    parser.add_argument("command", help="Specify command.", choices=["build", "upload"])

    pargs = parser.parse_args()

    sketch="gep.ino"

    if pargs.board == 'nano':
        board_id="arduino:avr:nano:cpu=atmega328old"
    else:
        board_id="arduino:avr:mega"

    output_dir=pathlib.Path("output-"+pargs.board)
    build_dir=pathlib.Path("build-"+pargs.board)

    if pargs.command == "build":
        args = ["compile", "--fqbn", board_id,  "--warnings", "default",
                '--build-cache-path', build_dir,
                '--build-path', build_dir,
                '--output-dir', output_dir]

        if pargs.board == "mega":
            args.extend(["--build-property", "build.extra_flags=-DMEGA_SHIELD"])

        args.append(sketch)

    elif pargs.command == "upload":
        hexfile = output_dir / (sketch+".hex")
        args = ["upload", "--fqbn", board_id, "--port", pargs.device, "--input-dir", output_dir]

    if pargs.verbose:
        args.insert(0,"-v")

    print("running:", ' '.join(map(str,[ARDUINO_CLI]+args)))
    ret = subprocess.run([ARDUINO_CLI]+args)
    sys.exit(ret.returncode)
