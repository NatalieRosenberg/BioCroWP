library(BioCro)
library(BioCroWP)
library(BioCroWater)

weatherData <- soybean_weather$'2002'

# listing direct modules
direct_modules_water_potential = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
                                 "BioCroWater:soil_water_downflow", "BioCroWater:soil_water_tiledrain", 
                                 "BioCroWater:soil_water_upflow","BioCroWater:soil_water_uptake",
                                 "BioCroWater:multilayer_soil_profile_avg",
                                 "BioCroWP:soil_temperature",
                                 "BioCroWP:soil_potential",
                                 "BioCroWP:total_potential",
                                 "BioCroWP:osmotic_potential")
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
  root_volume = 1
  stem_volume = 1
  leaf_volume = 1 # no leaves in initial development stages
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

setwd('C:/Users/natal/OneDrive/Documents/masters/research/literature/plant_water_transport/coussement/results/plots/')

R_values = list(0.001, 0.005, 0.01, 0.05, 0.1, 0.5, 1, 2, 3, 5, 10, 100, 1000)
for (R in R_values) {
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
    R_root_stem = R
    R_stem_leaf = R
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
  
  # Plotting results
  time_daily = result$time/24
  
  plots <- list(
    list(base="root_WC", y=result$root_water_content, ylab="Root Water Content (g)"),
    list(base="stem_WC",   y=result$stem_water_content,   ylab="Stem Water Content (g)"),
    list(base="leaf_WC",   y=result$leaf_water_content,   ylab="Leaf Water Content (g)"),
    list(base="root_volume", y=result$root_volume,         ylab="Root Volume (m3)"),
    list(base="stem_volume", y=result$stem_volume,         ylab="Stem Volume (m3)"),
    list(base="leaf_volume", y=result$leaf_volume,         ylab="Leaf Volume (m3)"),
    list(base="root_pp",   y=result$root_pressure_potential,   ylab="Root Pressure Potential (MPa)"),
    list(base="stem_pp",   y=result$stem_pressure_potential,   ylab="Stem Pressure Potential (MPa)"),
    list(base="leaf_pp",   y=result$leaf_pressure_potential,   ylab="Leaf Pressure Potential (MPa)"),
    list(base="root_op",   y=result$root_osmotic_potential,    ylab="Root Osmotic Potential (MPa)"),
    list(base="stem_op",   y=result$stem_osmotic_potential,    ylab="Stem Osmotic Potential (MPa)"),
    list(base="leaf_op",   y=result$leaf_osmotic_potential,    ylab="Leaf Osmotic Potential (MPa)"),
    list(base="root_tp",   y=result$root_total_potential,      ylab="Root Total Potential (MPa)"),
    list(base="stem_tp",   y=result$stem_total_potential,      ylab="Stem Total Potential (MPa)"),
    list(base="leaf_tp",   y=result$leaf_total_potential,      ylab="Leaf Total Potential (MPa)")
  )
  
  R_str <- sprintf("%.3f", R)  # keeps three decimal places
  
  for (p in plots) {
    filename <- paste0(p$base, "_R_", R_str, ".jpeg")
    
    # Save the plot
    jpeg(filename, width = 800, height = 600)
    plot(time_daily, p$y, 
         xlab = "DOY", 
         ylab = p$ylab,
         main = paste(p$ylab, "(R =", R_str, ")"))  # add R value to title
    dev.off()
  
  }
}
  
plots_2 <- list(
  list(base="precip",    y=weatherData$precip,    ylab="Precipitation"),
  list(base="root_dry_biomass", y=result$Root, ylab="Root Dry Biomass"),
  list(base="stem_dry_biomass", y=result$Stem, ylab="Stem Dry Biomass"),
  list(base="leaf_dry_biomass", y=result$Leaf, ylab="Leaf Dry Biomass")
)

for (p2 in plots_2) {
  filename <- paste0(p2$base, ".jpeg")
  jpeg(filename, width = 800, height = 600)
  plot(time_daily, p2$y,
       xlab = "DOY",
       ylab = p2$ylab)
  dev.off()
}

plot(time_daily[20:120], result$soil_temperature_avg[20:120], xlab="DOY", ylab="soil temperature (K)")


transpiration_abr = result$canopy_transpiration_rate[seq(11, length(result$canopy_transpiration_rate), by =24)]
time_abr = time_daily[seq(11, length(result$time), by = 24)]
plot(result$time, result$canopy_transpiration_rate, xlab='time', ylab='Canopy Transpiration Rate (Mg ha-1 hr-1)')
plot(time_abr, transpiration_abr, xlab='time', ylab='Canopy Transpiration Rate (Mg ha-1 hr-1)')

soil_temp_abr = result$soil_temperature_avg[seq(11, length(result$soil_temperature_avg), by =24)]
plot(time_abr, result$soil_temperature_avg, xlab='DOY', ylab = 'Soil Temperature (K)')

# plotting total water uptake
total_uptake = 0
for (layer in 1:parameters$max_rooting_layer) {
  total_uptake = total_uptake + result[[paste0("uptake_layer_", layer)]]
}
plot(time_daily, total_uptake, xlab='time', ylab='total RWU (Mg ha-1 hr-1)')
uptake_1 <- total_uptake[[1]]
print(uptake_1*1000000)

transpiration_1 <- result$canopy_transpiration_rate[[1]]
print(transpiration_1*1000000)

result$root_total_potential[[1]]
result$root_osmotic_potential[[1]]
