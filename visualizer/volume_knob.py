# /// script
# requires-python = ">=3.10,<3.13"
# dependencies = ["p5", "pyserial"]
# ///
"""Show the ESP32 disc rotation and use it as a system volume knob.

Each clockwise hole ("pulse dir=CW" from the ESP32) raises the volume one
step, each counter-clockwise hole lowers it. Left/Right arrow keys do the
same for testing without hardware.

Run: uv run host/volume_knob.py
"""

import math
import subprocess
import threading

import serial
from p5 import *

PORT = "/dev/ttyUSB0"
BAUD = 115200
VOLUME_STEP = "2%"
HOLES = 8  # updated from the ESP32's "holes=" field

steps = 0
direction = "STOPPED"
rpm = 0.0
angle = 0.0


def change_volume(sign):
    subprocess.run(["wpctl", "set-volume", "-l", "1.0",
                    "@DEFAULT_AUDIO_SINK@", VOLUME_STEP + sign])


def get_volume():
    output = subprocess.run(["wpctl", "get-volume", "@DEFAULT_AUDIO_SINK@"],
                            capture_output=True, text=True).stdout
    return float(output.split()[1])


volume = get_volume()


def turn(amount):
    global steps, volume
    steps += amount
    change_volume("+" if amount > 0 else "-")
    volume = get_volume()


def read_serial():
    global direction, rpm, HOLES
    with serial.Serial(PORT, BAUD) as connection:
        while True:
            line = connection.readline().decode(errors="ignore").split()
            fields = dict(token.split("=", 1) for token in line if "=" in token)
            if line and line[0] == "pulse":
                if fields["dir"] == "CW":
                    turn(1)
                elif fields["dir"] == "CCW":
                    turn(-1)
            elif "rpm" in fields:
                direction = fields["dir"]
                rpm = float(fields["rpm"])
                HOLES = int(fields["holes"])


def setup():
    size(400, 480)


def draw():
    global angle
    # Ease toward the counted position so each hole animates smoothly.
    angle += (steps * TWO_PI / HOLES - angle) * 0.2

    background(25)
    no_stroke()

    push_matrix()
    translate(200, 200)
    rotate(angle)
    fill(80)
    circle(0, 0, 340)
    fill(25)
    for i in range(HOLES):
        a = i * TWO_PI / HOLES
        circle(130 * math.cos(a), 130 * math.sin(a), 40)
    pop_matrix()

    fill(255, 200, 60)  # sensor position
    circle(200, 70, 12)

    fill(230)
    text_size(24)
    text(f"{direction}   {rpm:.1f} rpm", 20, 395)
    text(f"Volume {volume * 100:.0f}%", 20, 430)
    fill(80)
    rect(20, 455, 360, 12)
    fill(90, 200, 120)
    rect(20, 455, 360 * volume, 12)


def key_pressed(event):
    if event.key == "RIGHT":
        turn(1)
    elif event.key == "LEFT":
        turn(-1)


if __name__ == "__main__":
    threading.Thread(target=read_serial, daemon=True).start()
    run(renderer="skia")
