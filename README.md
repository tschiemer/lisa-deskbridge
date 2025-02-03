# lisa-deskbridge
Bridging custom controls to [L-ISA Controller](https://www.l-acoustics.com/products/l-isa-controller/)

Most immediate goal is to use existing/configurable controls of an Allen & Heath SQ6 mixing desk to control L-ISA Controller parameters, but in principle other controls could be used - or realized.

Open for other bridge adaptations ;)

https://github.com/tschiemer/lisa-deskbridge

## Compilation notes for Apple silicon 

The original `oscpack` (currently linked) will fail on Apple silicon. 

Use the changes as per [this pull request here](https://github.com/RossBencina/oscpack/pull/22).

## Usage

```shell
Usage: ./lisa-deskbridge [-h|-?] [-v] [(-o|--bridge-opt <key1>=<value1>)*] [...] [<bridge>]
Bridge different custom control elements to comfortably control L-ISA Controller.
For further documentation see https://github.com/tschiemer/lisa-deskbridge

Arguments:
	 <bridge>                Bridge to use: Generic, SQ6

Options:
	 -h, -?                  Show this help
	 -v                      Verbose output
	 -p,--local-port         Local port to receive L-ISA Controller OSC messages (default: 9000)
	 --lisa-host             L-ISA Controller host/ip (default: 127.0.0.1)
	 --lisa-port             L-ISA Controller port (default: 8880)
	 -o,--bridge-opt         Pass (multiple) options to bridge using form 'key=value'

Bridge options:
	SQ6 Options:
		 midiin    Name of MIDI In port to use (default: 'MIDI Control 1')
		 midiout    Name of MIDI out port to use (default: 'MIDI Control 1')


Examples:
./lisa-deskbridge SQ6 #to use SQ6 bridge with default options
./lisa-deskbridge -p 9000 --lisa-port 8880 --lisa-host 127.0.0.1 -o "midiin=MIDI Control 1" -o "midiout=MIDI Control 1" SQ6  # to use SQ6 bridge with custom options (which happen to be the default ones)
```

## Bridges

### Generic

Create a virtual MIDI device `L-ISA Deskbridge` and optionally connects to MIDI in/out ports.

| Type           | Channel | Value1   | Value2    | Function                                      |
|----------------|---------|----------|-----------|-----------------------------------------------|
| Note On        | 1       | N (1-96) |           | Select source N                               |
| Note On        | 2       | N (1-96) |           | Select group N                                |
| Note On        | 3       | N        |           | Fire snapshot N                               |
| Note On        | 4       |          |           | TBD                                           |
| Note On        | 5       |          |           | TBD                                           |
| Control Change | 1       | 1        | relative* | Relative pan of selected sources              |
| Control Change | 1       | 2        | relative* | Relative width of selected sources            |
| Control Change | 1       | 3        | relative* | Relative depth of selected sources            | 
| Control Change | 1       | 4        | relative* | Relative elevation spread of selected sources | 
| Control Change | 1       | 5        | relative* | Relative pan spread of selected sources       |
| Control Change | 1       | 6        | relative* | Relative aux send of selected source S        |
| Control Change | 2       | 1        | absolute  | Master fader                                  |
| Control Change | 2       | 2        | absolute  | Reverb fader                                  |
| Control Change | 2       | 3        | absolute  | Monitor fader                                 |
| Control Change | 2       | 4        | absolute  | User fader 1                                  || Control Change | 2       | 1        | absolute  | Master fader                                  |
| Control Change | 2       | 5        | absolute  | User fader 2                                  |

*7bit signed int, ie 1-64 are positive values, 127-64 are negative values

### SQ6

Was originally designed for the use of a A&H SQ6 mixing desk where the custom controls look as follows: 

![SQ6 Demo](resources/SQ6/SQ6%20Deskbridge%20demo.jpg)

#### Setup + Technical Docs

##### Channel Selection

Creates a virtual MIDI device 'L-ISA Deskbridge' which receives MIDI note on messages that are interpreted as channel select. The semantics are as follows:

Note On Channel 1 Note N Velocity V -> Selected source N

Note: Using [Mixing Station](https://mixingstation.app/) along with a custom setup helps to get actual channel selection on the SQ6 mixing desk. Also see
the provided [settings file](resources/SQ6/MixingStation_AppSettings_MIDI_SQ6_L-ISA.msz).

##### Controls

Connects to MIDI in/out ports and uses incoming messages as commands.

Note: By default connects to 'MIDI Control 1' as created by the [A&H MIDI Control](https://www.allen-heath.com/hardware/controllers/midi-control/resources/) application. 

Realizes 4 function buttons, upto 96 group select buttons and 4 rotaries and master + reverb fader.

SQ6 configuration can be done/designed as follows:

| SQ6 Control          | MIDI           | Channel | Value1 | Value2    | Effect                                        | Condition          |
|----------------------|----------------|---------|--------|-----------|-----------------------------------------------|--------------------|
| Soft rotary button 1 | NoteOn         | 1       | 1      |           | Button 1 (no solo effect, see below)          |                    |
| Soft rotary button 2 | NoteOn         | 1       | 2      |           | Button 2 (no solo effect, see below)          |                    |
| Soft rotary button 3 | NoteOn         | 1       | 3      |           | Button 3 (no solo effect, see below)          |                    |
| Soft rotary button 4 | NoteOn         | 1       | 4      |           | ALT Button 4 (no solo effect, see below)      |                    |
| Soft rotary 1        | CC             | 1       | 1      | relative* | Relative pan of selected sources              |                    |
| Soft rotary 2        | CC             | 1       | 2      | relative* | Relative pan spread of selected sources       |                    |
| Soft rotary 3        | CC             | 1       | 3      | relative* | Relative depth of selected sources            |                    |
| Soft rotary 4        | CC             | 1       | 4      | relative* | Relative elevation spread of selected sources |                    |
| Soft rotary 2**      | CC             | 1       | 2      | relative* | Relative width of selected sources            | ALT button pressed |
| Soft rotary 4**      | CC             | 1       | 4      | relative* | Relative aux send of last one-selected source | ALT button pressed |
| Soft rotary 6***     | CC             | 1       | 6      | relative* | Relative width of selected sources            |                    |
| Soft rotary 8***     | CC             | 1       | 8      | relative* | Relative aux send of last one-selected source |                    |
| Soft button X        | NoteOn         | 2       | Y      |           | Select group Y                                |                    |
| Soft button X        | NoteOn         | 3       | Y      |           | Fire snapshot Y ****                          |                    |
| Soft button X        | Program Change | 1       | Y      |           | Fire snapshot Y ****                          |                    |
| Soft button X        | NoteOn         | 4       | 1      |           | All sources controlled by OSC                 |                    |
| Soft button X        | NoteOn         | 4       | 2      |           | All sources controlled by Snapshots           |                    |
| Soft button X        | NoteOn         | 4       | 11     |           | Fire previous snapshot                        |                    |
| Soft button X        | NoteOn         | 4       | 12     |           | Refire current snapshot                       |                    |
| Soft button X        | NoteOn         | 4       | 13     |           | Fire next snapshot                            |                    |
| MIDI Fader 1         | CC             | 2       | 0      | absolute  | Master fader                                  |                    |
| MIDI Fader 2         | CC             | 2       | 1      | absolute  | Reverb fader                                  |                    |
| MIDI Fader 3         | CC             | 2       | 2      | absolute  | Monitoring fader                              |                    |
| MIDI Fader 4         | CC             | 2       | 3      | absolute  | User fader 1                                  |                    |
| MIDI Fader 5         | CC             | 2       | 4      | absolute  | User fader 2                                  |                    |

*7bit signed int, ie 1-64 are positive values, 127-64 are negative values

**These rotaries have two functions depending on wether the ALT-Button is pressed or not.

***In case somebody has a SQ7 with 8 soft rotaries, all can be assigned directly without having to rely on the ALT button.

****At the time of this writing the used (and documented) OSC command does not L-ISA Controller (..) 

The effect of the buttons in combination with channel selects is as follows:

| Actions                                      | Effect                            |
|----------------------------------------------|-----------------------------------|
| Button 1 + Channel select N                  | Select source N                   |
| ALT + Button 1                               | Enable/Disable auto select follow |
| Button 2 + Channel select N                  | Add source N to selection         |
| ALT + Button 2 + Channel select N            | Remove souce N from selection     |
| Button 3 + Channel select N                  | Snap source N to speaker          |
| ALT + Button 3 + Channel select N            | Solo source N                     |
| ALT + Button 3 + Button 2 + Channel select N | Un-Solo source N                  |



## License

Copyright (C) 2025 Philip Tschiemer

GNU Affero General Public License v3

## Third Party

- [libremidi](https://github.com/celtera/libremidi)
- [oscpack](https://github.com/RossBencina/oscpack)

