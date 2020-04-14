## Dithering Profiles used in ReSampler

- Generally, the noise shaping becomes more intense (has higher amplitude) as the Dither Profile ID increases,
with the ID of 0 corresponding to *flat* (no noise shaping; which is also equivalent to using the **--flat-tpdf** switch).

- As the dither profile ID increases, the noise shapes are designed to have progressively less *perceived noise* (despite actually having *more* noise added to the output). 

- The noise source for all dither profiles is TPDF noise (Triangular Probability Density Function).

- Most of these noise-shaping curves are designed for 44.1kHz playback. Curves for other samplerates may be provided in future versions.

- Although the noise-shaping curves are primarily designed for 44.1kHz, you can use any dither profile with any sampling rate, with the result being that the curve will be 'stretched' to scale with the sampling rate, which means that the notches and bumps may no longer be in their optimal positions.
*Nevertheless, for very high sample rates (88.2kHz+), ID 1 or ID 2 work well in practice*

*Final Note: At normal listening levels, the effects of dithering are quite noticeable with 8-bit audio, very subtle with 16-bit, and most likely completely insignificant for 24-bit* 

<table>
    <thead>
        <tr>
            <th>ID</th>
            <th>Name</th>
            <th>Description</th>
            <th>Comments</th>
        </tr>
    </thead>
    <tbody>
        <tr><td>0</td><td>flat tpdf</td><td>no noise shaping</td><td>no error correction feedback</td></tr>
        <tr><td>1</td><td>classic</td><td>subtle noise shaping (HF emphasis)</td><td>original noise shaping from older versions of ReSampler</td></tr>
        <tr><td>2</td><td>flat tpdf (with error-correction feedback)</td><td>"violet" TPDF noise</td><td>error correction feedback loop results in low frequency attenuation, and high frequency emphasis, with first-order (6dB/oct) magnitude response</td></tr>
        <tr><td>3</td><td>Modified E-Weighted</td><td>notch in 3-4kHz range (which our ears are sensitive to)</td><td>from the paper "Minimally Audible Noise Shaping"</td></tr>
        <tr><td>4</td><td>Wannamaker 3-tap</td><td>simple f-weighted curve, with notch around 4kHz</td><td>from the paper "Psychoacoustically Optimal Noise Shaping"</td></tr>
        <tr><td>5</td><td>Lipshitz</td><td>e-weighted curve with notches around 4k and 12k</td><td>from the paper "Minimally Audible Noise Shaping"</td></tr>
        <tr><td><b>6</b></td><td><b>standard</b></td><td><b>Smooth curve with notches at 3150 and 11250Hz, and HF emphasis</b></td><td><b>default noise shape. Very smooth curve - no sharp notches or bumps. Good balance between noise <i>audibility</i> and noise <i>amplitude</i> (Works well for most material)</b></td></tr>
        <tr><td>7</td><td>Wannamaker 24-tap</td><td>notches around 3.5kHz and 12kHz</td><td>from the paper "Psychoacoustically Optimal Noise Shaping"</td></tr>
        <tr><td>8</td><td>Wannamaker 9-tap</td><td>notches around 3.5kHz and 12kHz</td><td>from the paper "Psychoacoustically Optimal Noise Shaping". (This is the "f-weighted" noise-shaping curve from SoX)</td></tr>
        <tr><td>9</td><td>High28</td><td>notches at 3150Hz and 11.25kHz with 28dB high shelf</td><td></td></tr>
        <tr><td>10</td><td>Improved E-Weighted</td><td>widely used in many DAWs and audio software</td><td>from the paper "Minimally Audible Noise Shaping"</td></tr>
        <tr><td>11</td><td>High30</td><td>notches at 3150Hz and 11.25kHz with 30dB high shelf</td><td>sounds great to older listeners, but high-frequencies may annoy younger listeners. Nevertheless, at the intended playback volume, the high-frequency components will still be very quiet relative to the program material</td></tr>
        <tr><td>12</td><td>High32</td><td>notches at 3150Hz and 11.25kHz with 32dB high shelf</td><td>sounds great to older listeners, but high-frequencies may annoy younger listeners. Nevertheless, at the intended playback volume, the high-frequency components will still be very quiet relative to the program material</td></tr>
        <tr><td>13</td><td>Blue Noise</td><td>3dB/octave increase in amplitude vs frequency</td><td>in-between white(flat) noise and violet noise</td></tr>
        <tr><td>14</td><td>Flat RPDF</td><td>Random dither with rectangular probability density function (RPDF)</td><td>No Noise shaping. No Feedback</td></tr>
        <tr><td>15</td><td>Flat RPDF (with error-correction feedback)</td><td>Random dither with rectangular probability density function (RPDF)</td><td>With error-correction feedback</td></tr>
    </tbody>
</table>

### Frequency Response Plots

![Frequency Response Plots](/NoiseShapingPlots/ns.png)

A list of dither profiles can be obtained by using the **--showDitherProfiles** option.

### References

[[1] Lipshitz, Stanley P. and Vanderkooy, John and Wannamaker, Robert A. “Minimally Audible Noise Shaping" J. Audio Eng. Soc., vol. 39(11), pp. 836-852 (1991.).](http://www.aes.org/e-lib/browse.cfm?elib=5956)

[[2] Wannamaker, Robert A. "Psychoacoustically Optimal Noise Shaping" J. Audio Eng. Soc., vol. 40(7/8), pp. 611-620 (1992.).](http://www.aes.org/e-lib/browse.cfm?elib=7039)