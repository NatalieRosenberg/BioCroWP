#include "soil_temperature.h"
#include <cmath>

using BioCroWP::soil_temperature;

string_vector soil_temperature::get_inputs()
{
    return {
        "soil_depth_1", // m
        "soil_depth_2",
        "soil_depth_3",
        "soil_depth_4",
        "soil_depth_5",
        "soil_depth_6",

        "soil_bulk_density_1", // Mg/m3
        "soil_bulk_density_2",
        "soil_bulk_density_3",
        "soil_bulk_density_4",
        "soil_bulk_density_5",
        "soil_bulk_density_6",

        "soil_water_content", // volumetric swc for entire profile

        "minimum_temp_day", // degrees C
        "maximum_temp_day",

        "soil_clay_content_1", // % volume
        "soil_clay_content_2",
        "soil_clay_content_3",
        "soil_clay_content_4",
        "soil_clay_content_5",
        "soil_clay_content_6",

        "hour",

    };
}

string_vector soil_temperature::get_outputs()
{
    return {
        "soil_temperature_1_op", // K
        "soil_temperature_2_op",
        "soil_temperature_3_op",
        "soil_temperature_4_op",
        "soil_temperature_5_op",
        "soil_temperature_6_op"
    };
}

void soil_temperature::do_operation() const
{
    double soil_particle_density = 2650; // kg/m3
    double pi = 3.14159265358979323846;

    // convert cm to m
    double soil_depth_1 = soil_depth_1/100;
    double soil_depth_2 = soil_depth_2/100;
    double soil_depth_3 = soil_depth_3/100;
    double soil_depth_4 = soil_depth_4/100;
    double soil_depth_5 = soil_depth_5/100;
    double soil_depth_6 = soil_depth_6/100;

    double tot_soil_depth = soil_depth_1 + soil_depth_2 + soil_depth_3 + soil_depth_4 + soil_depth_5 + soil_depth_6;
    double soil_bulk_density_avg = (soil_bulk_density_1 + soil_bulk_density_2 + soil_bulk_density_3 + soil_bulk_density_4 + soil_bulk_density_5 + soil_bulk_density_6)/tot_soil_depth;
    double clay_content_avg = (soil_clay_content_1 + soil_clay_content_2 + soil_clay_content_3 + soil_clay_content_4 + soil_clay_content_5 + soil_clay_content_6)/tot_soil_depth;

    // fudged by 0.5 degrees celcius because air temp is used instead of soil surface temp
    // fudge factor was found from Moore et al. (2020)
    double min_K = minimum_temp_day + 273.15 + 0.5;
    double max_K = maximum_temp_day + 273.15 + 0.5;

    // % clay by weight = (% clay by volume * particle density of clay)/particle density of entire sample

    // standard porosity calculation
    // Soil Sampling and Methods of Analysis Second Edition; Danielson and Sutherland (1986), page 749
    double porosity = 1 - soil_bulk_density_avg/soil_particle_density; // dimensionless

    // calculations for soil thermal conductivity and volumetric heat capacity are from Wu and Nofgizer (1988)
    double g_a = 0.333 - 0.298*(1 - soil_water_content/porosity);
    double k_a = 2.25; // kJ m-1 d-1 K-1
    double k_w = 51.41; // kJ m-1 d-1 K-1
    double k_s = 504.58 - 2.85*clay_content_avg; // kJ m-1 d-1 K-1

    double F_a = (1/3)*(2/(1 + g_a*(k_a/k_w - 1)) + 1/(1 + (1 - 2*g_a)*(k_a/k_w -1))); // air temperature gradient, dimesionless
    double F_s = (1/3)*(2/(1 + 0.125*(k_s/k_w - 1)) + 1/(1 + 0.75*(k_s/k_w -1))); // soil particle temperature gradient, dimensionless

    // this only works for unsaturated soils
    double thermal_conductivity = ((F_s*(1 - porosity)*k_s + F_a*(porosity - soil_water_content)*k_a + soil_water_content*k_w)/
        (F_s*(1-porosity) + F_a*(porosity - soil_water_content) + soil_water_content)); // kJ m-1 d-1 K-1

    double heat_capacity = 2000*(1 - porosity) + 4200*soil_water_content; //kJ m-3 K-1

    // sinusoidal function for hourly soil temperature variation

    double w = (pi*2)/24; // frequency (1/hr)
    double thermal_diffusivity = thermal_conductivity/(24*heat_capacity); //(m2/hr)
    double d = pow((2*thermal_diffusivity)/w, 0.5); // m/hr
    double Amp = (max_K - min_K)/2; // amplitude, hr

    // average daily soil surface temp
    double T_a = (min_K + max_K)/2;

    // lag time (hrs) calculation for each soil layer
    // the amount of time it takes for a temperature fluctuation to travel from the surface to a given depth
    // lag time = 24hrs x (z/d)
    double lag_time_1 = (24/(2*pi))*(soil_depth_1/d);
    double lag_time_2 = (24/(2*pi))*(soil_depth_2/d);
    double lag_time_3 = (24/(2*pi))*(soil_depth_3/d);
    double lag_time_4 = (24/(2*pi))*(soil_depth_4/d);
    double lag_time_5 = (24/(2*pi))*(soil_depth_5/d);
    double lag_time_6 = (24/(2*pi))*(soil_depth_6/d);
    
    // soil temperature at each soil layer
    // -pi/2 = phase shift to coldest hour of the day
    // lag time = number of hours between coldest air temp and coldest soil temp
    double soil_temperature_1 = T_a + Amp*sin(w*(hour - lag_time_1) - (soil_depth_1/d) - (pi/2));
    double soil_temperature_2 = T_a + Amp*sin(w*(hour - lag_time_2) - (soil_depth_2/d) - (pi/2));
    double soil_temperature_3 = T_a + Amp*sin(w*(hour - lag_time_3) - (soil_depth_3/d) - (pi/2));
    double soil_temperature_4 = T_a + Amp*sin(w*(hour - lag_time_4) - (soil_depth_4/d) - (pi/2));
    double soil_temperature_5 = T_a + Amp*sin(w*(hour - lag_time_5) - (soil_depth_5/d) - (pi/2));
    double soil_temperature_6 = T_a + Amp*sin(w*(hour - lag_time_6) - (soil_depth_6/d) - (pi/2));

    update(soil_temperature_1_op, soil_temperature_1);
    update(soil_temperature_2_op, soil_temperature_2);
    update(soil_temperature_3_op, soil_temperature_3);
    update(soil_temperature_4_op, soil_temperature_4);
    update(soil_temperature_5_op, soil_temperature_5);
    update(soil_temperature_6_op, soil_temperature_6);
}