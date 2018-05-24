#include <LCDMenuLib2.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_MCP23017.h>
#include <Encoder.h>

Adafruit_MCP23017 mcp;
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);

#define _LCDML_DISP_w                 128            // LCD width
#define _LCDML_DISP_h                 64             // LCD height
#define _LCDML_DISP_font              u8g_font_6x13  // u8glib font (more fonts under u8g.h line 1520 ...)
#define _LCDML_DISP_font_w            6              // font width
#define _LCDML_DISP_font_h            13             // font height
#define _LCDML_DISP_cursor_char       ">"            // cursor char
#define _LCDML_DISP_cur_space_before  2              // cursor space between
#define _LCDML_DISP_cur_space_behind  4              // cursor space between
#define _LCDML_DISP_box_x0            0              // start point (x0, y0)
#define _LCDML_DISP_box_y0            0              // start point (x0, y0)
#define _LCDML_DISP_box_x1            128            // width x  (x0 + width)
#define _LCDML_DISP_box_y1            64             // hight y  (y0 + height)
//#define _LCDML_DISP_draw_frame        1              // draw a box around the menu
#define _LCDML_DISP_scrollbar_w       6  // scrollbar width (if this value is < 3, the scrollbar is disabled)
#define _LCDML_DISP_cols_max          ((_LCDML_DISP_box_x1-_LCDML_DISP_box_x0)/_LCDML_DISP_font_w)
#define _LCDML_DISP_rows_max          ((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0-((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0)/_LCDML_DISP_font_h))/_LCDML_DISP_font_h)
#define _LCDML_DISP_rows              _LCDML_DISP_rows_max  // max rows
#define _LCDML_DISP_cols              20                   // max cols

// Prototypes
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
boolean COND_hide();

// Objects
LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);

// LCDML MENU/DISP
LCDML_add         (0  , LCDML_0         , 1  , "Information"      , mFunc_information);       // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (1  , LCDML_0         , 2  , "Time info"        , mFunc_timer_info);        // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (2  , LCDML_0         , 3  , "Program"          , NULL);                    // NULL = no menu function
LCDML_add         (3  , LCDML_0_3       , 1  , "Program 1"        , NULL);                    // NULL = no menu function
LCDML_add         (4  , LCDML_0_3_1     , 1  , "P1 dummy"         , NULL);                    // NULL = no menu function
LCDML_add         (5  , LCDML_0_3_1     , 2  , "P1 Settings"      , NULL);                    // NULL = no menu function
LCDML_add         (6  , LCDML_0_3_1_2   , 1  , "Warm"             , NULL);                    // NULL = no menu function
LCDML_add         (7  , LCDML_0_3_1_2   , 2  , "Cold"             , NULL);                    // NULL = no menu function
LCDML_add         (8  , LCDML_0_3_1_2   , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (9  , LCDML_0_3_1     , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (10 , LCDML_0_3       , 2  , "Program 2"        , mFunc_p2);                // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (11 , LCDML_0_3       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (12 , LCDML_0         , 4  , "Special"          , NULL);                    // NULL = no menu function
LCDML_add         (13 , LCDML_0_4       , 1  , "Go to Root"       , mFunc_goToRootMenu);      // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (14 , LCDML_0_4       , 2  , "Jump to Time info", mFunc_jumpTo_timer_info); // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (15 , LCDML_0_4       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_addAdvanced (16 , LCDML_0         , 5  , NULL,          "Parameter"      , NULL,                0,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_addAdvanced (17 , LCDML_0_5       , 1  , NULL,          "Parameter 1"      , mFunc_para,       10,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_addAdvanced (18 , LCDML_0_5       , 2  , NULL,          "Parameter 2"      , mFunc_para,       20,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_addAdvanced (19 , LCDML_0_5       , 3  , NULL,          "Parameter 3"      , mFunc_para,       30,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_add         (20 , LCDML_0_5       , 4  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_addAdvanced (21 , LCDML_0         , 6  , NULL,          ""                  , mDyn_para,                0,   _LCDML_TYPE_dynParam);                     // NULL = no menu function
LCDML_addAdvanced (22 , LCDML_0         , 7  , COND_hide,  "screensaver"        , mFunc_screensaver,        0,   _LCDML_TYPE_default);       // this menu function can be found on "LCDML_display_menuFunction" tab
#define _LCDML_DISP_cnt    22 // this value must be the same as the last menu element
LCDML_createMenu(_LCDML_DISP_cnt);

void setup()
{
	u8g2.begin();
  u8g2.setContrast(0);

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
	// this function must called here, do not delete it
	LCDML.loop();
	delay(20);
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

	if(LCDML.FUNC_close()){
	}
}

uint8_t g_func_timer_info = 0;  // time counter (global variable)
unsigned long g_timer_1 = 0;    // timer variable (global variable)

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
		// this function is called every 100 milliseconds
		// this method checks every 1000 milliseconds if it is called
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

	if(LCDML.FUNC_close()) {
	}
}

uint8_t g_button_value = 0; // button value counter (global variable)

void mFunc_p2(uint8_t param)
{
	if(LCDML.FUNC_setup())          // ****** SETUP *********
	{
		// setup function
		// print LCD content
		char buf[17];
		sprintf (buf, "count: %d of 3", 0);

		u8g2.setFont(_LCDML_DISP_font);
		u8g2.firstPage();
		do {
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "press a or w button");
			u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), buf);
		} while( u8g2.nextPage() );
		// Reset Button Value
		g_button_value = 0;
		// Disable the screensaver for this function until it is closed
		LCDML.FUNC_disableScreensaver();
	}

	if(LCDML.FUNC_loop())           // ****** LOOP *********
	{
		if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
		{
			if (LCDML.BT_checkLeft() || LCDML.BT_checkUp()) // check if button left is pressed
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

uint8_t g_dynParam = 100; // when this value comes from an EEPROM, load it in setup
// at the moment here is no setup function (To-Do)
void mDyn_para(uint8_t line)
{
	if (line == LCDML.MENU_getCursorPos()){
		if(LCDML.BT_checkAny()){
			if(LCDML.BT_checkLeft()){
				g_dynParam++;
				LCDML.BT_resetLeft();
			}
			if(LCDML.BT_checkRight()){
				g_dynParam--;
				LCDML.BT_resetRight();
			}
		}
	}

	char buf[20];
	sprintf (buf, "dynValue: %d", g_dynParam);
	// setup function
	u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1+line)), buf);     // the value can be changed with left or right
}

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

#define g_LCDML_CONTROL_button_long_press    800   // ms
#define g_LCDML_CONTROL_button_short_press   20   // ms

Encoder ENCODER(15,33);
Encoder ENCODER2(32,14);

long  g_LCDML_CONTROL_button_press_time = 0;
bool  g_LCDML_CONTROL_button_prev       = HIGH;
bool  g_LCDML_CONTROL_button2_prev       = HIGH;

void lcdml_menu_control(void)
{
	// If something must init, put in in the setup condition
	if(LCDML.BT_setup())
	{
		// runs only once
	}

	//volatile Variable
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
		{
			g_LCDML_CONTROL_button_prev = LOW;
		}
		else if(button && !g_LCDML_CONTROL_button_prev) //rising edge, button not active
		{
			g_LCDML_CONTROL_button_prev = HIGH;
			LCDML.BT_quit();
		}

    if(!button2 && g_LCDML_CONTROL_button2_prev)  //falling edge, button pressed
    {
      g_LCDML_CONTROL_button2_prev = LOW;
    }
    else if(button2 && !g_LCDML_CONTROL_button2_prev) //rising edge, button not active
    {
      g_LCDML_CONTROL_button2_prev = HIGH;
      LCDML.BT_enter();
    }
	}
}

// check some errors - do not change here anything
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib2.h
# endif

# if(_LCDML_glcd_tft_box_x1 > _LCDML_glcd_tft_w)
# error _LCDML_glcd_tft_box_x1 is to big
# endif

# if(_LCDML_glcd_tft_box_y1 > _LCDML_glcd_tft_h)
# error _LCDML_glcd_tft_box_y1 is to big
# endif
