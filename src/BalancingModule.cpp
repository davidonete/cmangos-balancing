#include "BalancingModule.h"

namespace cmangos_module
{
    BalancingModule::BalancingModule()
    : Module("Balancing", new BalancingModuleConfig())
    {

    }
}