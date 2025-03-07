#include "SEUI.h"
#include "../Renderer/SERenderer.h"
#include <stdio.h>

MainComponent** gMainComponentList = nullptr;
ImFont* gFont = nullptr;

void SetFluentUITheme() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// General window settings
	style.WindowRounding = 5.0f;
	style.FrameRounding = 5.0f;
	style.ScrollbarRounding = 5.0f;
	style.GrabRounding = 5.0f;
	style.TabRounding = 5.0f;
	style.WindowBorderSize = 1.0f;
	style.FrameBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.PopupRounding = 5.0f;

	// Setting the colors
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.f);
	colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

	// Accent colors changed to darker olive-green/grey shades
	colors[ImGuiCol_CheckMark] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);        // Dark gray for check marks
	colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);       // Dark gray for sliders
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // Slightly lighter gray when active
	colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);           // Button background (dark gray)
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);    // Button hover state
	colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);     // Button active state
	colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);           // Dark gray for menu headers
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);    // Slightly lighter on hover
	colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);     // Lighter gray when active
	colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);        // Separators in dark gray
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f); // Resize grips in dark gray
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);        // Tabs background
	colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // Darker gray on hover
	colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

	// Additional styles
	style.FramePadding = ImVec2(8.0f, 4.0f);
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.IndentSpacing = 20.0f;
	style.ScrollbarSize = 16.0f;
}

void InitUserInterface(Window* pWindow)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(pWindow->wndHandle);

	SetFluentUITheme();

	ImGuiIO& io = ImGui::GetIO();

	char dir[MAX_FILE_PATH]{};
	GetCurrentPath(dir);
	strcat_s(dir, "Fonts\\framd.ttf");
	OutputDebugStringA(dir);

	gFont = io.Fonts->AddFontFromFileTTF(dir, 15.0f);
	io.FontDefault = gFont;
}

/*struct SubComponent
{
	SubComponentType type;
	union
	{
		SubComponentText* pText;
		SubComponentDropDown* pDropDown;
		SubComponentSliderInt* pSliderInt;
		SubComponentSliderFloat* pSliderFloat;
		SubComponentSliderFloat2* pSliderFloat2;
		SubComponentSliderFloat3* pSliderFloat3;
		SubComponentSliderFloat4* pSliderFloat4;
		SubComponentCheckBox* pCheckBox;
		SubComponentButton* pButton;
		SubComponentRadioButton* pRadioButton;
	};

	bool dynamic;
	bool show;
};*/

void DestroyUserInterface()
{
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	arrfree(gMainComponentList);
}

void CreateMainComponent(MainComponentInfo* pInfo, MainComponent* pMainComponent)
{
	pMainComponent->info = *pInfo;
	arrpush(gMainComponentList, pMainComponent);
}

void DestroyMainComponent(MainComponent* pMainComponent)
{
	for (uint32_t i = 0; i < arrlen(pMainComponent->pSubComponents); ++i)
	{
		if(pMainComponent->pSubComponents[i]->dynamic == false)
			free(pMainComponent->pSubComponents[i]);
	}
	arrfree(pMainComponent->pSubComponents);
}

void AddSubComponent(MainComponent* pMainComponent, SubComponent* pSubComponent)
{
	if (pSubComponent->dynamic == true)
	{
		arrpush(pMainComponent->pSubComponents, pSubComponent);
		return;
	}
	else
	{
		SubComponent* pSc = (SubComponent*)calloc(1, sizeof(SubComponent));
		*pSc = *pSubComponent;
		arrpush(pMainComponent->pSubComponents, pSc);
	}
}

void ProcessSubComponentText(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	vec4 color = pSubComponent->text.color;
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(color.GetX() * 255, color.GetY() * 255, color.GetZ() * 255, color.GetW() * 255));
	ImGui::Text(pSubComponent->text.text);
	ImGui::PopStyleColor();

	ImGui::PopID();
}

void ProcessSubComponentDropDown(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	uint32_t index = *pSubComponent->dropDown.pData;
	ImGui::Text(pSubComponent->dropDown.pLabel);
	if (ImGui::BeginCombo("##", pSubComponent->dropDown.pNames[index], ImGuiWindowFlags_None))
	{
		for (uint32_t n = 0; n < pSubComponent->dropDown.numNames; n++)
		{
			bool is_selected = (index == n);
			if (ImGui::Selectable(pSubComponent->dropDown.pNames[n], is_selected))
			{
				*pSubComponent->dropDown.pData = n;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::PopID();
}

bool ProcessSubComponentSliderInt(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	bool valueChanged = false;
	char textBuffer[16]{};
	snprintf(textBuffer, 16, pSubComponent->sliderInt.format, *pSubComponent->sliderInt.pData);
	ImGui::Text(pSubComponent->sliderInt.pLabel);

	int32_t* data = pSubComponent->sliderInt.pData;
	int32_t min = pSubComponent->sliderInt.min;
	int32_t max = pSubComponent->sliderInt.max;
	int32_t stepRate = pSubComponent->sliderInt.stepRate;
	if (ImGui::GetIO().WantTextInput)
	{
		valueChanged = ImGui::SliderInt("##", data, min, max, textBuffer);

		int32_t sliderValue = int32_t((*data - min) / stepRate);
		*data = min + sliderValue * stepRate;
	}
	else
	{
		const int32_t numValues = int32_t((max - min) / stepRate);

		int32_t sliderValue = int32_t((*data - min) / stepRate);

		valueChanged = ImGui::SliderInt("##", &sliderValue, 0, numValues, textBuffer);
		*data = min + sliderValue * stepRate;
	}

	if (*data < min)
		*data = min;

	if (*data > max)
		*data = max;

	ImGui::PopID();

	return valueChanged;
}

bool ProcessSubComponentFloat(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	bool valueChanged = false;
	char textBuffer[16]{};
	snprintf(textBuffer, 16, pSubComponent->sliderFloat.format, *pSubComponent->sliderFloat.pData);
	ImGui::Text(pSubComponent->sliderFloat.pLabel);

	float* data = pSubComponent->sliderFloat.pData;
	float min = pSubComponent->sliderFloat.min;
	float max = pSubComponent->sliderFloat.max;
	float stepRate = pSubComponent->sliderFloat.stepRate;

	if (ImGui::GetIO().WantTextInput)
	{
		valueChanged = ImGui::SliderFloat("##", data, min, max, textBuffer);

		int32_t sliderValue = int32_t((*data - min) / stepRate);
		*data = min + (float)sliderValue * stepRate;
	}
	else
	{
		const int32_t numValues = int32_t((max - min) / stepRate);

		int32_t sliderValue = int32_t((*data - min) / stepRate);

		valueChanged = ImGui::SliderInt("##", &sliderValue, 0, numValues, textBuffer);
		*data = min + (float)sliderValue * stepRate;
	}

	if (*data < min)
		*data = min;

	if (*data > max)
		*data = max;

	ImGui::PopID();

	return valueChanged;
}

bool ProcessSubComponentFloat2(SubComponent* pSubComponent, uint32_t id)
{
	bool valueChanged = false;
	char textBuffer[16]{};

	ImGui::Text(pSubComponent->sliderFloat2.pLabel);

	float data[2] = { pSubComponent->sliderFloat2.pData->GetX(), pSubComponent->sliderFloat2.pData->GetY() };
	float min[2] = { pSubComponent->sliderFloat2.min.GetX(), pSubComponent->sliderFloat2.min.GetY() };
	float max[2] = { pSubComponent->sliderFloat2.max.GetX(), pSubComponent->sliderFloat2.max.GetY() };
	float stepRate[2] = { pSubComponent->sliderFloat2.stepRate.GetX(), pSubComponent->sliderFloat2.stepRate.GetY() };

	for (uint32_t i = 0; i < 2; ++i)
	{
		ImGui::PushID(id + i);
		snprintf(textBuffer, 16, pSubComponent->sliderFloat2.format, data[i]);
		if (ImGui::GetIO().WantTextInput)
		{
			valueChanged |= ImGui::SliderFloat("##", &data[i], min[i], max[i], textBuffer);

			int32_t sliderValue = int32_t(((data[i] - min[i]) / stepRate[i]) + 0.5f);
			data[i] = min[i] + (float)sliderValue * stepRate[i];
		}
		else
		{
			const int32_t numValues = int32_t((max[i] - min[i]) / stepRate[i]);

			int32_t sliderValue = int32_t(((data[i] - min[i]) / stepRate[i]) + 0.5f);

			valueChanged |= ImGui::SliderInt("##", &sliderValue, 0, numValues, textBuffer);
			data[i] = min[i] + (float)sliderValue * stepRate[i];
		}

		if (data[i] < min[i])
			data[i] = min[i];

		if (data[i] > max[i])
			data[i] = max[i];

		ImGui::PopID();
	}
	pSubComponent->sliderFloat2.pData->Set(data[0], data[1]);

	return valueChanged;
}

bool ProcessSubComponentFloat3(SubComponent* pSubComponent, uint32_t id)
{
	bool valueChanged = false;
	char textBuffer[16]{};

	ImGui::Text(pSubComponent->sliderFloat3.pLabel);

	float data[3] = { pSubComponent->sliderFloat3.pData->GetX(), pSubComponent->sliderFloat3.pData->GetY(), pSubComponent->sliderFloat3.pData->GetZ() };
	float min[3] = { pSubComponent->sliderFloat3.min.GetX(), pSubComponent->sliderFloat3.min.GetY(), pSubComponent->sliderFloat3.min.GetZ() };
	float max[3] = { pSubComponent->sliderFloat3.max.GetX(), pSubComponent->sliderFloat3.max.GetY(), pSubComponent->sliderFloat3.max.GetZ() };
	float stepRate[3] = { pSubComponent->sliderFloat3.stepRate.GetX(), pSubComponent->sliderFloat3.stepRate.GetY(),
		pSubComponent->sliderFloat3.stepRate.GetZ() };

	for (uint32_t i = 0; i < 3; ++i)
	{
		ImGui::PushID(id + i);
		snprintf(textBuffer, 16, pSubComponent->sliderFloat3.format, data[i]);
		if (ImGui::GetIO().WantTextInput)
		{
			valueChanged |= ImGui::SliderFloat("##", &data[i], min[i], max[i], textBuffer);

			int32_t sliderValue = int32_t(((data[i] - min[i]) / stepRate[i]) + 0.5f);
			data[i] = min[i] + (float)sliderValue * stepRate[i];
		}
		else
		{
			const int32_t numValues = int32_t((max[i] - min[i]) / stepRate[i]);

			int32_t sliderValue = int32_t(((data[i] - min[i]) / stepRate[i]) + 0.5f);

			valueChanged |= ImGui::SliderInt("##", &sliderValue, 0, numValues, textBuffer);
			data[i] = min[i] + (float)sliderValue * stepRate[i];
		}

		if (data[i] < min[i])
			data[i] = min[i];

		if (data[i] > max[i])
			data[i] = max[i];

		ImGui::PopID();
	}
	pSubComponent->sliderFloat3.pData->Set(data[0], data[1], data[2]);

	return valueChanged;
}

bool ProcessSubComponentFloat4(SubComponent* pSubComponent, uint32_t id)
{
	//SubComponentSliderFloat4* pSliderFloat4 = &pSubComponent->subComponentSliderFloat4;
	bool valueChanged = false;
	char textBuffer[16]{};

	ImGui::Text(pSubComponent->sliderFloat4.pLabel);

	float data[4] = { pSubComponent->sliderFloat4.pData->GetX(), pSubComponent->sliderFloat4.pData->GetY(),
		pSubComponent->sliderFloat4.pData->GetZ(), pSubComponent->sliderFloat4.pData->GetW() };

	float min[4] = { pSubComponent->sliderFloat4.min.GetX(), pSubComponent->sliderFloat4.min.GetY(),
		pSubComponent->sliderFloat4.min.GetZ(), pSubComponent->sliderFloat4.min.GetW() };

	float max[4] = { pSubComponent->sliderFloat4.max.GetX(), pSubComponent->sliderFloat4.max.GetY(),
		pSubComponent->sliderFloat4.max.GetZ(), pSubComponent->sliderFloat4.max.GetW() };

	float stepRate[4] = { pSubComponent->sliderFloat4.stepRate.GetX(),pSubComponent->sliderFloat4.stepRate.GetY(),
		pSubComponent->sliderFloat4.stepRate.GetZ(), pSubComponent->sliderFloat4.stepRate.GetW() };

	for (uint32_t i = 0; i < 4; ++i)
	{
		ImGui::PushID(id + i);
		snprintf(textBuffer, 16, pSubComponent->sliderFloat4.format, data[i]);
		if (ImGui::GetIO().WantTextInput)
		{
			valueChanged |= ImGui::SliderFloat("##", &data[i], min[i], max[i], textBuffer);

			int32_t sliderValue = int32_t(((data[i] - min[i]) / stepRate[i]) + 0.5f);
			data[i] = min[i] + (float)sliderValue * stepRate[i];
		}
		else
		{
			const int32_t numValues = int32_t((max[i] - min[i]) / stepRate[i]);

			int32_t sliderValue = int32_t(((data[i] - min[i]) / stepRate[i]) + 0.5f);

			valueChanged |= ImGui::SliderInt("##", &sliderValue, 0, numValues, textBuffer);
			data[i] = min[i] + (float)sliderValue * stepRate[i];
		}

		if (data[i] < min[i])
			data[i] = min[i];

		if (data[i] > max[i])
			data[i] = max[i];

		ImGui::PopID();
	}
	pSubComponent->sliderFloat4.pData->Set(data[0], data[1], data[2], data[3]);

	return valueChanged;
}

void ProcessSubComponentCheckBox(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	//SubComponentCheckBox* pCheckBox = &pSubComponent->subComponentCheckBox;
	ImGui::Text(pSubComponent->checkBox.pLabel);
	ImGui::Checkbox("##", pSubComponent->checkBox.pData);

	ImGui::PopID();
}

void ProcessSubComponentButton(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	//SubComponentButton* pButton = &pSubComponent->subComponentButton;
	if (ImGui::Button(pSubComponent->button.pLabel))
		pSubComponent->pCallback(pSubComponent->button.pUserData);

	ImGui::PopID();
}

void ProcessSubComponentRadioButton(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	//SubComponentRadioButton* pRadioButton = &pSubComponent->subComponentRadioButton;
	ImGui::RadioButton(pSubComponent->radio.pLabel, pSubComponent->radio.pData, pSubComponent->radio.id);

	ImGui::PopID();
}

void ProcessSubComponent(SubComponent* pSubComponent, uint32_t id)
{
	switch (pSubComponent->type)
	{
	case SUB_COMPONENT_TYPE_TEXT:
		ProcessSubComponentText(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_DROPDOWN:
		ProcessSubComponentDropDown(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_SLIDER_INT:
		ProcessSubComponentSliderInt(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_SLIDER_FLOAT:
		ProcessSubComponentFloat(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_SLIDER_FLOAT2:
		ProcessSubComponentFloat2(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_SLIDER_FLOAT3:
		ProcessSubComponentFloat3(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_SLIDER_FLOAT4:
		ProcessSubComponentFloat4(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_CHECKBOX:
		ProcessSubComponentCheckBox(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_BUTTON:
		ProcessSubComponentButton(pSubComponent, id);
		break;

	case SUB_COMPONENT_TYPE_RADIO_BUTTON:
		ProcessSubComponentRadioButton(pSubComponent, id);
		break;
	}
}

void UpdateUserInterface()
{
	NewFrameApi();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (uint32_t i = 0; i < arrlen(gMainComponentList); ++i)
	{
		if (gMainComponentList[i]->show == false)
			continue;

		if (!ImGui::Begin(gMainComponentList[i]->info.pLabel, nullptr, gMainComponentList[i]->info.flags))
		{
			ImGui::End();
			return;
		}

		//ImGui::PushFont(gCousineRegularFont);
		ImVec2 position(gMainComponentList[i]->info.position.GetX(), gMainComponentList[i]->info.position.GetY());
		ImVec2 size(gMainComponentList[i]->info.size.GetX(), gMainComponentList[i]->info.size.GetY());

		ImGui::SetWindowPos(position, ImGuiCond_Once);
		ImGui::SetWindowSize(size, ImGuiCond_Once);

		uint32_t id = 0;
		for (uint32_t j = 0; j < arrlen(gMainComponentList[i]->pSubComponents); ++j)
		{
			SubComponent* subComponent = (SubComponent*)gMainComponentList[i]->pSubComponents[j];

			if(subComponent->dynamic == false || (subComponent->dynamic == true && subComponent->show == true))
				ProcessSubComponent(subComponent, id);

			if (subComponent->type == SUB_COMPONENT_TYPE_SLIDER_FLOAT2)
				id += 2;
			else if (subComponent->type == SUB_COMPONENT_TYPE_SLIDER_FLOAT3)
				id += 3;
			else if (subComponent->type == SUB_COMPONENT_TYPE_SLIDER_FLOAT4)
				id += 4;
			else
				++id;
		}

		//ImGui::PopFont();
		ImGui::End();
	}

	//ImGui::ShowDemoWindow();
}
