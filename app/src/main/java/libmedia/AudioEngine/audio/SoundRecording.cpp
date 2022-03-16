/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// THIS FILE IS MODIFIED

#include "SoundRecording.h"
#include "../../waveform/AudioTools.h"
#include "../../shell/env.h"
#include "../../shell/regex_str.h"
#include <OboeDebug.h>
#include <cmath>
#include <native.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <asm/fcntl.h>
#include <fcntl.h>

#include <soxr.h>
#include <vector>

#include <ffmobile-headers/fftools_ffmpeg.h>
#include <timer.h>
#include <directories.h>

extern AudioTime GlobalTime;

void SoundRecording::renderAudio(int16_t *targetData, uint64_t totalFrames, SoundRecording *Audio){
    std::chrono::time_point start = std::chrono::steady_clock::now();
    SoundRecordingAudioData * AudioData = Audio->AudioData;
    if (mIsPlaying) {

        // Check whether we're about to reach the end of the recording
        if (!mIsLooping && mReadFrameIndex + totalFrames >= mTotalFrames) {
            totalFrames = mTotalFrames - mReadFrameIndex;
            mIsPlaying = false;
        }

        if (mReadFrameIndex == 0) {
            GlobalTime.StartOfFile = true;
            GlobalTime.update(mReadFrameIndex, AudioData);
        }
        for (int i = 0; i < totalFrames; ++i) {
            for (int j = 0; j < AudioData->channelCount; ++j) {
                targetData[(i * AudioData->channelCount) + j] = Audio->Audio[(mReadFrameIndex * AudioData->channelCount) + j];
            }

            // Increment and handle wraparound
            if (++mReadFrameIndex >= mTotalFrames) {
                GlobalTime.EndOfFile = true;
                GlobalTime.update(mReadFrameIndex, AudioData);
                mReadFrameIndex = 0;
            } else {
                GlobalTime.update(mReadFrameIndex, AudioData);
            }
        }
    } else {
        // fill with zeros to output silence
        for (int i = 0; i < totalFrames * AudioData->channelCount; ++i) {
            targetData[i] = 0;
        }
    }
    std::chrono::time_point end = std::chrono::steady_clock::now();
    // Calculating total time taken by the function.
    function_duration__renderAudioNANO = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    function_duration__renderAudioMICRO = std::chrono::duration_cast<std::chrono::microseconds>(function_duration__renderAudioNANO);
    function_duration__renderAudioMILLI = std::chrono::duration_cast<std::chrono::milliseconds>(function_duration__renderAudioMICRO);
}

extern const int16_t *WAVEFORMAUDIODATA;
extern uint64_t WAVEFORMAUDIODATATOTALFRAMES;
extern std::string WAVEFORM_LOCATION;
extern bool WAVEFORM_READY;

// class AudioResampler

class AudioResampler {
public:
    // Construct audio resampler.
    // input_rate : input sampling rate.
    // output_rate: input sampling rate.
    AudioResampler(const double input_rate, const double output_rate);
    // Process monaural audio samples,
    // converting input_rate to output_rate.
    template <typename T>
    size_t process(const T * samples_in, size_t sizeIn, T ** samples_out) {
        size_t input_size = sizeIn;
        size_t output_size;
        if (m_ratio > 1) {
            output_size = (size_t)lrint((input_size * m_ratio) + 1);
        } else {
            output_size = input_size;
        }
        *samples_out = new T[output_size];
        size_t output_length;
        soxr_error_t error;

        error = soxr_process(
                m_soxr, static_cast<soxr_in_t>(samples_in), input_size, nullptr,
                static_cast<soxr_out_t>(*samples_out), output_size, &output_length);

        if (error) {
            soxr_delete(m_soxr);
            LOGE("AudioResampler: soxr_process error: %s\n", error);
        }

        return output_length;
    }

private:
    const double m_irate;
    const double m_orate;
    const double m_ratio;
    soxr_t m_soxr;
};

// class AudioResampler

AudioResampler::AudioResampler(const double input_rate,
                               const double output_rate)
        : m_irate(input_rate), m_orate(output_rate),
          m_ratio(output_rate / input_rate) {
    soxr_error_t error;
    // Use double
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_INT16_I, SOXR_INT16_I);
    // Not steep: passband_end = 0.91132832
    soxr_quality_spec_t quality_spec =
            soxr_quality_spec((SOXR_VHQ | SOXR_LINEAR_PHASE), 0);
    soxr_runtime_spec_t runtime_spec = soxr_runtime_spec(1);

    m_soxr = soxr_create(m_irate, m_orate, 2, &error, &io_spec, &quality_spec,
                         &runtime_spec);
    if (error) {
        soxr_delete(m_soxr);
        LOGE("AudioResampler: unable to create soxr: %s\n", error);
    }
}

void SOX__RESAMPLE(const char *inFileName, int inSampleRate, const char *outFileName, int outSampleRate, int mChannelCount) {
    double const irate = static_cast<double>(inSampleRate);
    double const orate = static_cast<double>(outSampleRate);

    LOGD("INPUT  SAMPLERATE = %G", irate);
    LOGD("OUTPUT SAMPLERATE = %G", orate);

    /* Allocate resampling input and output buffers in proportion to the input
     * and output rates: */
#define buf_total_len 15000  /* In samples. */
    size_t const olen = (size_t)(orate * buf_total_len / (irate + orate) + .5);
    size_t const ilen = buf_total_len - olen;
    size_t const osize = sizeof(float), isize = osize;
    void * obuf = malloc(osize * olen);
    void * ibuf = malloc(isize * ilen);

    LOGD("OPENING INFILE...");

    // Only open a file to read. The file must exist before hand. Do not allow any changes to the file.
    FILE * IN = fopen(inFileName, "r");

    LOGD("OPENED INFILE: %p", IN);
    LOGD("OPENING OUTFILE...");

    // Create an empty file. Allow both reading and writing.
    FILE * OUT = fopen(outFileName, "w+"); // read and write

    LOGD("OPENED OUTFILE: %p", OUT);

    size_t odone, written, need_input = 1;
    soxr_error_t error;

    /* specify input and output formats: */
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_INT16_I, SOXR_INT16_I);

    /* Create a stream resampler: */
    soxr_t soxr = soxr_create(
            irate, orate, mChannelCount,             /* Input rate, output rate, # of channels. */
            &error,                         /* To report any error during creation. */
            &io_spec, NULL, NULL);                        /* Use configuration defaults.*/

    if (!error) {                         /* If all is well, run the resampler: */
        /* Resample in blocks: */
        do {
            size_t ilen1 = 0;

            if (need_input) {

                /* Read one block into the buffer, ready to be resampled: */
                ilen1 = fread(ibuf, isize, ilen, IN);

                if (!ilen1) {     /* If the is no (more) input data available, */
                    free(ibuf);     /* set ibuf to NULL, to indicate end-of-input */
                    ibuf = NULL;    /* to the resampler. */
                }
            }

            /* Copy data from the input buffer into the resampler, and resample
             * to produce as much output as is possible to the given output buffer: */
            error = soxr_process(soxr, ibuf, ilen1, NULL, obuf, olen, &odone);

            written = fwrite(obuf, osize, odone, OUT); /* Consume output.*/

            /* If the actual amount of data output is less than that requested, and
             * we have not already reached the end of the input data, then supply some
             * more input next time round the loop: */
            need_input = odone < olen && ibuf;

        } while (!error && (need_input || written));
    }
    /* Tidy up: */
    fclose(IN);
    fclose(OUT);
    soxr_delete(soxr);
    free(obuf), free(ibuf);
    /* Diagnostics: */
    LOGV("%s %s; I/O: %s\n", "SOX__RESAMPLER", soxr_strerror(error),
         ferror(stdin) || ferror(stdout)? strerror(errno) : "no error");
}

void FFMPEG_GEN_AUDIO_WAVEFORM(const char *inFilename, bool isRaw, const char *ar, const char *ac,
                               const char *outFileName) {
    LOGD("file to generate waveform from: %s", inFilename);
    env_t argv = env__new();
    argv = env__add_allow_duplicates(argv, "ffmpeg");
    if (isRaw) {
        argv = env__add_allow_duplicates(argv, "-f");
        argv = env__add_allow_duplicates(argv, "s16le");
        argv = env__add_allow_duplicates(argv, "-ar");
        argv = env__add_allow_duplicates(argv, ar);
        argv = env__add_allow_duplicates(argv, "-ac");
        argv = env__add_allow_duplicates(argv, ac);
    }
    argv = env__add_allow_duplicates(argv, "-i");
    argv = env__add_allow_duplicates(argv, inFilename);
    argv = env__add_allow_duplicates(argv, "-filter_complex");
    argv = env__add_allow_duplicates(argv, "showwavespic=s=1080x720");
    argv = env__add_allow_duplicates(argv, "-frames:v");
    argv = env__add_allow_duplicates(argv, "1");
    argv = env__add_allow_duplicates(argv, "-y"); // overwrite if exists
    argv = env__add_allow_duplicates(argv, outFileName);

    printf("executing command: ");
    env__print__as__argument__vector(argv);
    printf("\n");

    double s = now_ms();
    LOGE("Started conversion at %G milliseconds", s);
    int ret = ffmpeg_execute(env__size(argv), argv);
    double e = now_ms();
    LOGE("Ended conversion at %G milliseconds", e);
    LOGE("TIME took %G milliseconds", e - s);
    printf("\n\nffmpeg_execute returned %d\n\n", ret);
    LOGD("ffmpeg returned %d", ret);
    if (ret == 0) {
        LOGE("SIGNALING WAVEFORM IS READY");
        WAVEFORM_READY = true;
        WAVEFORM_LOCATION = std::string(outFileName);
        LOGE("SIGNALED WAVEFORM IS READY");
    }
    LOGD("waveform written to: %s", outFileName);
};

void resample(const char * inFilename, int inSampleRate, int mChannelCount, char ** outFileName, int outSampleRate) {
    if (inSampleRate == outSampleRate) {
        LOGD("input and output sample rates are the same, resampling is not required");
        size_t len = strlen(inFilename)+1;
        *outFileName = new char[len];
        memcpy(*outFileName, inFilename, len);
    } else {
        LOGD("input and output sample rates are not the same, resampling is required");

        LOGD("SOXR VERSION: %s", soxr_version());

        LOGD("INFILE = %s", inFilename);
        std::string outfilename = std::string(inFilename) + ".resampled.raw";
        const char *outFilename = outfilename.c_str();
        size_t len = strlen(outFilename)+1;
        *outFileName = new char[len];
        memcpy(*outFileName, outFilename, len);
        LOGD("OUTFILE = %s", *outFileName);

        LOGD("RESAMPLING");
        clock__time__code__block(SOX__RESAMPLE(inFilename, inSampleRate, *outFileName, outSampleRate, mChannelCount), core_print_time);
    }
}

SoundRecording * SoundRecording::loadFromPath(const char *filename, int SampleRate, int mChannelCount) {
    int16_t * out = nullptr; /* signed 16 bit int */
    char * outFileName;
    size_t outsize = 0;
    clock__time__code__block(resample(filename, 48000, mChannelCount, &outFileName, SampleRate), core_print_time);
    LOGD("READING OUTPUT FILE");
    clock__time__code__block(outsize = read__(outFileName, reinterpret_cast<char **>(&out)), core_print_time);

    LOGD("GENERATING WAVEFORM");
    std::string ar_ = std::to_string(SampleRate);
    const char * ar = ar_.c_str();
    std::string ac_ = std::to_string(mChannelCount);
    const char * ac = ac_.c_str();

    std::string cacheWaveform_ = std::string(TMPDIR) + "waveform.png";
    const char *cacheWaveform = cacheWaveform_.c_str();

    // TODO: have jni callback to load image on availability
    clock__time__code__block(FFMPEG_GEN_AUDIO_WAVEFORM(outFileName, true, ar, ac, cacheWaveform), core_print_time);

    const uint64_t totalFrames = outsize / (2 * mChannelCount);
    WAVEFORMAUDIODATATOTALFRAMES = totalFrames;
    WAVEFORMAUDIODATA = out;

    SoundRecordingAudioData * AudioData = new SoundRecordingAudioData(totalFrames, mChannelCount, SampleRate);
    AudioTime * allFrames = new AudioTime();
    allFrames->executeCallbacks = false;
    allFrames->includeTimingInformation = false;
    allFrames->update(1, AudioData);
    allFrames->update(totalFrames, AudioData);
    LOGD("Opened backing track");
    LOGD("length in human time:                              %s", allFrames->format(true));
    LOGD("length in nanoseconds:                             %lld", allFrames->nanosecondsTotal);
    LOGD("length in microseconds:                            %lld", allFrames->microsecondsTotal);
    LOGD("length in milliseconds:                            %lld", allFrames->millisecondsTotal);
    LOGD("length in seconds:                                 %lld", allFrames->secondsTotal);
    LOGD("length in minutes:                                 %lld", allFrames->minutesTotal);
    LOGD("length in hours:                                   %lld", allFrames->hoursTotal);
    LOGD("length in days:                                    %lld", allFrames->daysTotal);
    LOGD("length in weeks:                                   %lld", allFrames->weeksTotal);
    LOGD("length in months:                                  %lld", allFrames->monthsTotal);
    LOGD("length in years:                                   %lld", allFrames->yearsTotal);
    LOGD("bytes:                                             %zu", outsize);
    LOGD("frames:                                            %lld", AudioData->totalFrames);
    LOGD("sample rate:                                       %d", AudioData->sampleRate);
    LOGD("length of 1 frame at %d sample rate:", AudioData->sampleRate);
    LOGD("Human Time:                                        %s", AudioData->TimeNormal);
    LOGD("Nanoseconds:                                       %lld", AudioData->nanosecondsPerFrame);
    LOGD("Microseconds:                                      %lld", AudioData->microsecondsPerFrame);
    LOGD("Milliseconds:                                      %lld", AudioData->millisecondsPerFrame);
    LOGD("Seconds:                                           %lld", AudioData->secondsPerFrame);
    LOGD("Minutes:                                           %lld", AudioData->minutesPerFrame);
    LOGD("Hours:                                             %lld", AudioData->hoursPerFrame);
    LOGD("Days:                                              %lld", AudioData->daysPerFrame);
    LOGD("Weeks:                                             %lld", AudioData->weeksPerFrame);
    LOGD("Months:                                            %lld", AudioData->monthsPerFrame);
    LOGD("Years:                                             %lld", AudioData->yearsPerFrame);
    allFrames->executeCallbacks = true;
    allFrames->includeTimingInformation = true;
    allFrames->update(0, AudioData);
    return new SoundRecording(reinterpret_cast<int16_t *>(out), AudioData);
}

SoundRecording * SoundRecording::loadFromAssets(AAssetManager *assetManager, const char *filename, int SampleRate, int mChannelCount) {

    // Load the backing track
    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);

    if (asset == nullptr){
        LOGE("Failed to open track, filename %s", filename);
        return nullptr;
    }

    // Get the length of the track (we assume it is stereo 48kHz)
    uint64_t trackSize = static_cast<uint64_t>(AAsset_getLength(asset));

    // Load it into memory
    const int16_t *audioBuffer = static_cast<const int16_t*>(AAsset_getBuffer(asset));
    if (audioBuffer == nullptr){
        LOGE("Could not get buffer for track");
        return nullptr;
    }
    const int actualSampleRate = 48000;
    const int actualChannelCount = 2;

    const uint64_t totalFrames = trackSize / (2 * actualChannelCount);
    WAVEFORMAUDIODATATOTALFRAMES = totalFrames;
    WAVEFORMAUDIODATA = audioBuffer;

    SoundRecordingAudioData * AudioData = new SoundRecordingAudioData(totalFrames, mChannelCount, SampleRate);
    AudioTime * allFrames = new AudioTime();
    allFrames->executeCallbacks = false;
    allFrames->includeTimingInformation = false;
    allFrames->update(1, AudioData);
    allFrames->update(totalFrames, AudioData);
    LOGD("Opened backing track");
    LOGD("length in human time:                              %s", allFrames->format(true));
    LOGD("length in nanoseconds:                             %lld", allFrames->nanosecondsTotal);
    LOGD("length in microseconds:                            %lld", allFrames->microsecondsTotal);
    LOGD("length in milliseconds:                            %lld", allFrames->millisecondsTotal);
    LOGD("length in seconds:                                 %lld", allFrames->secondsTotal);
    LOGD("length in minutes:                                 %lld", allFrames->minutesTotal);
    LOGD("length in hours:                                   %lld", allFrames->hoursTotal);
    LOGD("length in days:                                    %lld", allFrames->daysTotal);
    LOGD("length in weeks:                                   %lld", allFrames->weeksTotal);
    LOGD("length in months:                                  %lld", allFrames->monthsTotal);
    LOGD("length in years:                                   %lld", allFrames->yearsTotal);
    LOGD("bytes:                                             %lld", trackSize);
    LOGD("frames:                                            %lld", AudioData->totalFrames);
    LOGD("sample rate:                                       %d", AudioData->sampleRate);
    LOGD("length of 1 frame at %d sample rate:", AudioData->sampleRate);
    LOGD("Human Time:                                        %s", AudioData->TimeNormalPerFrame);
    LOGD("Nanoseconds:                                       %lld", AudioData->nanosecondsPerFrame);
    LOGD("Microseconds:                                      %lld", AudioData->microsecondsPerFrame);
    LOGD("Milliseconds:                                      %lld", AudioData->millisecondsPerFrame);
    LOGD("Seconds:                                           %lld", AudioData->secondsPerFrame);
    LOGD("Minutes:                                           %lld", AudioData->minutesPerFrame);
    LOGD("Hours:                                             %lld", AudioData->hoursPerFrame);
    LOGD("Days:                                              %lld", AudioData->daysPerFrame);
    LOGD("Weeks:                                             %lld", AudioData->weeksPerFrame);
    LOGD("Months:                                            %lld", AudioData->monthsPerFrame);
    LOGD("Years:                                             %lld", AudioData->yearsPerFrame);
    allFrames->executeCallbacks = true;
    allFrames->includeTimingInformation = true;
    allFrames->update(0, AudioData);
    return new SoundRecording(const_cast<int16_t *>(audioBuffer), AudioData);
}
