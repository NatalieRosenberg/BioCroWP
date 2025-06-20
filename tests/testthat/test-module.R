setwd('C:/Users/natal/masters/BioCroWP')

library(BioCro)
library(BioCroWP)

weatherData <- soybean_weather$'2002'


direct_modules_water_potential = list("BioCroWP:soil_temperature", "BioCroWP:osmotic_potential")
direct_modules_new = soybean$direct_modules
direct_modules = c(direct_modules_new,direct_modules_water_potential) # insert BioCroWP

differential_modules_water_potential = list("BioCroWP:pressure_potential")
differential_modules_new = soybean$differential_modules
differential_modules_new = c(differential_modules_new,
                             differential_modules_water_potential)

init_values =   within(soybean$initial_values,{
  root_volume = 10e-7
  stem_volume = 7.869e-7
  leaf_volume = 2.852e-5
  root_pressure_potential = -0.05
  stem_pressure_potential = -0.4
  leaf_pressure_potential = -0.6
  root_water_content = 0.0878416
  stem_water_content = 0.69122555
  leaf_water_content = 0.250524243
  soil_water_content_1 = soybean$initial_values$soil_water_content  #0-5cm
  soil_water_content_2 = soybean$initial_values$soil_water_content  #5-15cm
  soil_water_content_3 = soybean$initial_values$soil_water_content  #15-35cm
  soil_water_content_4 = soybean$initial_values$soil_water_content  #35-55 cm
  soil_water_content_5 = soybean$initial_values$soil_water_content  #55-75 cm
  soil_water_content_6 = soybean$initial_values$soil_water_content  #75-100cm
  sumes1               = 0.125
  sumes2               = 0
  time_factor = 0
})
init_values$soil_water_content=NULL

soiltype = 3 
parameters =   within(soybean$parameters, {
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
  R_root_stem = 0.0000005
  R_stem_leaf = 0.0000005
  minimum_temp_day = 20.9
  maximum_temp_day = 32.3
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
  soil_field_capacity = 0.3
  soil_wilting_point = 0.1
  soil_saturation_capacity = 0.7
  soil_saturated_conductivity = 0.000005
})
parameters[c('soil_field_capacity','soil_saturated_conductivity','soil_saturation_capacity','soil_wilting_point')]=NULL
parameters[c('kShell','net_assimilation_rate_shell')] = NULL

result <- run_biocro(
  init_values,
  parameters,
  weatherData,
  direct_modules,
  differential_modules_new
)

plot(result$time, result$pressure_potential, xlab='time',ylab='pressure potential')
