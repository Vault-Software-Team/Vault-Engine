#include <Expr/MathExpr.hpp>
#include <HyperScript/HyperScript.hpp>
#include <iostream>
#include <exprtk.hpp>
#include <uuid.hpp>
// trash garbage code for a shitty language
// Do NOT flame me programming gods

// Function Decl
namespace HyperScript {
    void ScriptEngine::Print(Function *func) {
        std::string to_print = "";
        for (auto &arg : func->arguments) {
            to_print += arg.value;
        }
        std::cout << "[print] " << to_print;

        func->SetReturn("1", GT_NUMBER);
    }

    void ScriptEngine::Println(Function *func) {
        std::string to_print = "";
        for (auto &arg : func->arguments) {
            to_print += arg.value;
        }
        std::cout << "[print] " << to_print << "\n";

        func->SetReturn("1", GT_NUMBER);
    }

    void ScriptEngine::FUNC_GetVariable(Function *func) {
        Variable v = func->arguments[0];
        ScriptEngine *engine = (ScriptEngine *)func->engine;
        auto var = engine->GetVariable(v.value);

        if (var) {
            func->SetReturn(var->value, var->type);
        } else {
            func->SetReturn("0", GT_ANY);
        }
    }

    void ScriptEngine::FUNC_CreateVariable(Function *func) {
        Variable arg_name = func->arguments[0];
        Variable arg_value = func->arguments[1];

        ScriptEngine *engine = (ScriptEngine *)func->engine;
        auto var = engine->CreateVariable(arg_value.type, arg_name.value, arg_value.value);
    }

    void ScriptEngine::FUNC_array_size(Function *func) {
        Variable arg_value = func->arguments[0];
        func->SetReturn(std::to_string(arg_value.json_handle.size()), GT_NUMBER);
    }

    void ScriptEngine::FUNC_push_back(Function *func) {
        auto engine = (ScriptEngine *)func->engine;
        auto arg_arr = engine->GetVariable(engine->scope_variables, func->arguments[0].addr);
        Variable arg_val = func->arguments[1];
        arg_arr->PushToJSONArray(arg_val.type, arg_val.value);
        func->SetReturn(arg_val.value, arg_val.type, arg_val.custom_type);
    }

    void ScriptEngine::FUNC_ref_count(Function *func) {
        Variable arg_val = func->arguments[0];
        func->SetReturn(std::to_string(arg_val.ref_count), GT_NUMBER);
    }
} // namespace HyperScript

namespace HyperScript {
    void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    }

    void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    }

    void Variable::CallLambda() {
        std::shared_ptr<Function> pFunc = std::static_pointer_cast<Function>(func_lambda);
        pFunc->Call();
    }

    void Variable::AddRef(void *engine) {
        this->engine = engine;
        ref_count++;
        if (ref != nullptr && ref != this)
            ref->AddRef(engine);
    }

    void Variable::RemoveRef(void *engine, bool freeing_currently) {
        this->engine = engine;
        ref_count--;
        std::cout << name << " : " << ref_count << "\n";
        if (ref != nullptr && ref != this) {
            ref->RemoveRef(engine, freeing_currently);
        }

        if (ref)
            std::cout << "COUNT: " << ref->ref_count << "\n";

        if (ref_count <= 0 && !freeing_currently) {
            std::cout << "freeing\n";
            ((ScriptEngine *)engine)->FreeVariable(addr);
        }
    }

    Variable::~Variable() {
    }

    void Variable::SetType(const std::string &name, void *engine) {
        ScriptEngine *s_Engine = (ScriptEngine *)engine;

        auto type = s_Engine->GetType(name);
        this->custom_type = type;
    }

    void Variable::PushJSONVariables(void *m_Engine, nlohmann::json obj, const std::string &parent_starter) {
        ScriptEngine *engine = (ScriptEngine *)m_Engine;
        for (auto &[key, value] : obj.items()) {
            std::cout << parent_starter + "." + key << "\n";
            if (value.is_object()) {
                auto var = engine->CreateVariable(*engine->current_scope_variables, GT_OBJECT, parent_starter + "." + key, nlohmann::to_string(value));
                var->parent_json = this;
                var->json_handle = nlohmann::json::parse(var->value);
                json_variable_tracking.push_back(var->addr);
                PushJSONVariables(m_Engine, value, parent_starter + "." + key);
                continue;
            }

            if (value.is_array()) {
                auto var = engine->CreateVariable(*engine->current_scope_variables, GT_ARRAY, parent_starter + "." + key, nlohmann::to_string(value));
                var->parent_json = this;
                var->json_handle = nlohmann::json::parse(var->value);

                json_variable_tracking.push_back(var->addr);
                PushJSONVariables(m_Engine, value, parent_starter + "." + key);
                continue;
            }

            std::cout << key << " -- " << value << "\n";

            if (value.is_string()) {
                auto var = engine->CreateVariable(*engine->current_scope_variables, GT_STRING, parent_starter + "." + key, (std::string)value);
                var->parent_json = this;
                json_variable_tracking.push_back(var->addr);
            }
            if (value.is_number()) {
                auto var = engine->CreateVariable(*engine->current_scope_variables, GT_NUMBER, parent_starter + "." + key, (std::string)std::to_string((double)value));
                var->parent_json = this;
                json_variable_tracking.push_back(var->addr);
            }
            if (value.is_boolean()) {
                auto var = engine->CreateVariable(*engine->current_scope_variables, GT_BOOLEAN, parent_starter + "." + key, (bool)value ? "true" : "false");
                var->parent_json = this;
                json_variable_tracking.push_back(var->addr);
            }
        }
    }

    void Variable::LoadValueToJSONHandle(void *m_Engine) {
        for (auto addr : json_variable_tracking) {
            ((ScriptEngine *)m_Engine)->FreeVariable(addr);
        }
        json_handle = nlohmann::json::parse(value);

        PushJSONVariables(m_Engine, json_handle, name);
    }

    void Variable::PushToJSONArray(GenericType type, const std::string &value) {
        switch (type) {
        case GT_STRING: {
            json_handle.push_back(value);
            break;
        }
        case GT_BOOLEAN: {
            json_handle.push_back(value == "true" ? true : false);
            break;
        }
        case GT_NUMBER: {
            json_handle.push_back(std::atof(value.c_str()));
            break;
        }
        default: {
            json_handle.push_back(value);
            break;
        }
        }

        this->value = nlohmann::to_string(json_handle);
    }

    void Function::Call() {
        call(this);
        arguments.clear();

        std::vector<uint64_t> addr_free;
        for (auto &var : variables) {
            ScriptEngine *engine = (ScriptEngine *)var.second->engine;
            addr_free.push_back(var.second->addr);
        }

        for (auto addr : addr_free) {
            auto f = variables.find(addr);
            if (f != variables.end()) {
                if (f->second->isRef) {
                    f->second->RemoveRef(this, true);
                }

                variables.erase(f);
            }
        }
    }

    void Function::CallWhileLoop(void *engine) {
        ScriptEngine *s_Engine = (ScriptEngine *)engine;
        if (expr == "")
            return;

        break_loop = false;

        while (s_Engine->StringToBoolExpr(expr)) {
            if (break_loop) break;
            call(this);
        }
        arguments.clear();
        variables.clear();
    }

    ScriptFunction::ScriptFunction() {
        is_global = false;
    }

    void Function::SetReturn(const std::string &val, GenericType generic_type, std::shared_ptr<Variable::Type> custom_type) {
        return_value.value = val;
        return_value.type = generic_type;
        return_value.custom_type = custom_type;
    }

    void Function::ClearReturn() {
        return_value.value = "\0";
        return_value.type = GT_ANY;
        return_value.custom_type = nullptr;
    }

    Variable Function::GetArgumentByName(const std::string &s_Name) {
        for (auto &arg : arguments) {
            if (arg.name == s_Name) {
                return arg;
            }
        }

        return Variable();
    }

    ScriptEngine::ScriptEngine() {
        auto f_Print = CreateFunction("print", 1, Print);
        f_Print->argument_names.push_back("content");
        f_Print->limitless_args = true;

        auto f_Println = CreateFunction("println", 1, Println);
        f_Println->argument_names.push_back("content");
        f_Println->limitless_args = true;

        auto f_CreateVariable = CreateFunction("create_var", 2, FUNC_CreateVariable);
        f_CreateVariable->argument_names.push_back("name");
        f_CreateVariable->argument_names.push_back("value");

        auto f_GetVariable = CreateFunction("get_var", 2, FUNC_GetVariable);
        f_GetVariable->argument_names.push_back("name");

        auto f_ArraySize = CreateFunction("array_size", 1, FUNC_array_size);
        f_ArraySize->argument_names.push_back("array");

        auto f_PushBack = CreateFunction("push_back", 2, FUNC_push_back);
        f_PushBack->argument_names.push_back("array");
        f_PushBack->argument_names.push_back("value");

        auto f_RefCount = CreateFunction("ref_count", 1, FUNC_ref_count);
        f_RefCount->argument_names.push_back("ref_var");
    }

    Module &ScriptEngine::LoadScript(const std::string &module_name, const std::string &script_path) {
        Module s_Module;
        s_Module.module_name = module_name;
        s_Module.script_path = script_path;

        std::ifstream stream(script_path);
        std::string line;
        while (std::getline(stream, line)) {
            s_Module.lines.push_back(line);
        }

        modules[s_Module.module_name] = s_Module;
        return modules[s_Module.module_name];
    }

    Module &ScriptEngine::GetModule(const std::string &module_name) {
        return modules[module_name];
    }

    Token ScriptEngine::CreateToken(EToken type, const std::string &value) {
        return Token{type, value};
    }

    Token ScriptEngine::CharToToken(const char ch) {
        switch (ch) {
        case '.':
            return Token{DOT, std::string(1, ch)};
        case '=':
            return Token{EQUALS, std::string(1, ch)};
        case ';':
            return Token{SEMI, std::string(1, ch)};
        case '(':
            return Token{L_PAREN, std::string(1, ch)};
        case ')':
            return Token{R_PAREN, std::string(1, ch)};
        case '{':
            return Token{L_BRACKET, std::string(1, ch)};
        case '}':
            return Token{R_BRACKET, std::string(1, ch)};
        case '[':
            return Token{L_SQUARE_BRACKET, std::string(1, ch)};
        case ']':
            return Token{R_SQUARE_BRACKET, std::string(1, ch)};
        case ',':
            return Token{COMMA, std::string(1, ch)};
        case '&':
            return Token{AMPERSAND, std::string(1, ch)};
        case '|':
            return Token{PIPE, std::string(1, ch)};
        case '\n':
            return Token{SEMI, std::string(1, ch)};
        }

        return Token{_NULL, std::string(1, ch)};
    }

    Token ScriptEngine::CollectComment(const std::string &str, int &i) {
        i++;

        std::string value = "";
        while (str[i] != '\n') {
            value += str[i];
            i++;
        }

        return CreateToken(COMMENT, value);
    }

    Token ScriptEngine::CollectMultilineComment(const std::string &str, int &i) {
        i++;
        i++;

        std::string value = "";
        while (str[i] != '*' && str[i + 1] != '/') {
            value += str[i];
            i++;
        }
        i++;
        i++;

        return CreateToken(COMMENT, value);
    }

    Token ScriptEngine::CollectString(const std::string &str, int &i) {
        i++;

        std::string value = "";
        while (str[i] != '"') {
            value += str[i];
            i++;
        }

        i++;

        return CreateToken(STRING, value);
    }

    void ScriptEngine::SkipWhitespace(const std::string &str, int &i) {
        while (str[i] == ' ' || str[i] == 10) {
            i++;
        }
    }

    Token ScriptEngine::CollectID(const std::string &str, int &i) {
        std::string value = "";
        while (isalnum(str[i]) || str[i] == '_' || str[i] == '-' || str[i] == '.') {
            value += str[i];
            i++;
        }

        for (auto &keyword : keywords) {
            if (keyword.str == value) {
                return CreateToken(keyword.token_type, value);
            }
        }

        return CreateToken(ID, value);
    }

    Token ScriptEngine::CollectNumber(const std::string &str, int &i) {
        std::string value = "";
        while (isdigit(str[i]) || str[i] == '.') {
            value += str[i];
            i++;
        }

        return CreateToken(NUMBER, value);
    }

    Token ScriptEngine::CollectObject(const std::string &str, int &i) {
        i++;

        std::string value = "";
        while (str[i] != '>') {
            value += str[i];
            i++;
        }
        i++;

        return CreateToken(OBJECT_SCOPE, value);
    }

    Token ScriptEngine::CollectMathExpression(const std::string &str, int &i) {
        i++;

        std::string value = "";
        while (str[i] != '\'') {
            value += str[i];
            i++;
        }
        i++;

        if (value.find("==") != std::string::npos ||
            value.find("!=") != std::string::npos ||
            value.find("<=") != std::string::npos ||
            value.find(">=") != std::string::npos ||
            value.find("<") != std::string::npos ||
            value.find(">") != std::string::npos ||
            value.find("&&") != std::string::npos ||
            value.find("||") != std::string::npos) {
            return CreateToken(BOOLEAN_EXPRESSION, value);
        }

        return CreateToken(MATH_EXPRESSION, value);
    }

    Token ScriptEngine::CollectHex(const std::string &str, int &i) {
        i++;
        i++;

        std::string value = "";
        while (isdigit(str[i]) || isalnum(str[i])) {
            value += str[i];
            i++;
        }

        uint64_t val = 0;
        std::stringstream ss;
        ss << std::hex << value;
        ss >> val;

        return CreateToken(NUMBER, std::to_string(val));
    }

    Token ScriptEngine::CollectScope(const std::string &str, int &i) {
        i++;

        std::vector<std::string> scopes = {""};

        bool condition_met = false;
        while (!condition_met) {
            if (str[i] == '{') {
                scopes.push_back("");
            }

            if (str[i] == '}') {
                if (scopes.size() > 1) {
                    scopes.pop_back();
                } else if (scopes.size() <= 1) {
                    condition_met = true;
                    continue;
                }
            }

            scopes[0] += str[i];

            // scopes[0] += str[i];
            i++;
        }

        i++;

        return CreateToken(FUNCTION_SCOPE, scopes[0]);
    }

    void ScriptEngine::ParseToTokens(Module &s_Module) {
        std::string content;
        for (auto &line : s_Module.lines) {
            content += line + "\n";
        }

        int i = 0;

        while (i < content.size()) {
            std::string &line = content;
            char ch = content[i];

            if (ch == ' ' || ch == 10) {
                SkipWhitespace(line, i);
                continue;
            }

            if (ch == '#') {
                CollectComment(line, i);
                continue;
            }

            if (ch == '/' && line[i + 1] == '*') {
                CollectMultilineComment(line, i);
                continue;
            }

            if (ch == '0' && line[i + 1] == 'x') {
                s_Module.tokens.push_back(CollectHex(line, i));
            }

            if (ch == '+' && line[i + 1] == '=') {
                i++;
                i++;
                s_Module.tokens.push_back(CreateToken(ADD_EQUALS, "+="));
                continue;
            }
            if (ch == '-' && line[i + 1] == '=') {
                i++;
                i++;
                s_Module.tokens.push_back(CreateToken(SUBTRACT_EQUALS, "-="));
                continue;
            }
            if (ch == '/' && line[i + 1] == '=') {
                i++;
                i++;
                s_Module.tokens.push_back(CreateToken(DIVIDE_EQUALS, "/="));
                continue;
            }
            if (ch == '*' && line[i + 1] == '=') {
                i++;
                i++;
                s_Module.tokens.push_back(CreateToken(MULTIPLY_EQUALS, "*="));
                continue;
            }
            if (ch == '^' && line[i + 1] == '=') {
                i++;
                i++;
                s_Module.tokens.push_back(CreateToken(POWER_EQUALS, "*="));
                continue;
            }

            if (isdigit(ch)) {
                s_Module.tokens.push_back(CollectNumber(line, i));
                continue;
            }

            if (isalnum(ch) || ch == '_') {
                s_Module.tokens.push_back(CollectID(line, i));
                continue;
            }

            if (ch == '{') {
                s_Module.tokens.push_back(CollectScope(line, i));
                continue;
            }

            if (ch == '"') {
                s_Module.tokens.push_back(CollectString(line, i));
                continue;
            }

            if (ch == '<') {
                s_Module.tokens.push_back(CollectObject(line, i));
                continue;
            }

            if (ch == '\'') {
                s_Module.tokens.push_back(CollectMathExpression(line, i));
                continue;
            }

            s_Module.tokens.push_back(CharToToken(ch));
            i++;
        }

        s_Module.tokens.push_back(CreateToken(_EOL, "\0"));
    }

    void ScriptEngine::JustRunTheScript(Module &s_Module) {
        ParseToTokens(s_Module);
        ExecuteModuleRoot(s_Module);
    }

    void PushArgumentToFunction(Function *func, const std::string &value) {
        func->arguments.push_back(Variable{GT_STRING, value, "content"});
        func->arguments.back().name = func->limitless_args ? uuid::generate_uuid_v4() : func->argument_names[func->arguments.size() - 1];
        // if (func->argument_names.size() > func->arguments.size()) {
        // throw std::runtime_error("Exceeded maximum arguments!");
        // }
    }

    void PushArgumentToFunction(Function *func, const std::string &value, GenericType type) {
        func->arguments.push_back(Variable{GT_STRING, value, "content"});
        func->arguments.back().name = func->limitless_args ? uuid::generate_uuid_v4() : func->argument_names[func->arguments.size() - 1];
        func->arguments.back().type = type;
        // if (func->argument_names.size() > func->arguments.size()) {
        // throw std::runtime_error("Exceeded maximum arguments!");
        // }
    }

    void PushArgumentToFunction(Function *func, Variable v) {
        func->arguments.push_back(v);
        func->arguments.back().name = func->limitless_args ? uuid::generate_uuid_v4() : func->argument_names[func->arguments.size() - 1];
        // if (func->argument_names.size() > func->arguments.size()) {
        // throw std::runtime_error("Exceeded maximum arguments!");
        // }
    }

    void PushArgumentNameToFunction(Function *func, const std::string &name) {
        func->argument_names.push_back(name);
    }

    double ScriptEngine::StringToMath(const std::string &math_expression) {
        exprtk::symbol_table<double> symbol_table;
        std::stringstream ss(math_expression);
        std::string line;
        std::unordered_map<std::string, double> symbols;

        while (std::getline(ss, line, ' ')) {
            auto v = GetVariable(line);
            if (v) {
                if (v->type == GT_NUMBER) {
                    symbols[v->name] = std::atoi(v->value.c_str());
                    symbol_table.add_variable(v->name, symbols[v->name]);
                }
            }
        }
        // symbol_table.add_variable("x", x);
        // symbol_table.add_variable("z", z);

        exprtk::expression<double> expression;
        expression.register_symbol_table(symbol_table);

        exprtk::parser<double> parser;

        parser.compile(math_expression, expression);

        return expression.value();
    }

    bool ScriptEngine::StringToBoolExpr(const std::string &math_expression) {
        MathExpr::ValueMap vars;
        std::stringstream ss(math_expression);
        std::string line;

        while (std::getline(ss, line, ' ')) {
            auto v = GetVariable(scope_variables, line);
            if (v) {
                switch (v->type) {
                case GT_NUMBER: {
                    vars[v->name] = std::atoi(v->value.c_str());
                    break;
                }
                case GT_BOOLEAN: {
                    vars[v->name] = v->value == "true" ? true : false;
                    break;
                }
                default: {
                    vars[v->name] = v->value;
                    break;
                }
                }
            }
        }
        MathExpr expr;

        float val = expr.eval(math_expression.c_str(), &vars).toNumber();

        return val ? true : false;
    }

    bool isInt(double val) {
        int truncated = (int)val;
        return (val == truncated);
    }

    std::string GenericTypeToString(GenericType type) {
        switch (type) {
        case GT_STRING: {
            return "GT_STRING";
        }
        case GT_NUMBER: {
            return "GT_NUMBER";
        }
        case GT_BOOLEAN: {
            return "GT_BOOLEAN";
        }
        case GT_OBJECT: {
            return "GT_OBJECT";
        }
        case GT_ANY: {
            return "GT_ANY";
        }
        case GT_LAMBDA: {
            return "GT_LAMBDA";
        }
        default: {
            return "INVALID_TYPE";
        }
        }
    }

    void Function::AddArgument(const std::string &name, const std::string &val) {
        bool found = false;
        for (auto &req : argument_names) {
            if (req == name) {
                found = true;
                break;
            }
        }

        if (!found)
            return;

        arguments.push_back(Variable{GT_STRING, val, name, false});
    }

    void ScriptEngine::__ExecuteModuleRoot(ScriptEngine::UnionOfState &state_union, Module &s_Module) {
        const static bool debug = true;
        auto *varBackup = state_union.variables;
        current_scope_variables = varBackup;

        auto SetState = [&](State state) {
            state_union.state = state;
        };

        auto SetVariableValue = [&](GenericType type, const std::string &value, std::string line, Variable *var = nullptr) {
            if (var == nullptr)
                var = state_union.value.variable;

            if (var->ref) {
                var = var->ref;
            }

            if (var) {
                for (auto addr : var->json_variable_tracking) {
                    FreeVariable(addr);
                }

                if (var->custom_type) {
                    if (var->custom_type->value_based) {
                        std::string values;
                        bool value_found = false;
                        for (auto &loopValue : var->custom_type->allowed_value) {
                            values += loopValue;
                            values += ", ";
                            if (loopValue == value) {
                                value_found = true;
                            }
                        }

                        if (!value_found) {
                            throw std::runtime_error("Variable " + var->name + " only accepts values: " + values);
                        }

                        var->value = value;
                        var->type = type;
                    } else {
                        std::string types;
                        bool type_found = false;
                        for (auto &loopType : var->custom_type->allowed_types) {
                            types += GenericTypeToString(loopType);
                            types += ", ";
                            if (loopType == type) {
                                type_found = true;
                            }
                        }

                        if (!type_found) {
                            throw std::runtime_error("Variable " + var->name + " only accepts types: " + types);
                        }

                        var->value = value;
                        var->type = type;
                    }
                } else {
                    if (var->readonly) {
                        throw std::runtime_error("Variable " + var->name + " is a constant (readonly)");
                    }
                    if (var->strict) {
                        if (var->type != type && var->type != GT_ANY) {
                            throw std::runtime_error("Variable is a strict type of \"" + GenericTypeToString(var->type) + "\" while you tried setting a value type of \"" + GenericTypeToString(type) + "\" to the variable!");
                        }
                    }
                }

                var->type = type;
                var->value = value;
                if (state_union.var_readonly) {
                    var->readonly = state_union.var_readonly;
                }
            }
        };

        auto SetLambdaValue = [&](GenericType type, std::function<void(Function *)> lambda_call, Variable *var = nullptr) {
            if (var == nullptr)
                var = state_union.value.variable;

            if (var->ref) {
                var = var->ref;
            }

            if (var) {
                for (auto addr : var->json_variable_tracking) {
                    FreeVariable(addr);
                }

                if (var->custom_type) {
                    if (var->custom_type->value_based) {
                        throw std::runtime_error("Cannot set a lambda on typevalue");
                    } else {
                        std::string types;
                        bool type_found = false;
                        for (auto &loopType : var->custom_type->allowed_types) {
                            types += GenericTypeToString(loopType);
                            types += ", ";
                            if (loopType == type) {
                                type_found = true;
                            }
                        }

                        if (!type_found) {
                            throw std::runtime_error("Variable " + var->name + " only accepts types: " + types);
                        }

                        var->isLambda = true;
                        var->func_lambda = CreateLambda(1, lambda_call);
                        var->type = type;
                    }
                } else {
                    if (var->readonly) {
                        throw std::runtime_error("Variable " + var->name + " is a constant (readonly)");
                    }
                    if (var->strict) {
                        if (var->type != type && var->type != GT_ANY) {
                            throw std::runtime_error("Variable is a strict type of \"" + GenericTypeToString(var->type) + "\" while you tried setting a value type of \"" + GenericTypeToString(type) + "\" to the variable!");
                        }
                    }
                }

                var->type = type;
                var->isLambda = true;
                var->func_lambda = CreateLambda(1, lambda_call);
                if (state_union.var_readonly) {
                    var->readonly = state_union.var_readonly;
                }
            }
        };

        bool isGlobal = false;
        bool isRef = false;

        int i = 0;
        Variable *var_backup = nullptr;
        Variable *var_array_backup = nullptr;
        bool functionReturnValueOnVariable = false;
        bool functionReturnValueOnVariable_isGlobal = false;
        bool return_called = false;
        bool break_called = false;
        bool continue_called = false;
        bool boolean_expression_save = false;
        std::string while_expr = "";
        std::shared_ptr<Variable::Type> typeof_ToSet = nullptr;

        for (auto &token : s_Module.tokens) {
            if (scope_func == state_union.func_scope_ref && scope_func != nullptr) {
                if (scope_func->break_loop) break;

                if (scope_func->continue_loop) {
                    scope_func->continue_loop = false;
                    break;
                }
            }

            if (return_called) {
                if (!scope_func) break;
                scope_func->break_loop = true;

                break;
            }

            if (break_called) {
                if (!scope_func) break;
                scope_func->break_loop = true;

                break;
            }

            if (continue_called) {
                if (!scope_func) break;
                scope_func->continue_loop = true;

                break;
            }

            if (debug) {
                std::cout << "MODULE: \"" << s_Module.module_name << "\" "
                          << "STATE: " << state_union.state << " TOKEN: " << token.token << "\n";
            }

            switch (token.token) {
            case SEMI: {
                SetState(NONE);
                break;
            }
            case BREAK: {
                break_called = true;
                break;
            }
            case CONTINUE: {
                SetState(NONE);
                continue_called = true;
                break;
            }
            case ID: {
                // std::cout << token.value << "\n";
                auto func = GetFunction(token.value);
                if (func) {
                    if (state_union.state != CHANGING_VARIABLE_VALUE && state_union.state != SETTING_VAR_VALUE)
                        SetState(FUNCTION_CALL);
                    state_union.value.function = func.get();
                }

                switch (state_union.state) {
                case LAMBDA_DECL: {
                    if (isGlobal) {
                        auto var = GetVariable(token.value);
                        if (!var) {
                            state_union.value.variable = CreateVariable(this->variables, GT_ANY, token.value).get();
                        } else {
                            state_union.value.variable = nullptr;
                        }
                    } else {
                        state_union.value.variable = CreateVariable(*state_union.variables, GT_ANY, token.value).get();
                    }

                    if (isRef) {
                        state_union.value.variable->isRef = true;
                        state_union.value.variable->ref = state_union.value.variable;
                        state_union.value.variable->AddRef(this);
                        isRef = false;
                    }

                    state_union.value.variable->strict = state_union.var_strict;
                    state_union.var_strict = false;

                    if (typeof_ToSet) {
                        state_union.value.variable->custom_type = typeof_ToSet;
                        typeof_ToSet = nullptr;
                    }

                    state_union.value.variable->isLambda = true;
                    state_union.value.variable->type = GT_LAMBDA;

                    var_backup = state_union.value.variable;
                    break;
                }
                case TEMPLATE_DEF: {
                    state_union.value.m_Template = CreateTemplate(token.value, "{}").get();
                    SetState(TEMPLATE_DEF_SET_VALUE);
                    break;
                }
                case TYPEDEF_INIT: {
                    std::vector<GenericType> a;
                    state_union.value.type = CreateTypeBasedType(token.value, a).get();
                    SetState(TYPEDEF_GET_TYPES);
                    break;
                }
                case TYPEVALUE_INIT: {
                    std::vector<std::string> a;
                    state_union.value.type = CreateValueBasedType(token.value, a).get();
                    SetState(TYPEVALUE_GET_TYPES);
                    break;
                }
                case SETTING_VAR_VALUE: {
                    if (func) {
                        functionReturnValueOnVariable = true;
                        functionReturnValueOnVariable_isGlobal = func->is_global;
                        SetState(FUNCTION_CALL);
                    } else {
                        auto var = GetVariable(scope_variables, token.value);
                        if (var) {
                            if (var->isRef && state_union.value.variable->isRef) {
                                var->AddRef(this);
                                state_union.value.variable->ref = var.get();
                            }

                            if (var->type == GT_ARRAY) {
                                SetState(GET_ELEMENT_FROM_ARRAY_AND_SET_VARIABLE);
                                var_backup = var.get();
                                break;
                                // if (state_union.state == VARIABLE_NAME_DETECTED) {
                                //     if (state_union.value.variable->type == GT_ARRAY) {
                                //         int index = std::atoi(token.value.c_str());
                                //         state_union.value.variable->json_handle[index];
                                //     }
                                // }
                            }

                            SetVariableValue(var->type, var->value, "679");
                        } else {
                            throw std::runtime_error("Invalid variable!");
                        }
                    }
                    break;
                }
                case ADDING_ARGUMENTS_TO_FUNCTION_DECL: {

                    PushArgumentNameToFunction(state_union.value.function, token.value);
                    break;
                }
                case CHANGING_VARIABLE_VALUE: {
                    if (func) {
                        functionReturnValueOnVariable = true;

                        SetState(FUNCTION_CALL);
                    } else {
                        auto var = GetVariable(scope_variables, token.value);
                        if (var) {
                            if (state_union.value.variable->array_value) {
                                switch (var->type) {
                                case GT_NUMBER: {
                                    *state_union.value.variable->array_value = std::atof(var->value.c_str());
                                    state_union.value.variable->array_value = nullptr;
                                    break;
                                }
                                case GT_STRING: {
                                    *state_union.value.variable->array_value = var->value;
                                    state_union.value.variable->array_value = nullptr;
                                    break;
                                }
                                case GT_BOOLEAN: {
                                    *state_union.value.variable->array_value = var->value == "true" ? true : false;
                                    state_union.value.variable->array_value = nullptr;
                                    break;
                                }
                                }
                            } else {
                                SetVariableValue(var->type, var->value, "702");
                            }
                        } else {
                            throw std::runtime_error("Invalid variable!");
                        }
                    }
                    break;
                }
                case RETURN_CALLED_SET_VALUE: {
                    auto var = GetVariable(scope_variables, token.value);
                    if (var) {
                        FuncReturnRef->SetReturn(var->value, var->type, var->custom_type);
                    } else {
                        throw std::runtime_error("Invalid variable name!");
                    }
                    return_called = true;
                    break;
                }
                case PUSHING_ARGUMENTS: {
                    auto var = GetVariable(scope_variables, token.value);
                    if (var) {
                        if (var->type == GT_ARRAY) {
                            var_array_backup = var.get();
                        } else {
                            std::cout << var->name << "\n";
                            PushArgumentToFunction(state_union.value.function, *var);
                        }
                    } else if (func) {

                        throw std::runtime_error("Invalid variable name!");
                    }
                    break;
                }
                case PUSHING_TO_ARRAY_DEF: {
                    auto var = GetVariable(scope_variables, token.value);
                    if (var) {
                        state_union.value.variable->PushToJSONArray(var->type, var->value);
                    } else {
                        throw std::runtime_error("Invalid variable name!");
                    }

                    break;
                }
                case VAR_DECLARATION: {
                    if (isGlobal) {
                        auto var = GetVariable(token.value);
                        if (!var) {
                            state_union.value.variable = CreateVariable(this->variables, GT_ANY, token.value).get();
                        } else {
                            state_union.value.variable = nullptr;
                        }
                    } else {
                        state_union.value.variable = CreateVariable(*state_union.variables, GT_ANY, token.value).get();
                    }

                    if (isRef) {
                        state_union.value.variable->isRef = true;
                        state_union.value.variable->ref = state_union.value.variable;
                        state_union.value.variable->AddRef(this);
                        isRef = false;
                    }

                    state_union.value.variable->strict = state_union.var_strict;
                    state_union.var_strict = false;

                    if (typeof_ToSet) {
                        state_union.value.variable->custom_type = typeof_ToSet;
                        typeof_ToSet = nullptr;
                    }

                    var_backup = state_union.value.variable;

                    break;
                }
                case PTR_DECLARATION: {
                    state_union.value.variable = CreateVariable(*state_union.variables, GT_NUMBER, token.value).get();
                    break;
                }
                case SETTING_PTR_VALUE: {
                    auto var = GetVariable(scope_variables, token.value);
                    if (var) {
                        SetVariableValue(GT_NUMBER, std::to_string(var->addr), "758");
                    } else {
                        throw std::runtime_error("Invalid variable name!");
                    }
                    break;
                }
                case OBJ_DECLARATION: {
                    state_union.value.variable = CreateVariable(*state_union.variables, GT_OBJECT, token.value).get();
                    if (!state_union.value.variable->strict)
                        state_union.value.variable->strict = state_union.var_strict;

                    state_union.var_strict = false;

                    if (!state_union.value.variable->readonly)
                        state_union.value.variable->readonly = state_union.var_readonly;

                    state_union.var_readonly = false;
                    std::cout << token.value << "\n";
                    break;
                }
                case FUNCTION_DECLARATION: {
                    state_union.value.function = CreateFunction(token.value, 0, [&](Function *f) { ExecuteFunctionScope(f); }).get();
                    break;
                }
                case FREE_VARIABLE: {
                    auto var = GetVariable(scope_variables, token.value);
                    if (!var)
                        break;

                    FreeVariable(var->addr);

                    break;
                }
                case NONE: {
                    auto var = GetVariable(scope_variables, token.value);

                    if (!var)
                        break;

                    state_union.value.variable = var.get();
                    var_backup = state_union.value.variable;

                    if (var && state_union.state == PUSH_FIND_ARRAY_VAR) {
                        SetState(PUSH_TO_ARRAY);
                        break;
                    }

                    if (var && var->isLambda) {

                        // state_union.value.function = (Function *)var->func_lambda.get();
                        SetState(SETTING_LAMBDA_VALUE);
                        break;
                    }

                    SetState(VARIABLE_NAME_DETECTED);

                    break;
                }
                default: {
                    break;
                }
                }

                if (state_union.state == NONE) {
                    throw std::runtime_error("Invalid keyword or a function/variable \"" + token.value + "\"");
                }

                break;
            }
            case L_PAREN: {
                if (state_union.state == SETTING_LAMBDA_VALUE) {
                    state_union.value.function = (Function *)state_union.value.variable->func_lambda.get();
                    SetState(FUNCTION_CALL);
                }

                if (state_union.state == CHANGING_VARIABLE_VALUE) {
                    SetLambdaValue(GT_LAMBDA, [&](Function *f) { ExecuteFunctionScope(f); });
                    state_union.value.function = (Function *)state_union.value.variable->func_lambda.get();
                    SetState(FUNCTION_DECLARATION);
                }

                if (state_union.state == FUNCTION_DECLARATION) {
                    SetState(ADDING_ARGUMENTS_TO_FUNCTION_DECL);
                } else {
                    SetState(state_union.state == FUNCTION_CALL ? PUSHING_ARGUMENTS : state_union.state);
                }
                state_union.previous_bracket = &token;
                break;
            }
            case R_PAREN: {
                if (state_union.previous_bracket->token != L_PAREN)
                    throw std::runtime_error("No L_PAREN!");

                if (state_union.state == PUSHING_ARGUMENTS) {
                    if (var_array_backup) {
                        PushArgumentToFunction(state_union.value.function, *var_array_backup);
                    }
                    state_union.value.function->Call();
                }

                if (functionReturnValueOnVariable) {
                    if (state_union.value.function->return_value.value != "\0") {
                        if (var_backup->array_value) {
                            switch (state_union.value.function->return_value.type) {
                            case GT_NUMBER: {
                                *var_backup->array_value = std::atof(state_union.value.function->return_value.value.c_str());
                                var_backup->array_value = nullptr;
                                break;
                            }
                            case GT_STRING: {
                                *var_backup->array_value = state_union.value.function->return_value.value;
                                var_backup->array_value = nullptr;
                                break;
                            }
                            case GT_BOOLEAN: {
                                *var_backup->array_value = state_union.value.function->return_value.value == "true" ? true : false;
                                var_backup->array_value = nullptr;
                                break;
                            }
                            }
                            *var_backup->array_value = token.value;
                            var_backup->array_value = nullptr;
                        } else {
                            SetVariableValue(state_union.value.function->return_value.type, state_union.value.function->return_value.value, "828", var_backup);
                        }
                    }

                    functionReturnValueOnVariable = false;
                    var_backup = nullptr;
                    var_array_backup = nullptr;
                }

                if (state_union.state == ADDING_ARGUMENTS_TO_FUNCTION_DECL) {
                    SetState(FUNCTION_DECLARATION);
                } else {
                    SetState(NONE);
                }
                break;
            }
            case L_SQUARE_BRACKET: {
                switch (state_union.state) {
                case SETTING_VAR_VALUE: {
                    state_union.value.variable->json_handle = nlohmann::json::parse("[]");
                    state_union.value.variable->type = GT_ARRAY;
                    SetState(PUSHING_TO_ARRAY_DEF);
                    break;
                }
                case CHANGING_VARIABLE_VALUE: {
                    state_union.value.variable->json_handle = nlohmann::json::parse("[]");
                    state_union.value.variable->type = GT_ARRAY;
                    SetState(PUSHING_TO_ARRAY_DEF);
                }
                }
                break;
            }
            case R_SQUARE_BRACKET: {
                SetState(NONE);
                break;
            }
            case STRING: {
                switch (state_union.state) {
                case PUSHING_ARGUMENTS: {
                    PushArgumentToFunction(state_union.value.function, token.value, GT_STRING);
                    break;
                }
                case PUSHING_TO_ARRAY_DEF: {
                    state_union.value.variable->PushToJSONArray(GT_STRING, token.value);
                    break;
                }
                case SETTING_VAR_VALUE: {
                    SetVariableValue(GT_STRING, token.value, "849");
                    break;
                }
                case CHANGING_VARIABLE_VALUE: {
                    SetState(NONE);

                    if (state_union.value.variable->array_value) {
                        *state_union.value.variable->array_value = token.value;
                        state_union.value.variable->array_value = nullptr;
                    } else {
                        SetVariableValue(GT_STRING, token.value, "853");
                    }

                    break;
                }
                case RETURN_CALLED_SET_VALUE: {
                    FuncReturnRef->SetReturn(token.value, GT_STRING);
                    return_called = true;
                    break;
                }
                case CHANGING_VARIABLE_VALUE_OPERATOR: {
                    state_union.value.variable->value += token.value;
                    break;
                }
                default: {
                    break;
                }
                }
                break;
            }
            case TRUE: {
                switch (state_union.state) {
                case PUSHING_ARGUMENTS: {
                    PushArgumentToFunction(state_union.value.function, token.value, GT_BOOLEAN);
                    break;
                }
                case PUSHING_TO_ARRAY_DEF: {
                    state_union.value.variable->PushToJSONArray(GT_BOOLEAN, token.value);
                    break;
                }
                case SETTING_VAR_VALUE: {
                    SetVariableValue(GT_BOOLEAN, token.value, "873");
                    break;
                }
                case CHANGING_VARIABLE_VALUE: {
                    SetState(NONE);

                    if (state_union.value.variable->array_value) {
                        *state_union.value.variable->array_value = token.value == "true" ? true : false;
                        state_union.value.variable->array_value = nullptr;
                    } else {
                        SetVariableValue(GT_BOOLEAN, token.value, "877");
                    }
                    break;
                }
                case RETURN_CALLED_SET_VALUE: {
                    if (state_union.m_Func) {
                        FuncReturnRef->SetReturn(token.value, GT_BOOLEAN);
                        return_called = true;
                    }
                    break;
                }
                case PUSH_TO_ARRAY: {
                    state_union.value.variable->json_handle.push_back(true);

                    break;
                }
                default: {
                    break;
                }
                }
                break;
            }
            case FALSE: {
                switch (state_union.state) {
                case PUSHING_ARGUMENTS: {
                    PushArgumentToFunction(state_union.value.function, token.value, GT_BOOLEAN);
                    break;
                }
                case SETTING_VAR_VALUE: {
                    SetVariableValue(GT_BOOLEAN, token.value, "899");
                    break;
                }
                case PUSHING_TO_ARRAY_DEF: {
                    state_union.value.variable->PushToJSONArray(GT_BOOLEAN, token.value);
                    break;
                }
                case CHANGING_VARIABLE_VALUE: {
                    SetState(NONE);
                    SetVariableValue(GT_BOOLEAN, token.value, "903");
                    break;
                }
                case RETURN_CALLED_SET_VALUE: {
                    if (state_union.m_Func) {
                        FuncReturnRef->SetReturn(token.value, GT_BOOLEAN);
                        return_called = true;
                    }
                    break;
                }
                case PUSH_TO_ARRAY: {
                    state_union.value.variable->json_handle.push_back(false);

                    break;
                }
                default: {
                    break;
                }
                }
                break;
            }
            case VAR: {
                if (state_union.state == GLOBAL_VARIABLE_SETTING) {
                    isGlobal = true;
                }

                if (state_union.state == REF_VARIABLE_SETTING) {
                    isRef = true;
                }

                SetState(VAR_DECLARATION);
                break;
            }
            case REF: {
                SetState(REF_VARIABLE_SETTING);
                break;
            }
            case PTR: {
                SetState(PTR_DECLARATION);
                break;
            }
            case OBJ: {
                SetState(OBJ_DECLARATION);
                break;
            }
            case OBJECT_SCOPE: {
                if (state_union.state == TEMPLATE_DEF_SET_VALUE) {
                    state_union.value.m_Template->json_value = token.value;
                }

                if (state_union.state == VARIABLE_NAME_DETECTED) {
                    if (state_union.value.variable->type == GT_ARRAY) {
                        auto v = GetVariable(scope_variables, token.value);
                        int index = -1;

                        if (v) {
                            index = std::atoi(v->value.c_str());
                        } else {
                            index = std::atoi(token.value.c_str());
                        }

                        state_union.value.variable->array_value = &state_union.value.variable->json_handle[index];
                        state_union.value.variable->array_value = nullptr;
                    }
                }

                if (state_union.state == PUSHING_ARGUMENTS && var_array_backup) {
                    if (var_array_backup->type == GT_ARRAY) {
                        auto v = GetVariable(scope_variables, token.value);
                        int index = -1;

                        if (v) {
                            index = std::atoi(v->value.c_str());
                        } else {
                            index = std::atoi(token.value.c_str());
                        }

                        auto j = var_array_backup->json_handle[index];
                        if (j.is_number()) {
                            PushArgumentToFunction(state_union.value.function, std::to_string((double)j), GT_NUMBER);
                        }

                        if (j.is_string()) {
                            PushArgumentToFunction(state_union.value.function, j, GT_STRING);
                        }

                        if (j.is_boolean()) {
                            PushArgumentToFunction(state_union.value.function, j ? "true" : "false", GT_BOOLEAN);
                        }

                        if (j.is_array()) {
                            PushArgumentToFunction(state_union.value.function, nlohmann::to_string(j), GT_ARRAY);
                        }

                        if (j.is_object()) {
                            PushArgumentToFunction(state_union.value.function, nlohmann::to_string(j), GT_OBJECT);
                        }

                        var_array_backup = nullptr;
                    }
                }

                if (state_union.state == GET_ELEMENT_FROM_ARRAY_AND_SET_VARIABLE) {
                    auto v = GetVariable(scope_variables, token.value);
                    int index = -1;

                    if (v) {
                        index = std::atoi(v->value.c_str());
                    } else {
                        index = std::atoi(token.value.c_str());
                    }

                    auto j = var_backup->json_handle[index];
                    if (j.is_number()) {
                        SetVariableValue(GT_NUMBER, std::to_string((double)j), "1079");
                    }

                    if (j.is_string()) {
                        SetVariableValue(GT_STRING, j, "1083");
                    }

                    if (j.is_boolean()) {
                        SetVariableValue(GT_BOOLEAN, j ? "true" : "false", "1083");
                    }

                    if (j.is_array()) {
                        SetVariableValue(GT_ARRAY, nlohmann::to_string(j), "1091");
                    }

                    if (j.is_object()) {
                        SetVariableValue(GT_OBJECT, nlohmann::to_string(j), "1095");
                    }
                }

                if (state_union.state == OBJ_DECLARATION) {
                    std::string temp = token.value;
                    ltrim(temp);
                    rtrim(temp);

                    auto s_Template = GetTemplate(temp);
                    if (s_Template)
                        state_union.value.variable->value = s_Template->json_value;
                    else
                        state_union.value.variable->value = token.value;

                    state_union.value.variable->LoadValueToJSONHandle(this);
                }

                if (state_union.state == TYPEDEF_GET_TYPES) {
                    std::stringstream ss(token.value);
                    std::string line;
                    std::vector<GenericType> types;

                    while (std::getline(ss, line, '|')) {
                        ltrim(line);
                        rtrim(line);

                        for (auto &strType : generic_types) {
                            if (strType.str == line) {
                                state_union.value.type->allowed_types.push_back(strType.type);
                            }
                        }
                    }
                }

                if (state_union.state == TYPEVALUE_GET_TYPES) {
                    std::stringstream ss(token.value);
                    std::string line;
                    std::vector<std::string> types;

                    while (std::getline(ss, line, '|')) {
                        ltrim(line);
                        rtrim(line);

                        state_union.value.type->allowed_value.push_back(line);
                    }
                }

                if (state_union.state == TYPEOF_GET_TYPENAME_AND_SET) {
                    std::string no_space_value = token.value;
                    std::string::iterator end_pos = std::remove(no_space_value.begin(), no_space_value.end(), ' ');
                    no_space_value.erase(end_pos, no_space_value.end());

                    auto custom_type = GetType(no_space_value);

                    if (custom_type) {
                        typeof_ToSet = custom_type;
                    } else {
                        throw std::runtime_error("Invalid type of \"" + no_space_value + "\"");
                    }
                    SetState(NONE);
                }

                break;
            }
            case EQUALS: {
                switch (state_union.state) {
                case SETTING_LAMBDA_VALUE: {
                    state_union.value.function = (Function *)state_union.value.variable->func_lambda.get();
                    SetState(FUNCTION_DECLARATION);
                    break;
                }
                case VAR_DECLARATION: {
                    SetState(SETTING_VAR_VALUE);
                    break;
                }
                case PTR_DECLARATION: {
                    SetState(SETTING_PTR_VALUE);
                    break;
                }
                case VARIABLE_NAME_DETECTED: {
                    SetState(CHANGING_VARIABLE_VALUE);
                    break;
                }
                case LAMBDA_DECL: {
                    state_union.value.variable->func_lambda = CreateLambda(1, [&](Function *func) { ExecuteFunctionScope(func); });
                    state_union.value.function = (Function *)state_union.value.variable->func_lambda.get();
                    SetState(FUNCTION_DECLARATION);
                    break;
                }
                }

                break;
            }
            case ADD_EQUALS: {
                switch (state_union.state) {
                case VARIABLE_NAME_DETECTED: {
                    SetState(CHANGING_VARIABLE_VALUE_OPERATOR);
                    break;
                }
                }

                break;
            }
            case SUBTRACT_EQUALS: {
                switch (state_union.state) {
                case VARIABLE_NAME_DETECTED: {
                    SetState(CHANGING_VARIABLE_VALUE_OPERATOR_SUBTRACT);
                    break;
                }
                }

                break;
            }

            case DIVIDE_EQUALS: {
                switch (state_union.state) {
                case VARIABLE_NAME_DETECTED: {
                    SetState(CHANGING_VARIABLE_VALUE_OPERATOR_DIVIDE);
                    break;
                }
                }

                break;
            }
            case MULTIPLY_EQUALS: {
                switch (state_union.state) {
                case VARIABLE_NAME_DETECTED: {
                    SetState(CHANGING_VARIABLE_VALUE_OPERATOR_MULTIPLY);
                    break;
                }
                }

                break;
            }
            case POWER_EQUALS: {
                switch (state_union.state) {
                case VARIABLE_NAME_DETECTED: {
                    SetState(CHANGING_VARIABLE_VALUE_OPERATOR_POWER);
                    break;
                }
                }

                break;
            }
            case NUMBER: {
                if (state_union.state == PUSHING_ARGUMENTS) {
                    PushArgumentToFunction(state_union.value.function, token.value, GT_NUMBER);
                }

                if (state_union.state == SETTING_VAR_VALUE || state_union.state == CHANGING_VARIABLE_VALUE) {
                    SetVariableValue(GT_NUMBER, token.value, "1020");
                }

                if (state_union.state == RETURN_CALLED_SET_VALUE) {
                    FuncReturnRef->SetReturn(token.value, GT_NUMBER);
                    return_called = true;
                }

                if (state_union.state == CHANGING_VARIABLE_VALUE_OPERATOR) {
                    if (!state_union.value.variable->readonly) {
                        state_union.value.variable->value = std::to_string(std::atof(state_union.value.variable->value.c_str()) + std::atof(token.value.c_str()));

                    } else {
                        throw std::runtime_error("Tried setting a readonly variable");
                    }
                }

                if (state_union.state == CHANGING_VARIABLE_VALUE_OPERATOR_SUBTRACT) {

                    if (!state_union.value.variable->readonly) {
                        state_union.value.variable->value = std::to_string(std::atof(state_union.value.variable->value.c_str()) - std::atof(token.value.c_str()));

                    } else {
                        throw std::runtime_error("Tried setting a readonly variable");
                    }
                }

                if (state_union.state == CHANGING_VARIABLE_VALUE_OPERATOR_DIVIDE) {

                    if (!state_union.value.variable->readonly) {
                        state_union.value.variable->value = std::to_string(std::atof(state_union.value.variable->value.c_str()) / std::atof(token.value.c_str()));

                    } else {
                        throw std::runtime_error("Tried setting a readonly variable");
                    }
                }

                if (state_union.state == CHANGING_VARIABLE_VALUE_OPERATOR_MULTIPLY) {

                    if (!state_union.value.variable->readonly) {
                        state_union.value.variable->value = std::to_string(std::atof(state_union.value.variable->value.c_str()) * std::atof(token.value.c_str()));

                    } else {
                        throw std::runtime_error("Tried setting a readonly variable");
                    }
                }

                if (state_union.state == CHANGING_VARIABLE_VALUE_OPERATOR_POWER) {

                    if (!state_union.value.variable->readonly) {
                        state_union.value.variable->value = std::to_string(pow(std::atof(state_union.value.variable->value.c_str()), std::atof(token.value.c_str())));

                    } else {
                        throw std::runtime_error("Tried setting a readonly variable");
                    }
                }

                if (state_union.state == PUSH_TO_ARRAY) {
                    state_union.value.variable->json_handle.push_back(std::atof(token.value.c_str()));
                }

                if (state_union.state == PUSHING_TO_ARRAY_DEF) {
                    state_union.value.variable->PushToJSONArray(GT_NUMBER, token.value);
                }
                break;
            }
            case DELETE: {
                SetState(FREE_VARIABLE);
                break;
            }
            case FUNC: {
                SetState(FUNCTION_DECLARATION);
                break;
            }
            case FUNCTION_SCOPE: {
                if (state_union.state == TEMPLATE_DEF_SET_VALUE) {
                    state_union.value.m_Template->json_value = "{" + token.value + "}";
                }

                if (state_union.state == FUNCTION_DECLARATION) {
                    state_union.value.function->is_global = false;
                    std::stringstream ss(token.value);
                    std::string s_Line;
                    while (std::getline(ss, s_Line)) {
                        state_union.value.function->scope_module.lines.push_back(s_Line);
                    }

                    ParseToTokens(state_union.value.function->scope_module);
                }

                if (state_union.state == IF_CHECK_DONE_CALL_SCOPE || state_union.state == ELSE_CHECKED_CALL_SCOPE || state_union.state == ELIF_CHECKED_CALL_SCOPE || state_union.state == WHILE_CHECKED_CALL_SCOPE) {
                    std::string funcID = uuid::generate_uuid_v4();
                    auto func = CreateFunction(funcID, 0, [&](Function *f) { ExecuteIfScope(f); });
                    std::stringstream ss(token.value);
                    std::string s_Line;
                    while (std::getline(ss, s_Line)) {
                        func->scope_module.lines.push_back(s_Line);
                    }

                    ParseToTokens(func->scope_module);
                    if (state_union.state == WHILE_CHECKED_CALL_SCOPE) {
                        func->expr = while_expr;
                        scope_func = func.get();

                        func->CallWhileLoop(this);

                        while_expr = "";
                    } else {
                        func->Call();
                    }
                    functions.erase(functions.find(funcID));
                }

                if (state_union.state != IF_CHECK_DONE_CALL_SCOPE)
                    state_union.state = NONE;
                break;
            }
            case ELSE: {
                if (!boolean_expression_save) {
                    SetState(ELSE_CHECKED_CALL_SCOPE);
                }
                break;
            }
            case ELSE_IF: {
                if (boolean_expression_save == false) {
                    SetState(ELIF_CHECK);
                }
                break;
            }
            case GLOBAL: {
                SetState(GLOBAL_VARIABLE_SETTING);
                state_union.variables = &this->variables;
                break;
            }
            case STRICT: {
                state_union.var_strict = true;
                break;
            }
            case CONST: {
                state_union.var_readonly = true;
                break;
            }
            case MATH_EXPRESSION: {
                switch (state_union.state) {
                case PUSHING_ARGUMENTS: {
                    double val = StringToMath(token.value);
                    PushArgumentToFunction(state_union.value.function, std::to_string(isInt(val) ? (int)val : val), GT_NUMBER);
                    break;
                }
                case SETTING_VAR_VALUE: {
                    double val = StringToMath(token.value);
                    // WHY DOESNT THIS FUCKING JUST WHY OH MY FUCKING GOD
                    if (isInt(val)) {
                        SetVariableValue(GT_NUMBER, std::to_string((int)val), "1112");
                    } else {
                        SetVariableValue(GT_NUMBER, std::to_string(val), "1114");
                    }

                    break;
                }
                case PUSHING_TO_ARRAY_DEF: {
                    double val = StringToMath(token.value);
                    state_union.value.variable->PushToJSONArray(GT_STRING, std::to_string(val));
                    break;
                }
                case PUSH_TO_ARRAY: {
                    double val = StringToMath(token.value);
                    state_union.value.variable->json_handle.push_back(val);

                    break;
                }
                case RETURN_CALLED_SET_VALUE: {
                    double val = StringToMath(token.value);
                    // WHY DOESNT THIS FUCKING JUST WHY OH MY FUCKING GOD
                    if (isInt(val)) {
                        FuncReturnRef->SetReturn(std::to_string((int)val), GT_NUMBER);
                    } else {
                        FuncReturnRef->SetReturn(std::to_string(val), GT_STRING);
                    }
                    return_called = true;
                    break;
                }
                case CHANGING_VARIABLE_VALUE: {
                    SetState(NONE);
                    double val = StringToMath(token.value);

                    if (state_union.value.variable->array_value) {
                        *state_union.value.variable->array_value = isInt(val) ? (int)val : val;
                        state_union.value.variable->array_value = nullptr;
                    } else {
                        SetVariableValue(GT_NUMBER, std::to_string(val), "1477");
                    }
                    break;
                }
                case IF_CHECK: {
                    bool b = StringToBoolExpr(token.value);
                    if (b)
                        SetState(IF_CHECK_DONE_CALL_SCOPE);
                    else
                        SetState(NONE);

                    boolean_expression_save = b;

                    break;
                }
                case WHILE_CHECK: {
                    bool b = StringToBoolExpr(token.value);
                    if (b) {
                        SetState(WHILE_CHECKED_CALL_SCOPE);
                        while_expr = token.value;
                    } else
                        SetState(NONE);

                    boolean_expression_save = b;

                    break;
                }
                case ELIF_CHECK: {
                    bool b = StringToBoolExpr(token.value);
                    if (b)
                        SetState(ELIF_CHECKED_CALL_SCOPE);
                    else
                        SetState(NONE);

                    boolean_expression_save = b;

                    break;
                }
                default: {
                    break;
                }
                }
                break;
            }
            case BOOLEAN_EXPRESSION: {
                switch (state_union.state) {
                case PUSHING_ARGUMENTS: {
                    bool val = StringToBoolExpr(token.value);
                    PushArgumentToFunction(state_union.value.function, val ? "true" : "false", GT_BOOLEAN);

                    break;
                }
                case PUSHING_TO_ARRAY_DEF: {
                    bool val = StringToBoolExpr(token.value);
                    state_union.value.variable->PushToJSONArray(GT_STRING, val ? "true" : "false");
                    break;
                }
                case SETTING_VAR_VALUE: {
                    bool val = StringToBoolExpr(token.value);
                    SetVariableValue(GT_BOOLEAN, val ? "true" : "false", "1186");

                    break;
                }
                case CHANGING_VARIABLE_VALUE: {
                    SetState(NONE);
                    bool val = StringToBoolExpr(token.value);

                    if (state_union.value.variable->array_value) {
                        *state_union.value.variable->array_value = val;
                        state_union.value.variable->array_value = nullptr;
                    } else {
                        SetVariableValue(GT_BOOLEAN, val ? "true" : "false", "1192");
                    }

                    break;
                }
                case WHILE_CHECK: {
                    bool b = StringToBoolExpr(token.value);
                    if (b) {
                        SetState(WHILE_CHECKED_CALL_SCOPE);
                        while_expr = token.value;
                    } else
                        SetState(NONE);

                    boolean_expression_save = b;

                    break;
                }
                case RETURN_CALLED_SET_VALUE: {
                    bool b = StringToBoolExpr(token.value);
                    FuncReturnRef->SetReturn(b ? "true" : "false", GT_BOOLEAN);
                    return_called = true;
                    break;
                }
                case IF_CHECK: {
                    bool b = StringToBoolExpr(token.value);
                    if (b)
                        SetState(IF_CHECK_DONE_CALL_SCOPE);
                    else
                        SetState(NONE);

                    boolean_expression_save = b;

                    break;
                }
                case ELIF_CHECK: {
                    bool b = StringToBoolExpr(token.value);
                    if (b)
                        SetState(ELIF_CHECKED_CALL_SCOPE);
                    else
                        SetState(NONE);

                    boolean_expression_save = b;

                    break;
                }
                default: {
                    break;
                }
                }
                break;
            }
            case RETURN: {
                if (!FuncReturnRef) {
                    return_called = true;
                    break;
                }

                SetState(RETURN_CALLED_SET_VALUE);

                break;
            }
            case IF: {
                SetState(IF_CHECK);
                break;
            }
            case WHILE: {
                SetState(WHILE_CHECK);
                break;
            }
            case TYPEDEF: {
                SetState(TYPEDEF_INIT);
                break;
            }
            case TYPEVALUE: {
                SetState(TYPEVALUE_INIT);
                break;
            }
            case TYPEOF: {
                SetState(TYPEOF_GET_TYPENAME_AND_SET);
                break;
            }
            case PUSH: {
                SetState(PUSH_FIND_ARRAY_VAR);
                break;
            }
            case TEMPLATE: {
                SetState(TEMPLATE_DEF);
                break;
            }
            case LAMBDA: {
                SetState(LAMBDA_DECL);
                break;
            }
            default:
                break;
            }
        }

        SetState(NONE);
    }

    void ScriptEngine::ExecuteFunctionScope(Function *func) {
        UnionOfState state_union;
        state_union.variables = &func->variables;
        state_union.state = NONE;
        state_union.m_Func = func;
        FuncReturnRef = func;

        scope_func = func;
        func->scope_module.module_name = func->name;
        state_union.func_scope_ref = func;

        scope_variables.push_back(&func->variables);
        for (auto &arg : func->arguments) {
            auto var = CreateVariable(*scope_variables.back(), arg.type, arg.name, arg.value);
            var->ref = arg.ref;
            var->isRef = arg.isRef;
            var->ref_count = arg.ref_count;
            var->isLambda = arg.isLambda;
            var->func_lambda = arg.func_lambda;
            var->json_handle = arg.json_handle;
            var->strict = arg.strict;
            var->readonly = arg.readonly;
            var->custom_type = arg.custom_type;
        }
        __ExecuteModuleRoot(state_union, func->scope_module);
        scope_variables.pop_back();

        FuncReturnRef = nullptr;
    }

    void ScriptEngine::ExecuteIfScope(Function *func) {
        UnionOfState state_union;
        state_union.variables = &func->variables;
        state_union.state = NONE;
        state_union.func_scope_ref = func;

        scope_variables.push_back(&func->variables);
        __ExecuteModuleRoot(state_union, func->scope_module);
        scope_variables.pop_back();
    }

    void ScriptEngine::ExecuteModuleRoot(Module &s_Module) {
        UnionOfState state_union;

        state_union.variables = &variables;
        __ExecuteModuleRoot(state_union, s_Module);
    }

    std::shared_ptr<Function> ScriptEngine::CreateFunction(const std::string &name, int argc, std::function<void(Function *)> callback_func) {
        functions[name] = std::make_shared<Function>();
        functions[name]->name = name;
        functions[name]->argument_count = argc;
        functions[name]->call = callback_func;
        functions[name]->engine = this;

        std::cout << "Created function " << name << "\n";
        return functions[name];
    }

    std::shared_ptr<Function> ScriptEngine::CreateLambda(int argc, std::function<void(Function *)> callback_func) {
        auto pFunc = std::make_shared<Function>();
        pFunc->name = uuid::generate_uuid_v4();
        pFunc->argument_count = argc;
        pFunc->call = callback_func;
        pFunc->engine = this;

        std::cout << "Created lambda " << pFunc->name << "\n";
        return pFunc;
    }

    std::shared_ptr<Function> ScriptEngine::GetFunction(const std::string &name) {
        auto f = functions.find(name);
        return f != functions.end() ? f->second : nullptr;
    }
    std::shared_ptr<Variable> ScriptEngine::CreateVariable(GenericType type, const std::string &name, const std::string &value) {
        current_max_address += 4096;
        variables[current_max_address - 4096] = std::make_shared<Variable>(type, value, name, current_max_address - 4096);
        variables[current_max_address - 4096]->addr = current_max_address - 4096;
        return variables[current_max_address - 4096];
    }

    std::shared_ptr<Variable> ScriptEngine::CreateVariable(std::unordered_map<uint64_t, std::shared_ptr<Variable>> &variables, GenericType type, const std::string &name, const std::string &value) {
        current_max_address += 4096;
        variables[current_max_address - 4096] = std::make_shared<Variable>(type, value, name, current_max_address - 4096);
        variables[current_max_address - 4096]->addr = current_max_address - 4096;
        return variables[current_max_address - 4096];
    }

    std::shared_ptr<Variable> ScriptEngine::CreateVariable(std::unordered_map<uint64_t, std::shared_ptr<Variable>> &variables, Variable v) {
        current_max_address += 4096;
        variables[current_max_address - 4096] = std::make_shared<Variable>();
        memcpy(variables[current_max_address - 4096].get(), &v, sizeof(Variable));
        return variables[current_max_address - 4096];
    }

    std::shared_ptr<Variable> ScriptEngine::GetVariable(const std::string &name) {
        for (auto &var : variables) {
            if (var.second->name == name)
                return var.second;
        }

        return nullptr;
    }
    std::shared_ptr<Variable> ScriptEngine::GetVariable(const uint64_t addr) {
        auto f = variables.find(addr);
        return f != variables.end() ? f->second : nullptr;
    }

    std::shared_ptr<Variable> ScriptEngine::GetVariable(std::vector<std::unordered_map<uint64_t, std::shared_ptr<Variable>> *> &variables, const std::string &name) {
        if (variables.size() > 0) {
            for (auto *scope : variables) {
                for (auto &var : *scope) {
                    if (var.second->name == name)
                        return var.second;
                }
            }
        }

        for (auto &var : this->variables) {
            if (var.second->name == name)
                return var.second;
        }

        return nullptr;
    }

    std::shared_ptr<Variable> ScriptEngine::GetVariable(std::vector<std::unordered_map<uint64_t, std::shared_ptr<Variable>> *> &variables, const uint64_t addr) {
        if (variables.size() > 0) {
            for (auto *scope : variables) {
                for (auto &var : *scope) {
                    if (var.second->addr == addr)
                        return var.second;
                }
            }
        }

        auto f = this->variables.find(addr);

        if (f != this->variables.end()) {
            return f->second;
        }

        return nullptr;
    }

    void ScriptEngine::FreeVariable(const uint64_t addr) {
        auto f = variables.find(addr);

        if (f == variables.end()) return;

        if (f->second->isRef) {
            f->second->RemoveRef(this, true);

            if (f->second->ref_count > 0) return;
        }

        variables.erase(f);

        current_max_address -= 4096;
    }
    void ScriptEngine::FreeVariable(const std::string &name) {
        for (auto &var : variables) {
            if (var.second->name != name) continue;

            if (var.second->isRef) {
                var.second->RemoveRef(this, true);
                if (var.second->ref_count > 0) return;
            }

            variables.erase(variables.find(var.first));
            current_max_address -= 4096;
            break;
        }
    }

    std::shared_ptr<Variable::Type> &ScriptEngine::CreateValueBasedType(const std::string &name, std::vector<std::string> &accepted_values) {
        typedefs[name] = std::make_shared<Variable::Type>();
        typedefs[name]->value_based = true;
        typedefs[name]->allowed_value = accepted_values;
        return typedefs[name];
    }

    std::shared_ptr<Variable::Type> &ScriptEngine::CreateTypeBasedType(const std::string &name, std::vector<GenericType> &accepted_types) {
        typedefs[name] = std::make_shared<Variable::Type>();
        typedefs[name]->value_based = false;
        typedefs[name]->allowed_types = accepted_types;
        return typedefs[name];
    }

    std::shared_ptr<Variable::Type> ScriptEngine::GetType(const std::string &name) {
        auto f = typedefs.find(name);
        return f != typedefs.end() ? f->second : nullptr;
    }

    std::shared_ptr<Template> ScriptEngine::GetTemplate(const std::string &name) {
        auto f = structure_templates.find(name);
        return f != structure_templates.end() ? f->second : nullptr;
    }

    std::shared_ptr<Template> ScriptEngine::CreateTemplate(const std::string &name, const std::string &json_data) {
        structure_templates[name] = std::make_shared<Template>(json_data, name);
        return structure_templates[name];
    }

} // namespace HyperScript