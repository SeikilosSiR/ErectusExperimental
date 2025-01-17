#include "gui.h"

#include "app.h"
#include "common.h"
#include "settings.h"

#include "ErectusProcess.h"
#include "ErectusMemory.h"
#include "threads.h"
#include "utils.h"
#include "dependencies/fmt/fmt/format.h"
#include "dependencies/imgui/imgui_internal.h"
#include "dependencies/imgui/imgui_stdlib.h"

#include "game/Game.h"

void Gui::Render()
{
	if (!(gApp->GetMode() == App::Mode::Overlay))
		Menu();

	if (gApp->GetMode() != App::Mode::Standalone)
		RenderOverlay();
}

void Gui::RenderOverlay()
{
	const auto camera = Game::GetPlayerCamera();

	RenderEntities(camera);
	RenderPlayers(camera);

	RenderInfoBox();
}

void Gui::RenderEntities(const Camera& cameraData)
{
	auto entities = ErectusMemory::entityDataBuffer;
	for (const auto& entity : entities)
	{
		if (entity.flag & CUSTOM_ENTRY_ENTITY)
			RenderItems(entity, cameraData, Settings::esp.entities);
		else if (entity.flag & CUSTOM_ENTRY_JUNK)
			RenderItems(entity, cameraData, Settings::esp.junk);
		else if (entity.flag & CUSTOM_ENTRY_ITEM)
			RenderItems(entity, cameraData, Settings::esp.items);
		else if (entity.flag & CUSTOM_ENTRY_CONTAINER)
			RenderItems(entity, cameraData, Settings::esp.containers);
		else if (entity.flag & CUSTOM_ENTRY_PLAN)
			RenderItems(entity, cameraData, Settings::esp.plans);
		else if (entity.flag & CUSTOM_ENTRY_MAGAZINE)
			RenderItems(entity, cameraData, Settings::esp.magazines);
		else if (entity.flag & CUSTOM_ENTRY_BOBBLEHEAD)
			RenderItems(entity, cameraData, Settings::esp.bobbleheads);
		else if (entity.flag & CUSTOM_ENTRY_FLORA)
			RenderItems(entity, cameraData, Settings::esp.flora);
		else if (entity.flag & CUSTOM_ENTRY_NPC)
			RenderActors(entity, cameraData, Settings::esp.npcs);
	}
}

void Gui::RenderPlayers(const Camera& cameraData)
{
	auto players = ErectusMemory::playerDataBuffer;
	for (const auto& player : players) {
		if (player.flag & CUSTOM_ENTRY_PLAYER)
			RenderActors(player, cameraData, Settings::esp.players);
	}
}

void Gui::RenderActors(const CustomEntry& entry, const Camera& camera, const EspSettings::Actors& settings)
{
	if (!settings.drawEnabled && !settings.drawDisabled)
		return;

	if (settings.enabledAlpha == 0.0f && settings.disabledAlpha == 0.0f)
		return;

	if (!settings.drawNamed && !settings.drawUnnamed)
		return;

	TesObjectRefr entityData = {};
	if (!ErectusProcess::Rpm(entry.entityPtr, &entityData, sizeof entityData))
		return;

	auto epicRank = entityData.GetEpicRank();

	auto allowNpc = false;
	if (entry.flag & CUSTOM_ENTRY_NPC)
	{
		if (epicRank)
		{
			switch (entityData.GetActorState())
			{
			case ActorState::Alive:
			case ActorState::Downed:
			case ActorState::Dead:
				switch (epicRank)
				{
				case 1:
					allowNpc = Settings::esp.npcsExt.overrideLivingOneStar;
					break;
				case 2:
					allowNpc = Settings::esp.npcsExt.overrideLivingTwoStar;
					break;
				case 3:
					allowNpc = Settings::esp.npcsExt.overrideLivingThreeStar;
					break;
				default:
					break;
				}
				break;
			case ActorState::Unknown:
				break;
			}
		}
	}

	if (!settings.enabled && !allowNpc)
		return;

	if (entry.flag & CUSTOM_ENTRY_UNNAMED)
	{
		if (!settings.drawUnnamed)
			return;
	}
	else
	{
		if (!settings.drawNamed)
			return;
	}

	auto alpha = 0.f;

	if (entityData.spawnFlag == 0x02)
	{
		if (settings.drawEnabled)
			alpha = settings.enabledAlpha;
	}
	else
	{
		if (settings.drawDisabled)
			alpha = settings.disabledAlpha;
	}

	if (alpha == 0.f)
		return;

	auto showHealthText = false;

	const float* color = nullptr;

	auto legendaryAlpha = 1.0f;

	switch (entityData.GetActorState())
	{
	case ActorState::Alive:
		showHealthText = settings.showHealth;
		if (allowNpc)
		{
			switch (epicRank)
			{
			case 1:
				color = Settings::esp.npcsExt.livingOneStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			case 2:
				color = Settings::esp.npcsExt.livingTwoStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			case 3:
				color = Settings::esp.npcsExt.livingThreeStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			default:
				break;
			}
		}
		else if (settings.drawAlive)
			color = settings.aliveColor;
		break;
	case ActorState::Downed:
		showHealthText = settings.showHealth;
		if (allowNpc)
		{
			switch (epicRank)
			{
			case 1:
				color = Settings::esp.npcsExt.livingOneStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			case 2:
				color = Settings::esp.npcsExt.livingTwoStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			case 3:
				color = Settings::esp.npcsExt.livingThreeStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			default:
				break;
			}
		}
		else if (settings.drawDowned)
			color = settings.downedColor;
		break;
	case ActorState::Dead:
		showHealthText = settings.showDeadHealth;
		if (allowNpc)
		{
			switch (epicRank)
			{
			case 1:
				color = Settings::esp.npcsExt.deadOneStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			case 2:
				color = Settings::esp.npcsExt.deadTwoStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			case 3:
				color = Settings::esp.npcsExt.deadThreeStarColor;
				if (entityData.spawnFlag == 0x02)
					alpha = legendaryAlpha;
				break;
			default:
				break;
			}
		}
		else if (settings.drawDead)
			color = settings.deadColor;
		break;
	case ActorState::Unknown:
		showHealthText = settings.showHealth;
		if (settings.drawUnknown)
			color = settings.unknownColor;
		break;
	}

	if (color == nullptr)
		return;

	auto distance = static_cast<int>(entityData.position.DistanceTo(camera.origin) * 0.01f);
	if (distance > settings.enabledDistance)
		return;

	if (entry.entityPtr == ErectusMemory::targetLockedEntityPtr)
		color = Settings::targetting.lockedColor;

	auto screenPosition = camera.World2Screen(entityData.position);
	if (screenPosition.x == 0.f && screenPosition.y == 0.f)
		return;

	auto health = entityData.GetCurrentHealth();
	std::string itemText;
	if (settings.showName && showHealthText && settings.showDistance) //Name, Health, Distance
		itemText = format(FMT_STRING("{0}\n{1:.0f} hp [{2:d} m]"), entry.name, health, distance);
	else if (settings.showName && showHealthText && !settings.showDistance) //Name, Health
		itemText = format(FMT_STRING("{0}\n{1:.0f} hp"), entry.name, health);
	else if (settings.showName && !showHealthText && settings.showDistance) //Name, Distance
		itemText = format(FMT_STRING("{0}\n[{1:d} m]"), entry.name, distance);
	else if (!settings.showName && showHealthText && settings.showDistance) //Health, Distance
		itemText = format(FMT_STRING("{0:.0f} hp [{1:d} m]"), health, distance);
	else if (settings.showName && !showHealthText && !settings.showDistance) //Name
		itemText = entry.name;
	else if (!settings.showName && showHealthText && !settings.showDistance) //Health
		itemText = format(FMT_STRING("{:.0f} hp"), health);
	else if (!settings.showName && !showHealthText && settings.showDistance) //Distance
		itemText = format(FMT_STRING("[{:d} m]"), distance);

	if (!itemText.empty())
	{
		if (Settings::utilities.debugEsp)
			itemText = format(FMT_STRING("{0:08x}\n{1:08x}"), entry.entityFormId, entry.baseObjectFormId);

		RenderText(Utils::strtowstr(itemText).c_str(), screenPosition, IM_COL32(color[0] * 255.f, color[1] * 255.f, color[2] * 255.f, alpha * 255.f));
	}
}

void Gui::RenderText(const char* text, const Vector2& position, const ImU32 color)
{
	if (text == nullptr)
		return;

	//centering
	auto* font = ImGui::GetIO().Fonts->Fonts[1];
	const auto  textSize = font->CalcTextSizeA(13.f, FLT_MAX, 0.f, text);
	const ImVec2 pos = { position.x - textSize.x / 2.f, position.y - textSize.y / 2.f };

	//outline
	ImGui::GetBackgroundDrawList()->AddText(font, 13.f, { pos.x + 1.f, pos.y + 1.f }, IM_COL32_BLACK, text);
	ImGui::GetBackgroundDrawList()->AddText(font, 13.f, { pos.x + 1.f, pos.y - 1.f }, IM_COL32_BLACK, text);
	ImGui::GetBackgroundDrawList()->AddText(font, 13.f, { pos.x - 1.f, pos.y + 1.f }, IM_COL32_BLACK, text);
	ImGui::GetBackgroundDrawList()->AddText(font, 13.f, { pos.x - 1.f, pos.y - 1.f }, IM_COL32_BLACK, text);

	ImGui::GetBackgroundDrawList()->AddText(font, 13.f, pos, color, text);
}

void Gui::RenderItems(const CustomEntry& entry, const Camera& camera, const EspSettings::Items& settings)
{
	if (!(entry.flag & CUSTOM_ENTRY_WHITELISTED) && !settings.enabled)
		return;

	if (!settings.drawEnabled && !settings.drawDisabled)
		return;

	if (settings.enabledAlpha == 0.0f && settings.disabledAlpha == 0.0f)
		return;

	if (!settings.drawNamed && !settings.drawUnnamed)
		return;

	TesObjectRefr entityData{};
	if (!ErectusProcess::Rpm(entry.entityPtr, &entityData, sizeof entityData))
		return;

	if (entry.flag & CUSTOM_ENTRY_UNNAMED)
	{
		if (!settings.drawUnnamed)
			return;
	}
	else if (!settings.drawNamed)
		return;

	if (entry.flag & CUSTOM_ENTRY_PLAN)
	{
		if (!Settings::esp.plansExt.knownRecipesEnabled && !Settings::esp.plansExt.unknownRecipesEnabled)
			return;

		if (!Settings::esp.plansExt.knownRecipesEnabled && entry.flag & CUSTOM_ENTRY_KNOWN_RECIPE)
			return;
		if (!Settings::esp.plansExt.unknownRecipesEnabled && entry.flag & CUSTOM_ENTRY_UNKNOWN_RECIPE)
			return;
	}

	auto alpha = 0.f;

	if (entityData.spawnFlag == 0x02)
	{
		if (settings.drawEnabled)
		{
			if (entry.flag & CUSTOM_ENTRY_FLORA)
			{
				if (!ErectusMemory::IsFloraHarvested(entityData.harvestFlagA, entityData.harvestFlagB))
					alpha = settings.enabledAlpha;
				else if (settings.drawDisabled)
					alpha = settings.disabledAlpha;
			}
			else
				alpha = settings.enabledAlpha;
		}
	}
	else
	{
		if (settings.drawDisabled)
			alpha = settings.disabledAlpha;
	}

	if (alpha == 0.f)
		return;

	const auto distance = static_cast<int>(entityData.position.DistanceTo(camera.origin) * 0.01f);
	if (distance > settings.enabledDistance)
		return;

	const auto screenPosition = camera.World2Screen(entityData.position);
	if (screenPosition.x == 0.f && screenPosition.y == 0.f)
		return;

	std::string itemText{};
	if (settings.showName && settings.showDistance)
		itemText = format(FMT_STRING("{0}\n[{1:d} m]"), entry.name, distance);
	else if (settings.showName && !settings.showDistance)
		itemText = entry.name;
	else if (!settings.showName && settings.showDistance)
		itemText = format(FMT_STRING("[{0:d} m]"), distance);

	if (!itemText.empty())
	{
		if (Settings::utilities.debugEsp)
			itemText = format(FMT_STRING("{0:16x}\n{1:08x}\n{2:16x}\n{3:08x}"), entry.entityPtr, entry.entityFormId, entry.baseObjectPtr, entry.baseObjectFormId);

		RenderText(Utils::strtowstr(itemText).c_str(), screenPosition, IM_COL32(settings.color[0] * 255.f, settings.color[1] * 255.f, settings.color[2] * 255.f, alpha * 255.f));
	}
}

void Gui::RenderInfoBox()
{
	std::vector<std::pair<std::string, bool>> infoTexts = {};

	std::string featureText = {};
	auto featureState = false;

	ImVec4 enabledTextColor = { 0.f, 1.f, 0.f, 1.f };
	ImVec4 disabledTextColor = { 1.f, 0.f, 0.f, 1.f };

	if (Settings::esp.infobox.drawPlayerInfo) {
		auto player = Game::GetLocalPlayer();

		featureText = format(FMT_STRING("Player FormId: {:08x}"), player.formId);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("STASH FormId: {:08x}"), player.playerStashFormId);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("Cell FormId: {:08x}"), player.GetCurrentCell().formId);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("X: {:.2f}"), player.position.x);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("Y: {:.2f}"), player.position.y);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("Z: {:.2f}"), player.position.z);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("Yaw: {:.2f}"), player.yaw);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("Pitch: {:.2f}"), player.pitch);
		infoTexts.emplace_back(featureText, true);

		featureText = format(FMT_STRING("Health: {:.2f}"), player.GetCurrentHealth());
		infoTexts.emplace_back(featureText, true);
	}

	if (Settings::esp.infobox.drawPositionSpoofingStatus)
	{
		featureText = fmt::format((const char*)u8"����� ������� (���.): {0:d} (Height: {1:d})", static_cast<int>(Threads::positionSpoofingToggle), Settings::localPlayer.positionSpoofingHeight);
		featureState = Settings::localPlayer.positionSpoofingEnabled;
		infoTexts.emplace_back(featureText, featureState);
	}

	if (Settings::esp.infobox.drawNukeCodes)
	{
		featureText = format((const char*)u8"{} - �����", fmt::join(ErectusMemory::alphaCode, " "));
		featureState = ErectusMemory::alphaCode == std::array<int, 8>{} ? false : true;
		infoTexts.emplace_back(featureText, featureState);

		featureText = format((const char*)u8"{} - �����", fmt::join(ErectusMemory::bravoCode, " "));
		featureState = ErectusMemory::bravoCode == std::array<int, 8>{} ? false : true;
		infoTexts.emplace_back(featureText, featureState);

		featureText = format((const char*)u8"{} - �����", fmt::join(ErectusMemory::charlieCode, " "));
		featureState = ErectusMemory::charlieCode == std::array<int, 8>{} ? false : true;
		infoTexts.emplace_back(featureText, featureState);
	}

	if (Settings::esp.infobox.drawFps)
	{
		featureText = format(FMT_STRING("FPS: {:.2f}"), ImGui::GetIO().Framerate);
		featureState = true;
		infoTexts.emplace_back(featureText, featureState);
	}

	if (infoTexts.empty())
		return;

	ImGui::SetNextWindowBgAlpha(.7f);
	ImGui::SetNextWindowPos(ImVec2(10.f, 10.f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("##infobox", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		for (const auto& [text, enabled] : infoTexts)
		{
			ImGui::TextColored(enabled ? enabledTextColor : disabledTextColor, text.c_str());
		}
	}
	ImGui::End();
}

void Gui::MenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("�����"))
			gApp->Shutdown();

		if (mode == Menu::ProcessMenu)
		{
			if (ImGui::MenuItem("���������"))
				mode = Menu::SettingsMenu;
		}
		else
		{
			if (ImGui::MenuItem("��������� ��������"))
				mode = Menu::ProcessMenu;
		}

		if (gApp->GetMode() == App::Mode::Standalone)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

			ImGui::MenuItem("���������");

			ImGui::PopStyleVar();
			ImGui::PopItemFlag();
		}
		else
		{
			if (ImGui::MenuItem("���������"))
				gApp->ToggleOverlay();
		}

		ImGui::EndMenuBar();
	}
}

void Gui::Menu()
{
	auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize;
	if (gApp->GetMode() == App::Mode::Standalone)
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Main", nullptr, windowFlags))
	{
		MenuBar();

		if (mode == Menu::ProcessMenu)
			ProcessMenu();
		else if (mode == Menu::SettingsMenu)
			SettingsMenu();

		//auto resize host window if in standalone mode
		if (gApp->GetMode() == App::Mode::Standalone)
		{
			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

			const auto  requestedSize = ImGui::GetWindowSize();
			if (requestedSize.x != 32 && requestedSize.y != 32)
				gApp->appWindow->SetSize(static_cast<LONG>(requestedSize.x), static_cast<LONG>(requestedSize.y));
		}
	}
	ImGui::End();
}
void Gui::ProcessMenu()
{
	ImGui::SetWindowSize(ImVec2(384, 224));

	ImGui::SetNextItemWidth(-16.f);
	auto processText = ErectusProcess::pid ? "Fallout76.exe - " + std::to_string(ErectusProcess::pid) : (const char*)u8"��� ���������� ��������.";
	if (ImGui::BeginCombo("###ProcessList", processText.c_str()))
	{
		for (auto item : ErectusProcess::GetProcesses())
		{
			processText = item ? "Fallout76.exe - " + std::to_string(item) : "���";
			if (ImGui::Selectable(processText.c_str()))
				gApp->Attach(item);
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();

	switch (ErectusProcess::processErrorId)
	{
	case 0:
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ErectusProcess::processError.c_str());
		break;
	case 1:
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ErectusProcess::processError.c_str());
		break;
	case 2:
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ErectusProcess::processError.c_str());
		break;
	default:
		ImGui::Text(ErectusProcess::processError.c_str());
		break;
	}

	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::Columns(2);
	ImGui::Separator();
	ImGui::Text("��������� ���� Keybind");
	ImGui::NextColumn();
	ImGui::Text("CTRL+ENTER");
	ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("ImGui (D3D9) FPS");
	ImGui::NextColumn();
	ImGui::Text("%.2f", ImGui::GetIO().Framerate);
	ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("PID (Process Id)");
	ImGui::NextColumn();
	ImGui::Text("%lu", ErectusProcess::pid);
	ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("HWND (Window)");
	ImGui::NextColumn();
	ImGui::Text("%p", ErectusProcess::hWnd);
	ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("Base Address");
	ImGui::NextColumn();
	ImGui::Text("%016llX", ErectusProcess::exe);
	ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("HANDLE");
	ImGui::NextColumn();
	ImGui::Text("%p", ErectusProcess::handle);
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::PopItemFlag();
}

void Gui::ButtonToggle(const char* label, bool& state)
{
	if (state)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x / 2, 0)))
			state = false;
		ImGui::PopStyleColor(3);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
			state = true;
		ImGui::PopStyleColor(3);
	}
}

void Gui::LargeButtonToggle(const char* label, bool& state)
{
	if (state)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(-FLT_MIN, 0)))
			state = false;
		ImGui::PopStyleColor(3);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(-FLT_MIN, 0)))
			state = true;
		ImGui::PopStyleColor(3);
	}
}

void Gui::SmallButtonToggle(const char* label, bool& state)
{
	if (state)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(110.0f, 0.0f)))
			state = false;
		ImGui::PopStyleColor(3);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(110.0f, 0.0f)))
			state = true;
		ImGui::PopStyleColor(3);
	}
}

void Gui::EspSettings(EspSettings::Actors& actorEsp)
{
	ImGui::PushID(&actorEsp);
	ImGui::Columns(2, nullptr, false);

	LargeButtonToggle("ESP ��������", actorEsp.enabled);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::SliderInt("##Distance", &actorEsp.enabledDistance, 0, 3000, "���������: %d");
	ImGui::NextColumn();

	LargeButtonToggle("���������� �����", actorEsp.drawAlive);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::ColorEdit3("##AliveColor", actorEsp.aliveColor);
	Utils::ValidateRgb(actorEsp.aliveColor);
	ImGui::NextColumn();

	LargeButtonToggle("���������� �������", actorEsp.drawDowned);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::ColorEdit3("##DownedColor", actorEsp.downedColor);
	Utils::ValidateRgb(actorEsp.downedColor);
	ImGui::NextColumn();

	LargeButtonToggle("���������� �������", actorEsp.drawDead);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::ColorEdit3("##DeadColor", actorEsp.deadColor);
	Utils::ValidateRgb(actorEsp.deadColor);
	ImGui::NextColumn();

	LargeButtonToggle("���������� �����������", actorEsp.drawUnknown);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::ColorEdit3("##UnknownColor", actorEsp.unknownColor);
	Utils::ValidateRgb(actorEsp.unknownColor);
	ImGui::NextColumn();

	LargeButtonToggle("���������� ��������", actorEsp.drawEnabled);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::SliderFloat("##EnabledAlpha", &actorEsp.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");
	ImGui::NextColumn();

	LargeButtonToggle("���������� ����������", actorEsp.drawDisabled);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::SliderFloat("##DisabledAlpha", &actorEsp.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");
	ImGui::NextColumn();

	LargeButtonToggle("���������� ���������", actorEsp.drawNamed);
	ImGui::NextColumn();
	LargeButtonToggle("���������� ����������", actorEsp.drawUnnamed);
	ImGui::NextColumn();

	LargeButtonToggle("�������� ���", actorEsp.showName);
	ImGui::NextColumn();
	LargeButtonToggle("�������� ���������", actorEsp.showDistance);
	ImGui::NextColumn();

	LargeButtonToggle("�������� ����� �����", actorEsp.showHealth);
	ImGui::NextColumn();
	LargeButtonToggle("�������� ����� �������", actorEsp.showDeadHealth);
	ImGui::NextColumn();

	ImGui::Columns();
	ImGui::PopID();
}

void Gui::EspSettings(EspSettings::Items& itemEsp)
{
	ImGui::PushID(&itemEsp);
	ImGui::Columns(2, nullptr, false);

	LargeButtonToggle("ESP ��������", itemEsp.enabled);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::SliderInt("##Distance", &itemEsp.enabledDistance, 0, 3000, "���������: %d");
	ImGui::NextColumn();

	LargeButtonToggle("���������� ��������", itemEsp.drawEnabled);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::SliderFloat("##EnabledAlpha", &itemEsp.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");
	ImGui::NextColumn();

	LargeButtonToggle("���������� ����������", itemEsp.drawDisabled);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::SliderFloat("##DisabledAlpha", &itemEsp.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");
	ImGui::NextColumn();

	LargeButtonToggle("���������� ���������", itemEsp.drawNamed);
	ImGui::NextColumn();
	LargeButtonToggle("���������� ����������", itemEsp.drawUnnamed);
	ImGui::NextColumn();

	LargeButtonToggle("���������� ���", itemEsp.showName);
	ImGui::NextColumn();
	LargeButtonToggle("���������� ���������", itemEsp.showDistance);
	ImGui::NextColumn();

	ImGui::Columns();

	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::ColorEdit3("##Color", itemEsp.color);
	Utils::ValidateRgb(itemEsp.color);

	ImGui::PopID();
}
void Gui::OverlayMenuTabEsp()
{
	if (ImGui::BeginTabItem("ESP###ESPTab"))
	{
		if (ImGui::CollapsingHeader("��������� ESP ������"))
			EspSettings(Settings::esp.players);

		if (ImGui::CollapsingHeader("��������� ESP NPC"))
		{
			EspSettings(Settings::esp.npcs);

			ImGui::PushItemWidth(-FLT_MIN);
			ButtonToggle("������ ���������� ����� 1* NPCs", Settings::esp.npcsExt.overrideLivingOneStar);
			ImGui::SameLine();
			ImGui::ColorEdit3("###LivingOneStarColor", Settings::esp.npcsExt.livingOneStarColor);
			Utils::ValidateRgb(Settings::esp.npcsExt.livingOneStarColor);

			ButtonToggle("������ ���������� ������� 1* NPCs", Settings::esp.npcsExt.overrideDeadOneStar);
			ImGui::SameLine();
			ImGui::ColorEdit3("###DeadOneStarColor", Settings::esp.npcsExt.deadOneStarColor);
			Utils::ValidateRgb(Settings::esp.npcsExt.deadOneStarColor);

			ButtonToggle("������ ���������� ����� 2* NPCs", Settings::esp.npcsExt.overrideLivingTwoStar);
			ImGui::SameLine();
			ImGui::ColorEdit3("###LivingTwoStarColor", Settings::esp.npcsExt.livingTwoStarColor);
			Utils::ValidateRgb(Settings::esp.npcsExt.livingTwoStarColor);

			ButtonToggle("������ ���������� ������� 2* NPCs", Settings::esp.npcsExt.overrideDeadTwoStar);
			ImGui::SameLine();
			ImGui::ColorEdit3("###DeadTwoStarColor", Settings::esp.npcsExt.deadTwoStarColor);
			Utils::ValidateRgb(Settings::esp.npcsExt.deadTwoStarColor);

			ButtonToggle("������ ���������� ����� 3* NPCs", Settings::esp.npcsExt.overrideLivingThreeStar);
			ImGui::SameLine();
			ImGui::ColorEdit3("###LivingThreeStarColor", Settings::esp.npcsExt.livingThreeStarColor);
			Utils::ValidateRgb(Settings::esp.npcsExt.livingThreeStarColor);

			ButtonToggle("������ ���������� ������� 3* NPCs", Settings::esp.npcsExt.overrideDeadThreeStar);
			ImGui::SameLine();
			ImGui::ColorEdit3("###DeadThreeStarColor", Settings::esp.npcsExt.deadThreeStarColor);
			Utils::ValidateRgb(Settings::esp.npcsExt.deadThreeStarColor);
			ImGui::PopItemWidth();
		}

		if (ImGui::CollapsingHeader("��������� ESP ���������"))
			EspSettings(Settings::esp.containers);

		if (ImGui::CollapsingHeader("��������� ESP �����"))
			EspSettings(Settings::esp.junk);

		if (ImGui::CollapsingHeader("��������� ESP ��������"))
			EspSettings(Settings::esp.magazines);

		if (ImGui::CollapsingHeader("��������� ESP ������"))
			EspSettings(Settings::esp.bobbleheads);

		if (ImGui::CollapsingHeader("��������� ESP ���������"))
			EspSettings(Settings::esp.items);

		if (ImGui::CollapsingHeader("��������� ESP ����/�������"))
		{
			EspSettings(Settings::esp.plans);

			ButtonToggle("���������� ���������", Settings::esp.plansExt.knownRecipesEnabled);
			ImGui::SameLine();
			LargeButtonToggle("���������� �����������", Settings::esp.plansExt.unknownRecipesEnabled);
		}

		if (ImGui::CollapsingHeader("��������� ESP �����"))
		{
			EspSettings(Settings::esp.flora);

			LargeButtonToggle("���������� ����� ����� �����", Settings::esp.floraExt.crimsonFluxEnabled);
			LargeButtonToggle("���������� ����� ���������� �����", Settings::esp.floraExt.cobaltFluxEnabled);
			LargeButtonToggle("���������� ����� ��������� �����", Settings::esp.floraExt.yellowcakeFluxEnabled);
			LargeButtonToggle("���������� ����� ��������������� �����", Settings::esp.floraExt.fluorescentFluxEnabled);
			LargeButtonToggle("���������� ����� ����������� �����", Settings::esp.floraExt.violetFluxEnabled);
		}

		if (ImGui::CollapsingHeader("��������� ESP ��������"))
			EspSettings(Settings::esp.entities);

		if (ImGui::CollapsingHeader("����������"))
		{
			LargeButtonToggle("�������� ������ ���������� ������", Settings::esp.infobox.drawPlayerInfo);
			LargeButtonToggle("�������� ������ �������", Settings::esp.infobox.drawPositionSpoofingStatus);
			LargeButtonToggle("�������� ������� ����", Settings::esp.infobox.drawNukeCodes);
			LargeButtonToggle("�������� FPS", Settings::esp.infobox.drawFps);
		}

		if (ImGui::CollapsingHeader("����� ������"))
		{
			ImGui::Columns(2, nullptr, false);

			for (auto& [formId, isEnabled] : Settings::esp.whitelist)
			{
				auto toggleLabel = fmt::format("��������##espwhiteList{0:x}Enabled", formId);
				LargeButtonToggle(toggleLabel.c_str(), isEnabled);

				ImGui::NextColumn();

				auto inputLabel = format(FMT_STRING("##espWhiteList{0:x}Item"), formId);
				auto key = formId;
				auto value = isEnabled;
				if (ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
				{
					Settings::esp.whitelist.erase(formId);
					if (key)
						Settings::esp.whitelist.try_emplace(key, value);
				}

				ImGui::NextColumn();
			}
			//this is for inserting new records into the map
			{
				DWORD key = 0;
				auto value = false;

				LargeButtonToggle("��������##espWhiteListNewEnabled", value);

				ImGui::NextColumn();

				if (ImGui::InputScalar("##espWhiteListNewItem", ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
				{
					if (key)
						Settings::esp.whitelist.try_emplace(key, value);
				}
			}

			ImGui::Columns();
		}

		if (ImGui::CollapsingHeader("׸���� ������"))
		{
			ImGui::Columns(2, nullptr, false);

			for (auto& [formId, isEnabled] : Settings::esp.blacklist)
			{
				auto toggleLabel = format(FMT_STRING("��������##espBlackList{0:x}Enabled"), formId);
				LargeButtonToggle(toggleLabel.c_str(), isEnabled);

				ImGui::NextColumn();

				auto inputLabel = format(FMT_STRING("##espBlackList{0:x}Item"), formId);
				auto key = formId;
				auto value = isEnabled;
				if (ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
				{
					Settings::esp.blacklist.erase(formId);
					if (key)
						Settings::esp.blacklist.try_emplace(key, value);
				}

				ImGui::NextColumn();
			}
			//this is for inserting new records into the map
			{
				DWORD key = 0;
				auto value = false;

				LargeButtonToggle("��������##espBlackListNewEnabled", value);

				ImGui::NextColumn();

				if (ImGui::InputScalar("##espBlackListNewItem", ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
				{
					if (key)
						Settings::esp.blacklist.try_emplace(key, value);
				}
			}

			ImGui::Columns();
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabLooter()
{
	if (ImGui::BeginTabItem("�������"))
	{
		if (ImGui::CollapsingHeader("�����"))
		{
			if (ImGui::RadioButton("��������", Settings::looter.mode == LooterSettings::Mode::Disabled))
				Settings::looter.mode = LooterSettings::Mode::Disabled;
			if (ImGui::RadioButton("��������", Settings::looter.mode == LooterSettings::Mode::Auto))
				Settings::looter.mode = LooterSettings::Mode::Auto;
			if (ImGui::RadioButton("�������� ������ (CTRL + R)", Settings::looter.mode == LooterSettings::Mode::Keybind))
				Settings::looter.mode = LooterSettings::Mode::Keybind;
		}

		if (ImGui::CollapsingHeader("�������"))
		{
			LargeButtonToggle("���� � NPCs (76m)", Settings::looter.looters.npcs);
			LargeButtonToggle("���� ��������� (76m)", Settings::looter.looters.groundItems);
			LargeButtonToggle("���� ����������� (6m)", Settings::looter.looters.containers);
			LargeButtonToggle("���� ����� (6m)", Settings::looter.looters.flora);
		}

		if (ImGui::TreeNodeEx("�����", ImGuiTreeNodeFlags_Framed)) {

			if (ImGui::CollapsingHeader("������"))
			{
				LargeButtonToggle("��##weapons", Settings::looter.selection.weapons.all);
				LargeButtonToggle("1*##weapons", Settings::looter.selection.weapons.oneStar);
				LargeButtonToggle("2*##weapons", Settings::looter.selection.weapons.twoStar);
				LargeButtonToggle("3*##weapons", Settings::looter.selection.weapons.threeStar);
			}

			if (ImGui::CollapsingHeader("�����"))
			{
				LargeButtonToggle("��##apparel", Settings::looter.selection.apparel.all);
				LargeButtonToggle("1*##apparel", Settings::looter.selection.apparel.oneStar);
				LargeButtonToggle("2*##apparel", Settings::looter.selection.apparel.twoStar);
				LargeButtonToggle("3*##apparel", Settings::looter.selection.apparel.threeStar);
			}

			if (ImGui::CollapsingHeader("������"))
			{
				LargeButtonToggle("��##aid", Settings::looter.selection.aid.all);
				LargeButtonToggle("�����##aid", Settings::looter.selection.aid.bobbleheads);
				LargeButtonToggle("�������##aid", Settings::looter.selection.aid.magazines);
			}

			if (ImGui::CollapsingHeader("������"))
			{
				LargeButtonToggle("��##misc", Settings::looter.selection.misc.all);
			}

			if (ImGui::CollapsingHeader("����������"))
			{
				LargeButtonToggle("��##holo", Settings::looter.selection.holo.all);
			}

			if (ImGui::CollapsingHeader("�������"))
			{
				LargeButtonToggle("��##notes", Settings::looter.selection.notes.all);
				LargeButtonToggle("��������� �����##notes", Settings::looter.selection.notes.plansKnown);
				LargeButtonToggle("����������� �����##notes", Settings::looter.selection.notes.plansUnknown);
				LargeButtonToggle("����� ��������##notes", Settings::looter.selection.notes.treasureMaps);
			}

			if (ImGui::CollapsingHeader("����"))
			{
				LargeButtonToggle("��##junk", Settings::looter.selection.junk.all);

				ImGui::Columns(2, nullptr, false);
				for (auto& [formId, isEnabled] : Settings::looter.selection.junk.components)
				{
					auto label = format(FMT_STRING("{}##junk"), JUNK_COMPONENT_NAMES.find(formId)->second);
					LargeButtonToggle(label.c_str(), isEnabled);

					ImGui::NextColumn();
				}
				ImGui::Columns();
			}

			if (ImGui::CollapsingHeader("�����"))
			{
				LargeButtonToggle("��##flora", Settings::looter.selection.flora.all);

				ImGui::Columns(2, nullptr, false);
				for (auto& [formId, isEnabled] : Settings::looter.selection.flora.components)
				{
					auto label = format(FMT_STRING("{}##flora"), FLORA_COMPONENT_NAMES.find(formId)->second);
					LargeButtonToggle(label.c_str(), isEnabled);

					ImGui::NextColumn();
				}
				ImGui::Columns();
			}

			if (ImGui::CollapsingHeader("����"))
			{
				LargeButtonToggle("��##mods", Settings::looter.selection.mods.all);
			}

			if (ImGui::CollapsingHeader("�������"))
			{
				LargeButtonToggle("��##ammo", Settings::looter.selection.ammo.all);
			}

			if (ImGui::CollapsingHeader("������"))
			{
				LargeButtonToggle("������##other", Settings::looter.selection.other.caps);
			}

			if (ImGui::CollapsingHeader("����� ������"))
			{
				ImGui::Columns(2, nullptr, false);

				for (auto& [formId, isEnabled] : Settings::looter.selection.whitelist)
				{
					auto toggleLabel = fmt::format("��������##whiteList{0:x}Enabled", formId);
					LargeButtonToggle(toggleLabel.c_str(), isEnabled);

					ImGui::NextColumn();

					auto inputLabel = format(FMT_STRING("##whiteList{0:x}Item"), formId);
					auto key = formId;
					auto value = isEnabled;
					if (ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
					{
						Settings::looter.selection.whitelist.erase(formId);
						if (key)
							Settings::looter.selection.whitelist.try_emplace(key, value);
					}

					ImGui::NextColumn();
				}
				//this is for inserting new records into the map
				{
					DWORD key = 0;
					auto value = false;

					LargeButtonToggle("��������##whiteListNewEnabled", value);

					ImGui::NextColumn();

					if (ImGui::InputScalar("##whiteListNewItem", ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
					{
						if (key)
							Settings::looter.selection.whitelist.try_emplace(key, value);
					}
				}

				ImGui::Columns();
			}

			if (ImGui::CollapsingHeader("׸���� ������"))
			{
				ImGui::Columns(2, nullptr, false);

				for (auto& [formId, isEnabled] : Settings::looter.selection.blacklist)
				{
					auto toggleLabel = format(FMT_STRING("��������##blackList{0:x}Enabled"), formId);
					LargeButtonToggle(toggleLabel.c_str(), isEnabled);

					ImGui::NextColumn();

					auto inputLabel = format(FMT_STRING("##blackList{0:x}Item"), formId);
					auto key = formId;
					auto value = isEnabled;
					if (ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
					{
						Settings::looter.selection.blacklist.erase(formId);
						if (key)
							Settings::looter.selection.blacklist.try_emplace(key, value);
					}

					ImGui::NextColumn();
				}
				//this is for inserting new records into the map
				{
					DWORD key = 0;
					auto value = false;

					LargeButtonToggle("��������##blackListNewEnabled", value);

					ImGui::NextColumn();

					if (ImGui::InputScalar("##blackListNewItem", ImGuiDataType_U32, &key, nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal) && ImGui::IsItemDeactivated())
					{
						if (key)
							Settings::looter.selection.blacklist.try_emplace(key, value);
					}
				}

				ImGui::Columns();
			}

			ImGui::TreePop();
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabCombat()
{
	if (ImGui::BeginTabItem("���###CombatTab"))
	{
		if (ImGui::CollapsingHeader("�������� ������"))
		{
			ImGui::Columns(2, nullptr, false);

			LargeButtonToggle("��� ������", Settings::weapons.noRecoil);
			ImGui::NextColumn();
			LargeButtonToggle("����������� �������", Settings::weapons.infiniteAmmo);
			ImGui::NextColumn();

			LargeButtonToggle("��� ��������", Settings::weapons.noSpread);
			ImGui::NextColumn();
			LargeButtonToggle("������� �����������", Settings::weapons.instantReload);
			ImGui::NextColumn();

			LargeButtonToggle("��� ��������", Settings::weapons.noSway);
			ImGui::NextColumn();
			LargeButtonToggle("���� ���������������###WeaponAutomatic", Settings::weapons.automaticflag);
			ImGui::NextColumn();

			LargeButtonToggle("�������###WeaponCapacityEnabled", Settings::weapons.capacityEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###WeaponCapacity", &Settings::weapons.capacity, 0, 999, "�������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("����������������###WeaponSpeedEnabled", Settings::weapons.speedEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderFloat("###WeaponSpeed", &Settings::weapons.speed, 1.0f, 100.0f, "��������: %.2f");
			ImGui::NextColumn();

			LargeButtonToggle("���������###WeaponReachEnabled", Settings::weapons.reachEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderFloat("###WeaponReach", &Settings::weapons.reach, 1.0f, 999.0f, "���������: %.0f");
			ImGui::NextColumn();

			LargeButtonToggle("������###WeaponRangeEnabled", Settings::weapons.rangeEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderFloat("###WeaponRange", &Settings::weapons.range, 0.0f, 5000.f, "������: %.0f");
			ImGui::NextColumn();

			ImGui::Columns();
		}

		if (ImGui::CollapsingHeader("��������� ���������"))
		{
			LargeButtonToggle("��������� �� NPC (Keybind: T)", Settings::targetting.lockNpCs);

			ButtonToggle("������������� ����", Settings::targetting.dmgRedirect);
			ImGui::SameLine();
			LargeButtonToggle("��������� ����", Settings::targetting.dmgSend);

			LargeButtonToggle("��������� �� NPC � ����������� ����������###TargetUnknown", Settings::targetting.targetUnknown);

			ButtonToggle("������������ ������ NPCs###IgnoreEssentialNPCs", Settings::targetting.ignoreEssentialNpcs);
			ImGui::SameLine();
			LargeButtonToggle("������������ �� ���������� NPCs", Settings::targetting.ignoreNonHostileNpcs);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
			ImGui::SliderFloat("###TargetLockingFOV", &Settings::targetting.lockingFov, 5.0f, 40.0f, "��������� FOV: %.2f");

			ImGui::SameLine();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::ColorEdit3("###TargetLockingColor", Settings::targetting.lockedColor);
			Utils::ValidateRgb(Settings::esp.players.unknownColor);

			ButtonToggle("�������������� ����������###TargetLockingRetargeting", Settings::targetting.retargeting);

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				const auto sliderText = format(FMT_STRING("�����������: {0:d} ({1:d} ms)"), Settings::targetting.cooldown, Settings::targetting.cooldown * 16);
				ImGui::SliderInt("###TargetLockingCooldown", &Settings::targetting.cooldown, 0, 120, sliderText.c_str());
			}

			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
				const auto sliderText = format(FMT_STRING("��������� ���� (Min): {0:d} ({1:d} ms)"), Settings::targetting.sendDamageMin, Settings::targetting.sendDamageMin * 16);
				if (ImGui::SliderInt("###SendDamageMin", &Settings::targetting.sendDamageMin, 1, 60, sliderText.c_str()))
				{
					if (Settings::targetting.sendDamageMax < Settings::targetting.sendDamageMin)
						Settings::targetting.sendDamageMax = Settings::targetting.sendDamageMin;
				}
			}

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				const auto sliderText = format(FMT_STRING("��������� ���� (Max): {0:d} ({1:d} ms)"), Settings::targetting.sendDamageMax, Settings::targetting.sendDamageMax * 16);
				if (ImGui::SliderInt("###SendDamageMax", &Settings::targetting.sendDamageMax, 1, 60, sliderText.c_str()))
				{
					if (Settings::targetting.sendDamageMax < Settings::targetting.sendDamageMin)
						Settings::targetting.sendDamageMin = Settings::targetting.sendDamageMax;
				}
			}

			{
				auto favoritedWeapons = ErectusMemory::GetFavoritedWeapons();
				const auto favoritedWeaponsPreview = favoritedWeapons.at(Settings::targetting.favoriteIndex + 1);

				ImGui::SetNextItemWidth(-FLT_MIN);
				if (ImGui::BeginCombo("###BeginTempCombo", favoritedWeaponsPreview.c_str()))
				{
					for (const auto& [itemIndex, itemName] : favoritedWeapons)
					{
						if (ImGui::Selectable(itemName.c_str()))
						{
							if (itemIndex)
								Settings::targetting.favoriteIndex = itemIndex - 1;
							else
								Settings::targetting.favoriteIndex = 12;
						}
					}
					ImGui::EndCombo();
				}
			}
		}

		if (ImGui::CollapsingHeader("������� ���"))
		{
			LargeButtonToggle("�������� ������� ��� (Keybind: U)", Settings::melee.enabled);

			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
				const auto sliderText = format(FMT_STRING("������� ��� �������� (Min): {0:d} ({1:d} ms)"), Settings::melee.speedMin, Settings::melee.speedMin * 16);
				if (ImGui::SliderInt("###MeleeSpeedMin", &Settings::melee.speedMin, 1, 60, sliderText.c_str()))
				{
					if (Settings::melee.speedMax < Settings::melee.speedMin)
						Settings::melee.speedMax = Settings::melee.speedMin;
				}
			}

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				const auto sliderText = format(FMT_STRING("������� ��� �������� (Max): {0:d} ({1:d} ms)"), Settings::melee.speedMax, Settings::melee.speedMax * 16);
				if (ImGui::SliderInt("###MeleeSpeedMax", &Settings::melee.speedMax, 1, 60, sliderText.c_str()))
				{
					if (Settings::melee.speedMax < Settings::melee.speedMin)
						Settings::melee.speedMin = Settings::melee.speedMax;
				}
			}
		}

		if (ImGui::CollapsingHeader("������������"))
		{
			LargeButtonToggle("������������ NPCs (CTRL+N)", Settings::opk.enabled);
		}

		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabPlayer()
{
	if (ImGui::BeginTabItem("�����###PlayerTab"))
	{
		if (ImGui::CollapsingHeader("��������� ������"))
		{
			ImGui::Columns(2, nullptr, false);

			LargeButtonToggle("����� ������� (CTRL+L)##LocalPlayerPositionSpoofingEnabled", Settings::localPlayer.positionSpoofingEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerPositionSpoofingHeight", &Settings::localPlayer.positionSpoofingHeight, -524287, 524287, "���������� ������ �������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("����� (CTRL+Y)###NoclipEnabled", Settings::localPlayer.noclipEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderFloat("###NoclipSpeed", &Settings::localPlayer.noclipSpeed, 0.0f, 1.0f, "��������: %.5f");
			ImGui::NextColumn();

			LargeButtonToggle("������ �������", Settings::localPlayer.clientState);
			ImGui::NextColumn();
			LargeButtonToggle("�������������� ������ �������", Settings::localPlayer.automaticClientState);
			ImGui::NextColumn();

			LargeButtonToggle("���������� ���� ��������###LocalPlayerFreezeApEnabled", Settings::localPlayer.freezeApEnabled);
			ImGui::NextColumn();
			ImGui::NextColumn();

			LargeButtonToggle("���� ��������###LocalPlayerAPEnabled", Settings::localPlayer.actionPointsEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerAP", &Settings::localPlayer.actionPoints, 0, 99999, "���� ��������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("����###LocalPlayerStrengthEnabled", Settings::localPlayer.strengthEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerStrength", &Settings::localPlayer.strength, 0, 99999, "����: %d");
			ImGui::NextColumn();

			LargeButtonToggle("����������###LocalPlayerPerceptionEnabled", Settings::localPlayer.perceptionEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerPerception", &Settings::localPlayer.perception, 0, 99999, "����������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("������������###LocalPlayerEnduranceEnabled", Settings::localPlayer.enduranceEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerEndurance", &Settings::localPlayer.endurance, 0, 99999, "������������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("�������###LocalPlayerCharismaEnabled", Settings::localPlayer.charismaEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerCharisma", &Settings::localPlayer.charisma, 0, 99999, "�������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("��������###LocalPlayerIntelligenceEnabled", Settings::localPlayer.intelligenceEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerIntelligence", &Settings::localPlayer.intelligence, 0, 99999, "��������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("��������###LocalPlayerAgilityEnabled", Settings::localPlayer.agilityEnabled);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerAgility", &Settings::localPlayer.agility, 0, 99999, "��������: %d");
			ImGui::NextColumn();

			LargeButtonToggle("�����###LocalPlayerLuckEnabled", Settings::localPlayer.luckEnabled); ImGui::SameLine(235.0f);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderInt("###LocalPlayerLuck", &Settings::localPlayer.luck, 0, 99999, "�����: %d");

			ImGui::Columns();
		}

		if (ImGui::CollapsingHeader("��������� ���������"))
		{
			LargeButtonToggle("�������� �������������� �������� ���� ���������###ChargenEditingEnabled", Settings::characterEditor.enabled);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderFloat("###ChargenThin", &Settings::characterEditor.thin, 0.0f, 1.0f, "������� ��� ��������� (����): %f");

			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderFloat("###ChargenMuscular", &Settings::characterEditor.muscular, 0.0f, 1.0f, "������� ��� ��������� (����): %f");

			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::SliderFloat("###ChargenLarge", &Settings::characterEditor.large, 0.0f, 1.0f, "������� ��� ��������� (������): %f");
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabUtilities()
{
	if (ImGui::BeginTabItem("�����������###UtilityTab"))
	{
		if (ImGui::CollapsingHeader("�����������"))
		{
			LargeButtonToggle("����� ������� ESP", Settings::utilities.debugEsp);

			{
				if (Settings::utilities.ptrFormId)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

					if (ImGui::Button("�������� �����###GetPointerEnabled", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
						getPtrResult = ErectusMemory::GetPtr(Settings::utilities.ptrFormId);

					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::Button("�������� �����###GetPointerDisabled", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f));
					ImGui::PopItemFlag();

					ImGui::PopStyleColor(3);
				}
			}

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
				if (ImGui::InputScalar("###PtrFormIdText", ImGuiDataType_U32, &Settings::utilities.ptrFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal))
					getPtrResult = 0;
			}

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				auto inputText = format(FMT_STRING("{:16X}"), getPtrResult);
				ImGui::InputText("###PtrPointerText", &inputText, ImGuiInputTextFlags_ReadOnly);
			}

			{
				if (Settings::utilities.addressFormId)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

					if (ImGui::Button("�������� �����###GetAddressEnabled", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
						getAddressResult = ErectusMemory::GetAddress(Settings::utilities.addressFormId);

					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::Button("�������� �����###GetAddressDisabled", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f));
					ImGui::PopItemFlag();

					ImGui::PopStyleColor(3);
				}
			}

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);

				if (ImGui::InputScalar("###AddressFormIdText", ImGuiDataType_U32, &Settings::utilities.addressFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal))
					getAddressResult = 0;
			}

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(-FLT_MIN);

				auto inputText = format(FMT_STRING("{:16X}"), getAddressResult);
				ImGui::InputText("###AddressPointerText", &inputText, ImGuiInputTextFlags_ReadOnly);
			}
		}

		if (ImGui::CollapsingHeader("�������� ������"))
		{
			ButtonToggle("�������� FormId###SwapperSourceFormIdToggle", swapperSourceToggle);

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputScalar("###SwapperSourceFormIdText", ImGuiDataType_U32, &Settings::swapper.sourceFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			ButtonToggle("���� FormId###SwapperDestinationFormIdToggle", swapperDestinationToggle);

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputScalar("###SwapperDestinationFormIdText", ImGuiDataType_U32, &Settings::swapper.destinationFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			if (swapperSourceToggle && Settings::swapper.sourceFormId && swapperDestinationToggle && Settings::swapper.destinationFormId)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("������������� ������ (������������ ����)###EditReferenceEnabled", ImVec2(-FLT_MIN, 0.0f)))
				{
					if (ErectusMemory::ReferenceSwap(Settings::swapper.sourceFormId, Settings::swapper.destinationFormId))
					{
						Settings::swapper.destinationFormId = Settings::swapper.sourceFormId;
						swapperSourceToggle = false;
						swapperDestinationToggle = false;
					}
				}

				ImGui::PopStyleColor(3);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::Button("������������� ������ (������������ ����)###EditReferenceDisabled", ImVec2(-FLT_MIN, 0.0f));
				ImGui::PopItemFlag();

				ImGui::PopStyleColor(3);
			}
		}

		if (ImGui::CollapsingHeader("�������� ���������"))
		{
			SmallButtonToggle("��������###TransferSourceFormIdToggle", transferSourceToggle);

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 3);
				ImGui::InputScalar("###TransferSourceFormIdText", ImGuiDataType_U32, &Settings::customTransferSettings.sourceFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			ImGui::SameLine();

			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("�����###TransferSourceLocalPlayer", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
					Settings::customTransferSettings.sourceFormId = Game::GetLocalPlayer().formId;

				ImGui::SameLine();

				if (ImGui::Button("������###TransferSourceSTASH", ImVec2(-FLT_MIN, 0.0f)))
					Settings::customTransferSettings.sourceFormId = Game::GetLocalPlayer().GetStashFormId();

				ImGui::PopStyleColor(3);
			}

			SmallButtonToggle("����###TransferDestinationFormIdToggle", transferDestinationToggle);

			ImGui::SameLine();

			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 3);
				ImGui::InputScalar("###TransferDestinationFormIdText", ImGuiDataType_U32, &Settings::customTransferSettings.destinationFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
				ImGui::SameLine();
				if (ImGui::Button("�����###TransferDestinationLocalPlayer", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
					Settings::customTransferSettings.destinationFormId = Game::GetLocalPlayer().formId;

				ImGui::SameLine();
				if (ImGui::Button("������###TransferDestinationSTASH", ImVec2(-FLT_MIN, 0.0f)))
					Settings::customTransferSettings.destinationFormId = Game::GetLocalPlayer().GetStashFormId();
				ImGui::PopStyleColor(3);
			}

			auto allowTransfer = false;

			if (transferSourceToggle && Settings::customTransferSettings.sourceFormId && transferDestinationToggle && Settings::customTransferSettings.destinationFormId)
			{
				if (Settings::customTransferSettings.useWhitelist)
					allowTransfer = ErectusMemory::CheckItemTransferList();
				else
					allowTransfer = true;
			}

			if (allowTransfer)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("�������� ��������###TransferItemsEnabled", ImVec2(-FLT_MIN, 0.0f)))
					ErectusMemory::TransferItems(Settings::customTransferSettings.sourceFormId, Settings::customTransferSettings.destinationFormId);

				ImGui::PopStyleColor(3);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::Button("�������� ��������###TransferItemsDisabled", ImVec2(-FLT_MIN, 0.0f));
				ImGui::PopItemFlag();

				ImGui::PopStyleColor(3);
			}

			LargeButtonToggle("������������ ����� ������", Settings::customTransferSettings.useWhitelist);
			LargeButtonToggle("������������ ������ ������", Settings::customTransferSettings.useBlacklist);

			if (ImGui::CollapsingHeader("��������� ������ ������"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = format(FMT_STRING("���� ��������: {0:d}"), i);
					ButtonToggle(toggleLabel.c_str(), Settings::customTransferSettings.whitelisted[i]);

					ImGui::SameLine();
					ImGui::SetNextItemWidth(-FLT_MIN);

					auto inputLabel = format(FMT_STRING("###ItemTransferWhitelist{:d}"), i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::customTransferSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}

			if (ImGui::CollapsingHeader("��������� ������� ������"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = format(FMT_STRING("���� ��������: {0:d}"), i);
					ButtonToggle(toggleLabel.c_str(), Settings::customTransferSettings.blacklisted[i]);

					ImGui::SameLine();
					ImGui::SetNextItemWidth(-FLT_MIN);

					auto inputLabel = format(FMT_STRING("###ItemTransferBlacklist{:d}"), i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::customTransferSettings.blacklist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("������� ����"))
		{
			ButtonToggle("������������� �������� ����", Settings::customNukeCodeSettings.automaticNukeCodes);

			ImGui::SameLine();

			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("�������� ������� ����", ImVec2(-FLT_MIN, 0.0f)))
					ErectusMemory::UpdateNukeCodes();

				ImGui::PopStyleColor(3);
			}

			auto text = format("{} - �����", fmt::join(ErectusMemory::alphaCode, " "));
			ImGui::Text(text.c_str());

			text = format("{} - �����", fmt::join(ErectusMemory::bravoCode, " "));
			ImGui::Text(text.c_str());

			text = format("{} - �����", fmt::join(ErectusMemory::charlieCode, " "));
			ImGui::Text(text.c_str());
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabTeleporter()
{
	if (ImGui::BeginTabItem("��������###TeleporterTab"))
	{
		for (auto i = 0; i < 16; i++)
		{
			auto teleportHeaderText = format(FMT_STRING("���� ���������: {0:d}"), i);
			if (ImGui::CollapsingHeader(teleportHeaderText.c_str()))
			{
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 4);
					auto inputLabel = format(FMT_STRING("###TeleportDestinationX{:d}"), i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].position.x);
				}

				ImGui::SameLine();

				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 3);
					auto inputLabel = format(FMT_STRING("###TeleportDestinationY{:d}"), i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].position.y);
				}

				ImGui::SameLine();

				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
					auto inputLabel = format(FMT_STRING("###TeleportDestinationZ{:d}"), i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].position.z);
				}

				ImGui::SameLine();

				{
					ImGui::SetNextItemWidth(-FLT_MIN);
					auto inputLabel = format(FMT_STRING("###TeleportDestinationW{:d}"), i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].rotation.z);
				}

				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 4);
					auto inputLabel = format(FMT_STRING("###TeleportCellFormId{:d}"), i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::teleporter.entries[i].cellFormId,
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}

				ImGui::SameLine();

				{
					auto buttonLabel = format(FMT_STRING("���������� �������###TeleportDestination{:d}"), i);
					if (!Settings::teleporter.entries[i].disableSaving)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

						if (ImGui::Button(buttonLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x / 3, 0.0f)))
							ErectusMemory::SaveTeleportPosition(i);

						ImGui::PopStyleColor(3);
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::Button(buttonLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x / 3, 0.0f));
						ImGui::PopItemFlag();

						ImGui::PopStyleColor(3);
					}
				}

				ImGui::SameLine();

				{
					auto buttonLabel = format(FMT_STRING("�������������###DisableSaving{:d}"), i);
					ButtonToggle(buttonLabel.c_str(), Settings::teleporter.entries[i].disableSaving);
				}

				ImGui::SameLine();

				if (Settings::teleporter.entries[i].cellFormId)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

					auto buttonLabel = format(FMT_STRING("��������###TeleportRequestEnabled{:d}"), i);
					if (ImGui::Button(buttonLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
						ErectusMemory::RequestTeleport(i);
					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

					auto buttonLabel = format(FMT_STRING("��������###TeleportRequestDisabled{:d}"), i);
					ImGui::Button(buttonLabel.c_str(), ImVec2(-FLT_MIN, 0.0f));
					ImGui::PopStyleColor(3);
					ImGui::PopItemFlag();
				}
			}
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabBitMsgWriter()
{
	if (ImGui::BeginTabItem("BitMsgWriter###BitMsgWriterTab"))
	{
		LargeButtonToggle("�������� �������� ���������", Settings::msgWriter.enabled);

		ImGui::EndTabItem();
	}
}

void Gui::SettingsMenu()
{
	ImGui::SetWindowSize(ImVec2(480, 720));

	if (ImGui::BeginTabBar("###OverlayMenuTabBar", ImGuiTabBarFlags_None))
	{
		OverlayMenuTabEsp();
		OverlayMenuTabLooter();
		OverlayMenuTabCombat();
		OverlayMenuTabPlayer();
		OverlayMenuTabUtilities();
		OverlayMenuTabTeleporter();
		OverlayMenuTabBitMsgWriter();

		ImGui::EndTabBar();
	}
}
