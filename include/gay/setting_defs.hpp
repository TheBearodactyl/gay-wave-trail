#include <gay/color.hpp>
#include <gay/settings.hpp>
#include <gay/ui/color_cell.hpp>
#include <gay/ui/color_popup.hpp>
#include <gay/ui/settings_popup.hpp>

using ST = gay::SettingType;

static constexpr std::string_view DISPLAY_MODE_CHOICES[] = {"Gay mode BABYYYYYYYY :3333", "straight mode."};
static constexpr std::array SETTING_DISPLAY_INFO = {

	gay::SettingDisplayInfo {
		"display-mode",
		"Display Mode",
		"you gay or what?",
		"Display Mode",
		"you gay or what?",
		ST::Choice,
		/*section_straight=*/"General",
		/*section_gay=*/"all the most important ones :)",
		/*section_tooltip=*/"Core settings for enabling and tuning the mod.",
		0,
		1,
		DISPLAY_MODE_CHOICES,
	},
	gay::SettingDisplayInfo {"enabled", "Gay Mode", "Turn the gay on/off", "Enable GWT", "Enable the mod", ST::Bool},
	gay::SettingDisplayInfo {
		"speed",
		"rainbow road",
		"How fast do the colors go?",
		"Color Cycle Speed",
		"Sets the speed at which the colors of the trail cycle",
		ST::Float,
		"",
		"",
		"",
		0.0,
		10.0
	},
	gay::SettingDisplayInfo {
		"disable-behind-streak",
		"not from behind",
		"i want you to look me in my eyes when you disable my regular streak >:3",
		"Disable Regular Streak",
		"Disables the secondary streak",
		ST::Bool,
		"",
		"",
		"",
	},
	gay::SettingDisplayInfo {
		"saturation",
		"gayness",
		"how gay are you????",
		"Color Saturation",
		"The saturation of the colors the wave trail cycles through",
		ST::Float,
		"",
		"",
		"",
		0.0,
		100.0
	},
	gay::SettingDisplayInfo {
		"trail-brightness",
		"MY EYES!!!! I'M BLIND NOOOOO",
		"too many colors, my eyes are legit about to explode",
		"Trail Brightness",
		"Multiplier for trail brightness",
		ST::Float,
		"",
		"",
		"",
		0.0,
		3.0
	},
	gay::SettingDisplayInfo {
		"sync-to-music",
		"vibing",
		"so in tune with rhythm you could say i'm... UNBEATABLE",
		"Sync to Music",
		"Ties color cycling to level time position",
		ST::Bool
	},

	gay::SettingDisplayInfo {
		"smooth-colors",
		"smoooooooooooOOth",
		"if snoop dogg wasn't homophobic",
		"Smooth Color Cycling",
		"Smooths the transition between each color",
		ST::Bool,
		/*section_straight=*/"Colors",
		/*section_gay=*/"the plus in LGBTQIA+ stands for RGB",
		/*section_tooltip=*/"Control how the trail colors look and cycle."
	},
	gay::SettingDisplayInfo {
		"enable-gaydient",
		"The G A Y D I E N T",
		"it's a gradient... BUT GAY",
		"Enable Gaydient",
		"Enables the custom gradient and disables the base rainbow",
		ST::Bool
	},
	gay::SettingDisplayInfo {
		"gaydient-colors",
		"the RGB mafia",
		"\"you disrespect me, you disrespect my sexuality, and you expect to get a loan?\" - john gay",
		"Gaydient Color List",
		"The colors used for the custom gradient",
		ST::ColorList
	},
	gay::SettingDisplayInfo {
		"separate-player-colors",
		"split fiction",
		"you killed john gay and now the mafia has been dissolved. this is the result.",
		"Independent Player Colors",
		"Allow P1 and P2 to have different color cycles",
		ST::Bool
	},
	gay::SettingDisplayInfo {
		"p2-gaydient-colors",
		"the RGB cartel",
		"yeah i don't have a storyline for this one",
		"P2 Gaydient Color List",
		"The colors used for player 2's custom gradient",
		ST::ColorList
	},

	gay::SettingDisplayInfo {
		"solid-trail",
		"it's meaty...",
		"i'm just gonna take a guess... batman? shit you were jim carrey, here's that 5 dollars i bet",
		"Solid Trail",
		"Makes the wave trail solid",
		ST::Bool,
		/*section_straight=*/"Trail Shape",
		/*section_gay=*/"Size matters, but not as much as shape (iykyk)",
		/*section_tooltip=*/"Adjust the physical dimensions and style of the wave trail."
	},
	gay::SettingDisplayInfo {
		"trail-size",
		"it's big...",
		"size does in fact matte",
		"Wave Trail Size",
		"Controls the size of the wave trail",
		ST::Float,
		"",
		"",
		"",
		0.1,
		5.0
	},
	gay::SettingDisplayInfo {
		"pulse-size",
		"it's pulsing...",
		"How chonky is the pulse?",
		"Wave Pulse Size",
		"Controls the pulse size of the wave trail",
		ST::Float,
		"",
		"",
		"",
		0.0,
		5.0
	},
	gay::SettingDisplayInfo {
		"disable-trail",
		"it's gone?",
		"how could you kill your trail, you're a monster.",
		"Disable Wave Trail",
		"Disables the wave trail completely",
		ST::Bool
	},
	gay::SettingDisplayInfo {
		"disable-pulse",
		"flaccid",
		"idk what you did but it's not erect anymore",
		"Disable Wave Pulse",
		"Disables the wave trail pulse completely",
		ST::Bool
	},

	gay::SettingDisplayInfo {
		"trail-opacity",
		"i can see the veins...",
		"damn...",
		"Trail Opacity",
		"Sets the base opacity of the wave trail",
		ST::Int,
		/*section_straight=*/"Trail Opacity",
		/*section_gay=*/"i see through your mask. so you're jim carrey huh?",
		/*section_tooltip=*/"Control how transparent the trail is, with optional fade effects.",
		0,
		255
	},
	gay::SettingDisplayInfo {
		"enable-trail-fade",
		"gay ghost call me a ghas- wait please microsoft i didn't mean to break copyright laws PLEASE SPARE ME NOOOOOOOOOOOOOOOOOOOO-",
		"fade out of existence",
		"Trail Fade",
		"Enables the trail fade-out effect",
		ST::Bool
	},
	gay::SettingDisplayInfo {
		"trail-fade-speed",
		"get kidnapped by the FBI speedrun",
		"wr is 0.0",
		"Trail Fade Speed",
		"Controls how quickly the trail fades out",
		ST::Float,
		"",
		"",
		"",
		0.0,
		5.0
	},
	gay::SettingDisplayInfo {
		"trail-min-opacity",
		"got that ghost on the floor",
		"how ghost are you?",
		"Minimum Trail Opacity",
		"Sets the minimum opacity the trail will fade to",
		ST::Int,
		"",
		"",
		"",
		0,
		255
	},
	gay::SettingDisplayInfo {
		"persist-trail",
		"eternaly gayful",
		"immortality...",
		"Persist Trail",
		"Disables fading the wave trail out in portals",
		ST::Bool
	},

	gay::SettingDisplayInfo {
		"wave-outline",
		"glow stick mode",
		"waving around like a shaken up minion",
		"Wave Outline",
		"Enables an outline effect on the wave trail",
		ST::Bool,
		/*section_straight=*/"Outline",
		/*section_gay=*/"Outline",
		/*section_tooltip=*/"Add a colorful glow/outline around the wave trail."
	},
	gay::SettingDisplayInfo {
		"wave-outline-width",
		"girthy glow...",
		"apparently also as WIDE as a minion",
		"Wave Outline Width",
		"Width of the outline effect",
		ST::Float,
		"",
		"",
		"",
		0.0,
		20.0
	},
	gay::SettingDisplayInfo {
		"wave-outline-blur",
		"blurring the lines between straight and gay",
		"is that a trail outline? sorry i can't see without my glasses",
		"Wave Outline Blur",
		"How blurry the outline is",
		ST::Int,
		"",
		"",
		"",
		0,
		10
	},
	gay::SettingDisplayInfo {
		"wave-outline-opacity",
		"how glow can you go?",
		"this glow >:3",
		"Wave Outline Opacity",
		"The opacity of the trail outline",
		ST::Int,
		"",
		"",
		"",
		0,
		255
	},
	gay::SettingDisplayInfo {
		"outline-colors",
		"The G L O W D I E N T",
		"the RGB mafia boss body guards",
		"Outline Color List",
		"The colors used for the wave outline",
		ST::ColorList
	},

	gay::SettingDisplayInfo {
		"tilt-enabled",
		"he got me limping...",
		"waving at an angle",
		"Tilted Trail",
		"Enable/Disable wave trail tilting",
		ST::Bool,
		/*section_straight=*/"Tilt",
		/*section_gay=*/"Tilt",
		/*section_tooltip=*/"Offset the trail position diagonally for a tilted look."
	},
	gay::SettingDisplayInfo {
		"tilt-x-offset",
		"first he did me on the bed...",
		"horizontally gay",
		"Tilt X Offset",
		"Horizontal tilt offset",
		ST::Float,
		"",
		"",
		"",
		-50.0,
		50.0
	},
	gay::SettingDisplayInfo {
		"tilt-y-offset",
		"then standing up!!",
		"vertically gay",
		"Tilt Y Offset",
		"Vertical tilt offset",
		ST::Float,
		"",
		"",
		"",
		-50.0,
		50.0
	},
	gay::SettingDisplayInfo {
		"trail-dashing",
		"S P I N",
		"got doordash and the delivery boy was quite cute :3",
		"Trail Dash Effect",
		"Makes the wave icon rotate when dashing",
		ST::Bool
	},

	gay::SettingDisplayInfo {
		"trail-segments",
		"segmented sexuality",
		"sometimes gay sometimes not call me abro",
		"Segmented Trail",
		"Breaks trail into discrete segments",
		ST::Bool,
		/*section_straight=*/"Segments",
		/*section_gay=*/"Segments",
		/*section_tooltip=*/"Break the wave trail into dashed segments with adjustable gap/length."
	},
	gay::SettingDisplayInfo {
		"segment-length",
		"hookup length",
		"some were long, some were small, all were amazing",
		"Segment Length",
		"Length of each trail segment",
		ST::Float,
		"",
		"",
		"",
		1.0,
		200.0
	},
	gay::SettingDisplayInfo {
		"segment-gap",
		"we were going for that long???",
		"got me gaping",
		"Segment Gap",
		"Gap between each segment",
		ST::Float,
		"",
		"",
		"",
		0.0,
		100.0
	},
	gay::SettingDisplayInfo {
		"segment-persist",
		"static segmentation",
		"eternally gaping...",
		"Persist Trail Segments",
		"Keeps trail segments visible through portals",
		ST::Bool
	},

	gay::SettingDisplayInfo {
		"add-min-width",
		"minimum girth",
		"i have standards. you have to at least have SOME girth for me to even consider you",
		"Add Min Width",
		"Adds minimum width to the trail",
		ST::Bool,
		/*section_straight=*/"Thickness",
		/*section_gay=*/"Thickness",
		/*section_tooltip=*/"Make the trail thicker or vary its thickness based on movement speed."
	},
	gay::SettingDisplayInfo {
		"trail-thickness-variation",
		"oh? you say you're a grower?",
		"well, maybe i SHOULD give you a chance...",
		"Dynamic Thickness",
		"Makes trail thickness vary based on speed",
		ST::Bool
	},
	gay::SettingDisplayInfo {
		"thickness-multiplier",
		"how big?????",
		"woah.....",
		"Thickness Speed Multiplier",
		"How much speed affects trail thickness",
		ST::Float,
		"",
		"",
		"",
		0.0,
		5.0
	},

	gay::SettingDisplayInfo {
		"editor-trail",
		"slid into his DMs AND his editor...",
		"no wonder you use so many bright colors in your levels, you're GAY",
		"Editor Trail",
		"Enables the wave trail in the editor",
		ST::Bool,
		/*section_straight=*/"Misc",
		/*section_gay=*/"Misc",
		/*section_tooltip=*/"Miscellaneous settings."
	},
	gay::SettingDisplayInfo {
		"enable-pause-btn",
		"Pause Button",
		"Show the button in pause menu",
		"Enable Pause Menu Button",
		"Enable the quick-settings button in the pause menu",
		ST::Bool
	},
	gay::SettingDisplayInfo {
		"verbose-logging",
		"Verbose Logging",
		"Do a LOT of logging",
		"Verbose Logging",
		"Do a LOT of logging",
		ST::Bool
	}
};
