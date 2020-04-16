# qdsp
Fixed point audio library for embedded synths

I made this library for making audio come out of a teensy 3.2 without using Paul Stoffregen's Audio library, which is lovely, but I like doing things from scratch.

This library lets you do common dsp things with Q-notation - which is a way of representing and manipulating floating point numbers with fixed point numbers. See here: https://en.wikipedia.org/wiki/Q_(number_format)

So basically it helps you manipulate `qfloats` which are 32 bit ints, but represent floats. e.g.

```
typedef int32_t qfloat;
```

There are 3 modules in the library: `qfloat`, `qdsp`, and `qmusic`.

`qfloat` takes care of converting between floats and qfloats (and a secondary type called an lfloat which is less accurate than qfloat but allows for bigger numbers.

`qdsp` is a bunch of dsp classes for doing things like oscillators, filters envelopes etc.

`qmusic` is some chromatic music helpers for scales and chords etc.

Motivation: Teensy 3.2 does floating point maths a lot slower than int maths, I guess it's a hardware thing, maybe it doesn't have an fpu, or maybe it does and it's slow.
