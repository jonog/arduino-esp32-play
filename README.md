# Arduino ESP32 Play

Experiments for a Waveshare ESP32-S3 1.69inch Touch Display, including a basic upload test, a display text sketch, and a small touch game.

## Hardware

Target device:

- Waveshare ESP32-S3 1.69inch Touch Display
- LCD: ST7789, 240x280
- Touch: CST816T

## Recommended Arduino Settings

Use these as the starting settings in Arduino IDE:

| Setting | Value |
| --- | --- |
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enabled |
| Upload Mode | USB-OTG CDC, or UART0 if that fails |
| Flash Size | 16MB |
| PSRAM | OPI PSRAM / Enabled |
| Partition Scheme | 16MB Flash, large app if available |
| Upload Speed | 921600, or 460800 if upload is unreliable |

Known-good Arduino CLI FQBN used for this board:

```bash
esp32:esp32:esp32s3:USBMode=default,CDCOnBoot=cdc,UploadMode=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB,UploadSpeed=921600
```

If USB-OTG CDC upload fails, try the default UART0 / Hardware CDC upload mode:

```bash
esp32:esp32:esp32s3:USBMode=default,CDCOnBoot=cdc,UploadMode=default,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB,UploadSpeed=460800
```

## Arduino CLI Examples

List connected boards:

```bash
arduino-cli board list
```

Compile Max Says:

```bash
arduino-cli compile --fqbn 'esp32:esp32:esp32s3:USBMode=default,CDCOnBoot=cdc,UploadMode=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB,UploadSpeed=921600' max_says
```

Upload Max Says, replacing the port with the one shown by `arduino-cli board list`:

```bash
arduino-cli upload -p /dev/cu.usbmodem1020BA46AFB01 --fqbn 'esp32:esp32:esp32s3:USBMode=default,CDCOnBoot=cdc,UploadMode=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB,UploadSpeed=921600' max_says
```

Open serial monitor:

```bash
arduino-cli monitor -p /dev/cu.usbmodem1020BA46AFB01 --config baudrate=115200
```

## Sketches

- `hello_waveshare_s3`: minimal connectivity and serial hello-world sketch.
- `hello_max_display`: displays "hello Max" and cycles text colors.
- `max_says`: Simon-style touch game using the LCD quadrants.

## Notes

- If upload reports that the port is busy, close Arduino IDE's serial monitor or any other serial monitor using the device.
- The board may re-enumerate to a new `/dev/cu.usbmodem...` port after upload or reset.
