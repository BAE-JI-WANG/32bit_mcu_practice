//#ifndef	COLORS__
#define	COLORS__

#include "sdk_common.h"

/* -------------------------------------------------------------------------------- */
/* -- µGUI COLORS                                                                -- */
/* -- Source: http://www.rapidtables.com/web/color/RGB_Color.htm                 -- */
/* -------------------------------------------------------------------------------- */

//Colors taken from µGUI's "ugui.h"

//16-bit 565 RGBcolors   0b RRRRRGGG GGGBBBBB
#define NO_CURSOR                    0x0000


#define TITLE_BG                     0x29CB

#define WHITE                        0xFFFF
#define BLACK                        0x0000
#define YELLOW                       0xFFE0

#define TEXT_GRAY                    0x8C51
#define TEXT_RED                     0xC800
#define TEXT_YELLOW                  0xFEA2


#define TEXT_FOOTER_MENU             0x6BB1


#define INDICATOR_BG                 0x0000
#define BODY_BG                      0x1926//0x1926





#define VERSION_BG                   0xFDA2

#define WARMING_UP_PROGRESS_BG       0x4A05
#define PROGRESS_BAR                 0xFDA2

#define GAUGE_VIEW_BG                0x2988
#define TEXT_GAUGE_UNIT              0x4AF0


#define CARE_BG                      0xFEA2
#define WARNING_BG                   0xF800
#define NORMAL_BG                    0x04FF



//	#define MAROON                       0x8000
//	#define DARK_RED                     0x8800
//	#define BROWN                        0xA145
//	#define FIREBRICK                    0xB104
//	#define CRIMSON                      0xD8A7
//	#define RED                          0xF800
//	#define TOMATO                       0xFB09
//	#define CORAL                        0xFBEA
//	#define INDIAN_RED                   0xCAEB
//	#define LIGHT_CORAL                  0xEC10
//	#define DARK_SALMON                  0xE4AF
//	#define SALMON                       0xF40E
//	#define LIGHT_SALMON                 0xFD0F
//	#define ORANGE_RED                   0xFA20
//	#define DARK_ORANGE                  0xFC60
//	#define ORANGE                       0xFD20
//	#define GOLD                         0xFEA0
//	#define DARK_GOLDEN_ROD              0xB421
//	#define GOLDEN_ROD                   0xDD24
//	#define PALE_GOLDEN_ROD              0xEF35
//	#define DARK_KHAKI                   0xBDAD
//	#define KHAKI                        0xEF31
//	#define OLIVE                        0x8400
//	
//	#define YELLOW_GREEN                 0x9E66
//	#define DARK_OLIVE_GREEN             0x5346
//	#define OLIVE_DRAB                   0x6C64
//	#define LAWN_GREEN                   0x7FC0
//	#define CHART_REUSE                  0x7FE0
//	#define GREEN_YELLOW                 0xAFE6
//	#define DARK_GREEN                   0x0320
//	#define GREEN                        0x07E0
//	#define FOREST_GREEN                 0x2444
//	#define LIME                         0x07E0
//	#define LIME_GREEN                   0x3666
//	#define LIGHT_GREEN                  0x9772
//	#define PALE_GREEN                   0x97D2
//	#define DARK_SEA_GREEN               0x8DD1
//	#define MEDIUM_SPRING_GREEN          0x07D3
//	#define SPRING_GREEN                 0x07EF
//	#define SEA_GREEN                    0x344B
//	#define MEDIUM_AQUA_MARINE           0x6675
//	#define MEDIUM_SEA_GREEN             0x3D8E
//	#define LIGHT_SEA_GREEN              0x2595
//	#define DARK_SLATE_GRAY              0x328A
//	#define TEAL                         0x0410
//	#define DARK_CYAN                    0x0451
//	#define AQUA                         0x07FF
//	#define CYAN                         0x07FF
//	#define LIGHT_CYAN                   0xDFFF
//	#define DARK_TURQUOISE               0x0679
//	#define TURQUOISE                    0x46F9
//	#define MEDIUM_TURQUOISE             0x4E99
//	#define PALE_TURQUOISE               0xAF7D
//	#define AQUA_MARINE                  0x7FFA
//	#define POWDER_BLUE                  0xAEFC
//	#define CADET_BLUE                   0x64F3
//	#define STEEL_BLUE                   0x4C16
//	#define CORN_FLOWER_BLUE             0x64BD
//	#define DEEP_SKY_BLUE                0x05FF
//	#define DODGER_BLUE                  0x249F
//	#define LIGHT_BLUE                   0xAEBC
//	#define SKY_BLUE                     0x867D
//	#define LIGHT_SKY_BLUE               0x867E
//	#define MIDNIGHT_BLUE                0x18CE
//	#define NAVY                         0x0010
//	#define DARK_BLUE                    0x0011
//	#define MEDIUM_BLUE                  0x0019
//	#define BLUE                         0x001F
//	#define ROYAL_BLUE                   0x435B
//	#define BLUE_VIOLET                  0x897B
//	#define INDIGO                       0x4810
//	#define DARK_SLATE_BLUE              0x49F1
//	#define SLATE_BLUE                   0x6AD9
//	#define MEDIUM_SLATE_BLUE            0x7B5D
//	#define MEDIUM_PURPLE                0x939B
//	#define DARK_MAGENTA                 0x8811
//	#define DARK_VIOLET                  0x901A
//	#define DARK_ORCHID                  0x9999
//	#define MEDIUM_ORCHID                0xBABA
//	#define PURPLE                       0x8010
//	#define THISTLE                      0xD5FA
//	#define PLUM                         0xDD1B
//	#define VIOLET                       0xEC1D
//	#define MAGENTA                      0xF81F
//	#define ORCHID                       0xDB9A
//	#define MEDIUM_VIOLET_RED            0xC0B0
//	#define PALE_VIOLET_RED              0xDB92
//	#define DEEP_PINK                    0xF8B2
//	#define HOT_PINK                     0xFB56
//	#define LIGHT_PINK                   0xFDB7
//	#define PINK                         0xFDF9
//	#define ANTIQUE_WHITE                0xF75A
//	#define BEIGE                        0xF7BB
//	#define BISQUE                       0xFF18
//	#define BLANCHED_ALMOND              0xFF59
//	#define WHEAT                        0xF6F6
//	#define CORN_SILK                    0xFFBB
//	#define LEMON_CHIFFON                0xFFD9
//	#define LIGHT_GOLDEN_ROD_YELLOW      0xF7DA
//	#define LIGHT_YELLOW                 0xFFFB
//	#define SADDLE_BROWN                 0x8A22
//	#define SIENNA                       0x9A85
//	#define CHOCOLATE                    0xD344
//	#define PERU                         0xCC28
//	#define SANDY_BROWN                  0xF52C
//	#define BURLY_WOOD                   0xDDB0
//	#define TAN                          0xD591
//	#define ROSY_BROWN                   0xBC71
//	#define MOCCASIN                     0xFF16
//	#define NAVAJO_WHITE                 0xFEF5
//	#define PEACH_PUFF                   0xFED6
//	#define MISTY_ROSE                   0xFF1B
//	#define LAVENDER_BLUSH               0xFF7E
//	#define LINEN                        0xF77C
//	#define OLD_LACE                     0xFFBC
//	#define PAPAYA_WHIP                  0xFF7A
//	#define SEA_SHELL                    0xFFBD
//	#define MINT_CREAM                   0xF7FE
//	#define SLATE_GRAY                   0x7412
//	#define LIGHT_SLATE_GRAY             0x7453
//	#define LIGHT_STEEL_BLUE             0xAE1B
//	#define LAVENDER                     0xE73E
//	#define FLORAL_WHITE                 0xFFDD
//	#define ALICE_BLUE                   0xEFBF
//	#define GHOST_WHITE                  0xF7BF
//	#define HONEYDEW                     0xEFFD
//	#define IVORY                        0xFFFD
//	#define AZURE                        0xEFFF
//	#define SNOW                         0xFFDE
//	
//	#define DIM_GRAY                     0x6B4D
//	#define GRAY                         0x8410
//	#define DARK_GRAY                    0xAD55
//	#define SILVER                       0xBDF7
//	#define LIGHT_GRAY                   0xD69A
//	#define GAINSBORO                    0xDEDB
//	#define WHITE_SMOKE                  0xF7BE


//#endif