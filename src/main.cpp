// ============================================================
// Example:     LCDML: graphic display with u8g
// ============================================================
// Author:      Nils Feldkämper
// Last update: 21.01.2018
// License:     MIT
// ============================================================

  #include <LCDMenuLib2.h>
  #include <Arduino.h>
  #include <U8g2lib.h>
  #include <Adafruit_MCP23017.h>
  #include <Encoder.h>

  Adafruit_MCP23017 mcp;

  U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ A1, /* dc=*/ 21, /* reset=*/ A5);
  // settings for u8g lib and LCD
  #define _LCDML_DISP_w                 128            // LCD width
  #define _LCDML_DISP_h                 64             // LCD height
  // font settings
  #define _LCDML_DISP_font              u8g_font_6x13  // u8glib font (more fonts under u8g.h line 1520 ...)
  #define _LCDML_DISP_font_w            6              // font width
  #define _LCDML_DISP_font_h            13             // font height
  // cursor settings
  #define _LCDML_DISP_cursor_char       "X"            // cursor char
  #define _LCDML_DISP_cur_space_before  2              // cursor space between
  #define _LCDML_DISP_cur_space_behind  4              // cursor space between
  // menu position and size
  #define _LCDML_DISP_box_x0            0              // start point (x0, y0)
  #define _LCDML_DISP_box_y0            0              // start point (x0, y0)
  #define _LCDML_DISP_box_x1            128            // width x  (x0 + width)
  #define _LCDML_DISP_box_y1            64             // hight y  (y0 + height)
  #define _LCDML_DISP_draw_frame        1              // draw a box around the menu
   // scrollbar width
  #define _LCDML_DISP_scrollbar_w       6  // scrollbar width (if this value is < 3, the scrollbar is disabled)
  // nothing change here
  #define _LCDML_DISP_cols_max          ((_LCDML_DISP_box_x1-_LCDML_DISP_box_x0)/_LCDML_DISP_font_w)
  #define _LCDML_DISP_rows_max          ((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0-((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0)/_LCDML_DISP_font_h))/_LCDML_DISP_font_h)
  // rows and cols
  // when you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
  // the program needs more ram with this changes
  #define _LCDML_DISP_rows              _LCDML_DISP_rows_max  // max rows
  #define _LCDML_DISP_cols              20                   // max cols

// *********************************************************************
// Prototypes
// *********************************************************************
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

// *********************************************************************
// Objects
// *********************************************************************
  LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
  LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
  // LCDML_0        => layer 0
  // LCDML_0_X      => layer 1
  // LCDML_0_X_X    => layer 2
  // LCDML_0_X_X_X  => layer 3
  // LCDML_0_...      => layer ...

  // For beginners
  // LCDML_add(id, prev_layer, new_num, lang_char_array, callback_function)
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


  // Advanced menu (for profit) part with more settings
  // Example for one function and different parameters
  // It is recommend to use parameters for switching settings like, (small drink, medium drink, big drink) or (200ml, 400ml, 600ml, 800ml) ...
  // the parameter change can also be released with dynParams on the next example
  // LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (16 , LCDML_0         , 5  , NULL,          "Parameter"      , NULL,                0,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (17 , LCDML_0_5       , 1  , NULL,          "Parameter 1"      , mFunc_para,       10,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (18 , LCDML_0_5       , 2  , NULL,          "Parameter 2"      , mFunc_para,       20,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (19 , LCDML_0_5       , 3  , NULL,          "Parameter 3"      , mFunc_para,       30,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_add         (20 , LCDML_0_5       , 4  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab

  // Example for dynamic content
  // 1. set the string to ""
  // 2. use type  _LCDML_TYPE_dynParam   instead of    _LCDML_TYPE_default
  // this function type can not be used in combination with different parameters
  // LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (21 , LCDML_0         , 6  , NULL,          ""                  , mDyn_para,                0,   _LCDML_TYPE_dynParam);                     // NULL = no menu function

  // Example for conditions (for example for a screensaver)
  // 1. define a condition as a function of a boolean type -> return false = not displayed, return true = displayed
  // 2. set the function name as callback (remove the braces '()' it gives bad errors)
  // LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (22 , LCDML_0         , 7  , COND_hide,  "screensaver"        , mFunc_screensaver,        0,   _LCDML_TYPE_default);       // this menu function can be found on "LCDML_display_menuFunction" tab

  // ***TIP*** Try to update _LCDML_DISP_cnt when you add a menu element.

  // menu element count - last element id
  // this value must be the same as the last menu element
  #define _LCDML_DISP_cnt    22

  // create menu
  LCDML_createMenu(_LCDML_DISP_cnt);

// *********************************************************************
// SETUP
// *********************************************************************
  void setup()
  {
    u8g2.begin();

    // serial init; only be needed if serial control is used
    Serial.begin(115200);                // start serial
    Serial.println(F(_LCDML_VERSION)); // only for examples

    mcp.begin(); // use default address 0
    mcp.pinMode(0, INPUT);
    for(int i=0; i<=9; i++)
            mcp.pullUp(i, HIGH);

    // LCDMenuLib Setup
    LCDML_setup(_LCDML_DISP_cnt);

    // Enable Menu Rollover
    LCDML.MENU_enRollover();

    // Enable Screensaver (screensaver menu function, time to activate in ms)
    LCDML.SCREEN_enable(mFunc_screensaver, 10000); // set to 10 seconds
    //LCDML.SCREEN_disable();

    // Some needful methods

    // You can jump to a menu function from anywhere with
    //LCDML.OTHER_jumpToFunc(mFunc_p2); // the parameter is the function name
  }

// *********************************************************************
// LOOP
// *********************************************************************
  void loop()
  {
    // this function must called here, do not delete it
    LCDML.loop();
    delay(20);
  }

  // *********************************************************************
  void mFunc_information(uint8_t param)
  // *********************************************************************
  {
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
      // setup function
      u8g2.setFont(_LCDML_DISP_font);
      u8g2.firstPage();
      do {
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "To close this");
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "function press");
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 3), "any button or use");
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 4), "back button");
      } while( u8g2.nextPage() );
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {
      // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
      // the quit button works in every DISP function without any checks; it starts the loop_end function
      if(LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
      {
        // LCDML_goToMenu stops a running menu function and goes to the menu
        LCDML.FUNC_goBackToMenu();
      }
    }

    if(LCDML.FUNC_close())      // ****** STABLE END *********
    {
      // you can here reset some global vars or do nothing
    }
  }

  // *********************************************************************
  uint8_t g_func_timer_info = 0;  // time counter (global variable)
  unsigned long g_timer_1 = 0;    // timer variable (global variable)
  void mFunc_timer_info(uint8_t param)
  // *********************************************************************
  {
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
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
      // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
      // the quit button works in every DISP function without any checks; it starts the loop_end function

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

      // this function can only be ended when quit button is pressed or the time is over
      // check if the function ends normally
      if (g_func_timer_info <= 0)
      {
        // leave this function
        LCDML.FUNC_goBackToMenu();
      }
    }

    if(LCDML.FUNC_close())      // ****** STABLE END *********
    {
      // you can here reset some global vars or do nothing
    }
  }

  // *********************************************************************
  uint8_t g_button_value = 0; // button value counter (global variable)
  void mFunc_p2(uint8_t param)
  // *********************************************************************
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
      // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
      // the quit button works in every DISP function without any checks; it starts the loop_end function

      // the quit button works in every DISP function without any checks; it starts the loop_end function
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


  // *********************************************************************
  void mFunc_screensaver(uint8_t param)
  // *********************************************************************
  {
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
      // setup function
      u8g2.setFont(_LCDML_DISP_font);
      u8g2.firstPage();
      do {
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "screensaver");
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "press any key");
        u8g2.drawStr( 0, (_LCDML_DISP_font_h * 3), "to leave it");
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



  // *********************************************************************
  void mFunc_back(uint8_t param)
  // *********************************************************************
  {
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
      // end function and go an layer back
      LCDML.FUNC_goBackToMenu(1);      // leave this function and go a layer back
    }
  }


  // *********************************************************************
  void mFunc_goToRootMenu(uint8_t param)
  // *********************************************************************
  {
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
      // go to root and display menu
      LCDML.MENU_goRoot();
    }
  }

  // *********************************************************************
  void mFunc_jumpTo_timer_info(uint8_t param)
  // *********************************************************************
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


  // *********************************************************************
  void mFunc_para(uint8_t param)
  // *********************************************************************
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
  // *********************************************************************
  {
    // check if this function is active (cursor stands on this line)
    if (line == LCDML.MENU_getCursorPos())
    {
      // make only an action when the cursor stands on this menu item
      //check Button
      if(LCDML.BT_checkAny())
      {
        if(LCDML.BT_checkEnter())
        {
          // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
          if(LCDML.MENU_getScrollDisableStatus() == 0)
          {
            // disable the menu scroll function to catch the cursor on this point
            // now it is possible to work with BT_checkUp and BT_checkDown in this function
            // this function can only be called in a menu, not in a menu function
            LCDML.MENU_disScroll();
          }
          else
          {
            // enable the normal menu scroll function
            LCDML.MENU_enScroll();
          }

          // do something
          // ...

          LCDML.BT_resetEnter();
        }

        // This check have only an effect when MENU_disScroll is set
        if(LCDML.BT_checkUp())
        {
          g_dynParam++;
          LCDML.BT_resetUp();
        }

        // This check have only an effect when MENU_disScroll is set
        if(LCDML.BT_checkDown())
        {
          g_dynParam--;
          LCDML.BT_resetDown();
        }


        if(LCDML.BT_checkLeft())
        {
          g_dynParam++;
          LCDML.BT_resetLeft();
        }

        if(LCDML.BT_checkRight())
        {
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
  // *********************************************************************
  {
    return false;  // hidden
  }

  // =====================================================================
//
// Output function
//
// =====================================================================


/* ******************************************************************** */
void lcdml_menu_clear()
/* ******************************************************************** */
{
}

/* ******************************************************************** */
void lcdml_menu_display()
/* ******************************************************************** */
{
  // for first test set font here
  u8g2.setFont(_LCDML_DISP_font);

  // declaration of some variables
  // ***************
  // content variable
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
    // update content
    // ***************

      // clear menu
      // ***************

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

    if(_LCDML_DISP_draw_frame == 1) {
       u8g2.drawFrame(_LCDML_DISP_box_x0, _LCDML_DISP_box_y0, (_LCDML_DISP_box_x1-_LCDML_DISP_box_x0), (_LCDML_DISP_box_y1-_LCDML_DISP_box_y0));
    }

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

// =====================================================================
//
// CONTROL
//
// =====================================================================
// *********************************************************************
// *********************************************************************
// content:
// (0) Control over serial interface  with asdw_e_q
// (1) Control over one analog input
// (2) Control over 4 - 6 digital input pins (internal pullups enabled)
// (3) Control over encoder [third party lib] (Download: https://github.com/PaulStoffregen/Encoder)
// (4) Control with Keypad  [third party lib] (Download: http://playground.arduino.cc/Main/KeypadTutorial )
// (5) Control with an IR remote [third party lib] (Download: https://github.com/z3t0/Arduino-IRremote )
// (6) Control with a joystick
// (7) Control over I2C PCF8574
// *********************************************************************

#define _LCDML_CONTROL_cfg      3

// theory:
// "#if" is a preprocessor directive and no error, look here:
// (English) https://en.wikipedia.org/wiki/C_preprocessor
// (German)  https://de.wikipedia.org/wiki/C-Pr%C3%A4prozessor


// *********************************************************************
// *************** (0) CONTROL OVER SERIAL INTERFACE *******************
// *********************************************************************
#if(_LCDML_CONTROL_cfg == 0)
// settings
  # define _LCDML_CONTROL_serial_enter           'e'
  # define _LCDML_CONTROL_serial_up              'w'
  # define _LCDML_CONTROL_serial_down            's'
  # define _LCDML_CONTROL_serial_left            'a'
  # define _LCDML_CONTROL_serial_right           'd'
  # define _LCDML_CONTROL_serial_quit            'q'
// *********************************************************************
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
  }

  // check if new serial input is available
  if (Serial.available()) {
    // read one char from input buffer
    switch (Serial.read())
    {
      case _LCDML_CONTROL_serial_enter:  LCDML.BT_enter(); break;
      case _LCDML_CONTROL_serial_up:     LCDML.BT_up();    break;
      case _LCDML_CONTROL_serial_down:   LCDML.BT_down();  break;
      case _LCDML_CONTROL_serial_left:   LCDML.BT_left();  break;
      case _LCDML_CONTROL_serial_right:  LCDML.BT_right(); break;
      case _LCDML_CONTROL_serial_quit:   LCDML.BT_quit();  break;
      default: break;
    }
  }
}

// *********************************************************************
// ******************************* END *********************************
// *********************************************************************





// *********************************************************************
// *************** (1) CONTROL OVER ONE ANALOG PIN *********************
// *********************************************************************
#elif(_LCDML_CONTROL_cfg == 1)

  unsigned long g_LCDML_DISP_press_time = 0;
// settings
  #define _LCDML_CONTROL_analog_pin              0
  // when you did not use a button set the value to zero
  #define _LCDML_CONTROL_analog_enter_min        850     // Button Enter
  #define _LCDML_CONTROL_analog_enter_max        920
  #define _LCDML_CONTROL_analog_up_min           520     // Button Up
  #define _LCDML_CONTROL_analog_up_max           590
  #define _LCDML_CONTROL_analog_down_min         700     // Button Down
  #define _LCDML_CONTROL_analog_down_max         770
  #define _LCDML_CONTROL_analog_back_min         950     // Button Back
  #define _LCDML_CONTROL_analog_back_max         1020
  #define _LCDML_CONTROL_analog_left_min         430     // Button Left
  #define _LCDML_CONTROL_analog_left_max         500
  #define _LCDML_CONTROL_analog_right_min        610     // Button Right
  #define _LCDML_CONTROL_analog_right_max        680
// *********************************************************************

void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
  }
  // check debounce timer
  if((millis() - g_LCDML_DISP_press_time) >= 200) {
    g_LCDML_DISP_press_time = millis(); // reset debounce timer

    uint16_t value = analogRead(_LCDML_CONTROL_analog_pin);  // analogpin for keypad

    if (value >= _LCDML_CONTROL_analog_enter_min && value <= _LCDML_CONTROL_analog_enter_max) { LCDML.BT_enter(); }
    if (value >= _LCDML_CONTROL_analog_up_min    && value <= _LCDML_CONTROL_analog_up_max)    { LCDML.BT_up();    }
    if (value >= _LCDML_CONTROL_analog_down_min  && value <= _LCDML_CONTROL_analog_down_max)  { LCDML.BT_down();  }
    if (value >= _LCDML_CONTROL_analog_left_min  && value <= _LCDML_CONTROL_analog_left_max)  { LCDML.BT_left();  }
    if (value >= _LCDML_CONTROL_analog_right_min && value <= _LCDML_CONTROL_analog_right_max) { LCDML.BT_right(); }
    if (value >= _LCDML_CONTROL_analog_back_min  && value <= _LCDML_CONTROL_analog_back_max)  { LCDML.BT_quit();  }
  }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************






// *********************************************************************
// *************** (2) CONTROL OVER DIGITAL PINS ***********************
// *********************************************************************
#elif(_LCDML_CONTROL_cfg == 2)
// settings
  unsigned long g_LCDML_DISP_press_time = 0;

  #define _LCDML_CONTROL_digital_low_active      0    // 0 = high active (pulldown) button, 1 = low active (pullup)
                                                      // http://playground.arduino.cc/CommonTopics/PullUpDownResistor
  #define _LCDML_CONTROL_digital_enable_quit     1
  #define _LCDML_CONTROL_digital_enable_lr       1
  #define _LCDML_CONTROL_digital_enter           8
  #define _LCDML_CONTROL_digital_up              9
  #define _LCDML_CONTROL_digital_down            10
  #define _LCDML_CONTROL_digital_quit            11
  #define _LCDML_CONTROL_digital_left            12
  #define _LCDML_CONTROL_digital_right           13
// *********************************************************************
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
    // init buttons
    pinMode(_LCDML_CONTROL_digital_enter      , INPUT_PULLUP);
    pinMode(_LCDML_CONTROL_digital_up         , INPUT_PULLUP);
    pinMode(_LCDML_CONTROL_digital_down       , INPUT_PULLUP);
    # if(_LCDML_CONTROL_digital_enable_quit == 1)
      pinMode(_LCDML_CONTROL_digital_quit     , INPUT_PULLUP);
    # endif
    # if(_LCDML_CONTROL_digital_enable_lr == 1)
      pinMode(_LCDML_CONTROL_digital_left     , INPUT_PULLUP);
      pinMode(_LCDML_CONTROL_digital_right    , INPUT_PULLUP);
    # endif
  }

  #if(_LCDML_CONTROL_digital_low_active == 1)
  #  define _LCDML_CONTROL_digital_a !
  #else
  #  define _LCDML_CONTROL_digital_a
  #endif

  uint8_t but_stat = 0x00;

  bitWrite(but_stat, 0, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_enter)));
  bitWrite(but_stat, 1, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_up)));
  bitWrite(but_stat, 2, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_down)));
  #if(_LCDML_CONTROL_digital_enable_quit == 1)
  bitWrite(but_stat, 3, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_quit)));
  #endif
  #if(_LCDML_CONTROL_digital_enable_lr == 1)
  bitWrite(but_stat, 4, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_left)));
  bitWrite(but_stat, 5, _LCDML_CONTROL_digital_a(digitalRead(_LCDML_CONTROL_digital_right)));
  #endif

  if (but_stat > 0) {
    if((millis() - g_LCDML_DISP_press_time) >= 200) {
      g_LCDML_DISP_press_time = millis(); // reset press time

      if (bitRead(but_stat, 0)) { LCDML.BT_enter(); }
      if (bitRead(but_stat, 1)) { LCDML.BT_up();    }
      if (bitRead(but_stat, 2)) { LCDML.BT_down();  }
      if (bitRead(but_stat, 3)) { LCDML.BT_quit();  }
      if (bitRead(but_stat, 4)) { LCDML.BT_left();  }
      if (bitRead(but_stat, 5)) { LCDML.BT_right(); }
    }
  }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************






// *********************************************************************
// *************** (3) CONTROL WITH ENCODER ****************************
// *********************************************************************
#elif(_LCDML_CONTROL_cfg == 3)

  #define g_LCDML_CONTROL_button_long_press    800   // ms
  #define g_LCDML_CONTROL_button_short_press   20   // ms

  Encoder ENCODER(15,33);

  long  g_LCDML_CONTROL_button_press_time = 0;
  bool  g_LCDML_CONTROL_button_prev       = HIGH;

// *********************************************************************
void lcdml_menu_control(void)
// *********************************************************************
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup())
  {
    // runs only once

    // init pins
    /*
    pinMode(encoder_A_pin      , INPUT_PULLUP);
    pinMode(encoder_B_pin      , INPUT_PULLUP);
    pinMode(encoder_button_pin  , INPUT_PULLUP);
    */
  }

  //volatile Variable
  //long g_LCDML_CONTROL_Encoder_position = ENCODER.read();
  long g_LCDML_CONTROL_Encoder_position = ENCODER.read();
  Serial.println(g_LCDML_CONTROL_Encoder_position);
  //bool button                           = digitalRead(encoder_button_pin);
  bool button                           = !mcp.digitalRead(8);

  if(g_LCDML_CONTROL_Encoder_position >= 3) {

    if(button)
    {
      LCDML.BT_left();
      g_LCDML_CONTROL_button_prev = LOW;
      g_LCDML_CONTROL_button_press_time = -1;
    }
    else
    {
      LCDML.BT_down();
    }
    ENCODER.write(g_LCDML_CONTROL_Encoder_position-4);
  }
  else if(g_LCDML_CONTROL_Encoder_position <= -3)
  {

    if(button)
    {
      LCDML.BT_right();
      g_LCDML_CONTROL_button_prev = LOW;
      g_LCDML_CONTROL_button_press_time = -1;
    }
    else
    {
      LCDML.BT_up();
    }
    ENCODER.write(g_LCDML_CONTROL_Encoder_position+4);
  }
  else
  {
    if(!button && g_LCDML_CONTROL_button_prev)  //falling edge, button pressed
    {
      g_LCDML_CONTROL_button_prev = LOW;
      g_LCDML_CONTROL_button_press_time = millis();
    }
    else if(button && !g_LCDML_CONTROL_button_prev) //rising edge, button not active
    {
       g_LCDML_CONTROL_button_prev = HIGH;

       if(g_LCDML_CONTROL_button_press_time < 0)
       {
         g_LCDML_CONTROL_button_press_time = millis();
         //Reset for left right action
       }
       else if((millis() - g_LCDML_CONTROL_button_press_time) >= g_LCDML_CONTROL_button_long_press)
       {
         LCDML.BT_quit();
       }
       else if((millis() - g_LCDML_CONTROL_button_press_time) >= g_LCDML_CONTROL_button_short_press)
       {
         LCDML.BT_enter();
       }
    }
  }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************





// *********************************************************************
// *************** (4) CONTROL WITH A KEYPAD ***************************
// *********************************************************************
#elif(_LCDML_CONTROL_cfg == 4)
// include
  // more information under http://playground.arduino.cc/Main/KeypadTutorial
  #include <Keypad.h>
// settings
  #define _LCDML_CONTROL_keypad_rows 4 // Four rows
  #define _LCDML_CONTROL_keypad_cols 3 // Three columns
// global vars
  char keys[_LCDML_CONTROL_keypad_rows][_LCDML_CONTROL_keypad_cols] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'#','0','*'}
  };
  byte rowPins[_LCDML_CONTROL_keypad_rows] = { 9, 8, 7, 6 };  // Connect keypad COL0, COL1 and COL2 to these Arduino pins.
  byte colPins[_LCDML_CONTROL_keypad_cols] = { 12, 11, 10 };  // Create the Keypad
// objects
  Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, _LCDML_CONTROL_keypad_rows, _LCDML_CONTROL_keypad_cols );
// *********************************************************************
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
  }
  char key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    switch (key)
    {
      case '#': LCDML.BT_enter(); break;
      case '2': LCDML.BT_up();    break;
      case '8': LCDML.BT_down();  break;
      case '4': LCDML.BT_left();  break;
      case '6': LCDML.BT_right(); break;
      case '*': LCDML.BT_quit();  break;
      default: break;
    }
  }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************


// *********************************************************************
// *************** (5) CONTROL WITH IR REMOTE ***************************
// *********************************************************************
#elif(_LCDML_CONTROL_cfg == 5)
    // IR include (this lib have to be installed)
    #include <IRremote.h>
    // IR global vars
    int RECV_PIN = 11;
    // IR objects
    IRrecv irrecv(RECV_PIN);
    decode_results results;

// *********************************************************************
// change in this function the IR values to your values
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
    irrecv.enableIRIn(); // Start the receiver
  }

  if (irrecv.decode(&results))
  {
      // comment this line out, to check the correct code
      //Serial.println(results.value, HEX);

      // in this switch case you have to change the value 0x...1 to the correct IR code
      switch (results.value)
      {
          case 0x00000001: LCDML.BT_enter(); break;
          case 0x00000002: LCDML.BT_up();    break;
          case 0x00000003: LCDML.BT_down();  break;
          case 0x00000004: LCDML.BT_left();  break;
          case 0x00000005: LCDML.BT_right(); break;
          case 0x00000006: LCDML.BT_quit();  break;
          default: break;
      }
  }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************



// *********************************************************************
// *************** (6) CONTROL OVER JOYSTICK ***************************
// *********************************************************************
#elif(_LCDML_CONTROL_cfg == 6)
  unsigned long g_LCDML_DISP_press_time = 0;
    // settings
    #define _LCDML_CONTROL_analog_pinx A0
    #define _LCDML_CONTROL_analog_piny A1
    #define _LCDML_CONTROL_digitalread 33 //don't work with u8glib

    // when you did not use a button set the value to zero
    #define _LCDML_CONTROL_analog_up_min 612 // Button Up
    #define _LCDML_CONTROL_analog_up_max 1023
    #define _LCDML_CONTROL_analog_down_min 0 // Button Down
    #define _LCDML_CONTROL_analog_down_max 412
    #define _LCDML_CONTROL_analog_left_min 612 // Button Left
    #define _LCDML_CONTROL_analog_left_max 1023
    #define _LCDML_CONTROL_analog_right_min 0 // Button Right
    #define _LCDML_CONTROL_analog_right_max 412
// *********************************************************************
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
    pinMode (_LCDML_CONTROL_digitalread, INPUT);
  }
  // check debounce timer
  if((millis() - g_LCDML_DISP_press_time) >= 200) {
    g_LCDML_DISP_press_time = millis(); // reset debounce timer

    uint16_t valuex = analogRead(_LCDML_CONTROL_analog_pinx);  // analogpinx
    uint16_t valuey = analogRead(_LCDML_CONTROL_analog_piny);  // analogpinx
    uint16_t valuee = digitalRead(_LCDML_CONTROL_digitalread);  //digitalpinenter


    if (valuey >= _LCDML_CONTROL_analog_up_min && valuey <= _LCDML_CONTROL_analog_up_max) { LCDML.BT_up(); }        // up
    if (valuey >= _LCDML_CONTROL_analog_down_min && valuey <= _LCDML_CONTROL_analog_down_max) { LCDML.BT_down(); }    // down
    if (valuex >= _LCDML_CONTROL_analog_left_min && valuex <= _LCDML_CONTROL_analog_left_max) { LCDML.BT_left(); }     // left
    if (valuex >= _LCDML_CONTROL_analog_right_min && valuex <= _LCDML_CONTROL_analog_right_max) { LCDML.BT_right(); }    // right

    if(valuee == true) {LCDML.BT_enter();}    // enter

    // back buttons have to be included as menu item
    // lock at the example LCDML_back_button
  }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************

// *********************************************************************
// *************** (7) CONTROL OVER PCF8574 ****************************
// *********************************************************************

#elif(_LCDML_CONTROL_cfg == 7)
  unsigned long g_LCDML_DISP_press_time = 0;
  #define PCF8574_1 0x26   #define PCF8574_1 0x26 // I2C address for the buttons

  #define PCF8574_Pin0 254
  #define PCF8574_Pin1 253
  #define PCF8574_Pin2 251
  #define PCF8574_Pin3 247
  #define PCF8574_Pin4 239
  #define PCF8574_Pin5 223
  #define PCF8574_Pin6 191
  #define PCF8574_Pin7 127

  // Specify the PCF8574 pins here
  #define _LCDML_CONTROL_PCF8574_enable_quit    0
  #define _LCDML_CONTROL_PCF8574_enable_lr      0
  #define _LCDML_CONTROL_PCF8574_enter          PCF8574_Pin1
  #define _LCDML_CONTROL_PCF8574_up             PCF8574_Pin2
  #define _LCDML_CONTROL_PCF8574_down           PCF8574_Pin0
  #define _LCDML_CONTROL_PCF8574_left           PCF8574_Pin2
  #define _LCDML_CONTROL_PCF8574l_right         PCF8574_Pin2
  #define _LCDML_CONTROL_PCF8574_quit           PCF8574_Pin2
// **********************************************************
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once
  }

  if((millis() - g_LCDML_DISP_press_time) >= 200) {
      g_LCDML_DISP_press_time = millis(); // reset press time

    Wire.write(0xff); // All pins as input?
    Wire.requestFrom(PCF8574_1, 1);
   if (Wire.available()) {
    switch (Wire.read())
    {
      case _LCDML_CONTROL_PCF8574_enter:  LCDML.BT_enter(); break;
      case _LCDML_CONTROL_PCF8574_up:     LCDML.BT_up();    break;
      case _LCDML_CONTROL_PCF8574_down:   LCDML.BT_down();  break;
       #if(_LCDML_CONTROL_PCF8574_enable_quit == 1)
      case _LCDML_CONTROL_PCF8574_left:   LCDML.BT_left();  break;
    #endif
       #if(_LCDML_CONTROL_PCF8574_enable_lr   == 1)
      case _LCDML_CONTROL_PCF8574l_right: LCDML.BT_right(); break;
      case _LCDML_CONTROL_PCF8574_quit:   LCDML.BT_quit();  break;
    #endif
      default: break;
    }
  }
 }
}
// *********************************************************************
// ******************************* END *********************************
// *********************************************************************


#else
  #error _LCDML_CONTROL_cfg is not defined or not in range
#endif
// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib2.h
# endif

# if(_LCDML_glcd_tft_box_x1 > _LCDML_glcd_tft_w)
# error _LCDML_glcd_tft_box_x1 is to big
# endif

# if(_LCDML_glcd_tft_box_y1 > _LCDML_glcd_tft_h)
# error _LCDML_glcd_tft_box_y1 is to big
# endif
