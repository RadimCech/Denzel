#include <jack/jack.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <atomic>
#include <thread>

jack_client_t *client;
jack_port_t *input_left, *input_right;
jack_port_t *output_left, *output_right;

// -------------------------------------------------------
// THE CALLBACK — called by JACK in real-time for every
// audio buffer (e.g. 512 samples at 48kHz = ~10ms)
// -------------------------------------------------------
int process(jack_nframes_t nframes, void *arg) {
    float *volume = static_cast<float *>(arg);
    float *in_l  = static_cast<float *>(jack_port_get_buffer(input_left,   nframes));
    float *in_r  = static_cast<float *>(jack_port_get_buffer(input_right,  nframes));
    float *out_l = static_cast<float *>(jack_port_get_buffer(output_left,  nframes));
    float *out_r = static_cast<float *>(jack_port_get_buffer(output_right, nframes));

    for (jack_nframes_t i = 0; i < nframes; i++) {
        out_l[i] = in_l[i] * *volume;
        out_r[i] = in_r[i] * *volume;
    }

    return 0;
}

void openJackClient(float *volume, std::atomic<bool> *running) {
    client = jack_client_open("Denzel-node", JackNullOption, NULL);
    if (!client) { fprintf(stderr, "JACK server not running\n"); return; }

    jack_set_process_callback(client, process, volume);

    input_left   = jack_port_register(client, "in_L",  JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput,  0);
    input_right  = jack_port_register(client, "in_R",  JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput,  0);
    output_left  = jack_port_register(client, "out_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_right = jack_port_register(client, "out_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    jack_activate(client);

    printf("Node running. Connect ports with 'jack_connect' or QjackCtl.\n");

    while (running->load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    jack_client_close(client);
}
