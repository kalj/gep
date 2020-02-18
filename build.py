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

    args = parser.parse_args()

    sketch="gep.ino"

    if args.board == 'nano':
        board_id="arduino:avr:nano"
    else:
        board_id="arduino:avr:mega"

    if args.command == "build":
        command = "compile"
    elif args.command == "upload":
        command = "--upload"

    device = args.device

    build_dir="build-"+args.board

    # if not os.path.exists(build_dir):
        # os.makedirs(build_dir)

    build_args = [command, "--fqbn", board_id] #, "--build-path", build_dir, "--build-cache-path", build_dir]

    if args.board == "mega":
        build_args.extend(["--build-properties", "build.extra_flags=-DMEGA_SHIELD"])

    if args.verbose:
        build_args.insert(0,"-v")

    subprocess.run(["arduino-cli"]+build_args+[sketch])
