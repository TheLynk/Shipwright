#include "randomizer_logic_tracker.h"

#include "location_access.h"
#include "logic_expression.h"

#include <src/Context.h>

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
uint64_t GetUnixTimestamp();
}

struct LogicTrackerCheck {
    struct Region {
        std::string RegionName;
        std::unique_ptr<ExpressionEvaluation> ChildDay;
        std::unique_ptr<ExpressionEvaluation> ChildNight;
        std::unique_ptr<ExpressionEvaluation> AdultDay;
        std::unique_ptr<ExpressionEvaluation> AdultNight;
    };

    std::string CheckName;
    std::vector<Region> Regions;
};

static std::vector<LogicTrackerCheck> checks;

void LogicTrackerWindow::ShowRandomizerCheck(RandomizerCheck check) {
    checks.clear();

    const auto& location = Rando::StaticData::GetLocation(check);
    
    LogicTrackerCheck logicTrackerCheck;
    logicTrackerCheck.CheckName = location->GetName();

    for (const auto& region : areaTable) {
        for (const auto& locationAccess : region.locations) {
            if (locationAccess.GetLocation() == check) {
                LogicTrackerCheck::Region regionAgeTime;
                regionAgeTime.RegionName = region.regionName;

                if (region.childDay) {
                    logic->IsChild = true;
                    logic->AtDay = true;

                    regionAgeTime.ChildDay =
                        std::make_unique<ExpressionEvaluation>(EvaluateExpression(locationAccess.GetConditionStr()));

                    logic->IsChild = false;
                    logic->AtDay = false;
                }
                if (region.childNight) {
                    logic->IsChild = true;
                    logic->AtNight = true;

                    regionAgeTime.ChildNight =
                        std::make_unique<ExpressionEvaluation>(EvaluateExpression(locationAccess.GetConditionStr()));

                    logic->IsChild = false;
                    logic->AtNight = false;
                }
                if (region.adultDay) {
                    logic->IsAdult = true;
                    logic->AtDay = true;

                    regionAgeTime.AdultDay =
                        std::make_unique<ExpressionEvaluation>(EvaluateExpression(locationAccess.GetConditionStr()));

                    logic->IsAdult = false;
                    logic->AtDay = false;
                }
                if (region.adultNight) {
                    logic->IsAdult = true;
                    logic->AtNight = true;

                    regionAgeTime.AdultNight =
                        std::make_unique<ExpressionEvaluation>(EvaluateExpression(locationAccess.GetConditionStr()));

                    logic->IsAdult = false;
                    logic->AtNight = false;
                }

                logicTrackerCheck.Regions.emplace_back(std::move(regionAgeTime));
            }
        }
    }

    checks.emplace_back(std::move(logicTrackerCheck));
    
    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Logic Tracker")->Show();
}

std::string TruncateText(const std::string& text, float maxWidth) {
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    if (textSize.x <= maxWidth)
        return text;

    std::string truncated = text;
    // Remove characters until the text fits with the ellipsis appended
    while (!truncated.empty() && ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth) {
        truncated.pop_back();
    }
    return truncated + "...";
}

static void DrawExpression(const ExpressionEvaluation& expression) {
    ImGuiTreeNodeFlags treeNodeFlags =
        expression.Children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_DefaultOpen;

    float availableWidth = ImGui::GetContentRegionAvail().x - ImGui::GetTreeNodeToLabelSpacing();
    auto text = TruncateText(ToString(expression.Result) + " = " + expression.Expression, availableWidth);

    if (ImGui::TreeNodeEx(&expression, treeNodeFlags, "%s", text.c_str())) {
        if (!expression.Children.empty()) {
            for (const auto& child : expression.Children) {
                DrawExpression(child);
            }
        }
        ImGui::TreePop();
    }
}

static void DrawCheckRegion(const LogicTrackerCheck::Region& region) {
    if (ImGui::TreeNodeEx(region.RegionName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        if (region.ChildDay != nullptr) {
            bool resultString = LogicExpression::GetValue<bool>(region.ChildDay->Result);
            if (ImGui::TreeNodeEx(region.ChildDay.get(), ImGuiTreeNodeFlags_DefaultOpen, "Child-Day Result: %s",
                                  resultString ? "true" : "false")) {
                DrawExpression(*region.ChildDay);
                ImGui::TreePop();
            }
        } else {
            if (ImGui::TreeNodeEx("Child-Day Inaccessible", ImGuiTreeNodeFlags_Leaf)) {
                ImGui::TreePop();
            }
        }

        if (region.ChildNight != nullptr) {
            bool resultString = LogicExpression::GetValue<bool>(region.ChildNight->Result);
            if (ImGui::TreeNodeEx(region.ChildNight.get(), ImGuiTreeNodeFlags_DefaultOpen, "Child-Night Result: %s",
                                  resultString ? "true" : "false")) {
                DrawExpression(*region.ChildNight);
                ImGui::TreePop();
            }
        } else {
            if (ImGui::TreeNodeEx("Child-Night Inaccessible", ImGuiTreeNodeFlags_Leaf)) {
                ImGui::TreePop();
            }
        }

        if (region.AdultDay != nullptr) {
            bool resultString = LogicExpression::GetValue<bool>(region.AdultDay->Result);
            if (ImGui::TreeNodeEx(region.AdultDay.get(), ImGuiTreeNodeFlags_DefaultOpen, "Adult-Day Result: %s",
                                  resultString ? "true" : "false")) {
                DrawExpression(*region.AdultDay);
                ImGui::TreePop();
            }
        } else {
            if (ImGui::TreeNodeEx("Adult-Day Inaccessible", ImGuiTreeNodeFlags_Leaf)) {
                ImGui::TreePop();
            }
        }

        if (region.AdultNight != nullptr) {
            bool resultString = LogicExpression::GetValue<bool>(region.AdultNight->Result);
            if (ImGui::TreeNodeEx(region.AdultNight.get(), ImGuiTreeNodeFlags_DefaultOpen, "Adult-Night Result: %s",
                                  resultString ? "true" : "false")) {
                DrawExpression(*region.AdultNight);
                ImGui::TreePop();
            }
        } else {
            if (ImGui::TreeNodeEx("Adult-Night Inaccessible", ImGuiTreeNodeFlags_Leaf)) {
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

static void DrawCheck(const LogicTrackerCheck& check) {
    ImGui::SeparatorText(("Check: " + check.CheckName).c_str());
    if (check.Regions.empty()) {
        ImGui::Text("No regions found for this check.");
        return;
    }
    for (const auto& region : check.Regions) {
        DrawCheckRegion(region);
    }
}

void LogicTrackerWindow::DrawElement() {
    for (const auto& check : checks) {
        DrawCheck(check);
    }
}

void LogicTrackerWindow::InitElement() {
    return;
}

void LogicTrackerWindow::UpdateElement() {
    return;
}
