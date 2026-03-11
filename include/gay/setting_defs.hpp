#include <gay/color.hpp>
#include <gay/settings.hpp>
#include <gay/ui/color_cell.hpp>
#include <gay/ui/color_popup.hpp>
#include <gay/ui/settings_popup.hpp>

using ST = gay::SettingType;

// thank you urban dictionary
static constexpr std::array INSULTS_FOR_DUMBASSES = {
	gay::Insult {
		.text = "dingbat",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "fool",
		.weight = 0.5,
	},
	gay::Insult {
		.text = "doofus",
		.weight = 0.5,
	},
	gay::Insult {
		.text = "uncultured swine",
		.weight = 0.9,
	},
	gay::Insult {
		.text = "drunkard",
		.weight = 0.3,
	},
	gay::Insult {
		.text = "peter griffin",
		.weight = 0.1,
	},
	gay::Insult {
		.text = "doorknob",
		.weight = 0.6,
	},
	gay::Insult {
		.text = "flivver",
		.weight = 0.9,
	},
	gay::Insult {
		.text = "floudering fish",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "laxidaisical bufoon",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "thoosie",
		.weight = 0.9,
	},
	gay::Insult {
		.text = "velvet hippo",
		.weight = 0.64,
	},
	gay::Insult {
		.text = "citizen of France",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "cabbage",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "chudling",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "britbong",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "lanklet",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "sephora kid",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "twinkfoot",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "gomer",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "pizza bones",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "clanker",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "Splenda Daddy",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "carbrain",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "clanker",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "unc",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "Etsy Witch",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "doomba",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "neden",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "juggahoe",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "wang",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "scrub",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "nugbone",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "big poopy head",
		.weight = 1.0,
	},
	gay::Insult {
		.text =
			"low down, good for nothin, deplorable, contemptible, despicable, loathsome, detestable...ignominious nitwit",
		.weight = 10e-16
	},
	gay::Insult {
		.text = "chronic geode user",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "Minecraft Youtuber",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "white republican cis-het christian man",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "twitter crypto bot",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "john",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "bocker",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "tank",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "idiot sandwhich",
		.weight = 1.0,
	},
	gay::Insult {
		.text = "donut",
		.weight = 1.0,
	}
};

static constexpr std::string_view DISPLAY_MODE_CHOICES[] = {
	"Gay mode BABYYYYYYYY :3333",
	"straight mode.",
	"granular mode. (advanced, configure at your own risk!!!!)",
};

static constexpr std::string_view JOIN_STYLE_CHOICES[] = {
	"Sharp",
	"Round",
	"None",
};

static constexpr std::array SECTION_INFO = {
	gay::SectionInfo {
		.key = "general",
		.titles =
			{
				"all the most important ones :)",
				"General",
				"General",
			},
		.tooltip = "Core settings for enabling and tuning the mod.",
	},
	gay::SectionInfo {
		.key = "colors",
		.titles =
			{
				"the plus in LGBTQIA+ stands for RGB",
				"Colors",
				"Colors",
			},
		.tooltip = "Control how the trail colors look and cycle.",
	},
	gay::SectionInfo {
		"trail-shape",
		{
			"Size matters, but not as much as shape (iykyk)",
			"Trail Shape",
			"Trail Shape",
		},
		"Adjust the physical dimensions and style of the wave trail.",
	},
	gay::SectionInfo {
		"trail-opacity",
		{
			"i see through your mask. so you're jim carrey huh?",
			"Trail Opacity",
			"Trail Opacity",
		},
		"Control how transparent the trail is, with optional fade effects.",
	},
	gay::SectionInfo {
		"outline",
		{
			"Outline",
			"Outline",
			"Outline",
		},
		"Add a colorful glow/outline around the wave trail.",
	},
	gay::SectionInfo {
		"tilt",
		{
			"Tilt",
			"Tilt",
			"Tilt",
		},
		"Offset the trail position diagonally for a tilted look.",
	},
	gay::SectionInfo {
		"segments",
		{
			"Segments",
			"Segments",
			"Segments",
		},
		"Break the wave trail into dashed segments with adjustable gap/length.",
	},
	gay::SectionInfo {
		"thickness",
		{
			"Thickness",
			"Thickness",
			"Thickness",
		},
		"Make the trail thicker or vary its thickness based on movement speed.",
	},
	gay::SectionInfo {
		"misc",
		{
			"Misc",
			"Misc",
			"Misc",
		},
		"Miscellaneous settings.",
	},
	gay::SectionInfo {
		"granular",
		{
			"Granular Controls",
			"Granular Controls",
			"Granular Controls",
		},
		"Advanced settings for fine-grained control over every trail aspect.",
	},
};

static constexpr std::array SETTING_DISPLAY_INFO = {
	gay::SettingDisplayInfo {
		"display-mode",
		ST::Choice,
		{
			gay::ModeText {
				"Display Mode",
				"you gay or what?",
			},
			gay::ModeText {
				"Display Mode",
				"you gay or what?",
			},
			gay::ModeText {
				"Display Mode",
				"you gay or what?",
			},
		},
		"general",
		0,
		2,
		DISPLAY_MODE_CHOICES,
	},
	gay::SettingDisplayInfo {
		"enabled",
		ST::Bool,
		{
			gay::ModeText {
				"Gay Mode",
				"Turn the gay on/off",
			},
			gay::ModeText {
				"Enable GWT",
				"Enable the mod",
			},
			gay::ModeText {
				"Enable GWT",
				"Enable the mod",
			},
		},
	},
	gay::SettingDisplayInfo {
		"speed",
		ST::Float,
		{
			gay::ModeText {
				"rainbow road",
				"How fast do the colors go?",
			},
			gay::ModeText {
				"Color Cycle Speed",
				"Sets the speed at which the colors of the trail cycle",
			},
			gay::ModeText {
				"Color Cycle Speed",
				"Sets the speed at which the colors of the trail cycle",
			},
		},
		"",
		0.0,
		10.0,
	},
	gay::SettingDisplayInfo {
		"disable-behind-streak",
		ST::Bool,
		{
			gay::ModeText {
				"not from behind",
				"i want you to look me in my eyes when you disable my regular streak >:3",
			},
			gay::ModeText {
				"Disable Regular Streak",
				"Disables the secondary streak",
			},
			gay::ModeText {
				"Disable Regular Streak",
				"Disables the secondary streak",
			},
		},
	},
	gay::SettingDisplayInfo {
		"saturation",
		ST::Float,
		{
			gay::ModeText {
				"gayness",
				"how gay are you????",
			},
			gay::ModeText {
				"Color Saturation",
				"The saturation of the colors the wave trail cycles through",
			},
			gay::ModeText {
				"Color Saturation",
				"The saturation of the colors the wave trail cycles through",
			},
		},
		"",
		0.0,
		100.0,
	},
	gay::SettingDisplayInfo {
		"trail-brightness",
		ST::Float,
		{
			gay::ModeText {
				"MY EYES!!!! I'M BLIND NOOOOO",
				"too many colors, my eyes are legit about to explode",
			},
			gay::ModeText {
				"Trail Brightness",
				"Multiplier for trail brightness",
			},
			gay::ModeText {
				"Trail Brightness",
				"Multiplier for trail brightness",
			},
		},
		"",
		0.0,
		3.0,
	},
	gay::SettingDisplayInfo {
		"sync-to-music",
		ST::Bool,
		{
			gay::ModeText {
				"vibing",
				"so in tune with rhythm you could say i'm... UNBEATABLE",
			},
			gay::ModeText {
				"Sync to Music",
				"Ties color cycling to level time position",
			},
			gay::ModeText {
				"Sync to Music",
				"Ties color cycling to level time position",
			},
		},
	},

	gay::SettingDisplayInfo {
		"smooth-colors",
		ST::Bool,
		{
			gay::ModeText {
				"smoooooooooooOOth",
				"if snoop dogg wasn't homophobic",
			},
			gay::ModeText {
				"Smooth Color Cycling",
				"Smooths the transition between each color",
			},
			gay::ModeText {
				"Smooth Color Cycling",
				"Smooths the transition between each color",
			},
		},
		"colors",
	},
	gay::SettingDisplayInfo {
		"enable-gaydient",
		ST::Bool,
		{
			gay::ModeText {
				"The G A Y D I E N T",
				"it's a gradient... BUT GAY",
			},
			gay::ModeText {
				"Enable Gaydient",
				"Enables the custom gradient and disables the base rainbow",
			},
			gay::ModeText {
				"Enable Gaydient",
				"Enables the custom gradient and disables the base rainbow",
			},
		},
	},
	gay::SettingDisplayInfo {
		"gaydient-colors",
		ST::ColorList,
		{
			gay::ModeText {
				"the RGB mafia",
				"\"you disrespect me, you disrespect my sexuality, and you expect to get a loan?\" - john gay",
			},
			gay::ModeText {
				"Gaydient Color List",
				"The colors used for the custom gradient",
			},
			gay::ModeText {
				"Gaydient Color List",
				"The colors used for the custom gradient",
			},
		},
	},
	gay::SettingDisplayInfo {
		"separate-player-colors",
		ST::Bool,
		{
			gay::ModeText {
				"split fiction",
				"you killed john gay and now the mafia has been dissolved. this is the result.",
			},
			gay::ModeText {
				"Independent Player Colors",
				"Allow P1 and P2 to have different color cycles",
			},
			gay::ModeText {
				"Independent Player Colors",
				"Allow P1 and P2 to have different color cycles",
			},
		},
	},
	gay::SettingDisplayInfo {
		"p2-gaydient-colors",
		ST::ColorList,
		{
			gay::ModeText {
				"the RGB cartel",
				"yeah i don't have a storyline for this one",
			},
			gay::ModeText {
				"P2 Gaydient Color List",
				"The colors used for player 2's custom gradient",
			},
			gay::ModeText {
				"P2 Gaydient Color List",
				"The colors used for player 2's custom gradient",
			},
		},
	},

	gay::SettingDisplayInfo {
		"solid-trail",
		ST::Bool,
		{
			gay::ModeText {
				"it's meaty...",
				"i'm just gonna take a guess... batman? crap you were jim carrey, here's that 5 dollars i bet",
			},
			gay::ModeText {
				"Solid Trail",
				"Makes the wave trail solid",
			},
			gay::ModeText {
				"Solid Trail",
				"Makes the wave trail solid",
			},
		},
		"trail-shape",
	},
	gay::SettingDisplayInfo {
		"rewrite-trail",
		ST::Bool,
		{
			gay::ModeText {
				"built different...",
				"threw away the blueprint and started from scratch. custom geometry baby",
			},
			gay::ModeText {
				"Rewrite Trail",
				"Replaces the default trail rendering with a custom draw pipeline",
			},
			gay::ModeText {
				"Rewrite Trail",
				"Replaces the default trail rendering with a custom draw pipeline",
			},
		},
	},
	gay::SettingDisplayInfo {
		"trail-size",
		ST::Float,
		{
			gay::ModeText {
				"it's big...",
				"size does in fact matte",
			},
			gay::ModeText {
				"Wave Trail Size",
				"Controls the size of the wave trail",
			},
			gay::ModeText {
				"Wave Trail Size",
				"Controls the size of the wave trail",
			},
		},
		"",
		0.1,
		5.0,
	},
	gay::SettingDisplayInfo {
		"pulse-size",
		ST::Float,
		{
			gay::ModeText {
				"it's pulsing...",
				"How chonky is the pulse?",
			},
			gay::ModeText {
				"Wave Pulse Size",
				"Controls the pulse size of the wave trail",
			},
			gay::ModeText {
				"Wave Pulse Size",
				"Controls the pulse size of the wave trail",
			},
		},
		"",
		0.0,
		5.0,
	},
	gay::SettingDisplayInfo {
		"disable-trail",
		ST::Bool,
		{
			gay::ModeText {
				"it's gone?",
				"how could you kill your trail, you're a monster.",
			},
			gay::ModeText {
				"Disable Wave Trail",
				"Disables the wave trail completely",
			},
			gay::ModeText {
				"Disable Wave Trail",
				"Disables the wave trail completely",
			},
		},
	},
	gay::SettingDisplayInfo {
		"disable-pulse",
		ST::Bool,
		{
			gay::ModeText {
				"flaccid",
				"idk what you did but it's not erect anymore",
			},
			gay::ModeText {
				"Disable Wave Pulse",
				"Disables the wave trail pulse completely",
			},
			gay::ModeText {
				"Disable Wave Pulse",
				"Disables the wave trail pulse completely",
			},
		},
	},

	gay::SettingDisplayInfo {
		"trail-opacity",
		ST::Int,
		{
			gay::ModeText {
				"i can see the veins...",
				"dang...",
			},
			gay::ModeText {
				"Trail Opacity",
				"Sets the base opacity of the wave trail",
			},
			gay::ModeText {
				"Trail Opacity",
				"Sets the base opacity of the wave trail",
			},
		},
		"trail-opacity",
		0,
		255,
	},
	gay::SettingDisplayInfo {
		"enable-trail-fade",
		ST::Bool,
		{
			gay::ModeText {
				"gay ghost call me a ghas- wait please microsoft i didn't mean to break copyright laws PLEASE SPARE ME NOOOOOOOOOOOOOOOOOOOO-",
				"fade out of existence",
			},
			gay::ModeText {
				"Trail Fade",
				"Enables the trail fade-out effect",
			},
			gay::ModeText {
				"Trail Fade",
				"Enables the trail fade-out effect",
			},
		},
	},
	gay::SettingDisplayInfo {
		"trail-fade-speed",
		ST::Float,
		{
			gay::ModeText {
				"get kidnapped by the FBI speedrun",
				"wr is 0.0",
			},
			gay::ModeText {
				"Trail Fade Speed",
				"Controls how quickly the trail fades out",
			},
			gay::ModeText {
				"Trail Fade Speed",
				"Controls how quickly the trail fades out",
			},
		},
		"",
		0.0,
		5.0,
	},
	gay::SettingDisplayInfo {
		"trail-min-opacity",
		ST::Int,
		{
			gay::ModeText {
				"got that ghost on the floor",
				"how ghost are you?",
			},
			gay::ModeText {
				"Minimum Trail Opacity",
				"Sets the minimum opacity the trail will fade to",
			},
			gay::ModeText {
				"Minimum Trail Opacity",
				"Sets the minimum opacity the trail will fade to",
			},
		},
		"",
		0,
		255,
	},
	gay::SettingDisplayInfo {
		"persist-trail",
		ST::Bool,
		{
			gay::ModeText {
				"eternaly gayful",
				"immortality...",
			},
			gay::ModeText {
				"Persist Trail",
				"Disables fading the wave trail out in portals",
			},
			gay::ModeText {
				"Persist Trail",
				"Disables fading the wave trail out in portals",
			},
		},
	},

	gay::SettingDisplayInfo {
		"wave-outline",
		ST::Bool,
		{
			gay::ModeText {
				"glow stick mode",
				"waving around like a shaken up minion",
			},
			gay::ModeText {
				"Wave Outline",
				"Enables an outline effect on the wave trail",
			},
			gay::ModeText {
				"Wave Outline",
				"Enables an outline effect on the wave trail",
			},
		},
		"outline",
	},
	gay::SettingDisplayInfo {
		"wave-outline-width",
		ST::Float,
		{
			gay::ModeText {
				"girthy glow...",
				"apparently also as WIDE as a minion",
			},
			gay::ModeText {
				"Wave Outline Width",
				"Width of the outline effect",
			},
			gay::ModeText {
				"Wave Outline Width",
				"Width of the outline effect",
			},
		},
		"",
		0.0,
		20.0,
	},
	gay::SettingDisplayInfo {
		"wave-outline-blur",
		ST::Int,
		{
			gay::ModeText {
				"blurring the lines between straight and gay",
				"is that a trail outline? sorry i can't see without my glasses",
			},
			gay::ModeText {
				"Wave Outline Blur",
				"How blurry the outline is",
			},
			gay::ModeText {
				"Wave Outline Blur",
				"How blurry the outline is",
			},
		},
		"",
		0,
		10,
	},
	gay::SettingDisplayInfo {
		"fix-outline-intersect",
		ST::Bool,
		{
			gay::ModeText {
				"rounded corners :3",
				"fills the gaps at bends so the trail has smooth rounded joins instead of sharp corners",
			},
			gay::ModeText {
				"Fix Outline Intersections",
				"Fills gaps between trail segments at bends with fan triangles (rounded joins)",
			},
			gay::ModeText {
				"Fix Outline Intersections",
				"Fills gaps between trail segments at bends with fan triangles (rounded joins)",
			},
		},
	},
	gay::SettingDisplayInfo {
		"wave-outline-opacity",
		ST::Int,
		{
			gay::ModeText {
				"how glow can you go?",
				"this glow >:3",
			},
			gay::ModeText {
				"Wave Outline Opacity",
				"The opacity of the trail outline",
			},
			gay::ModeText {
				"Wave Outline Opacity",
				"The opacity of the trail outline",
			},
		},
		"",
		0,
		255,
	},
	gay::SettingDisplayInfo {
		"outline-colors",
		ST::ColorList,
		{
			gay::ModeText {
				"The G L O W D I E N T",
				"the RGB mafia boss body guards",
			},
			gay::ModeText {
				"Outline Color List",
				"The colors used for the wave outline",
			},
			gay::ModeText {
				"Outline Color List",
				"The colors used for the wave outline",
			},
		},
	},

	gay::SettingDisplayInfo {
		"tilt-enabled",
		ST::Bool,
		{
			gay::ModeText {
				"he got me limping...",
				"waving at an angle",
			},
			gay::ModeText {
				"Tilted Trail",
				"Enable/Disable wave trail tilting",
			},
			gay::ModeText {
				"Tilted Trail",
				"Enable/Disable wave trail tilting",
			},
		},
		"tilt",
	},
	gay::SettingDisplayInfo {
		"tilt-x-offset",
		ST::Float,
		{
			gay::ModeText {
				"first he did me on the bed...",
				"horizontally gay",
			},
			gay::ModeText {
				"Tilt X Offset",
				"Horizontal tilt offset",
			},
			gay::ModeText {
				"Tilt X Offset",
				"Horizontal tilt offset",
			},
		},
		"",
		-50.0,
		50.0,
	},
	gay::SettingDisplayInfo {
		"tilt-y-offset",
		ST::Float,
		{
			gay::ModeText {
				"then standing up!!",
				"vertically gay",
			},
			gay::ModeText {
				"Tilt Y Offset",
				"Vertical tilt offset",
			},
			gay::ModeText {
				"Tilt Y Offset",
				"Vertical tilt offset",
			},
		},
		"",
		-50.0,
		50.0,
	},
	gay::SettingDisplayInfo {
		"trail-dashing",
		ST::Bool,
		{
			gay::ModeText {
				"S P I N",
				"got doordash and the delivery boy was quite cute :3",
			},
			gay::ModeText {
				"Trail Dash Effect",
				"Makes the wave icon rotate when dashing",
			},
			gay::ModeText {
				"Trail Dash Effect",
				"Makes the wave icon rotate when dashing",
			},
		},
	},

	gay::SettingDisplayInfo {
		"trail-segments",
		ST::Bool,
		{
			gay::ModeText {
				"segmented sexuality",
				"sometimes gay sometimes not call me abro",
			},
			gay::ModeText {
				"Segmented Trail",
				"Breaks trail into discrete segments",
			},
			gay::ModeText {
				"Segmented Trail",
				"Breaks trail into discrete segments",
			},
		},
		"segments",
	},
	gay::SettingDisplayInfo {
		"segment-length",
		ST::Float,
		{
			gay::ModeText {
				"hookup length",
				"some were long, some were small, all were amazing",
			},
			gay::ModeText {
				"Segment Length",
				"Length of each trail segment",
			},
			gay::ModeText {
				"Segment Length",
				"Length of each trail segment",
			},
		},
		"",
		1.0,
		200.0,
	},
	gay::SettingDisplayInfo {
		"segment-gap",
		ST::Float,
		{
			gay::ModeText {
				"we were going for that long???",
				"got me gaping",
			},
			gay::ModeText {
				"Segment Gap",
				"Gap between each segment",
			},
			gay::ModeText {
				"Segment Gap",
				"Gap between each segment",
			},
		},
		"",
		0.0,
		100.0,
	},
	gay::SettingDisplayInfo {
		"segment-persist",
		ST::Bool,
		{
			gay::ModeText {
				"static segmentation",
				"eternally gaping...",
			},
			gay::ModeText {
				"Persist Trail Segments",
				"Keeps trail segments visible through portals",
			},
			gay::ModeText {
				"Persist Trail Segments",
				"Keeps trail segments visible through portals",
			},
		},
	},

	gay::SettingDisplayInfo {
		"add-min-width",
		ST::Bool,
		{
			gay::ModeText {
				"minimum girth",
				"i have standards. you have to at least have SOME girth for me to even consider you",
			},
			gay::ModeText {
				"Add Min Width",
				"Adds minimum width to the trail",
			},
			gay::ModeText {
				"Add Min Width",
				"Adds minimum width to the trail",
			},
		},
		"thickness",
	},
	gay::SettingDisplayInfo {
		"trail-thickness-variation",
		ST::Bool,
		{
			gay::ModeText {
				"oh? you say you're a grower?",
				"well, maybe i SHOULD give you a chance...",
			},
			gay::ModeText {
				"Dynamic Thickness",
				"Makes trail thickness vary based on speed",
			},
			gay::ModeText {
				"Dynamic Thickness",
				"Makes trail thickness vary based on speed",
			},
		},
	},
	gay::SettingDisplayInfo {
		"thickness-multiplier",
		ST::Float,
		{
			gay::ModeText {
				"how big?????",
				"woah.....",
			},
			gay::ModeText {
				"Thickness Speed Multiplier",
				"How much speed affects trail thickness",
			},
			gay::ModeText {
				"Thickness Speed Multiplier",
				"How much speed affects trail thickness",
			},
		},
		"",
		0.0,
		5.0,
	},

	gay::SettingDisplayInfo {
		"editor-trail",
		ST::Bool,
		{
			gay::ModeText {
				"slid into his DMs AND his editor...",
				"no wonder you use so many bright colors in your levels, you're GAY",
			},
			gay::ModeText {
				"Editor Trail",
				"Enables the wave trail in the editor",
			},
			gay::ModeText {
				"Editor Trail",
				"Enables the wave trail in the editor",
			},
		},
		"misc",
	},
	gay::SettingDisplayInfo {
		"enable-pause-btn",
		ST::Bool,
		{
			gay::ModeText {
				"Pause Button",
				"Show the button in pause menu",
			},
			gay::ModeText {
				"Enable Pause Menu Button",
				"Enable the quick-settings button in the pause menu",
			},
			gay::ModeText {
				"Enable Pause Menu Button",
				"Enable the quick-settings button in the pause menu",
			},
		},
	},
	gay::SettingDisplayInfo {
		"verbose-logging",
		ST::Bool,
		{
			gay::ModeText {
				"Verbose Logging",
				"Do a LOT of logging",
			},
			gay::ModeText {
				"Verbose Logging",
				"Do a LOT of logging",
			},
			gay::ModeText {
				"Verbose Logging",
				"Do a LOT of logging",
			},
		},
	},

	gay::SettingDisplayInfo {
		"join-style",
		ST::Choice,
		{
			gay::ModeText {
				"Corner Join Style",
				"how pointy are your corners?",
			},
			gay::ModeText {
				"Corner Join Style",
				"Sharp = miter, Round = fan, None = gaps",
			},
			gay::ModeText {
				"Corner Join Style",
				"Sharp = miter, Round = fan, None = gaps",
			},
		},
		"granular",
		0,
		2,
		JOIN_STYLE_CHOICES,
		true,
	},
	gay::SettingDisplayInfo {
		"miter-limit",
		ST::Float,
		{
			gay::ModeText {
				"Miter Limit",
				"how far can the sharp corners extend?",
			},
			gay::ModeText {
				"Miter Limit",
				"Max miter extension before bevel fallback",
			},
			gay::ModeText {
				"Miter Limit",
				"Max miter extension before bevel fallback",
			},
		},
		"",
		1.0,
		10.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"min-point-distance",
		ST::Float,
		{
			gay::ModeText {
				"Min Point Distance",
				"minimum distance between trail samples",
			},
			gay::ModeText {
				"Min Point Distance",
				"Minimum distance between trail sample points",
			},
			gay::ModeText {
				"Min Point Distance",
				"Minimum distance between trail sample points",
			},
		},
		"",
		0.01,
		5.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"trail-length",
		ST::Float,
		{
			gay::ModeText {
				"Trail Length",
				"how long does the trail live?",
			},
			gay::ModeText {
				"Trail Length",
				"Trail time-to-live in seconds",
			},
			gay::ModeText {
				"Trail Length",
				"Trail time-to-live in seconds",
			},
		},
		"",
		0.5,
		30.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"speed-reference",
		ST::Float,
		{
			gay::ModeText {
				"Speed Reference",
				"reference speed for thickness variation",
			},
			gay::ModeText {
				"Speed Reference",
				"Reference speed for thickness variation calculations",
			},
			gay::ModeText {
				"Speed Reference",
				"Reference speed for thickness variation calculations",
			},
		},
		"",
		100.0,
		2000.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"speed-smoothing",
		ST::Float,
		{
			gay::ModeText {
				"Speed Smoothing",
				"how smooth is the speed tracking?",
			},
			gay::ModeText {
				"Speed Smoothing",
				"Exponential smoothing alpha for speed",
			},
			gay::ModeText {
				"Speed Smoothing",
				"Exponential smoothing alpha for speed",
			},
		},
		"",
		0.01,
		1.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"speed-clamp-min",
		ST::Float,
		{
			gay::ModeText {
				"Speed Clamp Min",
				"minimum speed factor",
			},
			gay::ModeText {
				"Speed Clamp Min",
				"Minimum speed factor clamp for thickness variation",
			},
			gay::ModeText {
				"Speed Clamp Min",
				"Minimum speed factor clamp for thickness variation",
			},
		},
		"",
		0.0,
		1.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"speed-clamp-max",
		ST::Float,
		{
			gay::ModeText {
				"Speed Clamp Max",
				"maximum speed factor",
			},
			gay::ModeText {
				"Speed Clamp Max",
				"Maximum speed factor clamp for thickness variation",
			},
			gay::ModeText {
				"Speed Clamp Max",
				"Maximum speed factor clamp for thickness variation",
			},
		},
		"",
		1.0,
		5.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"fade-curve-factor",
		ST::Float,
		{
			gay::ModeText {
				"Fade Curve Factor",
				"how steep is the fade?",
			},
			gay::ModeText {
				"Fade Curve Factor",
				"Fade-out curve steepness",
			},
			gay::ModeText {
				"Fade Curve Factor",
				"Fade-out curve steepness",
			},
		},
		"",
		0.01,
		1.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		"outline-blur-decay",
		ST::Float,
		{
			gay::ModeText {
				"Outline Blur Decay",
				"per-layer opacity falloff",
			},
			gay::ModeText {
				"Outline Blur Decay",
				"Per-layer opacity decay for blur effect",
			},
			gay::ModeText {
				"Outline Blur Decay",
				"Per-layer opacity decay for blur effect",
			},
		},
		"",
		0.1,
		1.0,
		{},
		.granular_only = true,
	},
	gay::SettingDisplayInfo {
		"outline-color-spread",
		ST::Float,
		{
			gay::ModeText {
				"Outline Color Spread",
				"phase spread between blur layers",
			},
			gay::ModeText {
				"Outline Color Spread",
				"Phase spread between outline blur layers",
			},
			gay::ModeText {
				"Outline Color Spread",
				"Phase spread between outline blur layers",
			},
		},
		"",
		0.0,
		30.0,
		{},
		true,
	},
	gay::SettingDisplayInfo {
		.key = "trail-z-order",
		.type = ST::Int,
		.mode_text =
			{
				gay::ModeText {
					"Trail Z-Order",
					"z-order of the trail node",
				},
				gay::ModeText {
					"Trail Z-Order",
					"Z-order of the trail node",
				},
				gay::ModeText {
					"Trail Z-Order",
					"Z-order of the trail node",
				},
			},
		.section_key = "",
		.min_val = -10,
		.max_val = 10,
		.choices = {},
		.granular_only = true,
	},
	gay::SettingDisplayInfo {
		.key = "phase-calc-speed-modifier",
		.type = ST::Float,
		.mode_text =
			{
				gay::ModeText {
					"Phase Calc Speed Modifier",
					"Modifier for the phase calculation when calculating rainbows/gradients",
				},
				gay::ModeText {
					"Phase Calc Speed Modifier",
					"Modifier for the phase calculation when calculating rainbows/gradients",
				},
				gay::ModeText {
					"Phase Calc Speed Modifier",
					"Modifier for the phase calculation when calculating rainbows/gradients",
				},
			},
		.section_key = "",
		.min_val = 0.1,
		.max_val = 1024.0,
		.choices = {},
		.granular_only = true,
	},
};
