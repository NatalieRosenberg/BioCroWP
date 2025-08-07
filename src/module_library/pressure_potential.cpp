#include "../framework/module.h"                  // for direct_module and update
#include "../framework/module_helper_functions.h" 
#include "pressure_potential.h"

pressure_potential::pressure_potential(
        state_map const& input_quantities,
        state_map* output_quantities)
        : 
          differential_module(),

          // Get references to input quantities
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")}, // Mg/(ha*hr)
          uptake_layer_1{get_input(input_quantities, "uptake_layer_1")}, // Mg/(ha*hr)
          uptake_layer_2{get_input(input_quantities, "uptake_layer_2")},
          uptake_layer_3{get_input(input_quantities, "uptake_layer_3")},
          uptake_layer_4{get_input(input_quantities, "uptake_layer_4")},
          uptake_layer_5{get_input(input_quantities, "uptake_layer_5")},
          uptake_layer_6{get_input(input_quantities, "uptake_layer_6")},

          root_total_potential{get_input(input_quantities, "root_total_potential")}, // MPa
          stem_total_potential{get_input(input_quantities, "stem_total_potential")},
          leaf_total_potential{get_input(input_quantities, "leaf_total_potential")},

          root_pressure_potential{get_input(input_quantities, "root_pressure_potential")}, // MPa
          stem_pressure_potential{get_input(input_quantities, "stem_pressure_potential")},
          leaf_pressure_potential{get_input(input_quantities, "leaf_pressure_potential")},

          root_water_content{get_input(input_quantities, "root_water_content")}, // g
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},

          root_volume{get_input(input_quantities, "root_volume")}, // Initial volume of organ, m3
          stem_volume{get_input(input_quantities, "stem_volume")},
          leaf_volume{get_input(input_quantities, "leaf_volume")},

          ext_root_x{get_input(input_quantities, "ext_root_x")},
          ext_root_z{get_input(input_quantities, "ext_root_z")},

          ext_stem_x{get_input(input_quantities, "ext_stem_x")},
          ext_stem_z{get_input(input_quantities, "ext_stem_z")},

          ext_leaf_x{get_input(input_quantities, "ext_leaf_x")},
          ext_leaf_y{get_input(input_quantities, "ext_leaf_y")},
          ext_leaf_z{get_input(input_quantities, "ext_leaf_z")},

          mod_root_x{get_input(input_quantities, "mod_root_x")},
          mod_root_z{get_input(input_quantities, "mod_root_z")},

          mod_stem_x{get_input(input_quantities, "mod_stem_x")},
          mod_stem_z{get_input(input_quantities, "mod_stem_z")},

          mod_leaf_x{get_input(input_quantities, "mod_leaf_x")},
          mod_leaf_y{get_input(input_quantities, "mod_leaf_y")},
          mod_leaf_z{get_input(input_quantities, "mod_leaf_z")},

          wp_crit{get_input(input_quantities, "wp_crit")},

          R_root_stem{get_input(input_quantities, "R_root_stem")},
          R_stem_leaf{get_input(input_quantities, "R_stem_leaf")},

          // Get pointers to output quantities
          root_pressure_potential_op{get_op(output_quantities, "root_pressure_potential")}, // MPa
          root_water_content_op{get_op(output_quantities, "root_water_content")}, // g
          root_volume_op{get_op(output_quantities, "root_volume")}, // m3

          stem_pressure_potential_op{get_op(output_quantities, "stem_pressure_potential")},
          stem_water_content_op{get_op(output_quantities, "stem_water_content")},
          stem_volume_op{get_op(output_quantities, "stem_volume")},

          leaf_pressure_potential_op{get_op(output_quantities, "leaf_pressure_potential")},
          leaf_water_content_op{get_op(output_quantities, "leaf_water_content")},
          leaf_volume_op{get_op(output_quantities, "leaf_volume")}
    {
    }

string_vector pressure_potential::get_inputs()
{
    return {
        "canopy_transpiration_rate",          // Mg/(ha*hr)
        "uptake_layer_1",                     // Mg/(ha*hr)
        "uptake_layer_2",
        "uptake_layer_3",
        "uptake_layer_4",
        "uptake_layer_5",
        "uptake_layer_6",

        "root_total_potential",         // MPa
        "stem_total_potential",
        "leaf_total_potential",

        "root_pressure_potential",         // MPa
        "stem_pressure_potential",
        "leaf_pressure_potential",

        "root_water_content",              // g
        "stem_water_content",
        "leaf_water_content",

        "root_volume",                     // m3
        "stem_volume", 
        "leaf_volume",

        "ext_root_x", // MPa-1 hr-1
        "ext_root_z",

        "ext_stem_x",
        "ext_stem_z",

        "ext_leaf_x",
        "ext_leaf_y",
        "ext_leaf_z",

        "mod_root_x", // MPa
        "mod_root_z",

        "mod_stem_x",
        "mod_stem_z",

        "mod_leaf_x",
        "mod_leaf_y",
        "mod_leaf_z",

        "wp_crit", // MPa

        "R_root_stem", // MPa m h g-1
        "R_stem_leaf",

    };
}

string_vector pressure_potential::get_outputs()
{
    return {
        "root_pressure_potential",  // MPa
        "root_water_content",       // g
        "root_volume",              // m3

        "stem_pressure_potential",  // MPa
        "stem_water_content",       // g
        "stem_volume",              // m3

        "leaf_pressure_potential",  // MPa
        "leaf_water_content",       // g
        "leaf_volume",              // m3
    };
}

void pressure_potential::do_operation() const
{
    // convert Mg/(ha*hr) to g/(ha*hr)
    double transpiration = (canopy_transpiration_rate*1000000); // keep in terms of ha

    // Density of water
    double pw = 998200/10000; // g/m*ha

    // Calculate total RWU in g/(ha*hr)
    double F_rwu = (uptake_layer_1 + uptake_layer_2 + uptake_layer_3 + uptake_layer_4 + uptake_layer_5 + uptake_layer_6);

    // Root water potential update
    // Calculate water flow at this time step using the initial values for total potential
    double F_root_stem;
    double F_root_stem_temp = (root_total_potential - stem_total_potential)/R_root_stem;
    if (F_rwu - F_root_stem_temp < 0.0){
        F_root_stem = 0.0;
    } else {
        F_root_stem = F_root_stem_temp;
    }

    // change in root water content = total root water uptake - total flow from root system to stem
    double root_water_content_new = root_water_content + F_rwu - F_root_stem;

    // make sure organ water content does not go below 0 if uptake < flow to stem
    // double root_water_content_new;
    // if (root_water_content_temp < 0) {
    //     root_water_content_new = 0;
    // } else {
    //     root_water_content_new = root_water_content_temp;
    // }

    double dW_root = root_water_content_new - root_water_content;

    double dV_root = dW_root/pw;
    double root_volume_new = root_water_content_new/pw;

    double root_dPP;
    double stem_dPP;
    double leaf_dPP;

    // Change in pressure potential accounts for both elastic and plastic organ growth
    if (root_pressure_potential > wp_crit) {
        double root_dPP = ((dW_root/(pw*root_volume_new) - (ext_root_z + 2*ext_root_x)*(root_pressure_potential - wp_crit))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x)));
    } else {
        double root_dPP = ((dW_root/(pw*root_volume_new))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x)));
    }

    // Stem water potential update
    double F_stem_leaf;
    double F_stem_leaf_temp = (stem_total_potential - leaf_total_potential)/R_stem_leaf;
    if (F_root_stem - F_stem_leaf < 0.0){
        F_stem_leaf = 0.0;
    } else {
        F_stem_leaf = F_stem_leaf_temp;
    }

    double stem_water_content_new = stem_water_content + F_root_stem - F_stem_leaf;

    // make sure organ water content does not go below 0 if uptake < flow to stem
    // double stem_water_content_new;
    // if (stem_water_content_temp < 0) {
    //     stem_water_content_new = 0;
    // } else {
    //     stem_water_content_new = stem_water_content_temp;
    // }

    double dW_stem = stem_water_content_new - stem_water_content;

    double dV_stem = dW_stem/pw;
    double stem_volume_new = stem_water_content_new/pw;

    if(stem_pressure_potential > wp_crit) {
        double stem_dPP = ((dW_stem/(pw*stem_volume_new) - (ext_stem_z + 2*ext_stem_x)*(stem_pressure_potential - wp_crit))
                    *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x)));
    } else {
        double stem_dPP = ((dW_stem/(pw*stem_volume_new))
                    *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x)));
    }

    // Leaf water potential update
    double F_transpiration;
    // double F_stem_leaf_temp = (stem_total_potential - leaf_total_potential)/R_stem_leaf;
    if (F_stem_leaf - transpiration < 0.0){
        F_transpiration = 0.0;
    } else {
        F_transpiration = transpiration;
    }

    double leaf_water_content_new = leaf_water_content + F_stem_leaf - F_transpiration;

    // make sure organ water content does not go below 0 if uptake < flow to stem
    // double leaf_water_content_new;
    // if (leaf_water_content_temp < 0) {
    //     leaf_water_content_new = 0;
    // } else {
    //     leaf_water_content_new = leaf_water_content_temp;
    // }

    double dW_leaf = leaf_water_content_new - leaf_water_content;

    double dV_leaf = dW_leaf/pw;
    double leaf_volume_new = leaf_water_content_new/pw;

    if (leaf_pressure_potential > wp_crit) {
        double leaf_dPP = ((dW_leaf/(pw*leaf_volume_new) - (ext_leaf_z + ext_leaf_x + ext_leaf_y)*(leaf_pressure_potential - wp_crit))
                    *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
    } else {
        double leaf_dPP = ((dW_leaf/(pw*leaf_volume_new))
                    *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
    }

    // Update the output quantity list
    update(root_pressure_potential_op, root_dPP);
    update(stem_pressure_potential_op, stem_dPP);
    update(leaf_pressure_potential_op, leaf_dPP);
    update(root_water_content_op, dW_root);
    update(stem_water_content_op, dW_stem);
    update(leaf_water_content_op, dW_leaf);
    update(root_volume_op, dV_root);
    update(stem_volume_op, dV_stem);
    update(leaf_volume_op, dV_leaf);

}