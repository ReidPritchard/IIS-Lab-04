# IIS Lab 04 - 3D Printed Interaction

[![Demo video](https://img.youtube.com/vi/oskhS9dLf7Y/0.jpg)](https://youtu.be/oskhS9dLf7Y)

For this lab I tried to make a rotary encoder using a photoresistor and a fancy 3d printed enclosure.

The idea is pretty simple. The photoresistor is placed beneath a rotating disk with tear shaped holes. As the disk rotates and the holes pass over the photoresistor, the resistance changes. The tear shaped holes are oriented along the disk's curvature. This means we can detect the direction of rotation by looking at if the gradually builds and then drops or if it quickly builds and then gradually drops (or is the peak of the signal at the beginning or end of the hole).

## The 3D Print

I actually tried to use SolidCraft for the modeling. It worked well enough, but ran into some issues with beveling two intersecting edges (aka it's not supported yet). So the model is a little rough in places. I also ran into issues with my slicer. I somehow lost all my presets :( SolidCraft's exports also seem to be a little broken. No matter the format or even using the "send to slicer" button, the imported model is the wrong size. Maybe that's a skill issue though. I ended up scaling the model in the slicer 2x to get it to the right size.

To be honest, I didn't get to put as much time into this lab as I would have liked. This specifically I wanted to spend more time with. I think experimenting with different hole shapes, sizes, and layouts would be interesting. I don't currently have an LED across from the photoresistor to make it more consistent, but I may add one by tomorrow (it would be nice if the enclosure had a better place for it). Last semester I made some similar circular, rotating, 3D printed gear/mechanisms, and for that final model I had included ball bearings connected on the center shaft (the experience of actually spinning this thing is a real pain).

## The Code

**ESP32 Code:**

Main file: `IIS-04-print3d-interaction/IIS-04-print3d-interaction.ino`

Contains the code for reading the photoresistor and determining the direction and speed of rotation.

**Python Code:**

`visualizer/volume_knob.py` - a simple p5py visualizer that reads the findings from the ESP32 and just displays them and based on that will also adjust your volume up or down (via `wpctl`, so Linux with PipeWire).

This is run after the ESP32 code is running and connected to the computer via USB. It's a `uv` script with its dependencies inline, so just run:

```bash
uv run visualizer/volume_knob.py
```

Or with a regular venv:

```bash
python -m venv .venv
source .venv/bin/activate
pip install p5 pyserial
python visualizer/volume_knob.py
```

## AI Usage

As for AI, I did use it.

For `IIS-04-print3d-interaction.ino`, I initially wrote out a rough (and broken) version of the code. I then used Claude to fix the code and then refactor it to improve readability. I also had it improve the serial output.

For `visualizer/volume_knob.py`, I just vibe coded it. While the visual was nice to have, I was more interested in the actual detection of the rotation and direction. So, I decided it was an alright use of AI to give myself more time on the ESP32 code. I feel like vibe coding is like a poisoned apple and each time I take a bite, it's slowly killing both my ability to code and maybe my brain. But then, because of vibe coding and reverse engineer anything, we get stuff like SolidCraft, which has potential to be an amazing free Fusion 360 alternative. So, I guess it's a double-edged sword.
