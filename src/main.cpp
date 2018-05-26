#include <LCDMenuLib2.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_MCP23017.h>
#include <Encoder.h>
#include "config.h"

void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();
void mFunc_information(uint8_t param);
void mFunc_timer_info(uint8_t param);
void mFunc_p2(uint8_t param);
void mFunc_screensaver(uint8_t param);
void mFunc_back(uint8_t param);
void mFunc_goToRootMenu(uint8_t param);
void mFunc_jumpTo_timer_info(uint8_t param);
void mFunc_para(uint8_t param);
void mDyn_para(uint8_t line);
void mDyn_contrast(uint8_t line);
void mDyn_volume(uint8_t line);
void start_game(byte game_type);
boolean COND_hide();

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
Encoder ENCODER(15,33);
Encoder ENCODER2(32,14);
Adafruit_MCP23017 mcp;

uint8_t g_func_timer_info = 0;  // time counter (global variable)
unsigned long g_timer_1 = 0;    // timer variable (global variable)
bool  g_LCDML_CONTROL_button_prev       = HIGH;
bool  g_LCDML_CONTROL_button2_prev       = HIGH;
byte contrast = 0;
byte volume = 10;

LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);

LCDML_addAdvanced (0  , LCDML_0         , 5  , NULL,          "Play Game"       , NULL,             0, _LCDML_TYPE_default);
LCDML_addAdvanced (1  , LCDML_0_5       , 1  , NULL,          "Hex to Bin"      , mFunc_para,       1, _LCDML_TYPE_default);
LCDML_addAdvanced (2  , LCDML_0_5       , 2  , NULL,          "Bin to Hex"      , mFunc_para,       2, _LCDML_TYPE_default);
LCDML_addAdvanced (3  , LCDML_0_5       , 3  , NULL,          "Bin to Dec"      , mFunc_para,       3, _LCDML_TYPE_default);
LCDML_add         (4  , LCDML_0_5       , 4  , "Back"             , mFunc_back);
LCDML_add         (5  , LCDML_0         , 3  , "Settings"          , NULL);
LCDML_addAdvanced (6  , LCDML_0_3       , 1  , NULL,          ""                  , mDyn_contrast,  0,   _LCDML_TYPE_dynParam);
LCDML_addAdvanced (7  , LCDML_0_3       , 2  , NULL,          ""                  , mDyn_volume,  0,   _LCDML_TYPE_dynParam);
LCDML_add         (8  , LCDML_0_3       , 3  , "Back"             , mFunc_back);
LCDML_addAdvanced (9  , LCDML_0         , 7  , COND_hide,  "screensaver"        , mFunc_screensaver,        0,   _LCDML_TYPE_default);       // this menu function can be found on "LCDML_display_menuFunction" tab
#define _LCDML_DISP_cnt    9 // this value must be the same as the last menu element
LCDML_createMenu(_LCDML_DISP_cnt);

void setup()
{
	u8g2.begin();
  u8g2.setContrast(contrast);

	Serial.begin(115200);                // start serial
	Serial.println(F(_LCDML_VERSION)); // only for examples

	mcp.begin(); // use default address 0
	mcp.pinMode(0, INPUT);
	for(int i=0; i<=9; i++)
		mcp.pullUp(i, HIGH);

	LCDML_setup(_LCDML_DISP_cnt);
	LCDML.MENU_enRollover();
	LCDML.SCREEN_enable(mFunc_screensaver, 10000); // set to 10 seconds
}

void loop()
{
	LCDML.loop();
	delay(20);
}

void start_game(byte game_type)
{
	if(LCDML.FUNC_setup())
	{
		u8g2.setFont(_LCDML_DISP_font);
		u8g2.firstPage();
		char buf[20];
		sprintf (buf, "mode: %d", game_type);
		do {
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "Starting Game");
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), buf);
		} while( u8g2.nextPage() );
	}
	if(LCDML.FUNC_loop()){
		if(LCDML.BT_checkAny())
			LCDML.FUNC_goBackToMenu();
	}
	if(LCDML.FUNC_close()){}
}

void mFunc_information(uint8_t param)
{
	if(LCDML.FUNC_setup())
	{
		u8g2.setFont(_LCDML_DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "To close this");
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "function press");
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 3), "any button or use");
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 4), "back button");
		} while( u8g2.nextPage() );
	}
	if(LCDML.FUNC_loop()){
		if(LCDML.BT_checkAny()){ // check if any button is pressed (enter, up, down, left, right)
			LCDML.FUNC_goBackToMenu();
		}
	}
	if(LCDML.FUNC_close()){}
}

void mFunc_timer_info(uint8_t param)
{
	if(LCDML.FUNC_setup()){
		g_func_timer_info = 20;       // reset and set timer

		char buf[20];
		sprintf (buf, "wait %d seconds", g_func_timer_info);

		u8g2.setFont(_LCDML_DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), buf);
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "or press back button");
		} while( u8g2.nextPage() );

		LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
		LCDML.TIMER_msReset(g_timer_1);
	}

	if(LCDML.FUNC_loop())           // ****** LOOP *********
	{
		// reset screensaver timer
		LCDML.SCREEN_resetTimer();
		if(LCDML.TIMER_ms(g_timer_1, 1000))
		{
			g_timer_1 = millis();
			g_func_timer_info--;                // increment the value every second
			char buf[20];
			sprintf (buf, "wait %d seconds", g_func_timer_info);

			u8g2.setFont(_LCDML_DISP_font);
			u8g2.firstPage();
			do {
				u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), buf);
				u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "or press back button");
			} while( u8g2.nextPage() );
		}

		if (g_func_timer_info <= 0){
			// leave this function
			LCDML.FUNC_goBackToMenu();
		}
	}

	if(LCDML.FUNC_close()) {}
}

uint8_t g_button_value = 0; // button value counter (global variable)

void mFunc_p2(uint8_t param)
{
	if(LCDML.FUNC_setup())
	{
		char buf[17];
		sprintf (buf, "count: %d of 3", 0);

		u8g2.setFont(_LCDML_DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "press a or w button");
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), buf);
		} while( u8g2.nextPage() );
		g_button_value = 0;
		LCDML.FUNC_disableScreensaver();
	}

	if(LCDML.FUNC_loop())
	{
		if (LCDML.BT_checkAny())
		{
			if (LCDML.BT_checkLeft() || LCDML.BT_checkUp())
			{
				LCDML.BT_resetLeft(); // reset the left button
				LCDML.BT_resetUp(); // reset the left button
				g_button_value++;
				// update LCD content
				char buf[20];
				sprintf (buf, "count: %d of 3", g_button_value);

				u8g2.setFont(_LCDML_DISP_font);
				u8g2.firstPage();
				do {
					u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "press a or w button");
					u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), buf);
				} while( u8g2.nextPage() );
			}
		}

		// check if button count is three
		if (g_button_value >= 3) {
			LCDML.FUNC_goBackToMenu();      // leave this function
		}
	}

	if(LCDML.FUNC_close())     // ****** STABLE END *********
	{
		// you can here reset some global vars or do nothing
	}
}

void mFunc_screensaver(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{
		// setup function
		u8g2.setFont(_LCDML_DISP_font);
		u8g2.firstPage();
		do {
			//u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "screensaver");
			//u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "press any key");
			//u8g2.drawStr( 0, (_LCDML_DISP_font_h * 3), "to leave it");
		} while( u8g2.nextPage() );

		LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
	}

	if(LCDML.FUNC_loop())           // ****** LOOP *********
	{
		if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
		{
			LCDML.FUNC_goBackToMenu();  // leave this function
		}
	}

	if(LCDML.FUNC_close())          // ****** STABLE END *********
	{
		// The screensaver go to the root menu
		LCDML.MENU_goRoot();
	}
}

void mFunc_back(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{
		// end function and go an layer back
		LCDML.FUNC_goBackToMenu(1);      // leave this function and go a layer back
	}
}

void mFunc_goToRootMenu(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{
		// go to root and display menu
		LCDML.MENU_goRoot();
	}
}

void mFunc_jumpTo_timer_info(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{
		// Jump to main screen
		if(!LCDML.OTHER_jumpToFunc(mFunc_timer_info))
		{
			// function not found or not callable
			LCDML.MENU_goRoot();
		}
	}
}

void mFunc_para(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{

		char buf[20];
		sprintf (buf, "parameter: %d", param);

		// setup function
		u8g2.setFont(_LCDML_DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), buf);
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "press any key");
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 3), "to leave it");
		} while( u8g2.nextPage() );

		LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
	}

	if(LCDML.FUNC_loop())               // ****** LOOP *********
	{
		// For example
		switch(param)
		{
		case 10:
			// do something
			break;

		case 20:
			// do something
			break;

		case 30:
			// do something
			break;

		default:
			// do nothing
			break;
		}

		if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
		{
			LCDML.FUNC_goBackToMenu();  // leave this function
		}
	}

	if(LCDML.FUNC_close())      // ****** STABLE END *********
	{
		// you can here reset some global vars or do nothing
	}
}

void mDyn_contrast(uint8_t line)
{
	if (line == LCDML.MENU_getCursorPos()){
		if(LCDML.BT_checkAny()){
			if(LCDML.BT_checkLeft()){
				contrast++;
				LCDML.BT_resetLeft();
				u8g2.setContrast(contrast);
			}
			if(LCDML.BT_checkRight()){
				contrast--;
				LCDML.BT_resetRight();
				u8g2.setContrast(contrast);
			}
		}
	}

	char buf[20];
	sprintf (buf, "Brightness: %d", contrast);
	u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1+line)), buf);     // the value can be changed with left or right
}

void mDyn_volume(uint8_t line)
{
	if (line == LCDML.MENU_getCursorPos()){
		if(LCDML.BT_checkAny()){
			if(LCDML.BT_checkLeft()){
				volume++;
				LCDML.BT_resetLeft();
				//setVolume(volume);
			}
			if(LCDML.BT_checkRight()){
				volume--;
				LCDML.BT_resetRight();
				//setVolume(volume);
			}
		}
	}

	char buf[20];
	sprintf (buf, "Volume: %d", volume);
	u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1+line)), buf);     // the value can be changed with left or right
}
void mDyn_volume(uint8_t line);

boolean COND_hide()  // hide a menu element
{
	return false;  // hidden
}

void lcdml_menu_clear()
{
}

void lcdml_menu_display()
{
	u8g2.setFont(_LCDML_DISP_font);
	char content_text[_LCDML_DISP_cols];  // save the content text of every menu element
	// menu element object
	LCDMenuLib2_menu *tmp;
	// some limit values
	uint8_t i = LCDML.MENU_getScroll();
	uint8_t maxi = _LCDML_DISP_rows + i;
	uint8_t n = 0;
	// init vars
	uint8_t n_max             = (LCDML.MENU_getChilds() >= _LCDML_DISP_rows) ? _LCDML_DISP_rows : (LCDML.MENU_getChilds());
	uint8_t scrollbar_min     = 0;
	uint8_t scrollbar_max     = LCDML.MENU_getChilds();
	uint8_t scrollbar_cur_pos = LCDML.MENU_getCursorPosAbs();
	uint8_t scroll_pos        = ((1.*n_max * _LCDML_DISP_rows) / (scrollbar_max - 1) * scrollbar_cur_pos);
	// generate content
	u8g2.firstPage();
	do {
		n = 0;
		i = LCDML.MENU_getScroll();
		// check if this element has children
		if ((tmp = LCDML.MENU_getObj()->getChild(LCDML.MENU_getScroll())))
		{
			// loop to display lines
			do
			{
				// check if a menu element has a condition and if the condition be true
				if (tmp->checkCondition())
				{
					// check the type off a menu element
					if(tmp->checkType_menu() == true)
					{
						// display normal content
						LCDML_getContent(content_text, tmp->getID());
						u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind, _LCDML_DISP_box_y0 + _LCDML_DISP_font_h * (n + 1), content_text);
					}
					else
					{
						if(tmp->checkType_dynParam()) {
							tmp->callback(n);
						}
					}
					// increment some values
					i++;
					n++;
				}
				// try to go to the next sibling and check the number of displayed rows
			} while (((tmp = tmp->getSibling(1)) != NULL) && (i < maxi));
		}
		// set cursor
		u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_cur_space_before, _LCDML_DISP_box_y0 + _LCDML_DISP_font_h * (LCDML.MENU_getCursorPos() + 1),  _LCDML_DISP_cursor_char);
		// display scrollbar when more content as rows available and with > 2
		if (scrollbar_max > n_max && _LCDML_DISP_scrollbar_w > 2)
		{
			// set frame for scrollbar
			u8g2.drawFrame(_LCDML_DISP_box_x1 - _LCDML_DISP_scrollbar_w, _LCDML_DISP_box_y0, _LCDML_DISP_scrollbar_w, _LCDML_DISP_box_y1-_LCDML_DISP_box_y0);
			// calculate scrollbar length
			uint8_t scrollbar_block_length = scrollbar_max - n_max;
			scrollbar_block_length = (_LCDML_DISP_box_y1-_LCDML_DISP_box_y0) / (scrollbar_block_length + _LCDML_DISP_rows);
			//set scrollbar
			if (scrollbar_cur_pos == 0) {                                   // top position     (min)
				u8g2.drawBox(_LCDML_DISP_box_x1 - (_LCDML_DISP_scrollbar_w-1), _LCDML_DISP_box_y0 + 1                                                     , (_LCDML_DISP_scrollbar_w-2)  , scrollbar_block_length);
			}
			else if (scrollbar_cur_pos == (scrollbar_max-1)) {            // bottom position  (max)
				u8g2.drawBox(_LCDML_DISP_box_x1 - (_LCDML_DISP_scrollbar_w-1), _LCDML_DISP_box_y1 - scrollbar_block_length                                , (_LCDML_DISP_scrollbar_w-2)  , scrollbar_block_length);
			}
			else {                                                                // between top and bottom
				u8g2.drawBox(_LCDML_DISP_box_x1 - (_LCDML_DISP_scrollbar_w-1), _LCDML_DISP_box_y0 + (scrollbar_block_length * scrollbar_cur_pos + 1),(_LCDML_DISP_scrollbar_w-2)  , scrollbar_block_length);
			}
		}
	} while ( u8g2.nextPage() );
}

void lcdml_menu_control(void)
{
	if(LCDML.BT_setup()){}

	long g_LCDML_CONTROL_Encoder_position = ENCODER.read();
  long g_LCDML_CONTROL_Encoder_position2 = ENCODER2.read();
	bool button                           = !mcp.digitalRead(9);
  bool button2                          = !mcp.digitalRead(8);

	if(g_LCDML_CONTROL_Encoder_position >= 3) {
		LCDML.BT_down();
		ENCODER.write(g_LCDML_CONTROL_Encoder_position-4);
	}
	else if(g_LCDML_CONTROL_Encoder_position <= -3)
	{
		LCDML.BT_up();
		ENCODER.write(g_LCDML_CONTROL_Encoder_position+4);
	}
  else if(g_LCDML_CONTROL_Encoder_position2 >= 3) {
		LCDML.BT_left();
		ENCODER2.write(g_LCDML_CONTROL_Encoder_position2-4);
	}
	else if(g_LCDML_CONTROL_Encoder_position2 <= -3)
	{
		LCDML.BT_right();
		ENCODER2.write(g_LCDML_CONTROL_Encoder_position2+4);
	}
	else
	{
		if(!button && g_LCDML_CONTROL_button_prev)  //falling edge, button pressed
			g_LCDML_CONTROL_button_prev = LOW;
		else if(button && !g_LCDML_CONTROL_button_prev) //rising edge, button not active
		{
			g_LCDML_CONTROL_button_prev = HIGH;
			LCDML.BT_quit();
		}

    if(!button2 && g_LCDML_CONTROL_button2_prev)  //falling edge, button pressed
      g_LCDML_CONTROL_button2_prev = LOW;
    else if(button2 && !g_LCDML_CONTROL_button2_prev) //rising edge, button not active
    {
      g_LCDML_CONTROL_button2_prev = HIGH;
      LCDML.BT_enter();
    }
	}
}
