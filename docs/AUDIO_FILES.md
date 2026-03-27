# Audio Files

## :bulb: Description.

This is the overview of specifications of the structure audio files
that will be used in this project.

## Data format of .wav file.

<img src="./assets/img/wav_file_structure">

## Structure of .wav file.

<img src="./assets/img/example_wav_file_structure.png">

## Overview of the file structure.

```
chunkId = RIFF
format = WAVE
subChunk1ID = fmt
subChunk1Size = 4 bytes
audioFormat = 2 bytes
numChannels = 4 bytes
sampleRate = 4 bytes
byteRate = sampleRate * numChannels * bitsPerSample / 8
blockAlign = numChannels * bitsPerSample / 8
bitsPerSample = 2 bytes
```

## Sources

- [Specification of .wav file format.](https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html)
- [Warmer specification of .wav file format](http://soundfile.sapp.org/doc/WaveFormat/)
