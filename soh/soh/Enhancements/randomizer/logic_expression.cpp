#include "logic_expression.h"
#include "randomizerTypes.h"
#include "location_access.h"
#include "dungeon.h"
#include "variables.h"

#include <sstream>
#include <stack>
#include <unordered_map>
#include <variant>

extern SaveContext gSaveContext;

std::string LogicExpression::Impl::GetTypeString() const {
    switch (type) {
        case Type::Value:
            switch (valueType) {
                case ValueType::Boolean: return "Boolean";
                case ValueType::Number: return "Number";
                case ValueType::Enum: return "Enum";
                case ValueType::Identifier: return "Variable";
                default: return "Unknown Value";
            }
        case Type::FunctionCall: return "Function: " + functionName;
        case Type::Not: return "Not";
        case Type::And: return "And";
        case Type::Or: return "Or";
        case Type::Comparison: return "Comparison: " + operation;
        case Type::Add: return "Add";
        case Type::Subtract: return "Subtract";
        case Type::Multiply: return "Multiply";
        case Type::Divide: return "Divide";
        case Type::Ternary: return "Ternary";
        default: return "Unknown";
    }
}

#pragma region Tokenizer

enum class LETokenType { Identifier, Number, Boolean, EnumConstant, ParenOpen, ParenClose, Comma, Operator, End };

struct Token {
    LETokenType Type;
    std::string Text;
    size_t StartIndex;
    size_t EndIndex;
};

static std::string GetCharacterErrorContext(const std::string& input, size_t pos, size_t contextLen = 10) {
    size_t start = (pos >= contextLen) ? pos - contextLen : 0;
    size_t end = std::min(input.size(), pos + contextLen + 1);
    std::string contextLine = input.substr(start, end - start);
    std::string pointerLine;
    pointerLine.append(pos - start, ' ');
    pointerLine.push_back('^');
    return "\n" + contextLine + "\n" + pointerLine;
}

static bool IsEnumConstant(const std::string& s) {
    if (LogicExpression::Impl::enumMap.empty()) {
        LogicExpression::Impl::PopulateEnumMap();
    }
    return LogicExpression::Impl::enumMap.find(s) != LogicExpression::Impl::enumMap.end();
}

static bool IsIdentifierChar(char c) {
    return std::isalnum(c) || c == '_' || c == '.' || c == ':';
}

static std::vector<Token> Tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t len = input.length();

    while (i < len) {
        if (std::isspace(input[i])) {
            ++i;
            continue;
        }

        size_t start = i;
        if (std::isalpha(input[i])) {
            while (i < len && IsIdentifierChar(input[i]))
                ++i;
            std::string id = input.substr(start, i - start);
            if (id == "true" || id == "false")
                tokens.emplace_back(Token{ LETokenType::Boolean, id, start, i });
            else if (IsEnumConstant(id))
                tokens.emplace_back(Token{ LETokenType::EnumConstant, id, start, i });
            else
                tokens.emplace_back(Token{ LETokenType::Identifier, id, start, i });
        } else if (std::isdigit(input[i])) {
            while (i < len && std::isdigit(input[i]))
                ++i;
            tokens.emplace_back(Token{ LETokenType::Number, input.substr(start, i - start), start, i });
        } else if (input[i] == '(') {
            tokens.emplace_back(Token{ LETokenType::ParenOpen, "(", start, start + 1 });
            ++i;
        } else if (input[i] == ')') {
            tokens.emplace_back(Token{ LETokenType::ParenClose, ")", start, start + 1 });
            ++i;
        } else if (input[i] == ',') {
            tokens.emplace_back(Token{ LETokenType::Comma, ",", start, start + 1 });
            ++i;
        } else if (input[i] == '?') {
            tokens.emplace_back(Token{ LETokenType::Operator, "?", start, start + 1 });
            ++i;
        } else if (input[i] == ':') {
            tokens.emplace_back(Token{ LETokenType::Operator, ":", start, start + 1 });
            ++i;
        } else if (i + 1 < len &&
                   ((input[i] == '!' && input[i + 1] == '=') || (input[i] == '&' && input[i + 1] == '&') ||
                    (input[i] == '|' && input[i + 1] == '|') || (input[i] == '=' && input[i + 1] == '=') ||
                    (input[i] == '>' && input[i + 1] == '=') || (input[i] == '<' && input[i + 1] == '='))) {
            tokens.emplace_back(Token{ LETokenType::Operator, input.substr(i, 2), start, start + 2 });
            i += 2;
        } else if (strchr("!<>=+-*/", input[i])) {
            tokens.emplace_back(Token{ LETokenType::Operator, std::string(1, input[i]), start, start + 1 });
            ++i;
        } else {
            throw std::runtime_error("Unknown character: '" + std::string(1, input[i]) + "' at position " +
                                     std::to_string(i) + GetCharacterErrorContext(input, i));
        }
    }
    tokens.emplace_back(Token{ LETokenType::End, "", i, i });
    return tokens;
}

#pragma endregion

#pragma region Parser

static std::string GetTokenErrorContext(const std::string& input, const Token& token, size_t contextLen = 10) {
    size_t start = (token.StartIndex >= contextLen) ? token.StartIndex - contextLen : 0;
    size_t end = std::min(input.size(), token.EndIndex + contextLen);
    std::string contextLine = input.substr(start, end - start);
    size_t pointerOffset = token.StartIndex - start;
    size_t pointerLength = (token.EndIndex > token.StartIndex) ? token.EndIndex - token.StartIndex : 1;
    std::string pointerLine(pointerOffset, ' ');
    pointerLine.append(std::string(pointerLength, '^'));
    return "\n" + contextLine + "\n" + pointerLine;
}

class Parser {
    const std::vector<Token>& tokens;
    const std::string& input;
    size_t pos = 0;

    const Token& Peek() const {
        return tokens[pos];
    }

    const Token& Next() {
        return tokens[pos++];
    }

    bool Match(const std::string& op) {
        if (Peek().Type == LETokenType::Operator && Peek().Text == op) {
            ++pos;
            return true;
        }
        return false;
    }

    std::shared_ptr<LogicExpression::Impl> ParsePrimary() {
        size_t initial_pos = pos;
        std::shared_ptr<LogicExpression::Impl> expr;

        if (Match("!")) {
            expr = std::make_shared<LogicExpression::Impl>();
            expr->type = LogicExpression::Impl::Type::Not;
            expr->children.emplace_back(ParsePrimary());
            expr->children.back()->parent = expr.get();
        } else if (Peek().Type == LETokenType::ParenOpen) {
            Next();
            expr = ParseExpression();
            if (Peek().Type != LETokenType::ParenClose) {
                throw std::runtime_error("Expected ')' at position " + std::to_string(Peek().StartIndex) +
                                         GetTokenErrorContext(input, Peek()));
            }
            Next();
        } else if (Peek().Type == LETokenType::Identifier || Peek().Type == LETokenType::Boolean ||
                   Peek().Type == LETokenType::Number || Peek().Type == LETokenType::EnumConstant) {
            Token token = Next();
            expr = std::make_shared<LogicExpression::Impl>();

            if (Peek().Type == LETokenType::ParenOpen && token.Type == LETokenType::Identifier) {
                --pos;
                std::string id = Next().Text;
                Next(); // consume '('
                expr->type = LogicExpression::Impl::Type::FunctionCall;
                expr->functionName = id;
                while (Peek().Type != LETokenType::ParenClose) {
                    if (!expr->children.empty()) {
                        if (Peek().Type != LETokenType::Comma) {
                            throw std::runtime_error("Expected ',' at position " + std::to_string(Peek().StartIndex) +
                                                     GetTokenErrorContext(input, Peek()));
                        }
                        Next(); // consume ','
                    }
                    expr->children.emplace_back(ParseExpression());
                    expr->children.back()->parent = expr.get();
                }
                Next(); // consume ')'
            } else {
                expr->type = LogicExpression::Impl::Type::Value;
                expr->value = token.Text;
                if (token.Type == LETokenType::Boolean)
                    expr->valueType = LogicExpression::Impl::ValueType::Boolean;
                else if (token.Type == LETokenType::Number)
                    expr->valueType = LogicExpression::Impl::ValueType::Number;
                else if (token.Type == LETokenType::EnumConstant)
                    expr->valueType = LogicExpression::Impl::ValueType::Enum;
                else
                    expr->valueType = LogicExpression::Impl::ValueType::Identifier;
            }
        }

        if (expr == nullptr) {
            throw std::runtime_error("Unexpected token: " + Peek().Text + GetTokenErrorContext(input, Peek()));
        }

        // Set startIndex/endIndex for error context
        expr->startIndex = tokens[initial_pos].StartIndex;
        expr->endIndex = tokens[pos - 1].EndIndex;
        return expr;
    }

    template <typename LowerFunc>
    std::shared_ptr<LogicExpression::Impl>
    ParseBinaryOp(size_t& pos, const std::vector<Token>& tokens, LowerFunc lowerFunc,
                  const std::vector<std::pair<std::string, LogicExpression::Impl::Type>>& operators) {

        size_t initial_pos = pos;
        auto left = (this->*lowerFunc)();

        while (pos < tokens.size() && tokens[pos].Type == LETokenType::Operator) {
            bool matched = false;
            for (const auto& [op, exprType] : operators) {
                if (tokens[pos].Text == op) {
                    ++pos; // consume operator
                    auto right = (this->*lowerFunc)();
                    auto expr = std::make_shared<LogicExpression::Impl>();
                    expr->type = exprType;

                    // For comparison operators, store the operation
                    if (exprType == LogicExpression::Impl::Type::Comparison) {
                        expr->operation = op;
                    }

                    expr->children.emplace_back(left);
                    expr->children.back()->parent = expr.get();
                    expr->children.emplace_back(right);
                    expr->children.back()->parent = expr.get();
                    expr->startIndex = tokens[initial_pos].StartIndex;
                    expr->endIndex = tokens[pos - 1].EndIndex;
                    left = expr;
                    matched = true;
                    break;
                }
            }
            if (!matched)
                break;
        }
        return left;
    }

    std::shared_ptr<LogicExpression::Impl> ParseMulDiv() {
        return ParseBinaryOp(
            pos, tokens, &Parser::ParsePrimary,
            { { "*", LogicExpression::Impl::Type::Multiply }, { "/", LogicExpression::Impl::Type::Divide } });
    }

    std::shared_ptr<LogicExpression::Impl> ParseAddSub() {
        return ParseBinaryOp(
            pos, tokens, &Parser::ParseMulDiv,
            { { "+", LogicExpression::Impl::Type::Add }, { "-", LogicExpression::Impl::Type::Subtract } });
    }

    std::shared_ptr<LogicExpression::Impl> ParseComparison() {
        return ParseBinaryOp(pos, tokens, &Parser::ParseAddSub,
                             { { "==", LogicExpression::Impl::Type::Comparison },
                               { "!=", LogicExpression::Impl::Type::Comparison },
                               { ">=", LogicExpression::Impl::Type::Comparison },
                               { "<=", LogicExpression::Impl::Type::Comparison },
                               { ">", LogicExpression::Impl::Type::Comparison },
                               { "<", LogicExpression::Impl::Type::Comparison } });
    }

    std::shared_ptr<LogicExpression::Impl> ParseAnd() {
        return ParseBinaryOp(pos, tokens, &Parser::ParseComparison,
                             { { "&&", LogicExpression::Impl::Type::And } });
    }

    std::shared_ptr<LogicExpression::Impl> ParseOr() {
        return ParseBinaryOp(pos, tokens, &Parser::ParseAnd,
                             { { "||", LogicExpression::Impl::Type::Or } });
    }

    std::shared_ptr<LogicExpression::Impl> ParseTernary() {
        size_t initial_pos = pos;
        auto cond = ParseOr();

        if (Peek().Type == LETokenType::Operator && Peek().Text == "?") {
            Next(); // consume '?'
            auto trueExpr = ParseTernary();

            if (!(Peek().Type == LETokenType::Operator && Peek().Text == ":")) {
                throw std::runtime_error("Expected ':' in ternary expression at position " +
                                         std::to_string(Peek().StartIndex) + GetTokenErrorContext(input, Peek()));
            }

            Next(); // consume ':'
            auto falseExpr = ParseTernary();

            auto expr = std::make_shared<LogicExpression::Impl>();
            expr->type = LogicExpression::Impl::Type::Ternary;

            expr->children.emplace_back(cond);
            expr->children.back()->parent = expr.get();
            expr->children.emplace_back(trueExpr);
            expr->children.back()->parent = expr.get();
            expr->children.emplace_back(falseExpr);
            expr->children.back()->parent = expr.get();

            expr->startIndex = tokens[initial_pos].StartIndex;
            expr->endIndex = tokens[pos - 1].EndIndex;
            return expr;
        }

        return cond;
    }

    std::shared_ptr<LogicExpression::Impl> ParseExpression() {
        return ParseTernary();
    }

  public:
    Parser(const std::string& input) : tokens(Tokenize(input)), input(input) {
    }

    std::shared_ptr<LogicExpression::Impl> Parse() {
        if (tokens.empty() || tokens[0].Type == LETokenType::End) {
            throw std::runtime_error("Empty input");
        }
        auto expr = ParseExpression();
        if (Peek().Type != LETokenType::End) {
            throw std::runtime_error("Unexpected token: " + Peek().Text + " at position " +
                                     std::to_string(Peek().StartIndex) + GetTokenErrorContext(input, Peek()));
        }
        expr->expressionString = std::make_unique<std::string>(input);
        return expr;
    };
};

#pragma endregion

std::unique_ptr<LogicExpression> LogicExpression::Parse(const std::string& exprStr) {
    Parser parser(exprStr);
    std::shared_ptr<LogicExpression::Impl> impl = parser.Parse();

    std::function<std::unique_ptr<LogicExpression>(const std::shared_ptr<LogicExpression::Impl>&)> populateChildren;
    populateChildren = [&](const std::shared_ptr<LogicExpression::Impl>& impl) {
        auto expr = std::make_unique<LogicExpression>();
        expr->impl = impl;
        for (const auto& child : impl->children) {
            expr->children.emplace_back(populateChildren(child));
        }
        return expr;
    };

    return populateChildren(impl);
}

const std::vector<std::unique_ptr<LogicExpression>>& LogicExpression::GetChildren() const {
    return children;
}

std::string LogicExpression::ToString() const {
    if (impl->expressionString != nullptr) {
        return *impl->expressionString;
    }

    const Impl* root = impl->parent;
    while (root->parent)
        root = root->parent;
    return (*root->expressionString).substr(impl->startIndex, impl->endIndex - impl->startIndex);
}

#pragma region Evaluator

std::string LogicExpression::Impl::GetExprErrorContext() const {
    size_t contextLen = 10;
    // Find root
    const Impl* root = this;
    while (root->parent)
        root = root->parent;
    const std::string& input = *root->expressionString;
    size_t exprStart = startIndex;
    size_t exprEnd = endIndex;
    size_t globalStart = (exprStart >= contextLen) ? exprStart - contextLen : 0;
    size_t globalEnd = std::min(input.size(), exprEnd + contextLen);
    std::string contextLine = input.substr(globalStart, globalEnd - globalStart);
    size_t pointerOffset = exprStart - globalStart;
    size_t pointerLength = (exprEnd > exprStart) ? exprEnd - exprStart : 1;
    std::string pointerLine(pointerOffset, ' ');
    pointerLine.append(std::string(pointerLength, '^'));
    return "\n" + contextLine + "\n" + pointerLine;
}

// Macro to register a function using its name.
// This macro simplifies the insertion of functions into the functionAdapters map by
// automatically converting the function pointer or lambda into a FunctionAdapter.
// Usage: REGISTER_FUNCTION(functionName)
#define REGISTER_FUNCTION(fn) \
    { #fn, LogicExpression::Impl::RegisterFunction(#fn, fn) }

#define REGISTER_LOGIC_FUNCTION(fn) \
    { #fn, LogicExpression::Impl::RegisterLogicFunction(#fn, &Rando::Logic::fn) }

#define REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(fn, ...) \
    { #fn, LogicExpression::Impl::RegisterLogicFunctionWithDefaults(#fn, &Rando::Logic::fn, std::make_tuple(__VA_ARGS__)) }

#define REGISTER_LOGIC_VARIABLE(var) \
    { #var, LogicExpression::Impl::RegisterLogicVariable(#var, &Rando::Logic::var) }

#pragma region Forwarding Functions
static uint8_t GetOption(const RandomizerSettingKey key) {
    return ctx->GetOption(key).Get();
}

static uint8_t GetTrickOption(const RandomizerTrick trick) {
    return ctx->GetTrickOption(trick).Get();
}

static bool IsDungeonVanilla(const Rando::DungeonKey dungeon) {
    return ctx->GetDungeon(dungeon)->IsVanilla();
}

static bool IsDungeonMQ(const Rando::DungeonKey dungeon) {
    return ctx->GetDungeon(dungeon)->IsMQ();
}

static bool IsTrialSkipped(const TrialKey trial) {
    return ctx->GetTrial(trial)->IsSkipped();
}

static uint8_t TriforcePiecesCollected() {
    return gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected;
}

static bool RegionAgeTimeAccess(const RandomizerRegion region, const RegionAgeTime ageTime) {
    if (ageTime == RegionAgeTime::childDay)
        return RegionTable(region)->childDay;
    if (ageTime == RegionAgeTime::childNight)
        return RegionTable(region)->childNight;
    if (ageTime == RegionAgeTime::adultDay)
        return RegionTable(region)->adultDay;
    if (ageTime == RegionAgeTime::adultNight)
        return RegionTable(region)->adultNight;
}
#pragma endregion


std::unordered_map<std::string, LogicExpression::Impl::FunctionAdapter> LogicExpression::Impl::functionAdapters;
void LogicExpression::Impl::PopulateFunctionAdapters() {
    functionAdapters = {
        REGISTER_FUNCTION(Here),
        REGISTER_FUNCTION(MQSpiritSharedBrokenWallRoom),
        REGISTER_FUNCTION(MQSpiritSharedStatueRoom),
        REGISTER_FUNCTION(CanBuyAnother),
        REGISTER_FUNCTION(GetOption),
        REGISTER_FUNCTION(GetTrickOption),
        REGISTER_FUNCTION(HasAccessTo),
        REGISTER_FUNCTION(ChildCanAccess),
        REGISTER_FUNCTION(IsDungeonVanilla),
        REGISTER_FUNCTION(IsDungeonMQ),
        REGISTER_FUNCTION(IsTrialSkipped),
        REGISTER_FUNCTION(TriforcePiecesCollected),
        REGISTER_FUNCTION(RegionAgeTimeAccess),
        REGISTER_FUNCTION(CanPlantBean),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanKillEnemy, RandomizerEnemy{}, ED_CLOSE, true, uint8_t{ 1 }, false,
                                              false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanGetEnemyDrop, RandomizerEnemy{}, ED_CLOSE, false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanHitSwitch, ED_CLOSE, false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanPassEnemy, RandomizerEnemy{}, ED_CLOSE, true),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanAvoidEnemy, RandomizerEnemy{}, false, uint8_t{ 1 }),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(SmallKeys, RandomizerRegion{}, uint8_t{}, uint8_t{ 255 }),
        REGISTER_LOGIC_FUNCTION(MQWaterLevel),
        REGISTER_LOGIC_FUNCTION(CanOpenOverworldDoor),
        REGISTER_LOGIC_FUNCTION(HasItem),
        REGISTER_LOGIC_FUNCTION(CanUse),
        REGISTER_LOGIC_FUNCTION(WaterTimer),
        REGISTER_LOGIC_FUNCTION(CanBreakMudWalls),
        REGISTER_LOGIC_FUNCTION(CanGetDekuBabaSticks),
        REGISTER_LOGIC_FUNCTION(CanGetDekuBabaNuts),
        REGISTER_LOGIC_FUNCTION(CanHitEyeTargets),
        REGISTER_LOGIC_FUNCTION(CanDetonateBombFlowers),
        REGISTER_LOGIC_FUNCTION(CanDetonateUprightBombFlower),
        REGISTER_LOGIC_FUNCTION(BottleCount),
        REGISTER_LOGIC_FUNCTION(OcarinaButtons),
        REGISTER_LOGIC_FUNCTION(HasBottle),
        REGISTER_LOGIC_FUNCTION(CanUseSword),
        REGISTER_LOGIC_FUNCTION(CanJumpslashExceptHammer),
        REGISTER_LOGIC_FUNCTION(CanJumpslash),
        REGISTER_LOGIC_FUNCTION(CanDamage),
        REGISTER_LOGIC_FUNCTION(CanAttack),
        REGISTER_LOGIC_FUNCTION(BombchusEnabled),
        REGISTER_LOGIC_FUNCTION(BombchuRefill),
        REGISTER_LOGIC_FUNCTION(HookshotOrBoomerang),
        REGISTER_LOGIC_FUNCTION(ScarecrowsSong),
        REGISTER_LOGIC_FUNCTION(BlueFire),
        REGISTER_LOGIC_FUNCTION(HasExplosives),
        REGISTER_LOGIC_FUNCTION(BlastOrSmash),
        REGISTER_LOGIC_FUNCTION(CanSpawnSoilSkull),
        REGISTER_LOGIC_FUNCTION(CanReflectNuts),
        REGISTER_LOGIC_FUNCTION(CanCutShrubs),
        REGISTER_LOGIC_FUNCTION(CanStunDeku),
        REGISTER_LOGIC_FUNCTION(CanLeaveForest),
        REGISTER_LOGIC_FUNCTION(CallGossipFairy),
        REGISTER_LOGIC_FUNCTION(CallGossipFairyExceptSuns),
        REGISTER_LOGIC_FUNCTION(EffectiveHealth),
        REGISTER_LOGIC_FUNCTION(Hearts),
        REGISTER_LOGIC_FUNCTION(StoneCount),
        REGISTER_LOGIC_FUNCTION(MedallionCount),
        REGISTER_LOGIC_FUNCTION(DungeonCount),
        REGISTER_LOGIC_FUNCTION(FireTimer),
        REGISTER_LOGIC_FUNCTION(TakeDamage),
        REGISTER_LOGIC_FUNCTION(CanOpenBombGrotto),
        REGISTER_LOGIC_FUNCTION(CanOpenStormsGrotto),
        REGISTER_LOGIC_FUNCTION(CanGetNightTimeGS),
        REGISTER_LOGIC_FUNCTION(CanBreakUpperBeehives),
        REGISTER_LOGIC_FUNCTION(CanBreakLowerBeehives),
        REGISTER_LOGIC_FUNCTION(CanBreakPots),
        REGISTER_LOGIC_FUNCTION(CanBreakCrates),
        REGISTER_LOGIC_FUNCTION(CanBreakSmallCrates),
        REGISTER_LOGIC_FUNCTION(HasFireSource),
        REGISTER_LOGIC_FUNCTION(HasFireSourceWithTorch),
        REGISTER_LOGIC_FUNCTION(TradeQuestStep),
        REGISTER_LOGIC_FUNCTION(CanFinishGerudoFortress),
        REGISTER_LOGIC_FUNCTION(CanStandingShield),
        REGISTER_LOGIC_FUNCTION(CanShield),
        REGISTER_LOGIC_FUNCTION(CanUseProjectile),
        REGISTER_LOGIC_FUNCTION(CanBuildRainbowBridge),
        REGISTER_LOGIC_FUNCTION(CanTriggerLACS),
        REGISTER_LOGIC_FUNCTION(GetGSCount),
        REGISTER_LOGIC_FUNCTION(HasBossSoul),
        REGISTER_LOGIC_FUNCTION(HasProjectile),
    };
}

LogicExpression::ValueVariant LogicExpression::Impl::EvaluateFunction(const std::string& path, int depth,
                                                                      const EvaluationCallback& callback) const {
    if (functionAdapters.empty()) {
        PopulateFunctionAdapters();
    }
    try {
        auto it = functionAdapters.find(functionName);
        if (it != functionAdapters.end()) {
            auto result = it->second(children, path, depth, callback);
            
            // If callback is provided, call it with function info
            if (callback) {
                std::string exprStr = functionName + "(" + (children.empty() ? "" : "...") + ")";
                callback(exprStr, path, depth, GetTypeString(), result);
            }
            
            return result;
        }
        throw std::runtime_error("Unknown function: " + functionName + GetExprErrorContext());
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Insufficient arguments for function: " + functionName + GetExprErrorContext());
    }
}

std::unordered_map<std::string, int> LogicExpression::Impl::enumMap;
void LogicExpression::Impl::PopulateEnumMap() {
#define DEFINE_ENEMY_DISTANCE(enum) { #enum, enum },
#define DEFINE_RANDOMIZER_CHECK(enum) { #enum, enum },
#define DEFINE_RANDOMIZER_ENEMY(enum) { #enum, enum },
#define DEFINE_RANDOMIZER_GET(enum) { #enum, enum },
#define DEFINE_RANDOMIZER_TRICK(enum) { #enum, enum },
#define DEFINE_RANDOMIZER_REGION(enum) { #enum, enum },
#define DEFINE_RANDO_WATER_LEVEL(enum) { #enum, enum },
#define DEFINE_RANDOMIZER_SETTING_KEY(enum) { #enum, enum },
#define DEFINE_DUNGEON_KEY(enum) { #enum, enum },
#define DEFINE_TRIAL_KEY(enum) { #enum, enum },
#define DEFINE_RANDO_OPTIONS

    enumMap = {
#include "randomizer_types/enemyDistance.h"
#include "randomizer_types/randomizerCheck.h"
#include "randomizer_types/randomizerEnemy.h"
#include "randomizer_types/randomizerGet.h"
#include "randomizer_types/randomizerTrick.h"
#include "randomizer_types/randomizerRegion.h"
#include "randomizer_types/randoWaterLevel.h"
#include "randomizer_types/randomizerSettingKey.h"
#include "randomizer_types/trialKey.h"
#include "randomizer_types/randoOptions.h"
        { "DEKU_TREE", Rando::DEKU_TREE },
        { "DODONGOS_CAVERN", Rando::DODONGOS_CAVERN },
        { "JABU_JABUS_BELLY", Rando::JABU_JABUS_BELLY },
        { "FOREST_TEMPLE", Rando::FOREST_TEMPLE },
        { "FIRE_TEMPLE", Rando::FIRE_TEMPLE },
        { "WATER_TEMPLE", Rando::WATER_TEMPLE },
        { "SPIRIT_TEMPLE", Rando::SPIRIT_TEMPLE },
        { "SHADOW_TEMPLE", Rando::SHADOW_TEMPLE },
        { "BOTTOM_OF_THE_WELL", Rando::BOTTOM_OF_THE_WELL },
        { "ICE_CAVERN", Rando::ICE_CAVERN },
        { "GERUDO_TRAINING_GROUND", Rando::GERUDO_TRAINING_GROUND },
        { "GANONS_CASTLE", Rando::GANONS_CASTLE },
        { "HasProjectileAge::Adult", (int)Rando::HasProjectileAge::Adult },
        { "HasProjectileAge::Child", (int)Rando::HasProjectileAge::Child },
        { "HasProjectileAge::Both", (int)Rando::HasProjectileAge::Both },
        { "HasProjectileAge::Either", (int)Rando::HasProjectileAge::Either },
        { "RegionAgeTime::childDay", (int)RegionAgeTime::childDay },
        { "RegionAgeTime::childNight", (int)RegionAgeTime::childNight },
        { "RegionAgeTime::adultDay", (int)RegionAgeTime::adultDay },
        { "RegionAgeTime::adultNight", (int)RegionAgeTime::adultNight },
    };

#undef DEFINE_ENEMY_DISTANCE
#undef DEFINE_RANDOMIZER_CHECK
#undef DEFINE_RANDOMIZER_ENEMY
#undef DEFINE_RANDOMIZER_GET
#undef DEFINE_RANDOMIZER_TRICK
#undef DEFINE_RANDOMIZER_REGION
#undef DEFINE_RANDO_WATER_LEVEL
#undef DEFINE_RANDOMIZER_SETTING_KEY
#undef DEFINE_DUNGEON_KEY
#undef DEFINE_TRIAL_KEY
#undef DEFINE_RANDO_OPTIONS
}

LogicExpression::ValueVariant LogicExpression::Impl::EvaluateEnum() const {
    if (enumMap.empty()) {
        PopulateEnumMap();
    }
    auto it = enumMap.find(value);
    if (it != enumMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Unknown enum constant: " + value + GetExprErrorContext());
}

std::unordered_map<std::string, LogicExpression::Impl::FunctionAdapter> LogicExpression::Impl::variableAdapters;
void LogicExpression::Impl::PopulateVariableAdapters() {
    variableAdapters = {
        REGISTER_LOGIC_VARIABLE(SkullMask),
        REGISTER_LOGIC_VARIABLE(MaskOfTruth),
        REGISTER_LOGIC_VARIABLE(FreedEpona),
        REGISTER_LOGIC_VARIABLE(WakeUpAdultTalon),
        REGISTER_LOGIC_VARIABLE(DekuTreeClear),
        REGISTER_LOGIC_VARIABLE(DodongosCavernClear),
        REGISTER_LOGIC_VARIABLE(JabuJabusBellyClear),
        REGISTER_LOGIC_VARIABLE(ForestTempleClear),
        REGISTER_LOGIC_VARIABLE(FireTempleClear),
        REGISTER_LOGIC_VARIABLE(WaterTempleClear),
        REGISTER_LOGIC_VARIABLE(SpiritTempleClear),
        REGISTER_LOGIC_VARIABLE(ShadowTempleClear),
        REGISTER_LOGIC_VARIABLE(ForestTrialClear),
        REGISTER_LOGIC_VARIABLE(FireTrialClear),
        REGISTER_LOGIC_VARIABLE(WaterTrialClear),
        REGISTER_LOGIC_VARIABLE(SpiritTrialClear),
        REGISTER_LOGIC_VARIABLE(ShadowTrialClear),
        REGISTER_LOGIC_VARIABLE(LightTrialClear),
        REGISTER_LOGIC_VARIABLE(IsFireLoopLocked),
        REGISTER_LOGIC_VARIABLE(Bottles),
        REGISTER_LOGIC_VARIABLE(NumBottles),
        REGISTER_LOGIC_VARIABLE(CanEmptyBigPoes),
        REGISTER_LOGIC_VARIABLE(CouldEmptyBigPoes),
        REGISTER_LOGIC_VARIABLE(AreCheckingBigPoes),
        REGISTER_LOGIC_VARIABLE(NutPot),
        REGISTER_LOGIC_VARIABLE(NutCrate),
        REGISTER_LOGIC_VARIABLE(DekuBabaNuts),
        REGISTER_LOGIC_VARIABLE(StickPot),
        REGISTER_LOGIC_VARIABLE(DekuBabaSticks),
        REGISTER_LOGIC_VARIABLE(BugShrub),
        REGISTER_LOGIC_VARIABLE(WanderingBugs),
        REGISTER_LOGIC_VARIABLE(BugRock),
        REGISTER_LOGIC_VARIABLE(BlueFireAccess),
        REGISTER_LOGIC_VARIABLE(FishGroup),
        REGISTER_LOGIC_VARIABLE(LoneFish),
        REGISTER_LOGIC_VARIABLE(GossipStoneFairy),
        REGISTER_LOGIC_VARIABLE(BeanPlantFairy),
        REGISTER_LOGIC_VARIABLE(ButterflyFairy),
        REGISTER_LOGIC_VARIABLE(FairyPot),
        REGISTER_LOGIC_VARIABLE(FreeFairies),
        REGISTER_LOGIC_VARIABLE(FairyPond),
        REGISTER_LOGIC_VARIABLE(AmmoCanDrop),
        REGISTER_LOGIC_VARIABLE(PieceOfHeart),
        REGISTER_LOGIC_VARIABLE(HeartContainer),
        REGISTER_LOGIC_VARIABLE(ChildScarecrow),
        REGISTER_LOGIC_VARIABLE(AdultScarecrow),
        REGISTER_LOGIC_VARIABLE(CarpetMerchant),
        REGISTER_LOGIC_VARIABLE(CouldPlayBowling),
        REGISTER_LOGIC_VARIABLE(IsChild),
        REGISTER_LOGIC_VARIABLE(IsAdult),
        REGISTER_LOGIC_VARIABLE(BigPoeKill),
        REGISTER_LOGIC_VARIABLE(BigPoes),
        REGISTER_LOGIC_VARIABLE(BaseHearts),
        REGISTER_LOGIC_VARIABLE(BuiltRainbowBridge),
        REGISTER_LOGIC_VARIABLE(AtDay),
        REGISTER_LOGIC_VARIABLE(AtNight),
        REGISTER_LOGIC_VARIABLE(ShowedMidoSwordAndShield),
        REGISTER_LOGIC_VARIABLE(CarpenterRescue),
        REGISTER_LOGIC_VARIABLE(GF_GateOpen),
        REGISTER_LOGIC_VARIABLE(GtG_GateOpen),
        REGISTER_LOGIC_VARIABLE(DampesWindmillAccess),
        REGISTER_LOGIC_VARIABLE(DrainWell),
        REGISTER_LOGIC_VARIABLE(GoronCityChildFire),
        REGISTER_LOGIC_VARIABLE(GCWoodsWarpOpen),
        REGISTER_LOGIC_VARIABLE(GCDaruniasDoorOpenChild),
        REGISTER_LOGIC_VARIABLE(StopGCRollingGoronAsAdult),
        REGISTER_LOGIC_VARIABLE(CanWaterTempleLowFromHigh),
        REGISTER_LOGIC_VARIABLE(CanWaterTempleMiddle),
        REGISTER_LOGIC_VARIABLE(CanWaterTempleHigh),
        REGISTER_LOGIC_VARIABLE(CanWaterTempleLowFromMid),
        REGISTER_LOGIC_VARIABLE(CouldWaterTempleLow),
        REGISTER_LOGIC_VARIABLE(CouldWaterTempleMiddle),
        REGISTER_LOGIC_VARIABLE(ReachedWaterHighEmblem),
        REGISTER_LOGIC_VARIABLE(KakarikoVillageGateOpen),
        REGISTER_LOGIC_VARIABLE(KingZoraThawed),
        REGISTER_LOGIC_VARIABLE(ForestTempleJoelle),
        REGISTER_LOGIC_VARIABLE(ForestTempleBeth),
        REGISTER_LOGIC_VARIABLE(ForestTempleAmy),
        REGISTER_LOGIC_VARIABLE(ForestTempleMeg),
        REGISTER_LOGIC_VARIABLE(FireLoopSwitch),
        REGISTER_LOGIC_VARIABLE(LinksCow),
        REGISTER_LOGIC_VARIABLE(DeliverLetter),
        REGISTER_LOGIC_VARIABLE(ClearMQDCUpperLobbyRocks),
        REGISTER_LOGIC_VARIABLE(LoweredWaterInsideBotw),
        REGISTER_LOGIC_VARIABLE(OpenedWestRoomMQBotw),
        REGISTER_LOGIC_VARIABLE(OpenedMiddleHoleMQBotw),
        REGISTER_LOGIC_VARIABLE(BrokeDeku1FWeb),
        REGISTER_LOGIC_VARIABLE(ClearedMQDekuSERoom),
        REGISTER_LOGIC_VARIABLE(MQDekuWaterRoomTorches),
        REGISTER_LOGIC_VARIABLE(PushedDekuBasementBlock),
        REGISTER_LOGIC_VARIABLE(OpenedLowestGoronCage),
        REGISTER_LOGIC_VARIABLE(OpenedUpperFireShortcut),
        REGISTER_LOGIC_VARIABLE(HitFireTemplePlatform),
        REGISTER_LOGIC_VARIABLE(OpenedFireMQFireMazeDoor),
        REGISTER_LOGIC_VARIABLE(MQForestBlockRoomTargets),
        REGISTER_LOGIC_VARIABLE(ForestCanTwistHallway),
        REGISTER_LOGIC_VARIABLE(ForestClearBelowBowChest),
        REGISTER_LOGIC_VARIABLE(ForestOpenBossCorridor),
        REGISTER_LOGIC_VARIABLE(ShadowTrialFirstChest),
        REGISTER_LOGIC_VARIABLE(MQGTGMazeSwitch),
        REGISTER_LOGIC_VARIABLE(MQGTGRightSideSwitch),
        REGISTER_LOGIC_VARIABLE(GTGPlatformSilverRupees),
        REGISTER_LOGIC_VARIABLE(MQJabuHolesRoomDoor),
        REGISTER_LOGIC_VARIABLE(JabuWestTentacle),
        REGISTER_LOGIC_VARIABLE(JabuEastTentacle),
        REGISTER_LOGIC_VARIABLE(JabuNorthTentacle),
        REGISTER_LOGIC_VARIABLE(LoweredJabuPath),
        REGISTER_LOGIC_VARIABLE(MQJabuLiftRoomCow),
        REGISTER_LOGIC_VARIABLE(MQShadowFloorSpikeRupees),
        REGISTER_LOGIC_VARIABLE(ShadowShortcutBlock),
        REGISTER_LOGIC_VARIABLE(MQWaterStalfosPit),
        REGISTER_LOGIC_VARIABLE(MQWaterDragonTorches),
        REGISTER_LOGIC_VARIABLE(MQWaterB1Switch),
        REGISTER_LOGIC_VARIABLE(MQWaterOpenedPillarB1),
        REGISTER_LOGIC_VARIABLE(MQSpiritCrawlBoulder),
        REGISTER_LOGIC_VARIABLE(MQSpiritMapRoomEnemies),
        REGISTER_LOGIC_VARIABLE(MQSpiritTimeTravelChest),
        REGISTER_LOGIC_VARIABLE(MQSpirit3SunsEnemies),
        REGISTER_LOGIC_VARIABLE(Spirit1FSilverRupees),
        REGISTER_LOGIC_VARIABLE(JabuRutoIn1F),
    };
}

LogicExpression::ValueVariant LogicExpression::Impl::EvaluateVariable() const {
    if (variableAdapters.empty()) {
        PopulateVariableAdapters();
    }
    
    auto it = variableAdapters.find(value);
    if (it != variableAdapters.end()) {
        std::vector<std::shared_ptr<LogicExpression::Impl>> emptyArgs;
        return it->second(emptyArgs, "var", 0, nullptr); // Call the variable adapter with empty arguments
    }
    
    throw std::runtime_error("Unknown variable: '" + value + "'" + GetExprErrorContext());
}

// Helper for arithmetic operations to reduce duplication
LogicExpression::ValueVariant LogicExpression::Impl::EvaluateArithmetic(char op, const std::string& path, int depth,
                                                                        const EvaluationCallback& callback) const {
    const auto lhs = children[0]->Evaluate(path + ".0", depth + 1, callback);
    const auto rhs = children[1]->Evaluate(path + ".1", depth + 1, callback);
    
    auto arith = [&](auto a, auto b) -> ValueVariant {
        // Accept int, uint8_t, but not bool
        if constexpr ((std::is_same_v<std::decay_t<decltype(a)>, int> ||
                       std::is_same_v<std::decay_t<decltype(a)>, uint8_t>) &&
                      (std::is_same_v<std::decay_t<decltype(b)>, int> ||
                       std::is_same_v<std::decay_t<decltype(b)>, uint8_t>)) {
            int l = static_cast<int>(a);
            int r = static_cast<int>(b);
            ValueVariant result;
            switch (op) {
                case '+':
                    result = l + r;
                    break;
                case '-':
                    result = l - r;
                    break;
                case '*':
                    result = l * r;
                    break;
                case '/':
                    if (r == 0)
                        throw std::runtime_error("Division by zero" + GetExprErrorContext());
                    result = l / r;
                    break;
                default:
                    throw std::runtime_error("Unknown arithmetic op" + GetExprErrorContext());
            }
            return result;
        } else {
            throw std::runtime_error("Invalid types for arithmetic (must be int or uint8_t, not bool)" +
                                     GetExprErrorContext());
        }
    };
    
    try {
        auto result = std::visit(arith, lhs, rhs);
        
        // If callback is provided, call it
        if (callback) {
            std::string opStr;
            switch (op) {
                case '+': opStr = "Add"; break;
                case '-': opStr = "Subtract"; break;
                case '*': opStr = "Multiply"; break;
                case '/': opStr = "Divide"; break;
                default: opStr = "Unknown";
            }
            
            // Get the sub-expression string 
            // Find root to get expression string
            const Impl* root = this;
            while (root->parent)
                root = root->parent;
                
            std::string exprStr;
            if (root->expressionString) {
                exprStr = root->expressionString->substr(startIndex, endIndex - startIndex);
            } else {
                exprStr = "Unknown expression";
            }
            
            callback(exprStr, path, depth, opStr, result);
        }
        
        return result;
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Invalid variant access in arithmetic" + GetExprErrorContext());
    }
}

LogicExpression::ValueVariant LogicExpression::Impl::Evaluate(const std::string& path, int depth,
                                                              const EvaluationCallback& callback) const {
    ValueVariant result;
    
    // Get the expression string
    std::string exprText;
    if (expressionString) {
        // This is the root node with the full expression
        exprText = *expressionString;
    } else if (parent) {
        // Find root to get expression string
        const Impl* root = this;
        while (root->parent)
            root = root->parent;
            
        if (root->expressionString) {
            exprText = root->expressionString->substr(startIndex, endIndex - startIndex);
        }
    }
    
    if (exprText.empty()) {
        exprText = "Unknown expression";
    }
    
    switch (type) {
        case Type::Value:
            if (valueType == ValueType::Boolean) {
                result = value == "true";
            } else if (valueType == ValueType::Number) {
                result = std::stoi(value);
            } else if (valueType == ValueType::Enum) {
                result = EvaluateEnum();
            } else if (valueType == ValueType::Identifier) {
                result = EvaluateVariable();
            } else {
                throw std::runtime_error("Unknown value type: " + value + GetExprErrorContext());
            }
            
            if (callback) {
                callback(exprText, path, depth, GetTypeString(), result);
            }
            return result;
            
        case Type::FunctionCall:
            return EvaluateFunction(path, depth, callback);
            
        case Type::Not: {
            auto childResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            result = !GetValue<bool>(childResult);
            if (callback) {
                callback(exprText, path, depth, GetTypeString(), result);
            }
            return result;
        }
        
        case Type::And: {
            // Short-circuit evaluation
            auto leftResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            if (!GetValue<bool>(leftResult)) {
                result = false;
                if (callback) {
                    callback(exprText, path, depth, GetTypeString() + " (short-circuit)", result);
                }
                return result;
            }
            
            auto rightResult = children[1]->Evaluate(path + ".1", depth + 1, callback);
            result = GetValue<bool>(rightResult);
            if (callback) {
                callback(exprText, path, depth, GetTypeString(), result);
            }
            return result;
        }
        
        case Type::Or: {
            // Short-circuit evaluation
            auto leftResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            if (GetValue<bool>(leftResult)) {
                result = true;
                if (callback) {
                    callback(exprText, path, depth, GetTypeString() + " (short-circuit)", result);
                }
                return result;
            }
            
            auto rightResult = children[1]->Evaluate(path + ".1", depth + 1, callback);
            result = GetValue<bool>(rightResult);
            if (callback) {
                callback(exprText, path, depth, GetTypeString(), result);
            }
            return result;
        }
        
        case Type::Comparison: {
            auto leftResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            auto rightResult = children[1]->Evaluate(path + ".1", depth + 1, callback);

            auto compare = [&](auto a, auto b) -> bool {
                using A = decltype(a);
                using B = decltype(b);
                
                // Compare booleans only for equality/inequality.
                if constexpr (std::is_same_v<A, bool> && std::is_same_v<B, bool>) {
                    if (operation == "==")
                        return a == b;
                    if (operation == "!=")
                        return a != b;
                    throw std::runtime_error("Unsupported operator for booleans: " + operation + GetExprErrorContext());
                }
                // Compare any integral types (int and uint8_t included) by promoting to int.
                else if constexpr (std::is_integral_v<A> && std::is_integral_v<B>) {
                    int l = static_cast<int>(a);
                    int r = static_cast<int>(b);
                    if (operation == "==")
                        return l == r;
                    if (operation == "!=")
                        return l != r;
                    if (operation == ">=")
                        return l >= r;
                    if (operation == "<=")
                        return l <= r;
                    if (operation == ">")
                        return l > r;
                    if (operation == "<")
                        return l < r;
                    throw std::runtime_error("Unknown comparison operator: " + operation + GetExprErrorContext());
                } else {
                    throw std::runtime_error("Invalid comparison between types" + GetExprErrorContext());
                }
            };

            try {
                result = std::visit(compare, leftResult, rightResult);
                if (callback) {
                    callback(exprText, path, depth, GetTypeString(), result);
                }
                return result;
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Invalid variant access in comparison" + GetExprErrorContext());
            }
        }
        
        case Type::Add:
            return EvaluateArithmetic('+', path, depth, callback);
        case Type::Subtract:
            return EvaluateArithmetic('-', path, depth, callback);
        case Type::Multiply:
            return EvaluateArithmetic('*', path, depth, callback);
        case Type::Divide:
            return EvaluateArithmetic('/', path, depth, callback);
            
        case Type::Ternary: {
            auto condResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            bool cond = GetValue<bool>(condResult);
            
            if (cond) {
                result = children[1]->Evaluate(path + ".1", depth + 1, callback);
            } else {
                result = children[2]->Evaluate(path + ".2", depth + 1, callback);
            }
            
            if (callback) {
                callback(exprText, path, depth, GetTypeString() + (cond ? " (true branch)" : " (false branch)"), result);
            }
            return result;
        }
        
        default:
            throw std::runtime_error("Unknown expression type" + GetExprErrorContext());
    }
    
    return false;
}

ExpressionEvaluation EvaluateExpression(std::string condition) {
    const auto& expression = LogicExpression::Parse(condition);

    // Create a vector to store the evaluation sequence
    std::vector<std::tuple<std::string, std::string, int, std::string, LogicExpression::ValueVariant>>
        evaluationSequence;

    // Define a callback that records each evaluation step
    auto recordCallback = [&evaluationSequence](const std::string& exprStr, const std::string& path, int depth,
                                                const std::string& type, const LogicExpression::ValueVariant& result) {
        evaluationSequence.emplace_back(exprStr, path, depth, type, result);
    };

    // Evaluate the expression with the callback
    auto finalResult = expression->Evaluate<LogicExpression::ValueVariant>(recordCallback);

    // Helper function to convert path string to a vector of integers for sorting
    auto pathToVector = [](const std::string& path) {
        std::vector<int> result;
        std::stringstream ss(path);
        std::string segment;

        while (std::getline(ss, segment, '.')) {
            try {
                result.push_back(std::stoi(segment));
            } catch (const std::exception&) {
                // If it's not a valid integer, just skip it
                result.push_back(0);
            }
        }

        return result;
    };

    // Sort the evaluation sequence by path
    std::sort(evaluationSequence.begin(), evaluationSequence.end(), [&pathToVector](const auto& a, const auto& b) {
        const auto& pathA = std::get<1>(a);
        const auto& pathB = std::get<1>(b);

        auto vecA = pathToVector(pathA);
        auto vecB = pathToVector(pathB);

        // Compare each component of the path
        size_t i = 0;
        while (i < vecA.size() && i < vecB.size()) {
            if (vecA[i] != vecB[i]) {
                return vecA[i] < vecB[i];
            }
            i++;
        }

        // If one path is a prefix of the other, the shorter one comes first
        return vecA.size() < vecB.size();
    });

    ExpressionEvaluation evaluation;
    evaluation.Expression = std::get<0>(evaluationSequence[0]);
    evaluation.Depth = std::get<2>(evaluationSequence[0]);
    evaluation.Type = std::get<3>(evaluationSequence[0]);
    evaluation.Result = std::get<4>(evaluationSequence[0]);

    // Stack to keep track of parent nodes at each depth
    std::stack<ExpressionEvaluation*> parentStack;
    parentStack.push(&evaluation);

    // Process remaining evaluations to build the tree
    for (size_t i = 1; i < evaluationSequence.size(); ++i) {
        ExpressionEvaluation child;
        child.Expression = std::get<0>(evaluationSequence[i]);
        child.Depth = std::get<2>(evaluationSequence[i]);
        child.Type = std::get<3>(evaluationSequence[i]);
        child.Result = std::get<4>(evaluationSequence[i]);

        // Pop parents from stack if we're at a shallower depth
        while (!parentStack.empty() && parentStack.top()->Depth >= child.Depth) {
            parentStack.pop();
        }

        // Add child to current parent
        if (!parentStack.empty()) {
            parentStack.top()->Children.push_back(std::move(child));
            // If this child might have children, push it onto the stack
            parentStack.push(&(parentStack.top()->Children.back()));
        }
    }

    return evaluation;
}

std::string ToString(const LogicExpression::ValueVariant& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<uint8_t>(value)) {
        return std::to_string(std::get<uint8_t>(value));
    } else if (std::holds_alternative<uint16_t>(value)) {
        return std::to_string(std::get<uint16_t>(value));
    } else {
        return "unknown";
    }
}
