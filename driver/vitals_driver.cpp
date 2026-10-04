#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <ctime>

#define DEVICE_PATH "/dev/vitals_sensor"

int main() {
    unlink(DEVICE_PATH);

    if (mkfifo(DEVICE_PATH, 0666) == -1) {
        std::cerr << "Failed to create device node\n";
        return 1;
    }

    chmod(DEVICE_PATH, 0666);
    std::cout << "[SUCCESS] Telemetry Driver active at " << DEVICE_PATH << "\n";

    srand(time(nullptr));

    while (true) {
        int fd = open(DEVICE_PATH, O_WRONLY);
        if (fd >= 0) {
            int hr = 70 + (rand() % 25);
            int spo2 = 95 + (rand() % 5);
            int bp = 115 + (rand() % 20);

            std::string telemetry = "HR:" + std::to_string(hr) + " bpm, SpO2:" +
                                    std::to_string(spo2) + "%, BP:" +
                                    std::to_string(bp) + " mmHg\n";

            write(fd, telemetry.c_str(), telemetry.size());
            close(fd);
        }
        sleep(1);
    }

    return 0;
}