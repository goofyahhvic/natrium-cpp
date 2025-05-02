#if !defined(NA_INPUT_MACROS_H)
#define NA_INPUT_MACROS_H

#define NA_RELEASE                0
#define NA_PRESS                  1
#define NA_REPEAT                 2

#define NA_HAT_CENTERED           0
#define NA_HAT_UP                 1
#define NA_HAT_RIGHT              2
#define NA_HAT_DOWN               4
#define NA_HAT_LEFT               8
#define NA_HAT_RIGHT_UP           (NA_HAT_RIGHT | NA_HAT_UP)
#define NA_HAT_RIGHT_DOWN         (NA_HAT_RIGHT | NA_HAT_DOWN)
#define NA_HAT_LEFT_UP            (NA_HAT_LEFT  | NA_HAT_UP)
#define NA_HAT_LEFT_DOWN          (NA_HAT_LEFT  | NA_HAT_DOWN)

#define NA_KEY_UNKNOWN            -1
#define NA_KEY_SPACE              32
#define NA_KEY_APOSTROPHE         39  /* ' */
#define NA_KEY_COMMA              44  /* , */
#define NA_KEY_MINUS              45  /* - */
#define NA_KEY_PERIOD             46  /* . */
#define NA_KEY_SLASH              47  /* / */
#define NA_KEY_0                  48
#define NA_KEY_1                  49
#define NA_KEY_2                  50
#define NA_KEY_3                  51
#define NA_KEY_4                  52
#define NA_KEY_5                  53
#define NA_KEY_6                  54
#define NA_KEY_7                  55
#define NA_KEY_8                  56
#define NA_KEY_9                  57
#define NA_KEY_SEMICOLON          59  /* ; */
#define NA_KEY_EQUAL              61  /* = */
#define NA_KEY_A                  65
#define NA_KEY_B                  66
#define NA_KEY_C                  67
#define NA_KEY_D                  68
#define NA_KEY_E                  69
#define NA_KEY_F                  70
#define NA_KEY_G                  71
#define NA_KEY_H                  72
#define NA_KEY_I                  73
#define NA_KEY_J                  74
#define NA_KEY_K                  75
#define NA_KEY_L                  76
#define NA_KEY_M                  77
#define NA_KEY_N                  78
#define NA_KEY_O                  79
#define NA_KEY_P                  80
#define NA_KEY_Q                  81
#define NA_KEY_R                  82
#define NA_KEY_S                  83
#define NA_KEY_T                  84
#define NA_KEY_U                  85
#define NA_KEY_V                  86
#define NA_KEY_W                  87
#define NA_KEY_X                  88
#define NA_KEY_Y                  89
#define NA_KEY_Z                  90
#define NA_KEY_LEFT_BRACKET       91  /* [ */
#define NA_KEY_BACKSLASH          92  /* \ */
#define NA_KEY_RIGHT_BRACKET      93  /* ] */
#define NA_KEY_GRAVE_ACCENT       96  /* ` */
#define NA_KEY_WORLD_1            161 /* non-US #1 */
#define NA_KEY_WORLD_2            162 /* non-US #2 */

#define NA_KEY_ESCAPE             256
#define NA_KEY_ENTER              257
#define NA_KEY_TAB                258
#define NA_KEY_BACKSPACE          259
#define NA_KEY_INSERT             260
#define NA_KEY_DELETE             261
#define NA_KEY_RIGHT              262
#define NA_KEY_LEFT               263
#define NA_KEY_DOWN               264
#define NA_KEY_UP                 265
#define NA_KEY_PAGE_UP            266
#define NA_KEY_PAGE_DOWN          267
#define NA_KEY_HOME               268
#define NA_KEY_END                269
#define NA_KEY_CAPS_LOCK          280
#define NA_KEY_SCROLL_LOCK        281
#define NA_KEY_NUM_LOCK           282
#define NA_KEY_PRINT_SCREEN       283
#define NA_KEY_PAUSE              284
#define NA_KEY_F1                 290
#define NA_KEY_F2                 291
#define NA_KEY_F3                 292
#define NA_KEY_F4                 293
#define NA_KEY_F5                 294
#define NA_KEY_F6                 295
#define NA_KEY_F7                 296
#define NA_KEY_F8                 297
#define NA_KEY_F9                 298
#define NA_KEY_F10                299
#define NA_KEY_F11                300
#define NA_KEY_F12                301
#define NA_KEY_F13                302
#define NA_KEY_F14                303
#define NA_KEY_F15                304
#define NA_KEY_F16                305
#define NA_KEY_F17                306
#define NA_KEY_F18                307
#define NA_KEY_F19                308
#define NA_KEY_F20                309
#define NA_KEY_F21                310
#define NA_KEY_F22                311
#define NA_KEY_F23                312
#define NA_KEY_F24                313
#define NA_KEY_F25                314
#define NA_KEY_KP_0               320
#define NA_KEY_KP_1               321
#define NA_KEY_KP_2               322
#define NA_KEY_KP_3               323
#define NA_KEY_KP_4               324
#define NA_KEY_KP_5               325
#define NA_KEY_KP_6               326
#define NA_KEY_KP_7               327
#define NA_KEY_KP_8               328
#define NA_KEY_KP_9               329
#define NA_KEY_KP_DECIMAL         330
#define NA_KEY_KP_DIVIDE          331
#define NA_KEY_KP_MULTIPLY        332
#define NA_KEY_KP_SUBTRACT        333
#define NA_KEY_KP_ADD             334
#define NA_KEY_KP_ENTER           335
#define NA_KEY_KP_EQUAL           336
#define NA_KEY_LEFT_SHIFT         340
#define NA_KEY_LEFT_CONTROL       341
#define NA_KEY_LEFT_ALT           342
#define NA_KEY_LEFT_SUPER         343
#define NA_KEY_RIGHT_SHIFT        344
#define NA_KEY_RIGHT_CONTROL      345
#define NA_KEY_RIGHT_ALT          346
#define NA_KEY_RIGHT_SUPER        347
#define NA_KEY_MENU               348
#define NA_KEY_LAST               NA_KEY_MENU

#define NA_MOD_SHIFT           0x0001
#define NA_MOD_CONTROL         0x0002
#define NA_MOD_ALT             0x0004
#define NA_MOD_SUPER           0x0008
#define NA_MOD_CAPS_LOCK       0x0010
#define NA_MOD_NUM_LOCK        0x0020

#define NA_MOUSE_BUTTON_1         0
#define NA_MOUSE_BUTTON_2         1
#define NA_MOUSE_BUTTON_3         2
#define NA_MOUSE_BUTTON_4         3
#define NA_MOUSE_BUTTON_5         4
#define NA_MOUSE_BUTTON_6         5
#define NA_MOUSE_BUTTON_7         6
#define NA_MOUSE_BUTTON_8         7
#define NA_MOUSE_BUTTON_LAST      NA_MOUSE_BUTTON_8
#define NA_MOUSE_BUTTON_LEFT      NA_MOUSE_BUTTON_1
#define NA_MOUSE_BUTTON_RIGHT     NA_MOUSE_BUTTON_2
#define NA_MOUSE_BUTTON_MIDDLE    NA_MOUSE_BUTTON_3

#define NA_JOYSTICK_1             0
#define NA_JOYSTICK_2             1
#define NA_JOYSTICK_3             2
#define NA_JOYSTICK_4             3
#define NA_JOYSTICK_5             4
#define NA_JOYSTICK_6             5
#define NA_JOYSTICK_7             6
#define NA_JOYSTICK_8             7
#define NA_JOYSTICK_9             8
#define NA_JOYSTICK_10            9
#define NA_JOYSTICK_11            10
#define NA_JOYSTICK_12            11
#define NA_JOYSTICK_13            12
#define NA_JOYSTICK_14            13
#define NA_JOYSTICK_15            14
#define NA_JOYSTICK_16            15
#define NA_JOYSTICK_LAST          NA_JOYSTICK_16

#define NA_GAMEPAD_BUTTON_A               0
#define NA_GAMEPAD_BUTTON_B               1
#define NA_GAMEPAD_BUTTON_X               2
#define NA_GAMEPAD_BUTTON_Y               3
#define NA_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define NA_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define NA_GAMEPAD_BUTTON_BACK            6
#define NA_GAMEPAD_BUTTON_START           7
#define NA_GAMEPAD_BUTTON_GUIDE           8
#define NA_GAMEPAD_BUTTON_LEFT_THUMB      9
#define NA_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define NA_GAMEPAD_BUTTON_DPAD_UP         11
#define NA_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define NA_GAMEPAD_BUTTON_DPAD_DOWN       13
#define NA_GAMEPAD_BUTTON_DPAD_LEFT       14
#define NA_GAMEPAD_BUTTON_LAST            NA_GAMEPAD_BUTTON_DPAD_LEFT

#define NA_GAMEPAD_BUTTON_CROSS       NA_GAMEPAD_BUTTON_A
#define NA_GAMEPAD_BUTTON_CIRCLE      NA_GAMEPAD_BUTTON_B
#define NA_GAMEPAD_BUTTON_SQUARE      NA_GAMEPAD_BUTTON_X
#define NA_GAMEPAD_BUTTON_TRIANGLE    NA_GAMEPAD_BUTTON_Y

#define NA_GAMEPAD_AXIS_LEFT_X        0
#define NA_GAMEPAD_AXIS_LEFT_Y        1
#define NA_GAMEPAD_AXIS_RIGHT_X       2
#define NA_GAMEPAD_AXIS_RIGHT_Y       3
#define NA_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define NA_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define NA_GAMEPAD_AXIS_LAST          NA_GAMEPAD_AXIS_RIGHT_TRIGGER

#endif // NA_INPUT_MACROS_H