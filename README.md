# BoatGaugesAlt
The alternative example sketch for six different boat gauges came about due to an issue with original code.

![image](https://user-images.githubusercontent.com/6739564/173200118-24206e48-b5ac-4df7-a65d-6add535acb31.png)

Which I also came across and reported it under original project [here](https://github.com/VolosR/BoatGauges/issues/1).

The solution boils down to removing reliance on the needle bounding box coordinates which are pre-populate in the ``setup()`` method:
```
  while (a != 44) {
    x[i] = r * cos(rad * a) + sx;
    y[i] = r * sin(rad * a) + sy;
    x2[i] = (r - 20) * cos(rad * a) + sx;
    y2[i] = (r - 20) * sin(rad * a) + sy;

    i++;
    a++;

    // reset "a" on overflow
    if (a == 360) {
      a = 0;
  }
```

## Code repositories
- For live demonstration visit [https://www.youtube.com/watch?v=y_H7HM0oyoo]
- For original code visit [VolosR/BoatGauges](https://github.com/VolosR/BoatGauges)
- For original code with comments visit [iiminov/BoatGauges](https://github.com/iiminov/BoatGauges)

# Arduino IDE Setup
1. Install ESP32 boards library [guide from randomnerdtutorials](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
2. Install TFT_eSPI library by Bodmer through Library Manager (Sketch > Include Library > Manage Libraries or Ctrl+Shift+l)

![image](https://user-images.githubusercontent.com/6739564/173196508-e732ded0-ca3b-419f-80a7-bce4eaf675de.png)


# Configure TFT_eSPI library
Open ``..\Documents\Arduino\libraries\TFT_eSPI\User_Setup.h`` and comment out line 44:
```
//#define ILI9341_DRIVER
```
On line 64 uncomment GC9A01 driver:
```
#define GC9A01_DRIVER
```

Open ``..\Documents\Arduino\libraries\TFT_eSPI\User_Setup_Select.h`` and comment out line 24:
```
//#include <User_Setups/Setup1_ILI9341.h>
```
On line 105 uncomment GC9A01 setup:
```
#include <User_Setups/Setup200_GC9A01.h>
```

Open ``..\Documents\Arduino\libraries\TFT_eSPI\User_Setups\Setup200_GC9A01.h`` and update pin assignments:
```
#define TFT_MISO 5
#define TFT_MOSI 2
#define TFT_SCLK 15  // Clock pin
#define TFT_CS   17  // Chip select control pin
#define TFT_DC   16  // Data Command control pin
#define TFT_RST  4   // Reset pin (could connect to Arduino RESET pin)
```

# ESP32 & GC9A01 Connections

![image](https://user-images.githubusercontent.com/6739564/172683255-a640ba47-1d2f-4fc0-b8f3-acbada262f81.png)
> Image is courtesy of WOKWI.com and some MS Paint work

```
ESP32     GC9A01
3V3       VCC
GND       GND
D15       SCL
D2        SDA
D4        RES
RX2       DC
TX2       CS
D5        BLK
```
