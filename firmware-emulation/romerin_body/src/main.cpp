#include <Arduino.h>
#include <CST816S.h>
#include <config/CST816S_pin_config.h>
#include <lvgl.h>

#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
#endif
#include <esp_sleep.h>
#include "QMI8658.h"
#include "RomBodyWiFi.h"

// Set Touch Driver
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ);	// sda, scl, rst, irq

/*Set the screen resolution and rotation*/
#define TFT_HOR_RES   240
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

//include images and logos
LV_IMG_DECLARE(romlogo256);

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void){return millis();}



/*Read the touchpad*/
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{
    
    bool touched = touch.available();
    if(!touched)data->state = LV_INDEV_STATE_RELEASED;
    else data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touch.data.x;
    data->point.y = touch.data.y;

    Serial.printf( "Data x %d\n", touch.data.x );
    Serial.printf( "Data y %d\n", touch.data.y );

}


#define NUM_SCREENS 4
lv_obj_t* screens[NUM_SCREENS];

lv_obj_t* label_voltage;

int current_screen = 0;

// Prototipos
void create_screen(int index);
void gesture_event_handler(lv_event_t* e);

lv_obj_t* create_home_screen(){
  lv_obj_t* scr=lv_obj_create(NULL);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
  auto icon = lv_image_create(scr);
  lv_image_set_src(icon, &romlogo256);
  lv_obj_add_event_cb(scr, gesture_event_handler, LV_EVENT_GESTURE, NULL);
  
label_voltage = lv_label_create(scr);
lv_obj_align(label_voltage, LV_ALIGN_CENTER, 0, 0);
lv_label_set_text(label_voltage, "Voltaje: -- V");

  return scr;
}

void sleep_btn_event_cb(lv_event_t* e) {
  Serial.printf("Clicked. Entrando en modo de sueño profundo...");
  esp_deep_sleep_start();
  LV_LOG_USER("Sleep button pressed");
}

void restore_btn_event_cb(lv_event_t* e) {
  // Aquí puedes restaurar los valores por defecto
  LV_LOG_USER("Restore Defaults button pressed");
}

lv_obj_t* create_config_screen(){
  lv_obj_t* scr=lv_obj_create(NULL);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
 
  // Título
  lv_obj_t* label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "Configuration");
  //lv_obj_set_style_text_font(label_title, &lv_font_montserrat_22, 0);
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

  // Botón Sleep
  lv_obj_t* btn_sleep = lv_btn_create(scr);
  lv_obj_set_size(btn_sleep, 160, 50);
  lv_obj_align(btn_sleep, LV_ALIGN_CENTER, 0, -30);
  lv_obj_add_event_cb(btn_sleep, sleep_btn_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t* label_sleep = lv_label_create(btn_sleep);
  lv_label_set_text(label_sleep, "SLEEP");

  // Botón Restore Defaults
  lv_obj_t* btn_restore = lv_btn_create(scr);
  lv_obj_set_size(btn_restore, 160, 50);
  lv_obj_align(btn_restore, LV_ALIGN_CENTER, 0, 40);
  lv_obj_add_event_cb(btn_restore, restore_btn_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t* label_restore = lv_label_create(btn_restore);
  lv_label_set_text(label_restore, "Restore Defaults");

  lv_obj_add_event_cb(scr, gesture_event_handler, LV_EVENT_GESTURE, NULL);
  return scr;
}
// Función para crear pantallas con contenido específico
void create_screen(int index) {
    screens[index] = lv_obj_create(NULL);
    lv_obj_clear_flag(screens[index], LV_OBJ_FLAG_SCROLLABLE);

    // Ejemplo: botón en cada pantalla
    lv_obj_t* btn = lv_btn_create(screens[index]);
    lv_obj_center(btn);
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text_fmt(label, "Pantalla %d", index + 1);
}

void gesture_event_handler(lv_event_t* e) {
  lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
  if(dir==LV_DIR_LEFT)current_screen++;
  if(dir==LV_DIR_RIGHT)current_screen--;
  current_screen = (current_screen+NUM_SCREENS)%NUM_SCREENS;
  lv_scr_load_anim(screens[current_screen], 
                  (dir==LV_DIR_LEFT)?LV_SCR_LOAD_ANIM_MOVE_LEFT:LV_SCR_LOAD_ANIM_MOVE_RIGHT, 
                  500, 0, false); //.5 secs without delay
}


// Inicialización
void initialize_screens(void) {
    screens[0]=create_home_screen();
    screens[1]=create_config_screen();
    for (int i = 2; i < NUM_SCREENS; ++i) {
        create_screen(i);
        lv_obj_add_event_cb(screens[i], gesture_event_handler, LV_EVENT_GESTURE, NULL);
    }
    current_screen = 0;
    lv_scr_load_anim(screens[0], LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, false);
}

void setup()
{
    RomBodyDefs::readConfiguration();
    RomBodyWiFi::setup();
    touch.begin();
    Serial.begin( 9600 ); //para debugging

    // Configura el pin GPIO para despertar el ESP32-S3
    auto wakeUpPin = GPIO_NUM_0; //boton de boot
    pinMode(wakeUpPin, INPUT_PULLUP);
    // Configura el ESP32-S3 para despertar por una señal en el pin GPIO
    esp_sleep_enable_ext0_wakeup(wakeUpPin, LOW);

    QMI8658_init();

    String init_message = "Romerin Body module: ";
    init_message += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println( init_message );
    
    lv_init();

    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);


    /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
    lv_display_t *disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);



    /*Initialize the (dummy) input device driver*/ 
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); //Touchpad should have POINTER type
    lv_indev_set_read_cb(indev, my_touchpad_read);
  

    /*
    auto icon = lv_image_create(lv_screen_active());
    lv_image_set_src(icon, &romlogo256);

    
    
    lv_obj_t *label = lv_label_create( lv_screen_active() );

    lv_label_set_text( label, "ROMERIN BODY" );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );


    //probando un boton

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn1, 200,80);
    lv_obj_add_event_cb(btn1, button_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -80);

    auto label2 = lv_label_create(btn1);
    lv_label_set_text(label2, "JUAN H");
    lv_obj_center(label2);

*/
    initialize_screens();
    Serial.println( "Setup done" );
}
void sendRegularMessages(){
    RomBodyState state{};
    RomBodyWiFi::sendMessage(state_message(state));
}
void loop()
{
    float acc[3], gyro[3];
    unsigned int tim_count = 0;
    uint16_t result;
    QMI8658_read_xyz(acc, gyro, &tim_count);
    
    lv_timer_handler(); /* let the GUI do its work */
    RomBodyWiFi::loop();
    if(RomBodyWiFi::isConected2Master())sendRegularMessages();
    const float conversion_factor = 3.3f / (1 << 12) * 3;
    auto lect=analogReadMilliVolts(BAT_ADC_PIN);
    auto volt=lect*conversion_factor;
    Serial.printf(" ACC: %5.2f %5.2f %5.2f   BAT:%5.2f\n",acc[0],acc[1],acc[2], volt);
    char buf[32];
    snprintf(buf, sizeof(buf), "Voltaje: %.2f V", volt);
    lv_label_set_text(label_voltage, buf);

    delay(5); /* let this time pass */
}

RomerinMsg executeMessage(const RomerinMsg &m)
{
  
  switch(m.id){

 
    case ROM_SCAN_WIFI:
      RomBodyWiFi::scanNetworks();
      ROM_DEBUG("SCAN_WIFIs");
      break;
    case ROM_INIT_WIFI:
      RomBodyWiFi::init();
      ROM_DEBUG("INIT_WIFI");
      break;
 
    case ROM_GET_CONFIG:
      ROM_DEBUG("GET_CONFIG");
      return configuration_message(); 
    case ROM_CONFIG:
      romerin_getConfiguration(m.info);
      RomBodyDefs::writeConfiguration();
      ROM_DEBUG("CONFIG: config changes will take effect after rebooting the robot");
      break; 

    case ROM_TEST_A_MESSAGE: //messages only for testing purposes
      ROM_DEBUG("TEST A: config read");
      RomBodyDefs::readConfiguration();
      break;
    case ROM_TEST_B_MESSAGE:
  
      break;
 
    default:
      ROM_DEBUG("UNKNOWN MESSAGE");
  }
  return RomerinMsg::none();
}