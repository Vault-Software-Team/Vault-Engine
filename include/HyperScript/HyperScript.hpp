#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <random>
#include <json.hpp>

// trash garbage code for a shitty language

namespace HyperScript {
    void ltrim(std::string &s);
    void rtrim(std::string &s);

    enum EToken {
        ID,
        EQUALS,
        AMPERSAND,
        STRING,
        SEMI,
        L_PAREN,
        R_PAREN,
        R_BRACKET,
        L_BRACKET,
        L_SQUARE_BRACKET,
        R_SQUARE_BRACKET,
        NUMBER,
        COMMA,
        VAR,
        PTR,
        OBJ,
        OBJECT_SCOPE,
        TRUE,
        FALSE,
        DELETE,
        PIPE,
        FUNCTION_SCOPE,
        FUNC,
        MATH_EXPRESSION,
        BOOLEAN_EXPRESSION,
        GLOBAL,
        STRICT,
        RETURN,
        IF,
        CONST,
        ELSE,
        ELSE_IF,
        WHILE,
        HEXNUM,
        ADD_EQUALS,
        SUBTRACT_EQUALS,
        DIVIDE_EQUALS,
        MULTIPLY_EQUALS,
        POWER_EQUALS,
        PLUSPLUS,
        MINUSMINUS,
        TYPEDEF,
        TYPEVALUE,
        TYPEOF,
        DOT,
        PUSH,
        REF,
        STRUCT,
        DATA,
        TEMPLATE,
        LAMBDA,
        _NULL,
        _EOL,
        COMMENT,
        BREAK,
        CONTINUE
    };

    struct Token {
        EToken token;
        std::string value;
    };

    struct Module {
        std::vector<std::string> lines;
        std::string module_name = "";
        std::string script_path = "";
        std::vector<Token> tokens;
    };

    enum GenericType {
        GT_STRING,
        GT_NUMBER,
        GT_BOOLEAN,
        GT_OBJECT,
        GT_ANY,
        GT_TYPEDEF,
        GT_ARRAY,
        GT_LAMBDA,
        GT_NONE
    };

    struct Variable {
        struct Type {
            struct VariableSetter {
                std::function<void(Variable *v, const std::string &old_value, const std::string &new_value)> call;
            };

            bool value_based = false;
            std::vector<GenericType> allowed_types;
            std::vector<std::string> allowed_value;
            // Setter
            std::shared_ptr<VariableSetter> setter = nullptr;
        };

        GenericType type;
        std::string value;
        std::string name;
        uint64_t addr;

        std::shared_ptr<Type> custom_type = nullptr;
        nlohmann::json json_handle;

        nlohmann::json *array_value = nullptr;
        Variable *parent_json;
        std::vector<uint64_t> json_variable_tracking;
        bool strict = false;
        bool readonly = false;

        // Ref
        bool isRef = false;
        int ref_count = 0;
        Variable *ref = nullptr;

        // Lambda
        bool isLambda = false;
        std::shared_ptr<void> func_lambda = nullptr;
        void CallLambda();

        void *engine;

        // Setter
        std::shared_ptr<Type::VariableSetter> setter = nullptr;

        ~Variable();

        void SetType(const std::string &name, void *engine);
        void LoadValueToJSONHandle(void *m_Engine);
        void PushJSONVariables(void *m_Engine, nlohmann::json obj, const std::string &parent_starter);
        void PushToJSONArray(GenericType type, const std::string &value);
        void AddRef(void *engine);
        void RemoveRef(void *engine, bool freeing_currently = false);
    };

    struct Template {
        std::string json_value;
        std::string name;
        std::shared_ptr<Variable::Type::VariableSetter> setter = nullptr;
    };

    struct Function {
        bool is_global = true;
        bool limitless_args = false;
        bool break_loop = false;
        bool continue_loop = false;
        std::string name;
        std::vector<Variable> arguments;
        std::vector<std::string> argument_names;
        std::unordered_map<uint64_t, std::shared_ptr<Variable>> variables;

        std::string expr = "";

        int argument_count;

        Module scope_module;

        std::function<void(Function *)> call;

        Variable return_value;
        void *engine;

        void Call();
        void CallWhileLoop(void *engine);
        Variable GetArgumentByName(const std::string &s_Name);
        Variable *GetArgumentByIndex(int i);
        void SetReturn(const std::string &val, GenericType generic_type = GT_ANY, std::shared_ptr<Variable::Type> custom_type = nullptr);
        void AddArgument(const std::string &name, const std::string &val);
        void ClearReturn();
    };

    struct ScriptFunction : public Function {
        ScriptFunction();
    };

    class ScriptEngine {
    public:
        enum State {
            NONE,
            FUNCTION_CALL,
            PUSHING_ARGUMENTS,
            VAR_DECLARATION,
            SETTING_VAR_VALUE,
            SETTING_PTR_VALUE,
            PTR_DECLARATION,
            CHANGING_VARIABLE_VALUE,
            VARIABLE_NAME_DETECTED,
            OBJ_DECLARATION,
            OBJ_NAME_DETECTED,
            FUNCTION_DECLARATION,
            FREE_VARIABLE,
            DEREFERENCE,
            GLOBAL_VARIABLE_SETTING,
            STRICT_VARIABLE_SETTING,
            RETURN_CALLED_SET_VALUE,
            IF_CHECK_DONE_CALL_SCOPE,
            IF_CHECK,
            ELSE_CHECKED_CALL_SCOPE,
            ELIF_CHECK,
            ELIF_CHECKED_CALL_SCOPE,
            ADDING_ARGUMENTS_TO_FUNCTION_DECL,
            WHILE_CHECK,
            WHILE_CHECKED_CALL_SCOPE,
            CHANGING_VARIABLE_VALUE_OPERATOR,
            CHANGING_VARIABLE_VALUE_OPERATOR_SUBTRACT,
            CHANGING_VARIABLE_VALUE_OPERATOR_DIVIDE,
            CHANGING_VARIABLE_VALUE_OPERATOR_MULTIPLY,
            CHANGING_VARIABLE_VALUE_OPERATOR_POWER,
            TYPEDEF_INIT,
            TYPEDEF_GET_TYPES,
            TYPEVALUE_INIT,
            TYPEVALUE_GET_TYPES,
            TYPEOF_GET_TYPENAME_AND_SET,
            PUSH_FIND_ARRAY_VAR,
            PUSH_TO_ARRAY,
            PUSHING_TO_ARRAY_DEF,
            GET_ELEMENT_FROM_ARRAY_AND_SET_VARIABLE,
            REF_VARIABLE_SETTING,
            TEMPLATE_DEF,
            TEMPLATE_DEF_SET_VALUE,
            LAMBDA_DECL,
            SETTING_LAMBDA_ARGUMENTS,
            SETTING_LAMBDA_VALUE
        };

        struct UnionOfState {
            union StateValues {
                Function *function = nullptr;
                Variable *variable;
                Variable::Type *type;
                Template *m_Template;
            } value;

            State state = NONE;
            Token *previous_token = nullptr;
            Token *previous_bracket = nullptr;
            bool var_strict = false;
            bool var_readonly = false;
            bool isWhile = false;
            Function *m_Func = nullptr;
            Function *func_scope_ref = nullptr;
            std::unordered_map<uint64_t, std::shared_ptr<Variable>> *variables;
        };

        struct Keyword {
            std::string str;
            EToken token_type;
        };

        struct GenericTypeDef {
            std::string str;
            GenericType type;
        };

        struct Structure {
            std::string scope_content;
            std::vector<Token> tokens;
        };

    private:
        GenericTypeDef generic_types[6] = {
            {"number", GT_NUMBER},
            {"bool", GT_BOOLEAN},
            {"string", GT_STRING},
            {"object", GT_OBJECT},
            {"boolean", GT_BOOLEAN},
            {"lambda", GT_LAMBDA},
            //
        };

        Keyword keywords[35] = {
            Keyword{"var", VAR},
            Keyword{"ptr", PTR},
            Keyword{"free", DELETE},
            Keyword{"delete", DELETE},
            Keyword{"true", TRUE},
            Keyword{"false", FALSE},
            Keyword{"func", FUNC},
            Keyword{"function", FUNC},
            Keyword{"fn", FUNC},
            Keyword{"global", GLOBAL},
            Keyword{"strict", STRICT},
            Keyword{"return", RETURN},
            Keyword{"if", IF},
            Keyword{"else", ELSE},
            Keyword{"elif", ELSE_IF},
            Keyword{"null", _NULL},
            Keyword{"undefined", _NULL},
            Keyword{"NULL", _NULL},
            Keyword{"const", CONST},
            Keyword{"constant", CONST},
            Keyword{"readonly", CONST},
            Keyword{"while", WHILE},
            Keyword{"typedef", TYPEDEF},
            Keyword{"typevalue", TYPEVALUE},
            Keyword{"typeof", TYPEOF},
            Keyword{"push", PUSH},
            Keyword{"ref", REF},
            Keyword{"object", OBJ},
            Keyword{"obj", OBJ},
            Keyword{"data", DATA},
            Keyword{"template", TEMPLATE},
            Keyword{"struct", TEMPLATE},
            Keyword{"lambda", LAMBDA},
            Keyword{"break", BREAK},
            Keyword{"continue", CONTINUE},

            //
        };

        std::random_device dev;
        std::mt19937 rng{dev()};
        std::uniform_int_distribution<std::mt19937::result_type> dist6{1, 6};
        uint64_t current_max_address = 0x0;

        static void Print(Function *func);
        static void Println(Function *func);
        static void FUNC_GetVariable(Function *func);
        static void FUNC_CreateVariable(Function *func);
        static void FUNC_array_size(Function *func);
        static void FUNC_push_back(Function *func);
        static void FUNC_ref_count(Function *func);

        // Module name as key
        std::unordered_map<std::string, Module> modules;
        std::unordered_map<std::string, std::shared_ptr<Function>> functions;
        std::unordered_map<std::string, std::shared_ptr<Variable::Type>> typedefs;

        Token CharToToken(const char ch);
        Token CreateToken(EToken type, const std::string &value);

        Token CollectString(const std::string &str, int &i);
        Token CollectComment(const std::string &str, int &i);
        Token CollectMultilineComment(const std::string &str, int &i);
        Token CollectObject(const std::string &str, int &i);
        Token CollectID(const std::string &str, int &i);
        Token CollectNumber(const std::string &str, int &i);
        Token CollectHex(const std::string &str, int &i);
        Token CollectScope(const std::string &str, int &i);
        Token CollectMathExpression(const std::string &str, int &i);
        void SkipWhitespace(const std::string &str, int &i);

        void ParseToTokens(Module &s_Module);
        void ExecuteModuleRoot(Module &s_Module);
        void __ExecuteModuleRoot(ScriptEngine::UnionOfState &state_union, Module &s_Module);
        void ExecuteFunctionScope(Function *func);
        void ExecuteIfScope(Function *func);

        // Parser of expressions

    public:
        std::unordered_map<uint64_t, std::shared_ptr<Variable>> variables;
        std::unordered_map<std::string, std::shared_ptr<Template>> structure_templates;
        std::unordered_map<std::string, std::shared_ptr<Variable::Type::VariableSetter>> variable_setters;
        std::vector<std::unordered_map<uint64_t, std::shared_ptr<Variable>> *> scope_variables;
        std::unordered_map<uint64_t, std::shared_ptr<Variable>> *current_scope_variables;

        Function *scope_func = nullptr;
        Function *FuncReturnRef = nullptr;

        std::shared_ptr<Variable::Type::VariableSetter> CreateVariableSetter(const std::string &name, std::function<void(Variable *v, const std::string &old_value, const std::string &new_value)> callback);

        std::shared_ptr<Template> GetTemplate(const std::string &name);
        std::shared_ptr<Template> CreateTemplate(const std::string &name, const std::string &json_data);

        std::shared_ptr<Function> GetFunction(const std::string &name);
        std::shared_ptr<Function> CreateFunction(const std::string &name, int argc, std::function<void(Function *)> callback_func);
        std::shared_ptr<Function> CreateLambda(int argc, std::function<void(Function *)> callback_func);
        std::shared_ptr<Variable> CreateVariable(GenericType type, const std::string &name, const std::string &value = "");
        std::shared_ptr<Variable> CreateVariable(std::unordered_map<uint64_t, std::shared_ptr<Variable>> &variables, GenericType type, const std::string &name, const std::string &value = "");
        std::shared_ptr<Variable> CreateVariable(std::unordered_map<uint64_t, std::shared_ptr<Variable>> &variables, Variable v);

        std::shared_ptr<Variable> GetVariable(const std::string &name);
        std::shared_ptr<Variable> GetVariable(const uint64_t addr);

        std::shared_ptr<Variable> GetVariable(std::vector<std::unordered_map<uint64_t, std::shared_ptr<Variable>> *> &variables, const std::string &name);
        std::shared_ptr<Variable> GetVariable(std::vector<std::unordered_map<uint64_t, std::shared_ptr<Variable>> *> &variables, const uint64_t addr);
        void FreeVariable(const uint64_t addr);
        void FreeVariable(const std::string &name);

        std::shared_ptr<Variable::Type> &CreateValueBasedType(const std::string &name, std::vector<std::string> &accepted_values);
        std::shared_ptr<Variable::Type> &CreateTypeBasedType(const std::string &name, std::vector<GenericType> &accepted_types);
        std::shared_ptr<Variable::Type> GetType(const std::string &name);

        double StringToMath(const std::string &math_expression);
        bool StringToBoolExpr(const std::string &math_expression);

        ScriptEngine();
        Module &LoadScript(const std::string &module_name, const std::string &script_path);
        void JustRunTheScript(Module &s_Module);
        Module &GetModule(const std::string &module_name);
    };
} // namespace HyperScript