#if !defined(NA_INPUT_CONSTANTS_HPP)
#define NA_INPUT_CONSTANTS_HPP

#include "Natrium/Core.hpp"

namespace Na {
	using KeyAction = i8;
	namespace KeyActions {
		constexpr KeyAction k_Unknown = -1;
		constexpr KeyAction k_Release = 0;
		constexpr KeyAction k_Press   = 1;
		constexpr KeyAction k_Repeat  = 2;
	} // namespace KeyActions

	using Key = i16;
	namespace Keys {
		constexpr Key k_Unknown      = -1;

		constexpr Key k_Space        = 32;
		constexpr Key k_Apostrophe   = 39;  /* ' */
		constexpr Key k_Comma        = 44;  /* , */
		constexpr Key k_Minus        = 45;  /* - */
		constexpr Key k_Period       = 46;  /* . */
		constexpr Key k_Slash        = 47;  /* / */
		constexpr Key k_0            = 48;
		constexpr Key k_1            = 49;
		constexpr Key k_2            = 50;
		constexpr Key k_3            = 51;
		constexpr Key k_4            = 52;
		constexpr Key k_5            = 53;
		constexpr Key k_6            = 54;
		constexpr Key k_7            = 55;
		constexpr Key k_8            = 56;
		constexpr Key k_9            = 57;
		constexpr Key k_Semicolon    = 59;  /* ; */
		constexpr Key k_Equal        = 61;  /* = */
		constexpr Key k_A            = 65;
		constexpr Key k_B            = 66;
		constexpr Key k_C            = 67;
		constexpr Key k_D            = 68;
		constexpr Key k_E            = 69;
		constexpr Key k_F            = 70;
		constexpr Key k_G            = 71;
		constexpr Key k_H            = 72;
		constexpr Key k_I            = 73;
		constexpr Key k_J            = 74;
		constexpr Key k_K            = 75;
		constexpr Key k_L            = 76;
		constexpr Key k_M            = 77;
		constexpr Key k_N            = 78;
		constexpr Key k_O            = 79;
		constexpr Key k_P            = 80;
		constexpr Key k_Q            = 81;
		constexpr Key k_R            = 82;
		constexpr Key k_S            = 83;
		constexpr Key k_T            = 84;
		constexpr Key k_U            = 85;
		constexpr Key k_V            = 86;
		constexpr Key k_W            = 87;
		constexpr Key k_X            = 88;
		constexpr Key k_Y            = 89;
		constexpr Key k_Z            = 90;
		constexpr Key k_LeftBracket  = 91;  /* [ */
		constexpr Key k_Backslash    = 92;  /* \ */
		constexpr Key k_RightBracket = 93;  /* ] */
		constexpr Key k_GraveAccent  = 96;  /* ` */
		constexpr Key k_World1       = 161; /* non-US #1 */
		constexpr Key k_World2       = 162; /* non-US #2 */

		constexpr Key k_Escape       = 256;
		constexpr Key k_Enter        = 257;
		constexpr Key k_Tab          = 258;
		constexpr Key k_Backspace    = 259;
		constexpr Key k_Insert       = 260;
		constexpr Key k_Delete       = 261;
		constexpr Key k_Right        = 262;
		constexpr Key k_Left         = 263;
		constexpr Key k_Down         = 264;
		constexpr Key k_Up           = 265;
		constexpr Key k_PageUp       = 266;
		constexpr Key k_PageDown     = 267;
		constexpr Key k_Home         = 268;
		constexpr Key k_End          = 269;
		constexpr Key k_CapsLock     = 280;
		constexpr Key k_ScrollLock   = 281;
		constexpr Key k_NumLock      = 282;
		constexpr Key k_PrintScreen  = 283;
		constexpr Key k_Pause        = 284;
		constexpr Key k_F1           = 290;
		constexpr Key k_F2           = 291;
		constexpr Key k_F3           = 292;
		constexpr Key k_F4           = 293;
		constexpr Key k_F5           = 294;
		constexpr Key k_F6           = 295;
		constexpr Key k_F7           = 296;
		constexpr Key k_F8           = 297;
		constexpr Key k_F9           = 298;
		constexpr Key k_F10          = 299;
		constexpr Key k_F11          = 300;
		constexpr Key k_F12          = 301;
		constexpr Key k_F13          = 302;
		constexpr Key k_F14          = 303;
		constexpr Key k_F15          = 304;
		constexpr Key k_F16          = 305;
		constexpr Key k_F17          = 306;
		constexpr Key k_F18          = 307;
		constexpr Key k_F19          = 308;
		constexpr Key k_F20          = 309;
		constexpr Key k_F21          = 310;
		constexpr Key k_F22          = 311;
		constexpr Key k_F23          = 312;
		constexpr Key k_F24          = 313;
		constexpr Key k_F25          = 314;

		constexpr Key k_KP0          = 320;
		constexpr Key k_KP1          = 321;
		constexpr Key k_KP2          = 322;
		constexpr Key k_KP3          = 323;
		constexpr Key k_KP4          = 324;
		constexpr Key k_KP5          = 325;
		constexpr Key k_KP6          = 326;
		constexpr Key k_KP7          = 327;
		constexpr Key k_KP8          = 328;
		constexpr Key k_KP9          = 329;
		constexpr Key k_KPDecimal    = 330;
		constexpr Key k_KPDivide     = 331;
		constexpr Key k_KPMultiply   = 332;
		constexpr Key k_KPSubtract   = 333;
		constexpr Key k_KPAdd        = 334;
		constexpr Key k_KPEnter      = 335;
		constexpr Key k_KPEqual      = 336;

		constexpr Key k_LeftShift    = 340;
		constexpr Key k_LeftControl  = 341;
		constexpr Key k_LeftAlt      = 342;
		constexpr Key k_LeftSuper    = 343;
		constexpr Key k_RightShift   = 344;
		constexpr Key k_RightControl = 345;
		constexpr Key k_RightAlt     = 346;
		constexpr Key k_RightSuper   = 347;
		constexpr Key k_Menu         = 348;

		constexpr Key k_Last         = k_Menu;
	} // namespace Keys

	using KeyMod = i8;
	namespace KeyMods {
		constexpr KeyMod k_Unknown  = -1;

		constexpr KeyMod k_Shift    = NA_BIT(0);
		constexpr KeyMod k_Control  = NA_BIT(1);
		constexpr KeyMod k_Alt      = NA_BIT(2);
		constexpr KeyMod k_Super    = NA_BIT(3);
		constexpr KeyMod k_CapsLock = NA_BIT(4);
		constexpr KeyMod k_NumLock  = NA_BIT(5);
	} // namespace KeyMods

	using MouseButtonAction = i8;
	namespace MouseButtonActions {
		constexpr MouseButtonAction k_Unknown = -1;
		constexpr MouseButtonAction k_Release = 0;
		constexpr MouseButtonAction k_Press   = 1;
	} // namespace MouseButtonActions

	using MouseButton = i8;
	namespace MouseButtons {
		constexpr MouseButton k_Unknown = -1;

		constexpr MouseButton k_1       = 0;
		constexpr MouseButton k_2       = 1;
		constexpr MouseButton k_3       = 2;
		constexpr MouseButton k_4       = 3;
		constexpr MouseButton k_5       = 4;
		constexpr MouseButton k_6       = 5;
		constexpr MouseButton k_7       = 6;
		constexpr MouseButton k_8       = 7;

		constexpr MouseButton k_Last    = k_8;

		constexpr MouseButton k_Left    = k_1;
		constexpr MouseButton k_Right   = k_2;
		constexpr MouseButton k_Middle  = k_3;
	} // namespace MouseButtons

	using Joystick = i8;
	namespace Joysticks {
		constexpr Joystick k_Unknown = -1;

		constexpr Joystick k_1       = 0;
		constexpr Joystick k_2       = 1;
		constexpr Joystick k_3       = 2;
		constexpr Joystick k_4       = 3;
		constexpr Joystick k_5       = 4;
		constexpr Joystick k_6       = 5;
		constexpr Joystick k_7       = 6;
		constexpr Joystick k_8       = 7;
		constexpr Joystick k_9       = 8;
		constexpr Joystick k_10      = 9;
		constexpr Joystick k_11      = 10;
		constexpr Joystick k_12      = 11;
		constexpr Joystick k_13      = 12;
		constexpr Joystick k_14      = 13;
		constexpr Joystick k_15      = 14;
		constexpr Joystick k_16      = 15;

		constexpr Joystick k_Last    = k_16;
	} // namespace Joysticks

	using GamepadButton = i8;
	namespace GamepadButtons {
		constexpr GamepadButton k_Unknown     = -1;

		constexpr GamepadButton k_A           = 0;
		constexpr GamepadButton k_B           = 1;
		constexpr GamepadButton k_X           = 2;
		constexpr GamepadButton k_Y           = 3;
		constexpr GamepadButton k_LeftBumper  = 4;
		constexpr GamepadButton k_RightBumper = 5;
		constexpr GamepadButton k_Back        = 6;
		constexpr GamepadButton k_Start       = 7;
		constexpr GamepadButton k_Guide       = 8;
		constexpr GamepadButton k_LeftThumb   = 9;
		constexpr GamepadButton k_RightThumb  = 10;
		constexpr GamepadButton k_DPadUp      = 11;
		constexpr GamepadButton k_DPadRight   = 12;
		constexpr GamepadButton k_DPadDown    = 13;
		constexpr GamepadButton k_DPadLeft    = 14;

		constexpr GamepadButton k_Last        = k_DPadLeft;

		constexpr GamepadButton k_Cross       = k_A;
		constexpr GamepadButton k_Circle      = k_B;
		constexpr GamepadButton k_Square      = k_X;
		constexpr GamepadButton k_Triangle    = k_Y;
	} // namespace GamepadButtons

	using GamepadAxis = i8;
	namespace GamepadAxes {
		constexpr GamepadAxis k_Unknown      = -1;

		constexpr GamepadAxis k_LeftX        = 0;
		constexpr GamepadAxis k_LeftY        = 1;
		constexpr GamepadAxis k_RightX       = 2;
		constexpr GamepadAxis k_RightY       = 3;
		constexpr GamepadAxis k_LeftTrigger  = 4;
		constexpr GamepadAxis k_RightTrigger = 5;

		constexpr GamepadAxis k_Last         = k_RightTrigger;
	} // namespace GamepadAxes

	using Hat = i8;
	namespace Hats {
		constexpr Hat k_Unknown = -1;

		constexpr Hat k_Centered  = 0;
		constexpr Hat k_Up        = NA_BIT(0);
		constexpr Hat k_Right     = NA_BIT(1);
		constexpr Hat k_Down      = NA_BIT(2);
		constexpr Hat k_Left      = NA_BIT(3);
		constexpr Hat k_RightUp   = k_Right | k_Up;
		constexpr Hat k_RightDown = k_Right | k_Down;
		constexpr Hat k_LeftUp	  = k_Left  | k_Up;
		constexpr Hat k_LeftDow   = k_Left  | k_Down;
	} // namespace Hats
} // namespace Na

#endif // NA_INPUT_CONSTANTS_HPP