#include "module_library.h"
#include "../framework/module_creator.h"  // for create_mc

// Include all the header files that define the modules.
#include "example_module.h"
#include "soil_temperature.h"
#include "osmotic_potential.h"
#include "pressure_potential.h"

creator_map BioCroWP::module_library::library_entries =
{
    {"example_module", &create_mc<example_module>},
    {"soil_temperature", &create_mc<soil_temperature>},
    {"osmotic_potential", &create_mc<osmotic_potential>},
    {"pressure_potential", &create_mc<pressure_potential>}
};
