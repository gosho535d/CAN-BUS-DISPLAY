#include <lvgl.h>   
#include <TFT_eSPI.h>
#include "CST820.h"
#include "ui.h"

static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 320;

#define RXD2 22 
#define TXD2 21 
#define I2C_SDA 33
#define I2C_SCL 32
#define TP_RST 25
#define TP_INT 21

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

TFT_eSPI tft = TFT_eSPI(); 
CST820 touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT); 

#if LV_USE_LOG != 0
void my_print(const char *buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

void lv_example_btn(void)
{
    static lv_style_prop_t props[] = {
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_TEXT_LETTER_SPACE};


    static lv_style_transition_dsc_t transition_dsc_def;
    lv_style_transition_dsc_init(&transition_dsc_def, props, lv_anim_path_overshoot, 250, 100, NULL);


    static lv_style_transition_dsc_t transition_dsc_pr;
    lv_style_transition_dsc_init(&transition_dsc_pr, props, lv_anim_path_ease_in_out, 250, 0, NULL);


    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &transition_dsc_def);


    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_transform_width(&style_pr, 10);
    lv_style_set_transform_height(&style_pr, -10);
    lv_style_set_text_letter_space(&style_pr, 10);
    lv_style_set_transition(&style_pr, &transition_dsc_pr);

    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -80);
    lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(btn1, &style_def, 0);

    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "btn1");


    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, 3);

    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_ofs_y(&style, 8);

    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);

    static lv_style_t style_pr_2;
    lv_style_init(&style_pr_2);

    lv_style_set_outline_width(&style_pr_2, 30);
    lv_style_set_outline_opa(&style_pr_2, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr_2, 5);
    lv_style_set_shadow_ofs_y(&style_pr_2, 3);
    lv_style_set_bg_color(&style_pr_2, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&style_pr_2, lv_palette_darken(LV_PALETTE_BLUE, 4));

    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props2[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA};
    lv_style_transition_dsc_init(&trans, props2, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr_2, &trans);

    lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
    lv_obj_remove_style_all(btn2);
    lv_obj_add_style(btn2, &style, 0);
    lv_obj_add_style(btn2, &style_pr_2, LV_STATE_PRESSED);
    lv_obj_set_size(btn2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn2);

    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "Button");
    lv_obj_center(label2);
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.pushImage(area->x1, area->y1, w, h, (uint16_t *)color_p);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{

    bool touched;
    uint8_t gesture;
    uint16_t touchX, touchY;

    touched = touch.getTouch(&touchX, &touchY, &gesture);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchY;
        data->point.y
        w= touchX;
    }
}

void setup()
{
    Serial.begin(115200); 
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println(LVGL_Arduino);
    Serial.println("I am LVGL_Arduino");

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print); 
#endif
    pinMode(27, OUTPUT);
    digitalWrite(27, LOW);
    tft.begin();        
    tft.setRotation(3); 
    tft.initDMA();      

    touch.begin(); 
    digitalWrite(27, HIGH);

    buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 200 , MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    buf2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 200 , MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * 200);

   
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenHeight; 
    disp_drv.ver_res = screenWidth; 
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

#if 0
     lv_example_btn();
#else
    ui_init();
#endif
    Serial.println("Setup done");
    Serial.println("Touchscreen initialization called.");
    tft.startWrite();
}

void parse_uart_data(String data) {
    data.trim();
    
    if (data.startsWith("SP:")) {
        int speed = -1, rpm = -1, coolant_temp = -1, gas_pedal = -1;

        // Extract Speed (SP)
        int spIndex = data.indexOf("SP:");
        if (spIndex != -1) {
            int rpmIndex = data.indexOf("RPM:", spIndex);
            if (rpmIndex != -1) {
                speed = data.substring(spIndex + 3, rpmIndex).toInt();
            }
        }

        // Extract RPM
        int rpmIndex = data.indexOf("RPM:");
        if (rpmIndex != -1) {
            int ctIndex = data.indexOf("CT:", rpmIndex);
            if (ctIndex != -1) {
                rpm = data.substring(rpmIndex + 4, ctIndex).toInt();
            }
        }

        // Extract Coolant Temperature (CT)
        int ctIndex = data.indexOf("CT:");
        if (ctIndex != -1) {
            int gpIndex = data.indexOf("GP:", ctIndex);
            if (gpIndex != -1) {
                coolant_temp = data.substring(ctIndex + 3, gpIndex).toInt();
            }
        }

        // Extract Gas Pedal Percentage (GP)
        int gpIndex = data.indexOf("GP:");
        if (gpIndex != -1) {
            gas_pedal = data.substring(gpIndex + 3).toInt();
        }

        lv_label_set_text_fmt(uic_SPEED, "%d", speed);
        lv_label_set_text_fmt(uic_RPM, "%d", rpm);
        lv_bar_set_value(uic_COOLANTTEMP, coolant_temp, LV_ANIM_ON);
        lv_label_set_text_fmt(uic_TempCool, "%d°C", coolant_temp);
        
        Serial.printf("Parsed SP Data - Speed: %d, RPM: %d, Coolant: %d, Gas Pedal: %d\n", 
                      speed, rpm, coolant_temp, gas_pedal);
    } 
    else if (data.startsWith("TO:")) {
        int temp_in = -1, temp_out = -1;

        // Extract TempOut (TO)
        int toIndex = data.indexOf("TO:");
        if (toIndex != -1) {
            int tiIndex = data.indexOf("TI:", toIndex);
            if (tiIndex == -1) tiIndex = data.length();
            temp_out = data.substring(toIndex + 3, tiIndex).toInt();
        }

        // Extract TempIn (TI)
        int tiIndex = data.indexOf("TI:");
        if (tiIndex != -1) {
            temp_in = data.substring(tiIndex + 3).toInt();
        }

        lv_label_set_text_fmt(uic_TEMPIN, "%d°C", temp_in);
        lv_label_set_text_fmt(uic_TEMPOUT, "%d°C", temp_out);

        Serial.printf("Parsed TO Data - TempIn: %d, TempOut: %d\n", temp_in, temp_out);
    } 
    else {
        Serial.println("Invalid data format");
    }
}



void loop()
{
    lv_timer_handler();
    delay(5);

    static String receivedData = "";
    while (Serial2.available()) {
        char receivedChar = Serial2.read();
        Serial.write(receivedChar);
        receivedData += receivedChar;

        if (receivedChar == '\n') {
            parse_uart_data(receivedData);
            receivedData = "";
        }
    }
}
