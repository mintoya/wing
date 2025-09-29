const validKeyValues = [{
  name: "NONE",
  value: "KEY_NONE"
},
  {
    name: "Layer 1",
    value: "L( 1 )"
  },
  {
    name: "Layer 2",
    value: "L( 2 )"
  },
  {
    name: "Layer 3",
    value: "L( 3 )"
  },
  {
    name: "Layer 4",
    value: "L( 4 )"
  },
  {
    name: "Layer 5",
    value: "L( 5 )"
  },
  {
    name: "Layer 6",
    value: "L( 6 )"
  },
  {
    name: "Layer 7",
    value: "L( 7 )"
  },
  {
    name: "Layer 8",
    value: "L( 8 )"
  },
  {
    name: "Layer 9",
    value: "L( 9 )"
  },
  {
    name: "Left Ctrl",
    value: "M( lc )"
  },
  {
    name: "Left Shift",
    value: "M( ls )"
  },
  {
    name: "Left Alt",
    value: "M( la )"
  },
  {
    name: "Left Meta",
    value: "M( lm )"
  },
  {
    name: "Right Ctrl",
    value: "M( rc )"
  },
  {
    name: "Right Shift",
    value: "M( rs )"
  },
  {
    name: "Right Alt",
    value: "M( ra )"
  },
  {
    name: "Right Meta",
    value: "M( rm )"
  },
  {
    name: "KEY_ERR_OVF",
    value: "KEY_ERR_OVF"
  },
  {
    name: "A",
    value: "KEY_A"
  },
  {
    name: "B",
    value: "KEY_B"
  },
  {
    name: "C",
    value: "KEY_C"
  },
  {
    name: "D",
    value: "KEY_D"
  },
  {
    name: "E",
    value: "KEY_E"
  },
  {
    name: "F",
    value: "KEY_F"
  },
  {
    name: "G",
    value: "KEY_G"
  },
  {
    name: "H",
    value: "KEY_H"
  },
  {
    name: "I",
    value: "KEY_I"
  },
  {
    name: "J",
    value: "KEY_J"
  },
  {
    name: "K",
    value: "KEY_K"
  },
  {
    name: "L",
    value: "KEY_L"
  },
  {
    name: "M",
    value: "KEY_M"
  },
  {
    name: "N",
    value: "KEY_N"
  },
  {
    name: "O",
    value: "KEY_O"
  },
  {
    name: "P",
    value: "KEY_P"
  },
  {
    name: "Q",
    value: "KEY_Q"
  },
  {
    name: "R",
    value: "KEY_R"
  },
  {
    name: "S",
    value: "KEY_S"
  },
  {
    name: "T",
    value: "KEY_T"
  },
  {
    name: "U",
    value: "KEY_U"
  },
  {
    name: "V",
    value: "KEY_V"
  },
  {
    name: "W",
    value: "KEY_W"
  },
  {
    name: "X",
    value: "KEY_X"
  },
  {
    name: "Y",
    value: "KEY_Y"
  },
  {
    name: "Z",
    value: "KEY_Z"
  },
  {
    name: "1",
    value: "KEY_1"
  },
  {
    name: "2",
    value: "KEY_2"
  },
  {
    name: "3",
    value: "KEY_3"
  },
  {
    name: "4",
    value: "KEY_4"
  },
  {
    name: "5",
    value: "KEY_5"
  },
  {
    name: "6",
    value: "KEY_6"
  },
  {
    name: "7",
    value: "KEY_7"
  },
  {
    name: "8",
    value: "KEY_8"
  },
  {
    name: "9",
    value: "KEY_9"
  },
  {
    name: "0",
    value: "KEY_0"
  },
  {
    name: "ENTER",
    value: "KEY_ENTER"
  },
  {
    name: "ESC",
    value: "KEY_ESC"
  },
  {
    name: "BACKSPACE",
    value: "KEY_BACKSPACE"
  },
  {
    name: "TAB",
    value: "KEY_TAB"
  },
  {
    name: "SPACE",
    value: "KEY_SPACE"
  },
  {
    name: "MINUS",
    value: "KEY_MINUS"
  },
  {
    name: "EQUAL",
    value: "KEY_EQUAL"
  },
  {
    name: "LEFTBRACE",
    value: "KEY_LEFTBRACE"
  },
  {
    name: "RIGHTBRACE",
    value: "KEY_RIGHTBRACE"
  },
  {
    name: "BACKSLASH",
    value: "KEY_BACKSLASH"
  },
  {
    name: "HASHTILDE",
    value: "KEY_HASHTILDE"
  },
  {
    name: "SEMICOLON",
    value: "KEY_SEMICOLON"
  },
  {
    name: "APOSTROPHE",
    value: "KEY_APOSTROPHE"
  },
  {
    name: "GRAVE",
    value: "KEY_GRAVE"
  },
  {
    name: "COMMA",
    value: "KEY_COMMA"
  },
  {
    name: "DOT",
    value: "KEY_DOT"
  },
  {
    name: "SLASH",
    value: "KEY_SLASH"
  },
  {
    name: "CAPSLOCK",
    value: "KEY_CAPSLOCK"
  },
  {
    name: "F1",
    value: "KEY_F1"
  },
  {
    name: "F2",
    value: "KEY_F2"
  },
  {
    name: "F3",
    value: "KEY_F3"
  },
  {
    name: "F4",
    value: "KEY_F4"
  },
  {
    name: "F5",
    value: "KEY_F5"
  },
  {
    name: "F6",
    value: "KEY_F6"
  },
  {
    name: "F7",
    value: "KEY_F7"
  },
  {
    name: "F8",
    value: "KEY_F8"
  },
  {
    name: "F9",
    value: "KEY_F9"
  },
  {
    name: "F10",
    value: "KEY_F10"
  },
  {
    name: "F11",
    value: "KEY_F11"
  },
  {
    name: "F12",
    value: "KEY_F12"
  },
  {
    name: "SYSRQ",
    value: "KEY_SYSRQ"
  },
  {
    name: "SCROLLLOCK",
    value: "KEY_SCROLLLOCK"
  },
  {
    name: "PAUSE",
    value: "KEY_PAUSE"
  },
  {
    name: "INSERT",
    value: "KEY_INSERT"
  },
  {
    name: "HOME",
    value: "KEY_HOME"
  },
  {
    name: "PAGEUP",
    value: "KEY_PAGEUP"
  },
  {
    name: "DELETE",
    value: "KEY_DELETE"
  },
  {
    name: "END",
    value: "KEY_END"
  },
  {
    name: "PAGEDOWN",
    value: "KEY_PAGEDOWN"
  },
  {
    name: "RIGHT",
    value: "KEY_RIGHT"
  },
  {
    name: "LEFT",
    value: "KEY_LEFT"
  },
  {
    name: "DOWN",
    value: "KEY_DOWN"
  },
  {
    name: "UP",
    value: "KEY_UP"
  },
  {
    name: "NUMLOCK",
    value: "KEY_NUMLOCK"
  },
  {
    name: "KPSLASH",
    value: "KEY_KPSLASH"
  },
  {
    name: "KPASTERISK",
    value: "KEY_KPASTERISK"
  },
  {
    name: "KPMINUS",
    value: "KEY_KPMINUS"
  },
  {
    name: "KPPLUS",
    value: "KEY_KPPLUS"
  },
  {
    name: "KPENTER",
    value: "KEY_KPENTER"
  },
  {
    name: "KP1",
    value: "KEY_KP1"
  },
  {
    name: "KP2",
    value: "KEY_KP2"
  },
  {
    name: "KP3",
    value: "KEY_KP3"
  },
  {
    name: "KP4",
    value: "KEY_KP4"
  },
  {
    name: "KP5",
    value: "KEY_KP5"
  },
  {
    name: "KP6",
    value: "KEY_KP6"
  },
  {
    name: "KP7",
    value: "KEY_KP7"
  },
  {
    name: "KP8",
    value: "KEY_KP8"
  },
  {
    name: "KP9",
    value: "KEY_KP9"
  },
  {
    name: "KP0",
    value: "KEY_KP0"
  },
  {
    name: "KPDOT",
    value: "KEY_KPDOT"
  },
  {
    name: "102ND",
    value: "KEY_102ND"
  },
  {
    name: "COMPOSE",
    value: "KEY_COMPOSE"
  },
  {
    name: "POWER",
    value: "KEY_POWER"
  },
  {
    name: "KPEQUAL",
    value: "KEY_KPEQUAL"
  },
  {
    name: "F13",
    value: "KEY_F13"
  },
  {
    name: "F14",
    value: "KEY_F14"
  },
  {
    name: "F15",
    value: "KEY_F15"
  },
  {
    name: "F16",
    value: "KEY_F16"
  },
  {
    name: "F17",
    value: "KEY_F17"
  },
  {
    name: "F18",
    value: "KEY_F18"
  },
  {
    name: "F19",
    value: "KEY_F19"
  },
  {
    name: "F20",
    value: "KEY_F20"
  },
  {
    name: "F21",
    value: "KEY_F21"
  },
  {
    name: "F22",
    value: "KEY_F22"
  },
  {
    name: "F23",
    value: "KEY_F23"
  },
  {
    name: "F24",
    value: "KEY_F24"
  },
  {
    name: "OPEN",
    value: "KEY_OPEN"
  },
  {
    name: "HELP",
    value: "KEY_HELP"
  },
  {
    name: "PROPS",
    value: "KEY_PROPS"
  },
  {
    name: "FRONT",
    value: "KEY_FRONT"
  },
  {
    name: "STOP",
    value: "KEY_STOP"
  },
  {
    name: "AGAIN",
    value: "KEY_AGAIN"
  },
  {
    name: "UNDO",
    value: "KEY_UNDO"
  },
  {
    name: "CUT",
    value: "KEY_CUT"
  },
  {
    name: "COPY",
    value: "KEY_COPY"
  },
  {
    name: "PASTE",
    value: "KEY_PASTE"
  },
  {
    name: "FIND",
    value: "KEY_FIND"
  },
  {
    name: "MUTE",
    value: "KEY_MUTE"
  },
  {
    name: "VOLUMEUP",
    value: "KEY_VOLUMEUP"
  },
  {
    name: "VOLUMEDOWN",
    value: "KEY_VOLUMEDOWN"
  },
  {
    name: "KPCOMMA",
    value: "KEY_KPCOMMA"
  },
  {
    name: "RO",
    value: "KEY_RO"
  },
  {
    name: "KATAKANAHIRAGANA",
    value: "KEY_KATAKANAHIRAGANA"
  },
  {
    name: "YEN",
    value: "KEY_YEN"
  },
  {
    name: "HENKAN",
    value: "KEY_HENKAN"
  },
  {
    name: "MUHENKAN",
    value: "KEY_MUHENKAN"
  },
  {
    name: "KPJPCOMMA",
    value: "KEY_KPJPCOMMA"
  },
  {
    name: "HANGEUL",
    value: "KEY_HANGEUL"
  },
  {
    name: "HANJA",
    value: "KEY_HANJA"
  },
  {
    name: "KATAKANA",
    value: "KEY_KATAKANA"
  },
  {
    name: "HIRAGANA",
    value: "KEY_HIRAGANA"
  },
  {
    name: "ZENKAKUHANKAKU",
    value: "KEY_ZENKAKUHANKAKU"
  },
  {
    name: "KPLEFTPAREN",
    value: "KEY_KPLEFTPAREN"
  },
  {
    name: "KPRIGHTPAREN",
    value: "KEY_KPRIGHTPAREN"
  },
  {
    name: "LEFTCTRL",
    value: "KEY_LEFTCTRL"
  },
  {
    name: "LEFTSHIFT",
    value: "KEY_LEFTSHIFT"
  },
  {
    name: "LEFTALT",
    value: "KEY_LEFTALT"
  },
  {
    name: "LEFTMETA",
    value: "KEY_LEFTMETA"
  },
  {
    name: "RIGHTCTRL",
    value: "KEY_RIGHTCTRL"
  },
  {
    name: "RIGHTSHIFT",
    value: "KEY_RIGHTSHIFT"
  },
  {
    name: "RIGHTALT",
    value: "KEY_RIGHTALT"
  },
  {
    name: "RIGHTMETA",
    value: "KEY_RIGHTMETA"
  },
  {
    name: "MEDIA_PLAYPAUSE",
    value: "KEY_MEDIA_PLAYPAUSE"
  },
  {
    name: "MEDIA_STOPCD",
    value: "KEY_MEDIA_STOPCD"
  },
  {
    name: "MEDIA_PREVIOUSSONG",
    value: "KEY_MEDIA_PREVIOUSSONG"
  },
  {
    name: "MEDIA_NEXTSONG",
    value: "KEY_MEDIA_NEXTSONG"
  },
  {
    name: "MEDIA_EJECTCD",
    value: "KEY_MEDIA_EJECTCD"
  },
  {
    name: "MEDIA_VOLUMEUP",
    value: "KEY_MEDIA_VOLUMEUP"
  },
  {
    name: "MEDIA_VOLUMEDOWN",
    value: "KEY_MEDIA_VOLUMEDOWN"
  },
  {
    name: "MEDIA_MUTE",
    value: "KEY_MEDIA_MUTE"
  },
  {
    name: "MEDIA_WWW",
    value: "KEY_MEDIA_WWW"
  },
  {
    name: "MEDIA_BACK",
    value: "KEY_MEDIA_BACK"
  },
  {
    name: "MEDIA_FORWARD",
    value: "KEY_MEDIA_FORWARD"
  },
  {
    name: "MEDIA_STOP",
    value: "KEY_MEDIA_STOP"
  },
  {
    name: "MEDIA_FIND",
    value: "KEY_MEDIA_FIND"
  },
  {
    name: "MEDIA_SCROLLUP",
    value: "KEY_MEDIA_SCROLLUP"
  },
  {
    name: "MEDIA_SCROLLDOWN",
    value: "KEY_MEDIA_SCROLLDOWN"
  },
  {
    name: "MEDIA_EDIT",
    value: "KEY_MEDIA_EDIT"
  },
  {
    name: "MEDIA_SLEEP",
    value: "KEY_MEDIA_SLEEP"
  },
  {
    name: "MEDIA_COFFEE",
    value: "KEY_MEDIA_COFFEE"
  },
  {
    name: "MEDIA_REFRESH",
    value: "KEY_MEDIA_REFRESH"
  },
  {
    name: "MEDIA_CALC",
    value: "KEY_MEDIA_CALC"
  },
]
