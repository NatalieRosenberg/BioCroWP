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

          root_pressure_potential_ip{get_input(input_quantities, "root_pressure_potential")}, // initial values to be updated, MPa
          stem_pressure_potential_ip{get_input(input_quantities, "stem_pressure_potential")},
          leaf_pressure_potential_ip{get_input(input_quantities, "leaf_pressure_potential")},

          root_osmotic_potential{get_input(input_quantities, "root_osmotic_potential")},
          stem_osmotic_potential{get_input(input_quantities, "stem_osmotic_potential")},
          leaf_osmotic_potential{get_input(input_quantities, "leaf_osmotic_potential")},

          root_water_content_ip{get_input(input_quantities, "root_water_content")}, // g
          stem_water_content_ip{get_input(input_quantities, "stem_water_content")},
          leaf_water_content_ip{get_input(input_quantities, "leaf_water_content")},

          root_volume_ip{get_input(input_quantities, "root_volume")}, // Initial volume of organ, m3
          stem_volume_ip{get_input(input_quantities, "stem_volume")},
          leaf_volume_ip{get_input(input_quantities, "leaf_volume")},

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

        "root_pressure_potential_ip",         // MPa
        "stem_pressure_potential_ip",
        "leaf_pressure_potential_ip",

        "root_osmotic_potential",             // MPa
        "stem_osmotic_potential",
        "leaf_osmotic_potential",

        "root_water_content_ip",              // g
        "stem_water_content_ip",
        "leaf_water_content_ip",

        "root_volume_ip",                     // m3
        "stem_volume_ip", 
        "leaf_volume_ip",

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
        "root_pressure_potential_op",  // MPa
        "root_water_content_op",       // g
        "root_volume_op",              // m3

        "stem_pressure_potential_op",  // MPa
        "stem_water_content_op",       // g
        "stem_volume_op",              // m3

        "leaf_pressure_potential_op",  // MPa
        "leaf_water_content_op",       // g
        "leaf_volume_op"               // m3
    };
}

void pressure_potential::do_operation() const
{
    // convert Mg/(ha*hr) to g/(ha*hr)
    double transpiration = (canopy_transpiration_rate*1000000); // keep in terms of ha

    // Extensibility (MPa-1 h-1)
    // double ext_root_x = 0.055;
    // double ext_root_z = 0.275;

    // double ext_stem_x = 0.055;
    // double ext_stem_z = 0.275;

    // double ext_leaf_x = 0.0;
    // double ext_leaf_y = 0.55;
    // double ext_leaf_z = 0.55;

    // Elastic modulus (MPa)
    // double mod_root_x = 57;
    // double mod_root_z = 57;

    // double mod_stem_x = 57;
    // double mod_stem_z = 57;

    // double mod_leaf_x = 9;
    // double mod_leaf_y = 2;
    // double mod_leaf_z = 9;

    // double wp_crit = 0.4;  // MPa

    // Density of water
    double pw = 998200; // g/m3

    // Calculate total RWU in g/(ha*hr)
    double F_rwu = (uptake_layer_1 + uptake_layer_2 + uptake_layer_3 + uptake_layer_4 + uptake_layer_5 + uptake_layer_6)*1000000;

    double root_total_potential = root_osmotic_potential + root_pressure_potential_ip;
    double stem_total_potential = stem_osmotic_potential + stem_pressure_potential_ip;
    double leaf_total_potential = leaf_osmotic_potential + leaf_pressure_potential_ip;

    // double R_root_stem = 0.0000005; // MPa m h g-1
    // double R_stem_leaf = 0.0000005;

    // Root water potential update
    double F_root_stem = (root_total_potential - stem_total_potential)/R_root_stem;

    double dW_root = F_rwu - F_root_stem;
    double root_water_content = root_water_content + dW_root;

    double dV_root = dW_root/pw;
    double root_volume = root_water_content/pw;

    double root_dPP;
    double stem_dPP;
    double leaf_dPP;

    if (root_pressure_potential_ip <= wp_crit) {
        double root_dPP = ((dW_root/root_volume - (ext_root_z + 2*ext_root_x)*(root_pressure_potential_ip - wp_crit))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x)));
    } else {
    double root_dPP = ((dW_root/root_volume)
                *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x)));
    };

    // Stem water potential update

    double F_stem_leaf = (stem_total_potential - leaf_total_potential)/R_stem_leaf;

    double dW_stem = F_root_stem - F_stem_leaf;
    double stem_water_content = stem_water_content + dW_stem;

    double dV_stem = dW_stem/pw;
    double stem_volume = stem_water_content/pw;

    if(stem_pressure_potential_ip <= wp_crit) {
        double stem_dPP = ((dW_stem/stem_volume - (ext_stem_z + 2*ext_stem_x)*(stem_pressure_potential_ip - wp_crit))
                    *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x)));
    } else {
        double stem_dPP = ((dW_stem/stem_volume)
                    *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x)));
    };

    // Leaf water potential update

    double dW_leaf = F_stem_leaf - transpiration;
    double leaf_water_content = leaf_water_content + dW_leaf;

    double dV_leaf = dW_leaf/pw;
    double leaf_volume = leaf_water_content/pw;

    if (leaf_pressure_potential_ip <= wp_crit) {
        double leaf_dPP = ((dW_leaf/leaf_volume - (ext_leaf_z + ext_leaf_x + ext_leaf_y)*(leaf_pressure_potential_ip - wp_crit))
                    *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
    } else {
    double leaf_dPP = ((dW_leaf/leaf_volume)
                *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
    };

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