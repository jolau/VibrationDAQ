# VibrationDAQ
Data Acquisition (DAQ) of the [Analog Devices ADcmXL3021](https://www.analog.com/en/products/adcmxl3021.html#product-overview) vibration sensor. The VibrationDAQ was developed as part of an internship at [Wingtra](https://wingtra.com/) by [Jonas Lauener](https://www.jolau.ch/).  

**For any technical questions, please open an issue: https://github.com/jolau/VibrationDAQ/issues/new/choose**

Copyright (c) 2020, Jonas Lauener & Wingtra AG\
[![License: MPL 2.0](https://img.shields.io/badge/License-MPL%202.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)

![VibrationDAQ](docs/vibrationdaq.jpg)

## Hardware
- Raspberry Pi 
- [Analog Devices ADcmXL3021](https://www.analog.com/en/products/adcmxl3021.html#product-overview) vibration sensor capabilities
    - [Corrected data sheet](docs/ADcmXL3021_corrected.pdf)
    - Sensor orientation:\
    ![Sensor Orientation](docs/sensor_orientation.png)
    - Mass: 13 g
    - Frequencies up to 110 kHz
    - ±50 g measurement range
    - MFFT mode: Spectral analysis through internal FFT\
    ![MFFT datapath](docs/MFFT_datapath.png)
        - 2048 bins per axis with user configurable bin sizes from 0.42 Hz to 53.7 Hz
        - Windowing options: rectangular, Hanning, flat top
    - MTC mode: Time domain capture\
    ![MTC datapath](docs/MTC_datapath.png)
        - 4096 samples per axis
- [Features a status LED](#status-led)

### Pinout Raspberry Pi
![pinout rpi](docs/rpi_pinout.png)

|       | pin | pin |       |
|-------|-----|-----|-------|
| RST 1 | 13  |     |       |
| BSY 1 | 15  | 16  | RST 2 |
| 3V3   | 17  | 18  | BSY 2 |
| MOSI  | 19  | 20  | EMPTY |
| MISO  | 21  | 22  | SYNC  |
| SCLK  | 23  | 24  | CE 1  |
| GND   | 25  | 26  | CE 2  |

The vibration sensor only works on the main SPI pins (SPI0) of the Raspberry Pi - the other SPI ports don't support SPI mode 3.

### Custom adapter board
The pins of the ADcmXL3021 vibration sensor can be accessed by using a custom adapter board. The PCB was created by using the software KiCad: [KiCad project files](ADcmXL3021_adapter/ADcmXL3021_adapter.pro)\
![adapter board](docs/adapter_board.jpg)

## Installation
_In case you want to setup your own VibrationDAQ on a Raspberry Pi:_
1. Install yaml-cpp (as described below)
2. Clone this repo.
3. `cd VibrationDAQ`
4. `mkdir build & cd build`
5. `cmake ..`
6. `sudo make install`

Make sure that you have [enabled SPI](https://www.raspberrypi-spy.co.uk/2014/08/enabling-the-spi-interface-on-the-raspberry-pi/).

You can use it now with `vibration_daq_app [full path to config.yaml]`.

#### Installation of yaml-cpp
1. `wget https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.6.3.tar.gz`
2. `tar -xvf yaml-cpp-0.6.3.tar.gz`
3. `cd yaml-cpp-yaml-cpp-0.6.3/`
4. `mkdir build`
5. `cd build`
6. `cmake .. -DYAML_BUILD_SHARED_LIBS=ON`
7. `sudo make install`

### Enable/Disable auto-start
1. Edit config.yaml path in vibration_daq.service file to own needs 
2. `sudo systemctl enable [full path to vibration_daq.service file]`    
    This automatically links the service and enables it. Replace `enable` with `disable` to disable service.

## Dependencies
Many thanks to the authors of the following libraries:
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [loguru](https://github.com/emilk/loguru)
- [c-periphery](https://github.com/vsergeev/c-periphery)
- [date](https://github.com/HowardHinnant/date)

## Usage
### Workflow
1. Connect to the Raspberry Pi and adapt the `~/Documents/config.yaml` to your requirements. Make sure that auto-start is enabled.
2. Mount the vibration sensor with the double sided tape [3M™ Adhesive Transfer Tape 950](https://www.digikey.ch/product-detail/en/3m-tc/3-4-5-950/3M9743-ND/2649288). This shouldn't distort the vibration too much.
3. Do your measurement.
4. Download the collected data over SFTP. I recommend to also download the used config file.
6. Open a vibration CSV file in Google Sheets. 
    - For FFT measurement: 
        - Hide the first two data points as these have usually very high magnitude and don't give meaningful information
        - Plot the data using a column chart.  

### Status led
If the status led is enabled in config, it will glow when running:
- Constant glow: data acquisition is running normally
- Blink: measurement completed, saving data to a new csv file

## Config file
Short primer on the syntax of yaml: https://learnxinyminutes.com/docs/yaml/

### Decimation Filter
Setting the right `decimation_factor` is crucial for getting meaningful data. Choose it according to the maximum vibration frequency you're expecting.
Of course, the decimation factor also affects the predefined FIR filters i.e. with a FACTOR_64, the LOW_PASS_10kHz is actually a 156 Hz low pass filter.  
 
| **decimation_factor** | **Effective Sample Rate, fS (SPS)** | **Effective FFT Bin Size, f_MIN (Hz)** | **Effective Maximum FFT Frequency, f_MAX (Hz)** |
|-------------------|---------------------------------|------------------------------------|---------------------------------------------|
| FACTOR_1          | 220000                          | 53.71094                           | 110000                                      |
| FACTOR_2          | 110000                          | 26.85547                           | 55000                                       |
| FACTOR_4          | 55000                           | 13.42773                           | 27500                                       |
| FACTOR_8          | 27500                           | 6.713867                           | 13750                                       |
| FACTOR_16         | 13750                           | 3.356934                           | 6875                                        |
| FACTOR_32         | 6875                            | 1.678467                           | 3437.5                                      |
| FACTOR_64         | 3437.5                          | 0.839233                           | 1718.75                                     |
| FACTOR_128        | 1718.75                         | 0.419617                           | 859.375                                     |

### Spectral average count
The `spectral_avg_count` determines the number of FFT records that the ADcmXL3021 averages when generating the final FFT result. Up to 255 records. Good for getting FFT measurements over longer time periods.

### Calculate measurement duration
#### FFT
4096 samples / (220'000 sample rate / `decimation_factor`) * `spectral_avg_count` = record time [s]

##### Example with FACTOR_64 and 255 spectral averages:
4096 samples / (220000 sample rate / 64) * 255 = 303.84 seconds = 5.06 minutes

#### MTC
4096 samples / (220'000 sample rate / `decimation_factor`) = record time [s]

### Example config with explanation
```yaml
storage_directory: "/home/pi/Documents/"
recordings_count: 2 #number of recurring measurements, infinite if == 0 
external_trigger: false # false: triggering over SPI; 
                        # true: triggering over dedicated pin, useful for triggering multiple sensor at exact same time (connect them to same pin)
external_trigger_pin: 4 # only read if external_trigger == true
status_led: true # enable/disable status led, blinks everytime a vibration file is written
status_led_pin: 21  # only read if status_led == true
sensors:
  - name: sensor1 #will be used for logging and filenames
    busy_pin: 22 #BCM pin number
    reset_pin: 27 #BCM pin number
    spi_path: "/dev/spidev0.0"
    recording_mode: MFFT # MTC and MFFT supported
    MFFT_config: &mfftConfig #only read if recording_mode == MFFT
      decimation_factor: FACTOR_2 #supported: [FACTOR_1 = 0, FACTOR_2 = 1, FACTOR_4 = 2, FACTOR_8 = 3, FACTOR_16 = 4, FACTOR_32 = 5, FACTOR_64 = 6, FACTOR_128 = 7]
      fir_filter: CUSTOM #supported: [NO_FILTER, LOW_PASS_1kHz, LOW_PASS_5kHz, LOW_PASS_10kHz, HIGH_PASS_1kHz, HIGH_PASS_5kHz, HIGH_PASS_10kHz, CUSTOM]
      custom_filter_taps: [6, 21, 53, 107, 193, 316, 480, 686, 930, 1203, 1490, 1774, 2034, 2251, 2407, 2489, 2489, 2407, 2251, 2034, 1774, 1490, 1203, 930, 686, 480, 316, 193, 107, 53, 21, 6]
      spectral_avg_count: 2 # value between 1-255
      window_setting: HANNING #supported: [RECTANGULAR, HANNING, FLAT_TOP]
    MTC_config: #only read if recording_mode == MTC
        decimation_factor: FACTOR_2
        fir_filter: CUSTOM
        custom_filter_taps: [6, 21, 53, 107, 193, 316, 480, 686, 930, 1203, 1490, 1774, 2034, 2251, 2407, 2489, 2489, 2407, 2251, 2034, 1774, 1490, 1203, 930, 686, 480, 316, 193, 107, 53, 21, 6]
  - name: sensor2
    busy_pin: 24
    reset_pin: 23
    spi_path: "/dev/spidev0.1"
    recording_mode: MFFT
    MFFT_config: *mfftConfig #copy config from sensor1 above
```

## Example data
The following data was collected on a self-made vibration bench. The bench consists of an unbalanced mass attached to an electrical motor. 
- [MFFT raw data example](docs/vibration_data_MFFT_2020-06-17T16_08_57.423_sensor1.csv)
    - Plot of MFFT data:\
       ![MFFT plot](docs/vibration_data_MFFT_2020-06-17T16_08_57.423_sensor1.png)
- [MTC raw data example](docs/vibration_data_MTC_2020-06-25T07_34_45.609_sensor1.csv)
