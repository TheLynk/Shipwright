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
        struct ExpressionRow {
            std::shared_ptr<LogicExpression> Expression;
            std::vector<ExpressionRow> Children;
            std::optional<LogicExpression::ValueVariant> ChildDay;
            std::optional<LogicExpression::ValueVariant> ChildNight;
            std::optional<LogicExpression::ValueVariant> AdultDay;
            std::optional<LogicExpression::ValueVariant> AdultNight;
            bool Expanded;
        };

        std::string RegionName;
        ExpressionRow Root;
        bool CombineAll = false;
        bool CombineChild = false;
        bool CombineAdult = false;
    };

    std::string CheckName;
    std::vector<Region> Regions;
};

static std::vector<LogicTrackerCheck> checks;

LogicTrackerCheck::Region::ExpressionRow CreateExpressionRows(const std::shared_ptr<LogicExpression>& expression) {
    LogicTrackerCheck::Region::ExpressionRow row;
    row.Expression = expression;
    row.Expanded = false;

    const auto& children = expression->GetChildren();
    row.Children.reserve(children.size());
    for (const auto& child : children) {
        row.Children.emplace_back(CreateExpressionRows(child));
    }

    return row;
}

enum class AgeTime {
    ChildDay,
    ChildNight,
    AdultDay,
    AdultNight
};


static void PopulateExpressionValues(LogicTrackerCheck::Region::ExpressionRow& row, const ExpressionEvaluation& eval, AgeTime ageTime) {
    if (ageTime == AgeTime::ChildDay) {
        row.ChildDay = eval.Result;
    } else if (ageTime == AgeTime::ChildNight) {
        row.ChildNight = eval.Result;
    } else if (ageTime == AgeTime::AdultDay) {
        row.AdultDay = eval.Result;
    } else if (ageTime == AgeTime::AdultNight) {
        row.AdultNight = eval.Result;
    }

    for (auto& rowChild : row.Children) {
        for (const auto& evalChild : eval.Children) {
            if (rowChild.Expression == evalChild.Expression) {
                PopulateExpressionValues(rowChild, evalChild, ageTime);
                break;
            }
        }
    }
}

static std::tuple<bool, bool, bool> CalculateCombines(const LogicTrackerCheck::Region::ExpressionRow& row) {
    bool combineChild = row.ChildDay == row.ChildNight;
    bool combineAdult = row.AdultDay == row.AdultNight;
    bool combineAll = combineChild && combineAdult && row.ChildDay == row.AdultDay;
    for (const auto& child : row.Children) {
        auto [childCombineAll, childCombineChild, childCombineAdult] = CalculateCombines(child);
        combineAll &= childCombineAll;
        combineChild &= childCombineChild;
        combineAdult &= childCombineAdult;
    }
    return {combineAll, combineChild, combineAdult};
}

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
                regionAgeTime.Root = CreateExpressionRows(LogicExpression::Parse(locationAccess.GetConditionStr()));

                if (region.childDay) {
                    logic->IsChild = true;
                    logic->AtDay = true;

                    const auto& eval = EvaluateExpression(regionAgeTime.Root.Expression);
                    PopulateExpressionValues(regionAgeTime.Root, eval, AgeTime::ChildDay);

                    logic->IsChild = false;
                    logic->AtDay = false;
                }
                if (region.childNight) {
                    logic->IsChild = true;
                    logic->AtNight = true;

                    const auto& eval = EvaluateExpression(regionAgeTime.Root.Expression);
                    PopulateExpressionValues(regionAgeTime.Root, eval, AgeTime::ChildNight);

                    logic->IsChild = false;
                    logic->AtNight = false;
                }
                if (region.adultDay) {
                    logic->IsAdult = true;
                    logic->AtDay = true;

                    const auto& eval = EvaluateExpression(regionAgeTime.Root.Expression);
                    PopulateExpressionValues(regionAgeTime.Root, eval, AgeTime::AdultDay);

                    logic->IsAdult = false;
                    logic->AtDay = false;
                }
                if (region.adultNight) {
                    logic->IsAdult = true;
                    logic->AtNight = true;

                    const auto& eval = EvaluateExpression(regionAgeTime.Root.Expression);
                    PopulateExpressionValues(regionAgeTime.Root, eval, AgeTime::AdultNight);

                    logic->IsAdult = false;
                    logic->AtNight = false;
                }

                auto [combineAll, combineChild, combineAdult] = CalculateCombines(regionAgeTime.Root);
                regionAgeTime.CombineAll = combineAll;
                regionAgeTime.CombineChild = combineChild;
                regionAgeTime.CombineAdult = combineAdult;

                logicTrackerCheck.Regions.emplace_back(std::move(regionAgeTime));
            }
        }
    }

    checks.emplace_back(std::move(logicTrackerCheck));
    
    auto window = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Logic Tracker");
    window->Show();
    ImGui::SetWindowFocus(window->GetName().c_str());
}

static std::string ToString(const std::optional<LogicExpression::ValueVariant>& value) {
    if (!value.has_value()) {
        return "";
    }
    return ToString(value.value());
}

static void DrawExpressionRow(const LogicTrackerCheck::Region& region, LogicTrackerCheck::Region::ExpressionRow& row,
                              int level) {
    ImGui::TableNextRow();
    if (level > 0) {
        ImGui::Indent(10.0f);
    }

    ImGui::TableNextColumn();
    ImGui::PushID(&row);
    if (!row.Children.empty()) {
        if (ImGui::Button(std::to_string(level).c_str())) {
            row.Expanded = !row.Expanded;
        }
    } else {
        ImGui::Text("%d", level);
    }
    ImGui::PopID();

    ImGui::TableNextColumn();
    ImGui::TextWrapped("%s", row.Expression->ToString().c_str());

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(ToString(row.ChildDay).c_str());

    if (!region.CombineAll) {
        if (!region.CombineChild) {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(ToString(row.ChildNight).c_str());
        }

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(ToString(row.AdultDay).c_str());

        if (!region.CombineAdult) {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(ToString(row.AdultNight).c_str());
        }
    }

    if (row.Expanded) {
        for (auto& child : row.Children) {
            DrawExpressionRow(region, child, level + 1);
        }
    }

    if (level > 0) {
        ImGui::Unindent(10.0f);
    }
}

static void DrawCheckRegion(LogicTrackerCheck::Region& region) {
    int columnCount = 3;
    if (!region.CombineAll) {
        if (!region.CombineChild) {
            columnCount += 1;
        }
        columnCount += 1;
        if (!region.CombineAdult) {
            columnCount += 1;
        }
    }

    if (ImGui::BeginTable(region.RegionName.c_str(), columnCount,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Expression", ImGuiTableColumnFlags_WidthStretch);
        if (region.CombineAll) {
            ImGui::TableSetupColumn("All", ImGuiTableColumnFlags_WidthFixed);
        } else {
            if (region.CombineChild) {
                ImGui::TableSetupColumn("Child", ImGuiTableColumnFlags_WidthFixed);
            } else {
                ImGui::TableSetupColumn("Child Day", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Child Night", ImGuiTableColumnFlags_WidthFixed);
            }
            if (region.CombineAdult) {
                ImGui::TableSetupColumn("Adult", ImGuiTableColumnFlags_WidthFixed);
            } else {
                ImGui::TableSetupColumn("Adult Day", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Adult Night", ImGuiTableColumnFlags_WidthFixed);
            }
        }
        ImGui::TableHeadersRow();
    }

    DrawExpressionRow(region, region.Root, 0);

    ImGui::EndTable();
}

static void DrawCheck(LogicTrackerCheck& check) {
    ImGui::SeparatorText(("Check: " + check.CheckName).c_str());
    if (check.Regions.empty()) {
        ImGui::Text("No regions found for this check.");
        return;
    }
    for (auto& region : check.Regions) {
        DrawCheckRegion(region);
    }
}

void LogicTrackerWindow::DrawElement() {
    for (auto& check : checks) {
        DrawCheck(check);
    }
}

void LogicTrackerWindow::InitElement() {
    return;
}

void LogicTrackerWindow::UpdateElement() {
    return;
}
