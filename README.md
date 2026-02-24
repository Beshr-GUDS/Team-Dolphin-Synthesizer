# Team Dolphin - Simple Synthesizer
**Project 2 | Coding Camp II: Interactivity**
**German University of Digital Science**

## Team Members
- Beshr Altermthy – s-001077
- Ademola Yisau – s-001065
- Jerome Chambon – s-001068

## About
Simple Synthesizer is a software synthesizer built with openFrameworks.
The user can play musical notes by clicking on a piano keyboard or using the computer keyboard.
There are three different sound types: Sine, Square, and Triangle.

## How to Run
1. Install openFrameworks 0.12.x
2. Place the src folder in your openFrameworks/apps/myApps/ directory
3. Run the Project Generator and import the project
4. Build and run in Xcode

## Controls
| Key | Action |
|-----|--------|
| A S D F G H J K | Play white keys (C D E F G A B C5) |
| W E T Y U | Play black keys (sharps) |
| 1 / 2 / 3 | Switch between Sine / Square / Triangle |
| Mouse click | Click piano keys on screen |
| Volume slider | Drag to adjust volume |

## OOP Structure
- **Inheritance** — SineOscillator, SquareOscillator, TriangleOscillator all inherit from BaseOscillator
- **Composition** — ofApp owns a list of PianoKey objects
