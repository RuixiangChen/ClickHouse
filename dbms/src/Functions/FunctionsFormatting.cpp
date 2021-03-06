#include <Functions/FunctionFactory.h>
#include <Functions/FunctionsFormatting.h>
#include "registerFunctions.h"

namespace DB
{

void registerFunctionsFormatting(FunctionFactory & factory)
{
    factory.registerFunction<FunctionBitmaskToList>();
    factory.registerFunction<FunctionFormatReadableSize>();
}

}
