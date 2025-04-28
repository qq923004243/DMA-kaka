#include "imgui.h"
#include "string"

inline char search_all_widgets[120] = { "" };

inline std::string search_all_widgets_;

namespace c {

	inline ImVec4 accent = ImColor(130, 143, 234);
	inline ImVec4 shadow = ImColor(2, 2, 2);
	inline ImVec4 white_light = ImColor(201, 201, 201);
	inline ImVec4 image = ImColor(255, 255, 255,255);
	namespace bg
	{
		inline ImVec4 background = ImColor(16, 16, 16, 255);
		inline ImVec4 roughness = ImColor(255, 255, 255, 15); 
		inline ImVec4 outline = ImColor(38, 38, 38, 255);
		inline ImVec4 top_bg = ImColor(22, 22, 27, 255);

		inline ImVec2 size = ImVec2(1160, 710);
		inline ImVec2 size2 = ImVec2(870, 547);
		inline float rounding = 0;	
	}

	namespace child
	{
		inline ImVec4 background = ImColor(20, 20, 20, 255);
		inline ImVec4 border = ImColor(26, 26, 26, 255);
		inline ImVec4 lines = ImColor(36, 36, 36, 255);
		inline float rounding = 4;
	}

	namespace popup_elements
	{
		inline ImVec4 filling = ImColor(10, 10, 10, 170);
		inline ImVec4 cog = ImColor(255, 255, 255, 255);
	}

	namespace checkbox
	{
		inline ImVec4 background = ImColor(26,26,26, 255);
		inline ImVec4 outline = ImColor(35, 35, 35, 255);
		inline ImVec4 mark = ImColor(0, 0, 0, 255);
		inline float rounding = 0;
	}

	namespace slider
	{
		inline ImVec4 background = ImColor(26,26,26, 255);
		inline float rounding = 0;
	}

	namespace button
	{
		inline ImVec4 background = ImColor(26,26,26, 255);
		inline ImVec4 outline = ImColor(36, 36, 36, 255);
		inline float rounding = 0;
	}

	namespace combo
	{
		inline ImVec4 background = ImColor(26,26,26, 255);
		inline ImVec4 outline = ImColor(36, 36, 36, 255);
		inline float rounding = 0;
	}

	namespace keybind
	{
		inline ImVec4 background = ImColor(26,26,26, 255);
		inline float rounding = 0;
	}

	namespace input
	{
		inline ImVec4 background = ImColor(26,26,26, 255);
		inline ImVec4 outline = ImColor(36, 36, 36, 255);
		inline float rounding = 0;
	}

	namespace picker
	{
		inline ImVec4 background = ImColor(26,26,26, 255);
		inline float rounding = 0;
	}

	namespace tabs
	{
		inline ImVec4 line = ImColor(36, 36, 36, 255);
		
	}

	namespace knobs
	{
		inline ImVec4 background = ImColor(32, 32, 34, 255);

	}

	namespace text
	{
		inline ImVec4 text_active = ImColor(255, 255, 255, 255);
		inline ImVec4 text_hov = ImColor(255, 255, 255, 185);
		inline ImVec4 text = ImColor(255, 255, 255, 75);
	}

	namespace scrollbar
	{
		inline ImVec4 bar_active = ImColor(255, 255, 255, 145);
		inline ImVec4 bar_hov = ImColor(255, 255, 255, 125);
		inline ImVec4 bar = ImColor(255, 255, 255, 75);
	}

}