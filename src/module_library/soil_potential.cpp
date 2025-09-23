#include "soil_potential.h"

using BioCroWP::soil_potential;

string_vector soil_potential::get_inputs()
{
  return {
    

    "max_rooting_layer",

    "soil_n_1",
    "soil_m_1",
    "soil_alpha_1",
    "soil_residual_wc_1",
    "soil_saturated_wc_1",

    "soil_n_2",
    "soil_m_2",
    "soil_alpha_2",
    "soil_residual_wc_2",
    "soil_saturated_wc_2",

    "soil_n_3",
    "soil_m_3",
    "soil_alpha_3",
    "soil_residual_wc_3",
    "soil_saturated_wc_3",

    "soil_n_4",
    "soil_m_4",
    "soil_alpha_4",
    "soil_residual_wc_4",
    "soil_saturated_wc_4",

    "soil_n_5",
    "soil_m_5",
    "soil_alpha_5",
    "soil_residual_wc_5",
    "soil_saturated_wc_5",

    "soil_n_6",
    "soil_m_6",
    "soil_alpha_6",
    "soil_residual_wc_6",
    "soil_saturated_wc_6",

    "soil_depth_1",
    "soil_water_content_1",

    "soil_depth_2",
    "soil_water_content_2",

    "soil_depth_3",
    "soil_water_content_3",

    "soil_depth_4",
    "soil_water_content_4",

    "soil_depth_5",
    "soil_water_content_5",

    "soil_depth_6",
    "soil_water_content_6",
  
  };
}

string_vector soil_potential::get_outputs()
{
  return {

    "soil_potential_1",
    "soil_potential_2",
    "soil_potential_3",
    "soil_potential_4",
    "soil_potential_5",
    "soil_potential_6",
    "soil_potential_avg"

  };
}

void soil_potential::do_operation() const
{

  // create variable arrays
  double soil_depth[] = { // m
      soil_depth_1/100,
      soil_depth_2/100,
      soil_depth_3/100,
      soil_depth_4/100,
      soil_depth_5/100,
      soil_depth_6/100
    };

  double soil_wc[] = {
    soil_water_content_1,
    soil_water_content_2,
    soil_water_content_3,
    soil_water_content_4,
    soil_water_content_5,
    soil_water_content_6
  };// dimensionless

  double soil_n[] = {
    soil_n_1,
    soil_n_2,
    soil_n_3,
    soil_n_4,
    soil_n_5,
    soil_n_6
  }; // dimensionless

  double soil_m[] = {
    soil_m_1,
    soil_m_2,
    soil_m_3,
    soil_m_4,
    soil_m_5,
    soil_m_6
  }; // dimensionless

  double soil_alpha[] = {
    soil_alpha_1,
    soil_alpha_2,
    soil_alpha_3,
    soil_alpha_4,
    soil_alpha_5,
    soil_alpha_6
  }; // dimensionless

  double soil_residual_wc[] = {
    soil_residual_wc_1,
    soil_residual_wc_2,
    soil_residual_wc_3,
    soil_residual_wc_4,
    soil_residual_wc_5,
    soil_residual_wc_6
  }; // dimensionless

  double soil_saturated_wc[] = {
    soil_saturated_wc_1,
    soil_saturated_wc_2,
    soil_saturated_wc_3,
    soil_saturated_wc_4,
    soil_saturated_wc_5,
    soil_saturated_wc_6
  }; // dimensionless

  // define empty arrays
  double pressure_potential[6] = {0};
  double gravitational_potential[6] = {0};
  double total_potential[6] = {0};
  double tot_soil_depth[6] = {soil_depth[0], 0, 0, 0, 0, 0};

  // calculate pressure head for a given soil layer and convert from cm to MPa
  // 1 m = 0.00979 MPa
  // 1 m * density of water (998.2 kg/ m3) * gravity (9.81 m/s2) = 98.1 kg/(m*s2) = 98.1 Pa = 0.0000979 MPa
  double convf = 0.00979; // head (m) to potential (MPa)

  // calculate cummulative depth at each soil layer
  for(int l = 1; l < max_rooting_layer; l++){
    tot_soil_depth[l] = tot_soil_depth[l-1] + soil_depth[l];
  }

  double z_wt;
  int saturated_layer;
  for(int l = 0; l < max_rooting_layer; l++){
    // if (soil_wc[l] < soil_saturated_wc[l]) { // van genuchten (1980) if soil is unsaturated (negative)
    pressure_potential[l] = convf*(1/soil_alpha[l])*pow(pow((soil_saturated_wc[l] - soil_residual_wc[l])/(soil_wc[l] - soil_residual_wc[l]), (-1/soil_m[l])) - 1, (1/soil_n[l])); // MPa
    gravitational_potential[l] = -convf*(tot_soil_depth[l] - (soil_depth[l]/2)); // calculated using soil depth in the middle of the layer
    total_potential[l] = pressure_potential[l] + gravitational_potential[l];
    // }
    // else{ // calculated as hydrostatic pressure if saturated (positive)

      // ** currently only running unsaturated portion of the model**

      // z_wt = tot_soil_depth[l-1]; // water table height assumed to be at the top of the soil layer
      // saturated_layer = l; // keep track of layer where saturation is reached
      // break; // break loop if saturation is reached
    // }
    
  }

  // double z_center_layer;
  // double z;
  // for(int l = saturated_layer; l < max_rooting_layer; l++){
  //   z_center_layer = tot_soil_depth[l] - (soil_depth[l]/2);
  //   z = z_wt - z_center_layer // hydrostatic pressure depth = depth of water table - depth at layer center 
  //   pressure_potential[l] = -998.2*9.81*z*pow(10, -6) //MPa, positive because saturated
  //   gravitational_potential[l] = -0.0000979*(tot_soil_depth[l] - (soil_depth[l]/2));
  //   total_potential[l] = pressure_potential[l] + gravitational_potential[l];
  // }

  // calculate average matric potential value for entire root zone
  double num;
  double denom;
  for(int l = 0; l < max_rooting_layer; l++){
    num += soil_wc[l]*soil_depth[l]*total_potential[l];
    denom += soil_wc[l]*soil_depth[l];
  }

  double volumetric_avg = num/denom;

  // update soil potential
  update(soil_potential_1_op, total_potential[1]);
  update(soil_potential_2_op, total_potential[2]);
  update(soil_potential_3_op, total_potential[3]);
  update(soil_potential_4_op, total_potential[4]);
  update(soil_potential_5_op, total_potential[5]);
  update(soil_potential_6_op, total_potential[6]);
  update(soil_potential_avg_op, volumetric_avg);
}