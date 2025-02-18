#pragma once

//imgui
#include "../ThirdParty/imgui/imgui.h"
#include "../ThirdParty/imgui/imgui_impl_win32.h"
#include "../Renderer/SEWindow.h"
#include "../ThirdParty/stb_ds.h"
#include "../Math/SEMath_Header.h"

void InitUserInterface(Window* pWindow);
void DestroyUserInterface();

enum MainComponentFlags
{
    MAIN_COMPONENT_FLAGS_NONE = 0,

    // Disable title-bar
    MAIN_COMPONENT_FLAGS_NO_TITLE_BAR = 0x1,

    // Disable user resizing with the lower-right grip
    MAIN_COMPONENT_FLAGS_NO_RESIZE = 0x2,

    // Disable user moving the window
    MAIN_COMPONENT_FLAGS_NO_MOVE = 0x4,

    // Disable scrollbars (window can still scroll with mouse or programmatically)
    MAIN_COMPONENT_FLAGS_NO_SCROLL_BAR = 0x8,

    // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    MAIN_COMPONENT_FLAGS_NO_SCROLL_WITH_MOUSE = 0x10,

    // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
    MAIN_COMPONENT_FLAGS_NO_COLLAPSE = 0x20,

    // Resize every window to its content every frame
    MAIN_COMPONENT_FLAGS_ALWAYS_AUTO_RESIZE = 0x40,

    // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    MAIN_COMPONENT_FLAGS_NO_BACKGROUND = 0x80,

    // Never load/save settings in .ini file
    MAIN_COMPONENT_FLAGS_NO_SAVED_SETTINGS = 0x100,

    // Disable catching mouse, hovering test with pass through.
    MAIN_COMPONENT_FLAGS_NO_MOUSE_INPUTS = 0x200,

    // Has a menu-bar
    MAIN_COMPONENT_FLAGS_MENU_BAR = 0x400,

    // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width.
    MAIN_COMPONENT_FLAGS_HORIZONTAL_SCROLL_BAR = 0x800,

    // Disable taking focus when transitioning from hidden to visible state
    MAIN_COMPONENT_FLAGS_NO_FOCUS_ON_APPEARING = 0x1000,

    // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    MAIN_COMPONENT_FLAGS_NO_BRING_TO_FRONT_ON_FOCUS = 0x2000,

    // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    MAIN_COMPONENT_FLAGS_ALWAYS_VERTICAL_SCROLL_BAR = 0x4000,

    // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    MAIN_COMPONENT_FLAGS_ALWAYS_HORIZONTAL_SCROLL_BAR = 0x8000,

    // No keyboard/gamepad navigation within the window
    MAIN_COMPONENT_FLAGS_NO_NAV_INPUTS = 0x10000,

    // No focusing toward this window with keyboard/gamepad navigation (e.g. skipped by CTRL+TAB)
    MAIN_COMPONENT_FLAGS_NO_NAV_FOUCS = 0x20000,

    MAIN_COMPONENT_FLAGS_NO_NAV = MAIN_COMPONENT_FLAGS_NO_NAV_INPUTS | MAIN_COMPONENT_FLAGS_NO_NAV_FOUCS,

    MAIN_COMPONENT_FLAGS_NO_DECORATION = MAIN_COMPONENT_FLAGS_NO_TITLE_BAR | MAIN_COMPONENT_FLAGS_NO_RESIZE 
    | MAIN_COMPONENT_FLAGS_NO_SCROLL_BAR | MAIN_COMPONENT_FLAGS_NO_COLLAPSE,

    MAIN_COMPONENT_FLAGS_NO_INPUTS = MAIN_COMPONENT_FLAGS_NO_MOUSE_INPUTS | MAIN_COMPONENT_FLAGS_NO_NAV_INPUTS | MAIN_COMPONENT_FLAGS_NO_NAV_FOUCS,

};

typedef void (*ComponentCallback)(void* pUserData);

enum SubComponentType
{
    SUB_COMPONENT_TYPE_TEXT,
    SUB_COMPONENT_TYPE_DROPDOWN,
    SUB_COMPONENT_TYPE_SLIDER_INT,
    SUB_COMPONENT_TYPE_SLIDER_FLOAT,
    SUB_COMPONENT_TYPE_SLIDER_FLOAT2,
    SUB_COMPONENT_TYPE_SLIDER_FLOAT3,
    SUB_COMPONENT_TYPE_SLIDER_FLOAT4,
    SUB_COMPONENT_TYPE_CHECKBOX,
    SUB_COMPONENT_TYPE_BUTTON,
    SUB_COMPONENT_TYPE_RADIO_BUTTON
};

struct SubComponentText
{
    char* text;
    vec4 color;
};

struct SubComponentDropDown
{
    const char* pLabel;
    uint32_t* pData;
    const char* const* pNames;
    uint32_t numNames;
};

struct SubComponentSliderInt
{
    const char* pLabel;
    int32_t min;
    int32_t max;
    int32_t stepRate;
    int32_t* pData;
    char format[16] = { "%d" };
};

struct SubComponentSliderFloat
{
    const char* pLabel;
    float min;
    float max;
    float stepRate;
    float* pData;
    char format[16] = { "%.3f" };
};

struct SubComponentSliderFloat2
{
    const char* pLabel;
    vec2 min;
    vec2 max;
    vec2 stepRate;
    vec2* pData;
    char format[16] = { "%.3f" };
};

struct SubComponentSliderFloat3
{
    const char* pLabel;
    vec3 min;
    vec3 max;
    vec3 stepRate;
    vec3* pData;
    char format[16] = { "%.3f" };
};

struct SubComponentSliderFloat4
{
    const char* pLabel;
    vec4 min;
    vec4 max;
    vec4 stepRate;
    vec4* pData;
    char format[16] = { "%.3f" };
};

struct SubComponentCheckBox
{
    const char* pLabel;
    bool* pData;
};

struct SubComponentButton
{
    const char* pLabel;
    ComponentCallback pCallback;
    void* pUserData;
};

struct SubComponentRadioButton
{
    const char* pLabel;
    int32_t* pData;
    int32_t id;
};

struct SubComponent
{
    SubComponentType type;
    union
    {
        SubComponentText subCompnentText;
        SubComponentDropDown subComponentDropDown;
        SubComponentSliderInt subComponentSliderInt;
        SubComponentSliderFloat subComponentSliderFloat;
        SubComponentSliderFloat2 subComponentSliderFloat2;
        SubComponentSliderFloat3 subComponentSliderFloat3;
        SubComponentSliderFloat4 subComponentSliderFloat4;
        SubComponentCheckBox subComponentCheckBox;
        SubComponentButton subComponentButton;
        SubComponentRadioButton subComponentRadioButton;
    };
};

struct MainComponentInfo
{
	const char* pLabel;
    uint32_t flags;
    vec2 position;
    vec2 size;
};

struct MainComponent
{
    MainComponentInfo info;
	SubComponent** pSubComponents = nullptr;
};


void CreateMainComponent(MainComponentInfo* pInfo, MainComponent* pMainComponent);
void DestroyMainComponent(MainComponent* pMainComponent);
void AddSubComponent(MainComponent* pMainComponent, void* pSubComponent, SubComponentType type);
