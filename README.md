## Description
If an executable exceeds the specified CPU usage limit, it will alert the user with a short sine wave at a frequency of 440 Hz

## Installation
1. Install the necessary `libpulse-dev` package:
* On Ubuntu/Debian:
    sudo apt-get update
    sudo apt-get install libpulse-dev
* On CentOS/Red Hat:
    sudo yum install epel-release
    sudo yum install pulseaudio-libs-devel

2. Place both cpu_usage_alert.c and play_sound.c in the same directory, and then compile them

3. Compilation:
    ``gcc cpu_usage_alert.c play_sound.c -o cpu_usage_alert `pkg-config --cflags --libs libpulse-simple` -lm``

## Usage
After successful compilation, you will get the cpu_usage_alert binary.
Place it in your $PATH or run it directly from the directory.

Run the program as follows:

`cpu_usage_alert` pid_of_executable cpu_limit_in_decimal polling_frequency_in_seconds

When the executable equals or exceeds the specified CPU usage limit, the program will produce a short sound and display the current usage percentage in the console.
