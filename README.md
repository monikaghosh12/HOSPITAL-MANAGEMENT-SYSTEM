cat << 'EOF' > README.md
# Embedded Hospital Management System with Linux Character Device Driver

An integrated C++17 Hospital Management System (HMS) coupled with a custom Linux Character Device Driver simulating live bedside ICU telemetry.

## Architecture
- **Kernel Space (`driver/`):** Character device driver registered at `/dev/vitals_sensor` providing randomized vital telemetry via `copy_to_user()`.
- **User Space (`app/`):** C++17 console management system performing POSIX I/O reads against the character device alongside patient, appointment, inventory, and billing workflows.

## Prerequisites
- Linux Kernel headers matching the running OS
- GCC / G++ (`build-essential`)
- Make

## Build & Execution Steps

### 1. Build and Insert Kernel Driver
```bash
cd driver
make
sudo insmod vitals_driver.ko
sudo chmod 666 /dev/vitals_sensor