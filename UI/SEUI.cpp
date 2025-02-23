#include "SEUI.h"
#include "../Renderer/SERenderer.h"
#include <stdio.h>

MainComponent** gMainComponentList = nullptr;
ImFont* gCousineRegularFont = nullptr;

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
	//gCousineRegularFont = io.Fonts->AddFontFromFileTTF("../../ThirdParty/imgui/Cousine-Regular.ttf", 15.0f);
}

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
		free(pMainComponent->pSubComponents[i]);
	}
	arrfree(pMainComponent->pSubComponents);
}

void AddSubComponent(MainComponent* pMainComponent, void* pSubComponent, SubComponentType type)
{
	SubComponent* pSc = (SubComponent*)calloc(1, sizeof(SubComponent));
	switch (type)
	{
		case SUB_COMPONENT_TYPE_TEXT:
		{
			SubComponentText* pScText = (SubComponentText*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_TEXT;
			pSc->subCompnentText = *pScText;
			break;
		}

		case SUB_COMPONENT_TYPE_DROPDOWN:
		{
			SubComponentDropDown* pScDropDown = (SubComponentDropDown*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_DROPDOWN;
			pSc->subComponentDropDown = *pScDropDown;
			break;
		}

		case SUB_COMPONENT_TYPE_SLIDER_INT:
		{
			SubComponentSliderInt* pScSliderInt = (SubComponentSliderInt*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_SLIDER_INT;
			pSc->subComponentSliderInt = *pScSliderInt;
			break;
		}

		case SUB_COMPONENT_TYPE_SLIDER_FLOAT:
		{
			SubComponentSliderFloat* pScSliderFloat = (SubComponentSliderFloat*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
			pSc->subComponentSliderFloat = *pScSliderFloat;
			break;
		}

		case SUB_COMPONENT_TYPE_SLIDER_FLOAT2:
		{
			SubComponentSliderFloat2* pScSliderFloat2 = (SubComponentSliderFloat2*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_SLIDER_FLOAT2;
			pSc->subComponentSliderFloat2 = *pScSliderFloat2;
			break;
		}

		case SUB_COMPONENT_TYPE_SLIDER_FLOAT3:
		{
			SubComponentSliderFloat3* pScSliderFloat3 = (SubComponentSliderFloat3*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_SLIDER_FLOAT3;
			pSc->subComponentSliderFloat3 = *pScSliderFloat3;
			break;
		}

		case SUB_COMPONENT_TYPE_SLIDER_FLOAT4:
		{
			SubComponentSliderFloat4* pScSliderFloat4 = (SubComponentSliderFloat4*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_SLIDER_FLOAT4;
			pSc->subComponentSliderFloat4 = *pScSliderFloat4;
			break;
		}

		case SUB_COMPONENT_TYPE_CHECKBOX:
		{
			SubComponentCheckBox* pScCheckBox = (SubComponentCheckBox*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_CHECKBOX;
			pSc->subComponentCheckBox = *pScCheckBox;
			break;
		}

		case SUB_COMPONENT_TYPE_BUTTON:
		{
			SubComponentButton* pScButton = (SubComponentButton*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_BUTTON;
			pSc->subComponentButton = *pScButton;
			break;
		}

		case SUB_COMPONENT_TYPE_RADIO_BUTTON:
		{
			SubComponentRadioButton* pScRadioButton = (SubComponentRadioButton*)pSubComponent;
			pSc->type = SUB_COMPONENT_TYPE_RADIO_BUTTON;
			pSc->subComponentRadioButton = *pScRadioButton;
			break;
		}
	}

	arrpush(pMainComponent->pSubComponents, pSc);
}

void ProcessSubComponentText(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	SubComponentText* pText = &pSubComponent->subCompnentText;
	vec4 color = pText->color;
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(color.GetX() * 255, color.GetY() * 255, color.GetZ() * 255, color.GetW() * 255));
	ImGui::Text(pText->text);
	ImGui::PopStyleColor();

	ImGui::PopID();
}

void ProcessSubComponentDropDown(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	SubComponentDropDown* pDropDown = &pSubComponent->subComponentDropDown;
	uint32_t index = *pDropDown->pData;
	ImGui::Text(pDropDown->pLabel);
	if (ImGui::BeginCombo("##", pDropDown->pNames[index], ImGuiWindowFlags_None))
	{
		for (int n = 0; n < 2; n++)
		{
			bool is_selected = (index == n);
			if (ImGui::Selectable(pDropDown->pNames[n], is_selected))
			{
				*pSubComponent->subComponentDropDown.pData = n;
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

	SubComponentSliderInt* pSliderInt = &pSubComponent->subComponentSliderInt;
	bool valueChanged = false;
	char textBuffer[16]{};
	snprintf(textBuffer, 16, pSliderInt->format, *pSliderInt->pData);
	ImGui::Text(pSliderInt->pLabel);
	if (ImGui::GetIO().WantTextInput)
	{
		valueChanged = ImGui::SliderInt("##", pSliderInt->pData, pSliderInt->min, pSliderInt->max, textBuffer);

		int32_t sliderValue = int32_t((*pSliderInt->pData - pSliderInt->min) / pSliderInt->stepRate);
		*pSliderInt->pData = pSliderInt->min + sliderValue * pSliderInt->stepRate;
	}
	else
	{
		const int32_t numValues = int32_t((pSliderInt->max - pSliderInt->min) / pSliderInt->stepRate);

		int32_t sliderValue = int32_t((*pSliderInt->pData - pSliderInt->min) / pSliderInt->stepRate);

		valueChanged = ImGui::SliderInt("##", &sliderValue, 0, numValues, textBuffer);
		*pSliderInt->pData = pSliderInt->min + sliderValue * pSliderInt->stepRate;
	}

	if (*pSliderInt->pData < pSliderInt->min)
		*pSliderInt->pData = pSliderInt->min;

	if (*pSliderInt->pData > pSliderInt->max)
		*pSliderInt->pData = pSliderInt->max;

	ImGui::PopID();

	return valueChanged;
}

bool ProcessSubComponentFloat(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	SubComponentSliderFloat* pSliderFloat = &pSubComponent->subComponentSliderFloat;
	bool valueChanged = false;
	char textBuffer[16]{};
	snprintf(textBuffer, 16, pSliderFloat->format, *pSliderFloat->pData);
	ImGui::Text(pSliderFloat->pLabel);

	if (ImGui::GetIO().WantTextInput)
	{
		valueChanged = ImGui::SliderFloat("##", pSliderFloat->pData, pSliderFloat->min, pSliderFloat->max, textBuffer);

		int32_t sliderValue = int32_t(((*pSliderFloat->pData - pSliderFloat->min) / pSliderFloat->stepRate) + 0.5f);
		*pSliderFloat->pData = pSliderFloat->min + (float)sliderValue * pSliderFloat->stepRate;
	}
	else
	{
		const int32_t numValues = int32_t((pSliderFloat->max - pSliderFloat->min) / pSliderFloat->stepRate);

		int32_t sliderValue = int32_t(((*pSliderFloat->pData - pSliderFloat->min) / pSliderFloat->stepRate) + 0.5f);

		valueChanged = ImGui::SliderInt("##", &sliderValue, 0, numValues, textBuffer);
		*pSliderFloat->pData = pSliderFloat->min + (float)sliderValue * pSliderFloat->stepRate;
	}

	if (*pSliderFloat->pData < pSliderFloat->min)
		*pSliderFloat->pData = pSliderFloat->min;

	if (*pSliderFloat->pData > pSliderFloat->max)
		*pSliderFloat->pData = pSliderFloat->max;

	ImGui::PopID();

	return valueChanged;
}

bool ProcessSubComponentFloat2(SubComponent* pSubComponent, uint32_t id)
{
	SubComponentSliderFloat2* pSliderFloat2 = &pSubComponent->subComponentSliderFloat2;
	bool valueChanged = false;
	char textBuffer[16]{};

	ImGui::Text(pSliderFloat2->pLabel);

	float data[2] = { pSliderFloat2->pData->GetX(), pSliderFloat2->pData->GetY() };
	float min[2] = { pSliderFloat2->min.GetX(), pSliderFloat2->min.GetY() };
	float max[2] = { pSliderFloat2->max.GetX(), pSliderFloat2->max.GetY() };
	float stepRate[2] = { pSliderFloat2->stepRate.GetX(), pSliderFloat2->stepRate.GetY() };

	for (uint32_t i = 0; i < 2; ++i)
	{
		ImGui::PushID(id + i);
		snprintf(textBuffer, 16, pSliderFloat2->format, data[i]);
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
	pSliderFloat2->pData->Set(data[0], data[1]);

	return valueChanged;
}

bool ProcessSubComponentFloat3(SubComponent* pSubComponent, uint32_t id)
{
	SubComponentSliderFloat3* pSliderFloat3 = &pSubComponent->subComponentSliderFloat3;
	bool valueChanged = false;
	char textBuffer[16]{};

	ImGui::Text(pSliderFloat3->pLabel);

	float data[3] = { pSliderFloat3->pData->GetX(), pSliderFloat3->pData->GetY(), pSliderFloat3->pData->GetZ() };
	float min[3] = { pSliderFloat3->min.GetX(), pSliderFloat3->min.GetY(), pSliderFloat3->min.GetZ() };
	float max[3] = { pSliderFloat3->max.GetX(), pSliderFloat3->max.GetY(), pSliderFloat3->max.GetZ() };
	float stepRate[3] = { pSliderFloat3->stepRate.GetX(), pSliderFloat3->stepRate.GetY(), pSliderFloat3->stepRate.GetZ() };

	for (uint32_t i = 0; i < 3; ++i)
	{
		ImGui::PushID(id + i);
		snprintf(textBuffer, 16, pSliderFloat3->format, data[i]);
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
	pSliderFloat3->pData->Set(data[0], data[1], data[2]);

	return valueChanged;
}

bool ProcessSubComponentFloat4(SubComponent* pSubComponent, uint32_t id)
{
	SubComponentSliderFloat4* pSliderFloat4 = &pSubComponent->subComponentSliderFloat4;
	bool valueChanged = false;
	char textBuffer[16]{};

	ImGui::Text(pSliderFloat4->pLabel);

	float data[4] = { pSliderFloat4->pData->GetX(), pSliderFloat4->pData->GetY(), pSliderFloat4->pData->GetZ(), pSliderFloat4->pData->GetW() };
	float min[4] = { pSliderFloat4->min.GetX(), pSliderFloat4->min.GetY(), pSliderFloat4->min.GetZ(), pSliderFloat4->min.GetW() };
	float max[4] = { pSliderFloat4->max.GetX(), pSliderFloat4->max.GetY(), pSliderFloat4->max.GetZ(), pSliderFloat4->max.GetW() };
	float stepRate[4] = { pSliderFloat4->stepRate.GetX(), pSliderFloat4->stepRate.GetY(), pSliderFloat4->stepRate.GetZ(), pSliderFloat4->stepRate.GetW() };

	for (uint32_t i = 0; i < 4; ++i)
	{
		ImGui::PushID(id + i);
		snprintf(textBuffer, 16, pSliderFloat4->format, data[i]);
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
	pSliderFloat4->pData->Set(data[0], data[1], data[2], data[3]);

	return valueChanged;
}

void ProcessSubComponentCheckBox(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	SubComponentCheckBox* pCheckBox = &pSubComponent->subComponentCheckBox;
	ImGui::Text(pCheckBox->pLabel);
	ImGui::Checkbox("##", pCheckBox->pData);

	ImGui::PopID();
}

void ProcessSubComponentButton(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	SubComponentButton* pButton = &pSubComponent->subComponentButton;
	if (ImGui::Button(pButton->pLabel))
		pButton->pCallback(pButton->pUserData);

	ImGui::PopID();
}

void ProcessSubComponentRadioButton(SubComponent* pSubComponent, uint32_t id)
{
	ImGui::PushID(id);

	SubComponentRadioButton* pRadioButton = &pSubComponent->subComponentRadioButton;
	ImGui::RadioButton(pRadioButton->pLabel, pRadioButton->pData, pRadioButton->id);

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
			SubComponent* subComponent = gMainComponentList[i]->pSubComponents[j];
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
