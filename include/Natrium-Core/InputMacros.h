#if !defined(NEO_INPUT_MACROS_H)
#define NEO_INPUT_MACROS_H

#define NEO_RELEASE                0
#define NEO_PRESS                  1
#define NEO_REPEAT                 2

#define NEO_HAT_CENTERED           0
#define NEO_HAT_UP                 1
#define NEO_HAT_RIGHT              2
#define NEO_HAT_DOWN               4
#define NEO_HAT_LEFT               8
#define NEO_HAT_RIGHT_UP           (NEO_HAT_RIGHT | NEO_HAT_UP)
#define NEO_HAT_RIGHT_DOWN         (NEO_HAT_RIGHT | NEO_HAT_DOWN)
#define NEO_HAT_LEFT_UP            (NEO_HAT_LEFT  | NEO_HAT_UP)
#define NEO_HAT_LEFT_DOWN          (NEO_HAT_LEFT  | NEO_HAT_DOWN)

#define NEO_KEY_UNKNOWN            -1
#define NEO_KEY_SPACE              32
#define NEO_KEY_APOSTROPHE         39  /* ' */
#define NEO_KEY_COMMA              44  /* , */
#define NEO_KEY_MINUS              45  /* - */
#define NEO_KEY_PERIOD             46  /* . */
#define NEO_KEY_SLASH              47  /* / */
#define NEO_KEY_0                  48
#define NEO_KEY_1                  49
#define NEO_KEY_2                  50
#define NEO_KEY_3                  51
#define NEO_KEY_4                  52
#define NEO_KEY_5                  53
#define NEO_KEY_6                  54
#define NEO_KEY_7                  55
#define NEO_KEY_8                  56
#define NEO_KEY_9                  57
#define NEO_KEY_SEMICOLON          59  /* ; */
#define NEO_KEY_EQUAL              61  /* = */
#define NEO_KEY_A                  65
#define NEO_KEY_B                  66
#define NEO_KEY_C                  67
#define NEO_KEY_D                  68
#define NEO_KEY_E                  69
#define NEO_KEY_F                  70
#define NEO_KEY_G                  71
#define NEO_KEY_H                  72
#define NEO_KEY_I                  73
#define NEO_KEY_J                  74
#define NEO_KEY_K                  75
#define NEO_KEY_L                  76
#define NEO_KEY_M                  77
#define NEO_KEY_N                  78
#define NEO_KEY_O                  79
#define NEO_KEY_P                  80
#define NEO_KEY_Q                  81
#define NEO_KEY_R                  82
#define NEO_KEY_S                  83
#define NEO_KEY_T                  84
#define NEO_KEY_U                  85
#define NEO_KEY_V                  86
#define NEO_KEY_W                  87
#define NEO_KEY_X                  88
#define NEO_KEY_Y                  89
#define NEO_KEY_Z                  90
#define NEO_KEY_LEFT_BRACKET       91  /* [ */
#define NEO_KEY_BACKSLASH          92  /* \ */
#define NEO_KEY_RIGHT_BRACKET      93  /* ] */
#define NEO_KEY_GRAVE_ACCENT       96  /* ` */
#define NEO_KEY_WORLD_1            161 /* non-US #1 */
#define NEO_KEY_WORLD_2            162 /* non-US #2 */

#define NEO_KEY_ESCAPE             256
#define NEO_KEY_ENTER              257
#define NEO_KEY_TAB                258
#define NEO_KEY_BACKSPACE          259
#define NEO_KEY_INSERT             260
#define NEO_KEY_DELETE             261
#define NEO_KEY_RIGHT              262
#define NEO_KEY_LEFT               263
#define NEO_KEY_DOWN               264
#define NEO_KEY_UP                 265
#define NEO_KEY_PAGE_UP            266
#define NEO_KEY_PAGE_DOWN          267
#define NEO_KEY_HOME               268
#define NEO_KEY_END                269
#define NEO_KEY_CAPS_LOCK          280
#define NEO_KEY_SCROLL_LOCK        281
#define NEO_KEY_NUM_LOCK           282
#define NEO_KEY_PRINT_SCREEN       283
#define NEO_KEY_PAUSE              284
#define NEO_KEY_F1                 290
#define NEO_KEY_F2                 291
#define NEO_KEY_F3                 292
#define NEO_KEY_F4                 293
#define NEO_KEY_F5                 294
#define NEO_KEY_F6                 295
#define NEO_KEY_F7                 296
#define NEO_KEY_F8                 297
#define NEO_KEY_F9                 298
#define NEO_KEY_F10                299
#define NEO_KEY_F11                300
#define NEO_KEY_F12                301
#define NEO_KEY_F13                302
#define NEO_KEY_F14                303
#define NEO_KEY_F15                304
#define NEO_KEY_F16                305
#define NEO_KEY_F17                306
#define NEO_KEY_F18                307
#define NEO_KEY_F19                308
#define NEO_KEY_F20                309
#define NEO_KEY_F21                310
#define NEO_KEY_F22                311
#define NEO_KEY_F23                312
#define NEO_KEY_F24                313
#define NEO_KEY_F25                314
#define NEO_KEY_KP_0               320
#define NEO_KEY_KP_1               321
#define NEO_KEY_KP_2               322
#define NEO_KEY_KP_3               323
#define NEO_KEY_KP_4               324
#define NEO_KEY_KP_5               325
#define NEO_KEY_KP_6               326
#define NEO_KEY_KP_7               327
#define NEO_KEY_KP_8               328
#define NEO_KEY_KP_9               329
#define NEO_KEY_KP_DECIMAL         330
#define NEO_KEY_KP_DIVIDE          331
#define NEO_KEY_KP_MULTIPLY        332
#define NEO_KEY_KP_SUBTRACT        333
#define NEO_KEY_KP_ADD             334
#define NEO_KEY_KP_ENTER           335
#define NEO_KEY_KP_EQUAL           336
#define NEO_KEY_LEFT_SHIFT         340
#define NEO_KEY_LEFT_CONTROL       341
#define NEO_KEY_LEFT_ALT           342
#define NEO_KEY_LEFT_SUPER         343
#define NEO_KEY_RIGHT_SHIFT        344
#define NEO_KEY_RIGHT_CONTROL      345
#define NEO_KEY_RIGHT_ALT          346
#define NEO_KEY_RIGHT_SUPER        347
#define NEO_KEY_MENU               348
#define NEO_KEY_LAST               NEO_KEY_MENU

#define NEO_MOD_SHIFT           0x0001
#define NEO_MOD_CONTROL         0x0002
#define NEO_MOD_ALT             0x0004
#define NEO_MOD_SUPER           0x0008
#define NEO_MOD_CAPS_LOCK       0x0010
#define NEO_MOD_NUM_LOCK        0x0020

#define NEO_MOUSE_BUTTON_1         0
#define NEO_MOUSE_BUTTON_2         1
#define NEO_MOUSE_BUTTON_3         2
#define NEO_MOUSE_BUTTON_4         3
#define NEO_MOUSE_BUTTON_5         4
#define NEO_MOUSE_BUTTON_6         5
#define NEO_MOUSE_BUTTON_7         6
#define NEO_MOUSE_BUTTON_8         7
#define NEO_MOUSE_BUTTON_LAST      NEO_MOUSE_BUTTON_8
#define NEO_MOUSE_BUTTON_LEFT      NEO_MOUSE_BUTTON_1
#define NEO_MOUSE_BUTTON_RIGHT     NEO_MOUSE_BUTTON_2
#define NEO_MOUSE_BUTTON_MIDDLE    NEO_MOUSE_BUTTON_3

#define NEO_JOYSTICK_1             0
#define NEO_JOYSTICK_2             1
#define NEO_JOYSTICK_3             2
#define NEO_JOYSTICK_4             3
#define NEO_JOYSTICK_5             4
#define NEO_JOYSTICK_6             5
#define NEO_JOYSTICK_7             6
#define NEO_JOYSTICK_8             7
#define NEO_JOYSTICK_9             8
#define NEO_JOYSTICK_10            9
#define NEO_JOYSTICK_11            10
#define NEO_JOYSTICK_12            11
#define NEO_JOYSTICK_13            12
#define NEO_JOYSTICK_14            13
#define NEO_JOYSTICK_15            14
#define NEO_JOYSTICK_16            15
#define NEO_JOYSTICK_LAST          NEO_JOYSTICK_16

#define NEO_GAMEPAD_BUTTON_A               0
#define NEO_GAMEPAD_BUTTON_B               1
#define NEO_GAMEPAD_BUTTON_X               2
#define NEO_GAMEPAD_BUTTON_Y               3
#define NEO_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define NEO_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define NEO_GAMEPAD_BUTTON_BACK            6
#define NEO_GAMEPAD_BUTTON_START           7
#define NEO_GAMEPAD_BUTTON_GUIDE           8
#define NEO_GAMEPAD_BUTTON_LEFT_THUMB      9
#define NEO_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define NEO_GAMEPAD_BUTTON_DPAD_UP         11
#define NEO_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define NEO_GAMEPAD_BUTTON_DPAD_DOWN       13
#define NEO_GAMEPAD_BUTTON_DPAD_LEFT       14
#define NEO_GAMEPAD_BUTTON_LAST            NEO_GAMEPAD_BUTTON_DPAD_LEFT

#define NEO_GAMEPAD_BUTTON_CROSS       NEO_GAMEPAD_BUTTON_A
#define NEO_GAMEPAD_BUTTON_CIRCLE      NEO_GAMEPAD_BUTTON_B
#define NEO_GAMEPAD_BUTTON_SQUARE      NEO_GAMEPAD_BUTTON_X
#define NEO_GAMEPAD_BUTTON_TRIANGLE    NEO_GAMEPAD_BUTTON_Y

#define NEO_GAMEPAD_AXIS_LEFT_X        0
#define NEO_GAMEPAD_AXIS_LEFT_Y        1
#define NEO_GAMEPAD_AXIS_RIGHT_X       2
#define NEO_GAMEPAD_AXIS_RIGHT_Y       3
#define NEO_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define NEO_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define NEO_GAMEPAD_AXIS_LAST          NEO_GAMEPAD_AXIS_RIGHT_TRIGGER

#endif // NEO_INPUT_MACROS_H