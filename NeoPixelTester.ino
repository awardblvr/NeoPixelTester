// Neopixel Color Tester
/*
 * Lights up single neopixel, then flashes it a few times.
 *
 *
 */

// Just SOME stuff.. You need tpo put thjis together to use it.

#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN        27


typedef struct {
    uint32_t color0;
    uint32_t color1;
    uint32_t durationMillis;
    uint8_t flashes;
    uint8_t current_color;    // always 0 (color0) or 1 (color1)
    uint32_t nextChangeMillis;
} PixelFlashEvent_t;

PixelFlashEvent_t pixelAction;
#define PIXEL_TRIG_NOW 1
// Neopixel colors
#define NeoBlack    0x000000
#define NeoWhite    0xFFFFFF
#define NeoRed      0xFF0000
#define NeoGreen    0x00FF00
#define NeoDimGreen 0x000F00
#define NeoMinGreen 0x000100
#define NeoBlue     0x0000FF
#define NeoYellow   0xFFFF00
#define NeoMagenta  0xFF00FF
#define NeoPurple   0x800080
#define NeoDarkPurple 0x2c0222
#define NeoOrange   0xFF8C00
#define NeoLime     0x00FF00
#define NeoNavyBlue 0x000080
#define NeoGray     0x696969
#define NeoSilver   0xC0C0C0
#define NeoBrown    0x8B4513

void flash_pixel(uint32_t color0, uint8_t flashes=1, uint32_t duration=500, uint32_t color1=NeoBlack);

// Only distinguishabe colors through Yellow mSeahorse case:
// uint32_t color_array[]={NeoWhite, NeoRed, NeoGreen,  NeoBlack};

void flash_pixel(uint32_t color0, uint8_t flashes, uint32_t duration, uint32_t color1)
{
    pixelAction = {color0, color1, duration, flashes, NeoBlack, PIXEL_TRIG_NOW};
}


uint32_t led_count=0, led_color_select=0, tasks_cycle=0;
uint32_t color_array[]={NeoWhite, NeoRed, NeoGreen,  NeoBlack};
char *color_array_str[]={"NeoWhite", "NeoRed", "NeoGreen",  "NeoBlack"};
uint32_t myColors = sizeof(color_array)/sizeof(uint32_t);

uint32_t max_task_cycles=20;

void task200ms(void)
{

    // if  (!(tasks_cycle++ % max_task_cycles)) {
    //     debug_printf("\nREACHED %d! Tcy:%d, c:%d (max=%d)\n", max_task_cycles, tasks_cycle, led_color_select, myColors);
    //     if (tasks_cycle > max_task_cycles) {
    //         debug_printf("reset Tcy\n",tasks_cycle, led_color_select);
    //         tasks_cycle = 0;
    //     }
    // }

    debug_printf("Tcy:%d, c:%d-%s",tasks_cycle, led_color_select, color_array_str[led_color_select]);
    if (tasks_cycle == 0) {
        pixel.setPixelColor(0, color_array[led_color_select]);
        pixel.show();
    } else if (tasks_cycle == (max_task_cycles - 7))  {
        pixel.setPixelColor(0, NeoBlack);
        pixel.show();
    } else if (tasks_cycle == (max_task_cycles - 6))  {
        flash_pixel(color_array[led_color_select], 3, 300);  //  uint32_t color0, uint8_t flashes=1, uint32_t duration=500,uint32_t color1=NeoBlack)
        pixel.show();
    } else {
        debug_printf(".");
    };
        // else  if (tasks_cycle == (max_task_cycles - 1)) {
    //    if (++led_color_select == myColors) {
    //        debug_printf("restart Colors\n");
    //        led_color_select = 0;
    //    }
    //}
    tasks_cycle++;
    //debug_printf("tasks_cycle now %d\n", tasks_cycle);
    if (!(tasks_cycle % max_task_cycles)) {
        debug_printf("\n");
        //debug_printf("\nREACHED %d! Tcy:%d, c:%d (max=%d)\n", max_task_cycles, tasks_cycle, led_color_select, myColors);
        tasks_cycle = 0;
        if (++led_color_select == myColors) {
            debug_printf("restart Colors\n");
            led_color_select = 0;
        }

    }

}


void task50ms(void)
{
    /* reference only
        typedef struct {
            uint32_t color0;
            uint32_t color1;
            uint32_t durationMillis;
            uint8_t flashes;
            uint8_t current_color;    // always 0 (color0) or 1 (color1)
            uint32_t nextChangeMillis;
        } PixelFlashEvent_t;

    */
    //static PixelFlashEvent_t last_pe={NeoBlack, NeoBlack, 0, 0, NeoBlack, PIXEL_TRIG_NOW};
    //static bool didAction=false;
    uint32_t current_millis = millis();

    //if (memcmp((const void *) &last_pe, (const void *) &pixelAction, sizeof(PixelFlashEvent_t)) != 0) {
    //    dump_pixelAction("task50ms new pe: ", pixelAction);
    //    last_pe = pixelAction;
    //}

    // can also pixel.setBrightness(50);  (before .show)

    if (pixelAction.nextChangeMillis > 0 && pixelAction.nextChangeMillis <= current_millis  ) {
        if (pixelAction.current_color == 0) {
            pixel.setPixelColor(0, pixelAction.color0);
            pixel.show();
            //debug_p("Sent color0 (");
            //debug_p(pixelAction.color0);
            //debug_pln(") to pixel");
            pixelAction.nextChangeMillis = millis() + (pixelAction.durationMillis / 2);
            pixelAction.current_color = 1;

        } else if (pixelAction.current_color == 1) {
            pixel.setPixelColor(0, pixelAction.color1);
            pixel.show();
            //debug_p("Sent color1 (");
            //debug_p(pixelAction.color1);
            //debug_pln(") to pixel");
            pixelAction.nextChangeMillis = millis() + (pixelAction.durationMillis / 2);
            pixelAction.current_color = 0 ;
            // pixelAction.flashes = pixelAction.flashes - 1;
            pixelAction.flashes -= 1;

        }
    }

    if (pixelAction.flashes == 0) {
        pixelAction.nextChangeMillis = 0;
    }
}



void setup()
{
    Serial.begin(115200);
    pixel.begin();
    pixel.setPixelColor(0, NeoMagenta);
    pixel.show();

    pinMode(BOARD_LED, OUTPUT);
    uint8_t lastBlink=3;
    for (uint8_t blinks=0; blinks < lastBlink;) {
        digitalWrite(BOARD_LED, HIGH);
        delay(200);
        digitalWrite(BOARD_LED, LOW);
        if (++blinks < lastBlink) {
            delay(200);
        }
    }

    // NOT necessary on THIS ESP32:  pinMode(NEOPIXEL_POWER, OUTPUT);
    // digitalWrite(NEOPIXEL_POWER, 1);
    pixel.begin();

}

void loop() {
    uint32_t sysTick = millis();

    // needed to manage the communication in the background!

    if (sysTick - tick50 > 50) {
        tick50 = sysTick;
        task50ms();
    }

    // only do after 30 secs, cuz if you mess something up,
    // you can't get hold of the cpu to upload a fixed sketch to!
    if ((sysTick > 30000) && (sysTick - tick200 > 200)) {
        tick200 = sysTick;
        task200ms();
    }


}