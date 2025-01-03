#include <pulse/simple.h>
#include <pulse/error.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

int play_sound() {

    pa_sample_spec ss = {
        .format = PA_SAMPLE_S16NE,
        .rate = 44100,
        .channels = 2
    };

    pa_simple *s = NULL;
    int error;

    // Connect to PulseAudio
    if (!(s = pa_simple_new(NULL, "Play_alert", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, "PulseAudio error: %s\n", pa_strerror(error));
        return 1;
    }

    // Generate and play a simple tone
    int16_t buffer[44100];
    for (int i = 0; i < 44100; i++)
        buffer[i] = (int16_t)(32767 * sin((2.0 * M_PI * 440.0 * i) / 44100));

    if (pa_simple_write(s, buffer, sizeof(buffer), &error) < 0) {
        fprintf(stderr, "PulseAudio error: %s\n", pa_strerror(error));
        return 1;
    }

    // Clean up
    //pa_simple_drain(s, &error);
    pa_simple_free(s);

    return 0;
}


