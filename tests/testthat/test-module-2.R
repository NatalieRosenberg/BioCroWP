library(BioCro)
library(BioCroWP)
library(BioCroWater)

weatherData <- soybean_weather$'2002'

# listing direct modules
direct_modules_water_potential = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
                                 "BioCroWater:soil_water_downflow", "BioCroWater:soil_water_tiledrain", 
                                 "BioCroWater:soil_water_upflow","BioCroWater:soil_water_uptake",
                                 "BioCroWater:multilayer_soil_profile_avg",
                                 "BioCroWP:soil_temperature", "BioCroWP:osmotic_potential",
                                 "BioCroWP:total_potential")
direct_modules_new = soybean$direct_modules
old_soil_evapo_index = which(direct_modules_new=="BioCro:soil_evaporation")
direct_modules_new = direct_modules_new[-old_soil_evapo_index] #remove soil evaporation
direct_modules = c(direct_modules_new[1:(old_soil_evapo_index-1)],direct_modules_water_potential,
                       direct_modules_new[old_soil_evapo_index:length(direct_modules_new)]) # insert BioCroWP

# listing differential modules
differential_modules_water_potential = list("BioCroWater:soil_evaporation","BioCroWater:multi_layer_soil_profile",
                                            "BioCroWP:pressure_potential")
differential_modules_new = soybean$differential_modules
old_soil_profile_index = which(differential_modules_new=="BioCro:two_layer_soil_profile")
differential_modules_new = differential_modules_new[-old_soil_profile_index] #remove soil_profile
differential_modules = c(differential_modules_new[1:(old_soil_profile_index-1)],
                             differential_modules_water_potential,
                             differential_modules_new[old_soil_profile_index:length(differential_modules_new)])

init_values =   within(soybean$initial_values,{
  #canopy_transpiration_rate = 0
  soil_water_content_1 = soybean$initial_values$soil_water_content  #0-5cm
  soil_water_content_2 = soybean$initial_values$soil_water_content  #5-15cm
  soil_water_content_3 = soybean$initial_values$soil_water_content  #15-35cm
  soil_water_content_4 = soybean$initial_values$soil_water_content  #35-55 cm
  soil_water_content_5 = soybean$initial_values$soil_water_content  #55-75 cm
  soil_water_content_6 = soybean$initial_values$soil_water_content  #75-100cm
  sumes1               = 0.125
  sumes2               = 0
  time_factor = 0
  soil_evaporation_rate = 0
  root_water_content = 878.416
  stem_water_content = 6912.2555
  leaf_water_content = 2505.24243
  root_volume = 10e-2
  stem_volume = 7.869e-2
  leaf_volume = 0 # no leaves in initial development stages
  root_pressure_potential = -0.04  # Initial value to be updated
  stem_pressure_potential = -0.3
  leaf_pressure_potential = -0.5
  #root_total_potential = -0.05  # For initial water flow calculation
  #stem_total_potential = -0.4
  #leaf_total_potential = -0.6
  leaf_temperature = 298.15
  #soil_temperature_avg = 298.15
  kd = 1 # Not sure what this should be
})
init_values$soil_water_content=NULL

soiltype = 3 
parameters =   within(soybean$parameters, {
  irrigation = 0.0
  swcon = 0.05/24
  curve_number = 61
  soil_type_indicator_1 = soiltype
  soil_type_indicator_2 = soiltype
  soil_type_indicator_3 = soiltype
  soil_type_indicator_4 = soiltype
  soil_type_indicator_5 = soiltype
  soil_type_indicator_6 = soiltype
  soil_depth_1 = 5
  soil_depth_2 = 10
  soil_depth_3 = 20
  soil_depth_4 = 20
  soil_depth_5 = 20
  soil_depth_6 = 25
  tile_drainage_rate          = 0.2 # dimensionless, as in DSSAT (1/d ?)
  tile_drain_depth            = 90  # cm (typical value 3-4 ft)
  skc    = 0.3
  kcbmax = 0.25
  elevation                   = 219
  bare_soil_albedo            = 0.15
  max_rooting_layer = 4
  ext_root_x = 0.055
  ext_root_z = 0.275
  ext_stem_x = 0.055
  ext_stem_z = 0.275
  ext_leaf_x = 0.0
  ext_leaf_y = 0.55
  ext_leaf_z = 0.55
  mod_root_x = 57
  mod_root_z = 57
  mod_stem_x = 57
  mod_stem_z = 57
  mod_leaf_x = 9
  mod_leaf_y = 2
  mod_leaf_z = 9
  wp_crit = 0.4
  storage_water_frac = 0.8
  R_root_stem = 0.05
  R_stem_leaf = 0.05
  minimum_temp_day = 20.9
  maximum_temp_day = 32.3
})
parameters[c('soil_field_capacity','soil_saturated_conductivity','soil_saturation_capacity','soil_wilting_point')]=NULL
parameters[c('kShell','net_assimilation_rate_shell')] = NULL

result <- run_biocro(
  init_values,
  parameters,
  weatherData,
  direct_modules,
  differential_modules
)

time_daily = result$time/24

plot(time_daily, result$root_water_content, xlab='time',ylab= "Root Water Content (g)")
plot(result$time, result$stem_water_content, xlab='time',ylab= "Stem Water Content (g)")
plot(result$time, result$leaf_water_content, xlab='time',ylab= "Leaf Water Content (g)")
plot(result$time, result$root_volume, xlab='time',ylab= "Root Volume (m3)")
plot(result$time, result$stem_volume, xlab='time',ylab= "Stem Volume (m3)")
plot(result$time, result$leaf_volume, xlab='time',ylab= "Leaf Volume (m3)")
plot(result$time, result$root_pressure_potential, xlab='time',ylab= "Root Pressure Potential (MPa)")
plot(result$time, result$stem_pressure_potential, xlab='time',ylab= "Stem Pressure Potential (MPa)")
plot(result$time, result$leaf_pressure_potential, xlab='time',ylab= "Leaf Pressure Potential (MPa)")
plot(result$time, result$root_osmotic_potential, xlab='time',ylab="Root Osmotic Potential (MPa)")
plot(result$time, result$stem_osmotic_potential, xlab='time',ylab="Stem Osmotic Potential (MPa)")
plot(result$time, result$leaf_osmotic_potential, xlab='time',ylab="Leaf Osmotic Potential (MPa)")
plot(result$time, result$root_total_potential, xlab='time',ylab="Root Total Potential (MPa)")
plot(result$time, result$stem_total_potential, xlab='time',ylab="Stem Total Potential (MPa)")
plot(result$time, result$leaf_total_potential, xlab='time',ylab="Leaf Total Potential (MPa)")
plot(result$time, weatherData$precip, xlab='time',ylab="Precipitation")
plot(result$time, result$Root, xlab='time', ylab='Dry Root Biomass')
plot(result$time, result$Stem, xlab='time', ylab='Dry Stem Biomass')
plot(result$time, result$Leaf, xlab='time', ylab='Dry Leaf Biomass')

transpiration_abr = result$canopy_transpiration_rate[seq(11, length(result$canopy_transpiration_rate), by =24)]
time_abr = time_daily[seq(11, length(result$time), by = 24)]
plot(result$time, result$canopy_transpiration_rate, xlab='time', ylab='Canopy Transpiration Rate (Mg ha-1 hr-1)')
plot(time_abr, transpiration_abr, xlab='time', ylab='Canopy Transpiration Rate (Mg ha-1 hr-1)')

