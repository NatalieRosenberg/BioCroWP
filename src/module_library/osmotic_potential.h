#ifndef BioCroWP_OSMOTIC_POTENTIAL_H
#define BioCroWP_OSMOTIC_POTENTIAL_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{
class osmotic_potential : public direct_module
{
   public:
    osmotic_potential(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          canopy_temperature{get_input(input_quantities, "canopy_temperature")},
          soil_temperature{get_input(input_quantities, "soil_temperature")},

        //   stem_soluble_sugar{get_input(input_qunatities, "stem_soluble_sugar")}, // waiting on partioning fraction
        //   leaf_soluble_sugar{get_input(input_qunatities, "leaf_soluble_sugar")},
        //   root_soluble_sugar{get_input(input_qunatities, "root_soluble_sugar")},

          root_volume{get_input(input_quantities, "root_volume")}, // Initial volume of organ, m3
          stem_volume{get_input(input_quantities, "stem_volume")},
          leaf_volume{get_input(input_quantities, "leaf_volume")},

          storage_water_frac{get_input(input_quantities, "storage_water_frac")},

          // Get pointers to output quantities
          stem_osmotic_potential_op{get_op(output_quantities, "stem_osmotic_potential")},
          leaf_osmotic_potential_op{get_op(output_quantities, "leaf_osmotic_potential")},
          root_osmotic_potential_op{get_op(output_quantities, "root_osmotic_potential")}    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "osmotic_potential"; }

   private:
    // References to input quantities
    double const& canopy_temperature;
    double const& soil_temperature;

    double const& root_volume;
    double const& stem_volume;
    double const& leaf_volume;

    double const& storage_water_frac;

    // Pointers to output quantities
    double* stem_osmotic_potential_op;
    double* leaf_osmotic_potential_op;
    double* root_osmotic_potential_op;

    // Main operation
    void do_operation() const;
};

}  // namespace BioCroWP
#endif