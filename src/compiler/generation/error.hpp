#include <string>

/**
 * @brief Enumeration representing possible errors during code generation.
 */
enum class CodeGenerationErrorType
{
    VariableAlreadyDefined,
    UndeclaredVariable
};

/**
 * @brief A possible errors during code generation.
 */
struct CodeGenerationError
{
    CodeGenerationErrorType type;
    std::string hint;
};