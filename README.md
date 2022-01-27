# QuadRough - Distortion Plugin

## Introduction

QuadRough is a Distortion Audio plugin effect built with the JUCE framework.

Its main features are:

* Four different distortion algorithms
* Mid/Side split Processing, distorting only the mid channel.
* Shaping the distortion using tone filters.
* Ceiling the output level to match the output knob.

## Setup

Download the QuadRough.vst3 file, available in this page, in the Windows or MacOS folder.

In order to use it in your Host system or in your DAW, consult their indications on using VST3. 

(e.g. FL Studio or Abletone Live on Windows looks for VST3 plugins in C/Programs/CommonFiles/VST3).

## Interface

![alt text](GUI.png)

* **Input Knob:** Controls the level Input.

* **Output Knob:** Controls the level Output (see: Ceiling option).

* **Drive Knob:** Controls the Amount of Distortion.

* **Dry/Wet Knob:** Controls the Mix between the original signal and the distorted one.

* **Algorithm Box:** Selects the algorithm to apply.

* **Tone Knob:** Shapes the sound before and after the distortion.

* **M/S Button:** Switchs the processing to Mid Side Processing, applying the selected distortion only to mid channel.

* **Ceiling Button:** Limits the Output level at the value selected using the Ouput knob.

* **Algorithm Visualization:** Shows the Input-Output curve of the current distortion settings.

The User Interface is scalable dragging the bottom left corner of the window.

## Features

### Distortion

QuadRough provides 4 different distortion algorithms selectable using the ComboWindow.

They are called:

* **Classic:** Tanh distortion, using compensation of the output volume.

* **Pristine:** Asymetric distortion, emphatize only certain harmonics. The algorithm is modelled on triodes analog distortion. Highly recommended to enable the ceiling option.

* **Hard:** Hard clipping.

* **Mad:** Sinusoidal Foldover, the drive knob controls both the input signal and the shape of the sinusoidal function.

### Tone

The tone knob can shape the sound before and after the distortion, attenuating or enhancing different frequency components. This feature allows to vary the distortion sound in high number of shades and tastes.

### M/S Processing

The M/S button allows to apply distortion only to the mid (mono) part of the sound, keeping the side (all stereo information) untouched. This could be particular useful in Bus processing, for example in Drums distortion.

### Ceiling

The ceiling button prevents the output signal to be uncontrolled due to non-linear distortion algorithms. If enabled, the final output volume will be at the same level as the output knob.   

