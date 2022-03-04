#pragma once

#include <string>

// The 4 possible places that a TEV register can output colors to.
const std::string TGXTevRegister[]{
	"TevPrev",
	"Reg0",
	"Reg1",
	"Reg2"
};

// The vec3 inputs for a TEV stage's color combiner.
const std::string TGXCombineColorInput[]{
	"TevPrev.rgb",
	"TevPrev.aaa",
	"Reg0.rgb",
	"Reg0.aaa",
	"Reg1.rgb",
	"Reg1.aaa",
	"Reg2.rgb",
	"Reg2.aaa",
	"TexTemp.rgb",
	"TexTemp.aaa",
	"RasTemp.rgb",
	"RasTemp.aaa",
	"ivec3(255, 255, 255)",
	"ivec3(128, 128, 128)",
	"KonstTemp.rgb",
	"ivec3(0, 0, 0)"
};

// The floating-point inputs for a TEV stage's alpha combiner.
const std::string TGXCombineAlphaInput[]{
	"TevPrev.a",
	"Reg0.a",
	"Reg1.a",
	"Reg2.a",
	"TexTemp.a",
	"RasTemp.a",
	"KonstTemp.a",
	"0"
};

// The bias values that can be used during the evaluation of a TEV stage.
const std::string TGXTevBias[]{
	"",
	" + 128",
	" - 128"
};

// The scale values that can be applied to the result of a TEV stage.
const std::string TGXTevScale[]{
	"",
	" * 2",
	" * 4",
	" / 2"
};

const std::string TGXTevColorChannelId[]{
	"oColor[0].rgb",
	"oColor[1].rgb",
	"oColor[0].a",
	"oColor[1].a",
	"oColor[0]",
	"oColor[1]",
	"ivec4(0, 0, 0, 0)",
	"NORMAL_BUMP_UNSUPPORTED",
	"ALPHA_BUMP_UNSUPPORTED"
};

const char TGXTevSwapComponents[]{
	'r',
	'g',
	'b',
	'a'
};

const std::string TGXTexCoordSlot[]{
	"oTexCoord0",
	"oTexCoord1",
	"oTexCoord2",
	"oTexCoord3",
	"oTexCoord4",
	"oTexCoord5",
	"oTexCoord6",
	"oTexCoord7",
};

// The vec3 inputs that can be used for konst colors.
const std::string TGXKonstColorSel[]{
	// Constants
	"255, 255, 255",
	"223, 223, 223", // 7/8
	"191, 191, 191", // 3/4
	"159, 159, 159", // 5/8
	"128, 128, 128", // 1/2
	"96, 96, 96",    // 3/8
	"64, 64, 64",    // 1/4
	"32, 32, 32",    // 1/8

	// These aren't valid
	"0, 0, 0",
	"0, 0, 0",
	"0, 0, 0",
	"0, 0, 0",

	// Konst color values
	"KonstColor[0].rgb",
	"KonstColor[1].rgb",
	"KonstColor[2].rgb",
	"KonstColor[3].rgb",

	"KonstColor[0].rrr",
	"KonstColor[1].rrr",
	"KonstColor[2].rrr",
	"KonstColor[3].rrr",

	"KonstColor[0].ggg",
	"KonstColor[1].ggg",
	"KonstColor[2].ggg",
	"KonstColor[3].ggg",

	"KonstColor[0].bbb",
	"KonstColor[1].bbb",
	"KonstColor[2].bbb",
	"KonstColor[3].bbb",

	"KonstColor[0].aaa",
	"KonstColor[1].aaa",
	"KonstColor[2].aaa",
	"KonstColor[3].aaa",
};

// The floating-point values that can be used for konst alpha.
const std::string TGXKonstAlphaSel[]{
	// Constants
	"255",
	"223", // 7/8
	"191",  // 3/4
	"159", // 5/8
	"128",   // 1/2
	"96", // 3/8
	"64",  // 1/4
	"32", // 1/8

	// These aren't valid
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",

	// Konst components
	"KonstColor[0].r",
	"KonstColor[1].r",
	"KonstColor[2].r",
	"KonstColor[3].r",

	"KonstColor[0].g",
	"KonstColor[1].g",
	"KonstColor[2].g",
	"KonstColor[3].g",

	"KonstColor[0].b",
	"KonstColor[1].b",
	"KonstColor[2].b",
	"KonstColor[3].b",

	"KonstColor[0].a",
	"KonstColor[1].a",
	"KonstColor[2].a",
	"KonstColor[3].a",
};
