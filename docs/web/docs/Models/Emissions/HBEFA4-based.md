---
title: HBEFA4-based
---

Since version 1.14.0, SUMO is able to simulate vehicular pollutant
emissions based on the database application HBEFA in version 4.2. Though
HBEFA contains information about other pollutants, we decided to
implement only the following, major ones:

- CO<sub>2</sub>
- CO
- HC
- NO<sub>x</sub>
- PM<sub>x</sub> (usually this is PM<sub>10</sub> as a sum of exhaust and non-exhaust)
- fuel consumption
- electricity (corresponds to HBEFA FC_MJ)

## Vehicle Emission Classes

The vehicle classes below map directly to vehicle subsegments supplied by
[HBEFA](http://www.hbefa.net/), the SUMO names just remove special characters.
The original name and the numeric HBEFA id are also given in the table below
To use the emission classes below prepend the class name with the HBEFA4
prefix, e.g. `emissionClass="HBEFA4/PC_petrol_Euro-4"`.

The following tables give the emissionClass strings to use in the
definition of vehicle types together with the fleet share in an average 2022 vehicle fleet
as estimated by HBEFA4.2 and the error when fitting the relevant emission (so please be
careful if you evaluate emissions with an error value above 40%).

### Passenger Cars
| SUMO emission class | HBEFA subsegment | HBEFA subsegment ID | fleet share 2022 | error CO2 | error CO | error HC | error FC | error NOx | error PM | error FC_MJ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| PC_petrol_ltECE | PC petrol <ECE | 111900 | 0.0332% | 5.41% | 26.38% | 7.35% | 5.41% | 17.23% | 66.33% + 3.07% | - |
| PC_petrol_ECE-15_00 | PC petrol ECE-15'00 | 111901 | 0.0000% | 5.41% | 26.38% | 7.26% | 5.41% | 17.23% | 66.33% + 3.07% | - |
| PC_petrol_ECE-15_01_02 | PC petrol ECE-15'01/02 | 111902 | 0.0000% | 5.41% | 26.38% | 7.55% | 5.41% | 17.23% | 66.33% + 3.07% | - |
| PC_petrol_ECE-15_03 | PC petrol ECE-15'03 | 111903 | 0.0000% | 5.41% | 26.38% | 7.41% | 5.41% | 16.92% | 66.33% + 3.07% | - |
| PC_petrol_ECE-15_04 | PC petrol ECE-15'04 | 111904 | 0.0180% | 5.41% | 26.05% | 7.02% | 5.41% | 16.79% | 66.33% + 3.07% | - |
| PC_petrol_AGV82_(CH) | PC petrol AGV82 (CH) | 111905 | 0.0000% | 5.41% | 32.84% | 7.31% | 5.41% | 17.41% | 66.33% + 3.07% | - |
| PC_petrol_conv_other_concepts | PC petrol conv other concepts | 111906 | 0.0090% | 5.41% | 32.84% | 7.31% | 5.41% | 17.41% | 66.33% + 3.07% | - |
| PC_petrol_Ucat | PC petrol Ucat | 111907 | 0.0125% | 5.41% | 31.55% | 10.03% | 5.41% | 17.81% | 66.33% + 3.07% | - |
| PC_petrol_Euro-1 | PC petrol Euro-1 | 111910 | 1.5978% | 4.91% | 46.58% | 30.13% | 4.91% | 13.09% | 56.05% + 3.08% | - |
| PC_petrol_PreEuro_3WCat_lt1987 | PC petrol PreEuro 3WCat <1987 | 111912 | 0.0000% | 4.91% | 46.58% | 30.13% | 4.91% | 13.09% | 56.05% + 3.08% | - |
| PC_petrol_PreEuro_3WCat_1987-90 | PC petrol PreEuro 3WCat 1987-90 | 111913 | 0.0000% | 4.91% | 46.58% | 30.13% | 4.91% | 13.09% | 56.05% + 3.08% | - |
| PC_petrol_Euro-2 | PC petrol Euro-2 | 111920 | 1.0044% | 4.74% | 77.42% | 27.27% | 4.74% | 21.60% | 56.17% + 3.08% | - |
| PC_petrol_Euro-3 | PC petrol Euro-3 | 111930 | 2.4911% | 4.73% | 79.68% | 29.58% | 4.73% | 21.87% | 51.83% + 3.08% | - |
| PC_petrol_Euro-4 | PC petrol Euro-4 | 111940 | 18.6045% | 4.32% | 89.31% | 55.25% | 4.32% | 15.90% | 50.77% + 3.07% | - |
| PC_petrol_Euro-5 | PC petrol Euro-5 | 111950 | 13.6421% | 4.36% | 92.95% | 44.90% | 4.36% | 13.61% | 26.28% + 3.08% | - |
| PC_petrol_Euro-6ab | PC petrol Euro-6ab | 111961 | 14.3411% | 4.17% | 60.35% | 27.12% | 4.17% | 15.20% | 21.70% + 3.09% | - |
| PC_petrol_Euro-6c | PC petrol Euro-6c | 111965 | 0.4879% | 4.13% | 41.44% | 34.08% | 4.13% | 21.85% | 10.26% + 3.09% | - |
| PC_petrol_Euro-6d-temp | PC petrol Euro-6d-temp | 111967 | 7.1574% | 4.01% | 22.76% | 22.73% | 4.01% | 51.77% | 37.66% + 3.09% | - |
| PC_petrol_Euro-6d | PC petrol Euro-6d | 111968 | 6.2798% | 3.97% | 14.25% | 29.81% | 3.97% | 57.77% | 40.51% + 3.09% | - |
| PC_petrol_Euro-7 | PC petrol Euro-7 | 111970 | 0.0000% | 4.13% | 13.19% | 28.80% | 4.13% | 58.96% | 40.52% + 3.09% | - |
| PC_diesel_conv | PC diesel conv | 121908 | 0.0266% | 8.05% | 19.94% | 31.24% | 8.05% | 13.77% | 19.21% + 3.02% | - |
| PC_diesel_1986-1988 | PC diesel 1986-1988 | 121909 | 0.0000% | 8.05% | 15.84% | 5.54% | 8.05% | 12.53% | 13.90% + 3.02% | - |
| PC_diesel_Euro-1 | PC diesel Euro-1 | 121910 | 0.2289% | 7.84% | 17.61% | 5.19% | 7.84% | 13.49% | 11.75% + 3.04% | - |
| PC_diesel_Euro-2 | PC diesel Euro-2 | 121920 | 0.3581% | 7.44% | 15.33% | 4.85% | 7.44% | 13.94% | 9.35% + 3.07% | - |
| PC_diesel_Euro-2_(DPF) | PC diesel Euro-2 (DPF) | 121921 | 0.0000% | 7.44% | 15.33% | 4.85% | 7.44% | 13.94% | 9.35% + 3.07% | - |
| PC_diesel_Euro-3 | PC diesel Euro-3 | 121930 | 1.2441% | 7.58% | 38.18% | 8.16% | 7.58% | 17.22% | 9.33% + 3.08% | - |
| PC_diesel_Euro-3_(DPF) | PC diesel Euro-3 (DPF) | 121931 | 0.2079% | 7.58% | 38.18% | 8.16% | 7.58% | 17.22% | 9.33% + 3.08% | - |
| PC_diesel_Euro-4 | PC diesel Euro-4 | 121940 | 1.0750% | 7.46% | 20.81% | 4.07% | 7.46% | 23.40% | 7.94% + 3.09% | - |
| PC_diesel_Euro-4_(DPF) | PC diesel Euro-4 (DPF) | 121941 | 3.3216% | 7.46% | 20.81% | 4.07% | 7.46% | 23.40% | 107.23% + 3.09% | - |
| PC_diesel_Euro-5 | PC diesel Euro-5 | 121950 | 4.1782% | 7.06% | 49.18% | 7.78% | 7.06% | 16.83% | 9.78% + 3.09% | - |
| PC_diesel_Euro-5_other_SU_before_software_update | PC diesel Euro-5 other SU before software update | 121951 | 0.1849% | 7.06% | 49.18% | 7.78% | 7.06% | 16.83% | 9.78% + 3.09% | - |
| PC_diesel_Euro-5_EA189_before_software_update | PC diesel Euro-5 EA189 before software update | 121952 | 0.0000% | 7.06% | 49.18% | 7.78% | 7.06% | 16.83% | 9.78% + 3.09% | - |
| PC_diesel_Euro-5_EA189_after_software_update | PC diesel Euro-5 EA189 after software update | 121953 | 3.4723% | 7.06% | 114.32% | 15.90% | 7.06% | 30.78% | 21.90% + 3.09% | - |
| PC_diesel_Euro-5_other_SU_after_software_update | PC diesel Euro-5 other SU after software update | 121954 | 1.1302% | 7.07% | 50.96% | 7.78% | 7.07% | 18.92% | 9.83% + 3.09% | - |
| PC_diesel_Euro-6ab | PC diesel Euro-6ab | 121955 | 6.2352% | 7.03% | 14.57% | 15.64% | 7.03% | 21.56% | 17.67% + 3.09% | - |
| PC_diesel_Euro-6ab_SU_before_software_update | PC diesel Euro-6ab SU before software update | 121961 | 0.1680% | 7.03% | 14.57% | 15.64% | 7.03% | 21.56% | 17.67% + 3.09% | - |
| PC_diesel_Euro-6ab_SU_after_software_update | PC diesel Euro-6ab SU after software update | 121962 | 1.5777% | 7.03% | 21.47% | 15.65% | 7.03% | 28.18% | 17.65% + 3.09% | - |
| PC_diesel_Euro-6c | PC diesel Euro-6c | 121965 | 0.4768% | 6.98% | 14.88% | 25.11% | 6.98% | 21.73% | 32.17% + 3.09% | - |
| PC_diesel_Euro-6d-temp | PC diesel Euro-6d-temp | 121968 | 3.5052% | 6.95% | 26.30% | 34.85% | 6.95% | 26.21% | 47.19% + 3.09% | - |
| PC_diesel_Euro-6d | PC diesel Euro-6d | 121969 | 3.2148% | 6.93% | 26.56% | 35.95% | 6.93% | 26.01% | 45.64% + 3.09% | - |
| PC_diesel_Euro-7 | PC diesel Euro-7 | 121970 | 0.0000% | 6.93% | 50.50% | 36.05% | 6.93% | 38.25% | 43.19% + 3.09% | - |
| PC_CNG_petrol_Euro-2_(CNG) | PC CNG/petrol Euro-2_(CNG) | 132921 | 0.0000% | 4.74% | 77.42% | 27.27% | 4.74% | 21.60% | 56.17% + 3.08% | - |
| PC_CNG_petrol_Euro-2_(P) | PC CNG/petrol Euro-2_(P) | 132922 | 0.0000% | 4.74% | 77.42% | 27.27% | 4.74% | 21.60% | 56.17% + 3.08% | - |
| PC_CNG_petrol_Euro-3_(CNG) | PC CNG/petrol Euro-3_(CNG) | 132931 | 0.0000% | 4.73% | 79.68% | 29.58% | 4.73% | 21.87% | 51.83% + 3.08% | - |
| PC_CNG_petrol_Euro-3_(P) | PC CNG/petrol Euro-3_(P) | 132932 | 0.0000% | 4.73% | 79.68% | 29.58% | 4.73% | 21.87% | 51.83% + 3.08% | - |
| PC_CNG_petrol_Euro-4_(CNG) | PC CNG/petrol Euro-4_(CNG) | 132941 | 0.0422% | 4.42% | 31.96% | 16.98% | 4.42% | 88.90% | 31.08% + 3.07% | - |
| PC_CNG_petrol_Euro-4_(P) | PC CNG/petrol Euro-4_(P) | 132942 | 0.0022% | 4.42% | 87.84% | 54.58% | 4.42% | 16.08% | 49.91% + 3.07% | - |
| PC_CNG_petrol_Euro-5_(CNG) | PC CNG/petrol Euro-5_(CNG) | 132951 | 0.0382% | 4.55% | 29.16% | 16.29% | 4.55% | 94.78% | 32.95% + 3.08% | - |
| PC_CNG_petrol_Euro-5_(P) | PC CNG/petrol Euro-5_(P) | 132952 | 0.0020% | 4.55% | 92.86% | 45.15% | 4.55% | 13.46% | 26.46% + 3.08% | - |
| PC_CNG_petrol_Euro-6_(CNG) | PC CNG/petrol Euro-6_(CNG) | 132961 | 0.0880% | 4.30% | 8.86% | 39.61% | 4.30% | 36.40% | 41.09% + 3.09% | - |
| PC_CNG_petrol_Euro-6_(P) | PC CNG/petrol Euro-6_(P) | 132962 | 0.0046% | 4.30% | 59.49% | 27.57% | 4.30% | 15.30% | 21.71% + 3.09% | - |
| PC_FFV_Euro-3__(E85) | PC FFV Euro-3 _(E85) | 141931 | 0.0000% | 4.73% | 79.68% | 29.58% | 4.73% | 21.87% | 51.83% + 3.08% | - |
| PC_FFV_Euro-3__(P) | PC FFV Euro-3 _(P) | 141932 | 0.0000% | 4.73% | 79.68% | 29.58% | 4.73% | 21.87% | 51.83% + 3.08% | - |
| PC_FFV_Euro-4__(E85) | PC FFV Euro-4 _(E85) | 141941 | 0.0000% | 4.32% | 89.31% | 55.25% | 4.32% | 15.90% | 50.77% + 3.07% | - |
| PC_FFV_Euro-4__(P) | PC FFV Euro-4 _(P) | 141942 | 0.0000% | 4.32% | 89.31% | 55.25% | 4.32% | 15.90% | 50.77% + 3.07% | - |
| PC_FFV_Euro-5__(E85) | PC FFV Euro-5 _(E85) | 141951 | 0.0000% | 4.36% | 92.95% | 44.90% | 4.36% | 13.61% | 26.28% + 3.08% | - |
| PC_FFV_Euro-5__(P) | PC FFV Euro-5 _(P) | 141952 | 0.0000% | 4.36% | 92.95% | 44.90% | 4.36% | 13.61% | 26.28% + 3.08% | - |
| PC_FFV_Euro-6__(E85) | PC FFV Euro-6 _(E85) | 141961 | 0.0000% | 4.17% | 60.35% | 27.12% | 4.17% | 15.20% | 21.70% + 3.09% | - |
| PC_FFV_Euro-6__(P) | PC FFV Euro-6 _(P) | 141962 | 0.0000% | 4.17% | 60.35% | 27.12% | 4.17% | 15.20% | 21.70% + 3.09% | - |
| PC_BEV | PC BEV | 153901 | 1.5055% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.09% | 5.04% |
| PC_PHEV_petrol_Euro-4_(El) | PC PHEV petrol Euro-4_(El) | 154941 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.03% | 3.95% |
| PC_PHEV_petrol_Euro-4_(P) | PC PHEV petrol Euro-4_(P) | 154942 | 0.0000% | 14.55% | 91.32% | 59.19% | 14.55% | 18.40% | 54.78% + 3.03% | - |
| PC_PHEV_petrol_Euro-5_(El) | PC PHEV petrol Euro-5_(El) | 154951 | 0.0047% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.02% | 4.05% |
| PC_PHEV_petrol_Euro-5_(P) | PC PHEV petrol Euro-5_(P) | 154952 | 0.0080% | 13.42% | 93.01% | 48.64% | 13.42% | 14.73% | 29.20% + 3.02% | - |
| PC_PHEV_petrol_Euro-6d_(El) | PC PHEV petrol Euro-6d_(El) | 154957 | 0.3666% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.03% | 4.06% |
| PC_PHEV_petrol_Euro-6d_(P) | PC PHEV petrol Euro-6d_(P) | 154958 | 0.6203% | 13.22% | 24.45% | 21.81% | 13.22% | 55.68% | 35.93% + 3.03% | - |
| PC_PHEV_petrol_Euro-6ab_(El) | PC PHEV petrol Euro-6ab_(El) | 154964 | 0.1064% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.02% | 4.24% |
| PC_PHEV_petrol_Euro-6ab_(P) | PC PHEV petrol Euro-6ab_(P) | 154965 | 0.1800% | 11.88% | 65.58% | 29.60% | 11.88% | 13.48% | 20.89% + 3.02% | - |
| PC_PHEV_diesel_Euro-4_(El) | PC PHEV diesel Euro-4_(El) | 155938 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.01% | 4.22% |
| PC_PHEV_diesel_Euro-4_(D) | PC PHEV diesel Euro-4_(D) | 155939 | 0.0000% | 12.02% | 21.26% | 4.14% | 12.02% | 25.58% | 7.97% + 3.01% | - |
| PC_PHEV_diesel_Euro-5_(El) | PC PHEV diesel Euro-5_(El) | 155951 | 0.0003% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.01% | 4.08% |
| PC_PHEV_diesel_Euro-5_(D) | PC PHEV diesel Euro-5_(D) | 155952 | 0.0004% | 12.49% | 49.46% | 7.66% | 12.49% | 18.47% | 8.63% + 3.01% | - |
| PC_PHEV_diesel_Euro-6d_(El) | PC PHEV diesel Euro-6d_(El) | 155957 | 0.0414% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.06% | 4.27% |
| PC_PHEV_diesel_Euro-6d_(D) | PC PHEV diesel Euro-6d_(D) | 155958 | 0.0701% | 12.54% | 25.36% | 34.13% | 12.54% | 26.70% | 46.46% + 3.06% | - |
| PC_PHEV_diesel_Euro-6ab_(El) | PC PHEV diesel Euro-6ab_(El) | 155964 | 0.0113% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.06% | 4.31% |
| PC_PHEV_diesel_Euro-6ab_(D) | PC PHEV diesel Euro-6ab_(D) | 155965 | 0.0191% | 12.95% | 14.77% | 15.92% | 12.95% | 22.67% | 18.14% + 3.06% | - |
| PC_LPG_petrol_Euro-2_(LPG) | PC LPG/petrol Euro-2_(LPG) | 161921 | 0.0885% | 4.74% | 77.42% | 27.27% | 4.74% | 21.60% | 0.00% + 3.08% | - |
| PC_LPG_petrol_Euro-2_(P) | PC LPG/petrol Euro-2_(P) | 161922 | 0.0047% | 4.74% | 77.42% | 27.27% | 4.74% | 21.60% | 56.17% + 3.08% | - |
| PC_LPG_petrol_Euro-3_(LPG) | PC LPG/petrol Euro-3_(LPG) | 161931 | 0.0020% | 4.73% | 79.68% | 29.58% | 4.73% | 21.87% | 0.00% + 3.08% | - |
| PC_LPG_petrol_Euro-3_(P) | PC LPG/petrol Euro-3_(P) | 161932 | 0.0001% | 4.73% | 79.68% | 29.58% | 4.73% | 21.87% | 51.83% + 3.08% | - |
| PC_LPG_petrol_Euro-4_(LPG) | PC LPG/petrol Euro-4_(LPG) | 161941 | 0.3081% | 4.32% | 89.31% | 55.25% | 4.32% | 15.90% | 0.00% + 3.07% | - |
| PC_LPG_petrol_Euro-4_(P) | PC LPG/petrol Euro-4_(P) | 161942 | 0.0162% | 4.32% | 89.31% | 55.25% | 4.32% | 15.90% | 50.77% + 3.07% | - |
| PC_LPG_petrol_Euro-5_(LPG) | PC LPG/petrol Euro-5_(LPG) | 161951 | 0.0909% | 4.36% | 92.95% | 44.90% | 4.36% | 13.61% | 0.00% + 3.08% | - |
| PC_LPG_petrol_Euro-5_(P) | PC LPG/petrol Euro-5_(P) | 161952 | 0.0048% | 4.36% | 92.95% | 44.90% | 4.36% | 13.61% | 26.28% + 3.08% | - |
| PC_LPG_petrol_Euro-6_(LPG) | PC LPG/petrol Euro-6_(LPG) | 161961 | 0.0848% | 4.17% | 60.35% | 27.12% | 4.17% | 15.20% | 0.00% + 3.09% | - |
| PC_LPG_petrol_Euro-6_(P) | PC LPG/petrol Euro-6_(P) | 161962 | 0.0045% | 4.17% | 60.35% | 27.12% | 4.17% | 15.20% | 21.70% + 3.09% | - |
| PC_FuelCell | PC FuelCell | 181901 | 0.0000% | 0.00% | 0.00% | 0.00% | 5.04% | 0.00% | 0.00% + 3.09% | - |
| PC_2S_EE | PC 2S EE | 191700 | 0.0000% | 5.41% | 42.25% | 34.93% | 5.41% | 24.35% | 66.33% + 3.07% | - |
| PC_4S_EE | PC 4S EE | 191800 | 0.0000% | 5.41% | 39.44% | 37.30% | 5.41% | 22.51% | 66.33% + 3.07% | - |

### Light Vehicles
| SUMO emission class | HBEFA subsegment | HBEFA subsegment ID | fleet share 2022 | error CO2 | error CO | error HC | error FC | error NOx | error PM | error FC_MJ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| LCV_petrol_M+N1-I_Conv_lt1981 | LCV petrol M+N1-I Conv <1981 | 212100 | 0.0000% | 6.51% | 30.77% | 10.23% | 6.51% | 29.31% | 47.95% + 2.97% | - |
| LCV_petrol_M+N1-I_Conv_gt1981 | LCV petrol M+N1-I Conv >1981 | 212101 | 0.1697% | 6.51% | 30.77% | 10.23% | 6.51% | 29.31% | 47.95% + 2.97% | - |
| LCV_petrol_M+N1-I_Euro-1 | LCV petrol M+N1-I Euro-1 | 212110 | 0.0186% | 6.52% | 50.25% | 29.38% | 6.52% | 12.67% | 46.14% + 2.96% | - |
| LCV_petrol_M+N1-I_3WCat_1987-1990 | LCV petrol M+N1-I 3WCat 1987-1990 | 212111 | 0.0000% | 6.52% | 50.25% | 29.38% | 6.52% | 12.67% | 46.14% + 2.96% | - |
| LCV_petrol_M+N1-I_Euro-2 | LCV petrol M+N1-I Euro-2 | 212120 | 0.1308% | 7.05% | 46.23% | 26.30% | 7.05% | 13.03% | 46.23% + 2.96% | - |
| LCV_petrol_M+N1-I_Euro-3 | LCV petrol M+N1-I Euro-3 | 212130 | 0.1491% | 6.88% | 50.34% | 30.24% | 6.88% | 12.41% | 49.40% + 2.98% | - |
| LCV_petrol_M+N1-I_Euro-4 | LCV petrol M+N1-I Euro-4 | 212140 | 0.3503% | 6.79% | 53.40% | 34.90% | 6.79% | 7.63% | 47.14% + 3.00% | - |
| LCV_petrol_M+N1-I_Euro-5 | LCV petrol M+N1-I Euro-5 | 212150 | 0.1523% | 6.87% | 58.23% | 38.52% | 6.87% | 7.12% | 51.57% + 3.03% | - |
| LCV_petrol_M+N1-I_Euro-6ab | LCV petrol M+N1-I Euro-6ab | 212161 | 0.1261% | 6.56% | 56.74% | 37.47% | 6.56% | 6.74% | 50.30% + 3.01% | - |
| LCV_petrol_M+N1-I_Euro-6c | LCV petrol M+N1-I Euro-6c | 212165 | 0.0135% | 6.57% | 56.81% | 37.57% | 6.57% | 6.76% | 50.32% + 3.01% | - |
| LCV_petrol_M+N1-I_Euro-6d-temp | LCV petrol M+N1-I Euro-6d-temp | 212166 | 0.0268% | 6.57% | 56.88% | 37.66% | 6.57% | 6.81% | 50.35% + 3.01% | - |
| LCV_petrol_M+N1-I_Euro-6d | LCV petrol M+N1-I Euro-6d | 212167 | 0.0215% | 6.63% | 56.99% | 37.77% | 6.63% | 6.80% | 50.42% + 3.02% | - |
| LCV_petrol_N1-II_Conv_lt1981 | LCV petrol N1-II Conv <1981 | 212200 | 0.0000% | 4.97% | 29.54% | 10.32% | 4.97% | 29.97% | 40.00% + 2.91% | - |
| LCV_petrol_N1-II_Conv_gt1981 | LCV petrol N1-II Conv >1981 | 212201 | 0.2672% | 4.97% | 29.54% | 10.32% | 4.97% | 29.97% | 40.00% + 2.91% | - |
| LCV_petrol_N1-II_Euro-1 | LCV petrol N1-II Euro-1 | 212210 | 0.0298% | 5.10% | 40.29% | 25.47% | 5.10% | 13.86% | 39.26% + 2.89% | - |
| LCV_petrol_N1-II_3WCat_1987-1990 | LCV petrol N1-II 3WCat 1987-1990 | 212211 | 0.0000% | 5.10% | 40.29% | 25.47% | 5.10% | 13.86% | 39.26% + 2.89% | - |
| LCV_petrol_N1-II_Euro-2 | LCV petrol N1-II Euro-2 | 212220 | 0.0426% | 5.30% | 44.14% | 24.13% | 5.30% | 12.66% | 44.13% + 2.92% | - |
| LCV_petrol_N1-II_Euro-3 | LCV petrol N1-II Euro-3 | 212230 | 0.0320% | 5.00% | 53.38% | 29.79% | 5.00% | 13.26% | 47.97% + 2.98% | - |
| LCV_petrol_N1-II_Euro-4 | LCV petrol N1-II Euro-4 | 212240 | 0.3073% | 5.34% | 49.52% | 32.11% | 5.34% | 7.27% | 50.72% + 2.94% | - |
| LCV_petrol_N1-II_Euro-5 | LCV petrol N1-II Euro-5 | 212250 | 0.4526% | 5.37% | 50.96% | 33.15% | 5.37% | 6.30% | 47.36% + 3.00% | - |
| LCV_petrol_N1-II_Euro-6ab | LCV petrol N1-II Euro-6ab | 212261 | 0.9679% | 5.40% | 56.51% | 36.37% | 5.40% | 6.50% | 48.40% + 3.00% | - |
| LCV_petrol_N1-II_Euro-6c | LCV petrol N1-II Euro-6c | 212265 | 0.0497% | 5.41% | 56.64% | 36.43% | 5.41% | 6.51% | 48.55% + 3.00% | - |
| LCV_petrol_N1-II_Euro-6d-temp | LCV petrol N1-II Euro-6d-temp | 212266 | 0.2804% | 5.44% | 56.89% | 36.59% | 5.44% | 6.53% | 48.77% + 3.00% | - |
| LCV_petrol_N1-II_Euro-6d | LCV petrol N1-II Euro-6d | 212267 | 0.3768% | 5.48% | 57.10% | 36.68% | 5.48% | 6.53% | 48.92% + 3.00% | - |
| LCV_petrol_N1-III_Conv_lt1981 | LCV petrol N1-III Conv <1981 | 212300 | 0.0000% | 5.99% | 30.36% | 10.00% | 5.99% | 29.31% | 46.41% + 2.95% | - |
| LCV_petrol_N1-III_Conv_gt1981 | LCV petrol N1-III Conv >1981 | 212301 | 0.2428% | 5.99% | 30.36% | 10.00% | 5.99% | 29.31% | 46.41% + 2.95% | - |
| LCV_petrol_N1-III_Euro-1 | LCV petrol N1-III Euro-1 | 212310 | 0.0472% | 5.95% | 44.26% | 26.69% | 5.95% | 13.35% | 37.75% + 2.92% | - |
| LCV_petrol_N1-III_3WCat_1987-1990 | LCV petrol N1-III 3WCat 1987-1990 | 212311 | 0.0000% | 5.95% | 44.26% | 26.69% | 5.95% | 13.35% | 37.75% + 2.92% | - |
| LCV_petrol_N1-III_Euro-2 | LCV petrol N1-III Euro-2 | 212320 | 0.0805% | 5.75% | 46.88% | 25.71% | 5.75% | 12.40% | 47.10% + 2.95% | - |
| LCV_petrol_N1-III_Euro-3 | LCV petrol N1-III Euro-3 | 212330 | 0.1111% | 5.70% | 52.34% | 29.83% | 5.70% | 12.76% | 48.04% + 2.98% | - |
| LCV_petrol_N1-III_Euro-4 | LCV petrol N1-III Euro-4 | 212340 | 0.1557% | 5.97% | 52.32% | 33.35% | 5.97% | 7.49% | 46.53% + 2.98% | - |
| LCV_petrol_N1-III_Euro-5 | LCV petrol N1-III Euro-5 | 212350 | 0.1840% | 5.88% | 54.74% | 35.47% | 5.88% | 6.48% | 48.09% + 3.00% | - |
| LCV_petrol_N1-III_Euro-6ab | LCV petrol N1-III Euro-6ab | 212361 | 0.2815% | 5.93% | 55.02% | 35.66% | 5.93% | 6.52% | 48.29% + 2.99% | - |
| LCV_petrol_N1-III_Euro-6c | LCV petrol N1-III Euro-6c | 212365 | 0.0115% | 5.91% | 55.07% | 35.73% | 5.91% | 6.50% | 48.09% + 3.00% | - |
| LCV_petrol_N1-III_Euro-6d-temp | LCV petrol N1-III Euro-6d-temp | 212366 | 0.1308% | 5.93% | 55.51% | 36.04% | 5.93% | 6.56% | 48.48% + 3.00% | - |
| LCV_petrol_N1-III_Euro-6d | LCV petrol N1-III Euro-6d | 212367 | 0.1532% | 5.94% | 55.77% | 36.18% | 5.94% | 6.58% | 48.70% + 3.00% | - |
| LCV_diesel_M+N1-I_convlt_1986 | LCV diesel M+N1-I conv< 1986 | 222100 | 0.0000% | 5.89% | 15.13% | 5.93% | 5.89% | 9.32% | 12.14% + 3.01% | - |
| LCV_diesel_M+N1-I_Euro-1 | LCV diesel M+N1-I Euro-1 | 222110 | 0.0416% | 6.42% | 12.27% | 5.16% | 6.42% | 10.85% | 10.77% + 3.02% | - |
| LCV_diesel_M+N1-I_XXIII | LCV diesel M+N1-I XXIII | 222111 | 0.0913% | 5.89% | 15.13% | 5.93% | 5.89% | 9.32% | 12.14% + 3.01% | - |
| LCV_diesel_M+N1-I_Euro-2 | LCV diesel M+N1-I Euro-2 | 222120 | 0.1653% | 6.50% | 13.64% | 3.88% | 6.50% | 11.36% | 8.43% + 3.01% | - |
| LCV_diesel_M+N1-I_Euro-3 | LCV diesel M+N1-I Euro-3 | 222130 | 0.3753% | 7.91% | 40.46% | 10.68% | 7.91% | 13.60% | 8.72% + 3.01% | - |
| LCV_diesel_M+N1-I_Euro-3_(DPF) | LCV diesel M+N1-I Euro-3 (DPF) | 222131 | 0.0000% | 7.91% | 40.46% | 10.68% | 7.91% | 13.60% | 8.72% + 3.01% | - |
| LCV_diesel_M+N1-I_Euro-4 | LCV diesel M+N1-I Euro-4 | 222140 | 0.3053% | 7.97% | 18.85% | 4.54% | 7.97% | 17.07% | 5.95% + 3.03% | - |
| LCV_diesel_M+N1-I_Euro-4_(DPF) | LCV diesel M+N1-I Euro-4 (DPF) | 222141 | 0.0000% | 7.97% | 18.85% | 4.54% | 7.97% | 17.07% | 58.63% + 3.03% | - |
| LCV_diesel_M+N1-I_Euro-5 | LCV diesel M+N1-I Euro-5 | 222150 | 0.1250% | 8.04% | 55.47% | 11.57% | 8.04% | 13.82% | 15.54% + 3.04% | - |
| LCV_diesel_M+N1-I_Euro-6ab | LCV diesel M+N1-I Euro-6ab | 222161 | 0.0226% | 7.05% | 14.46% | 18.82% | 7.05% | 17.31% | 27.54% + 3.02% | - |
| LCV_diesel_M+N1-I_Euro-6c | LCV diesel M+N1-I Euro-6c | 222165 | 0.0003% | 7.08% | 14.31% | 24.22% | 7.08% | 17.13% | 93.61% + 3.02% | - |
| LCV_diesel_M+N1-I_Euro-6d-temp | LCV diesel M+N1-I Euro-6d-temp | 222166 | 0.0028% | 7.11% | 35.66% | 41.03% | 7.11% | 26.49% | 114.11% + 3.02% | - |
| LCV_diesel_M+N1-I_Euro-6d | LCV diesel M+N1-I Euro-6d | 222167 | 0.0007% | 7.17% | 35.89% | 41.16% | 7.17% | 26.06% | 112.76% + 3.02% | - |
| LCV_diesel_N1-II_convlt_1986 | LCV diesel N1-II conv< 1986 | 222200 | 0.0000% | 5.27% | 11.34% | 5.40% | 5.27% | 7.91% | 6.68% + 2.94% | - |
| LCV_diesel_N1-II_Euro-1 | LCV diesel N1-II Euro-1 | 222210 | 0.2602% | 5.21% | 11.85% | 5.57% | 5.21% | 8.48% | 10.27% + 2.96% | - |
| LCV_diesel_N1-II_XXIII | LCV diesel N1-II XXIII | 222211 | 0.4444% | 5.27% | 11.34% | 5.40% | 5.27% | 7.91% | 6.68% + 2.94% | - |
| LCV_diesel_N1-II_Euro-2 | LCV diesel N1-II Euro-2 | 222220 | 0.2754% | 5.05% | 12.71% | 5.51% | 5.05% | 8.42% | 7.25% + 2.97% | - |
| LCV_diesel_N1-II_Euro-3 | LCV diesel N1-II Euro-3 | 222230 | 0.8154% | 5.48% | 35.84% | 10.40% | 5.48% | 8.72% | 7.55% + 2.98% | - |
| LCV_diesel_N1-II_Euro-3_(DPF) | LCV diesel N1-II Euro-3 (DPF) | 222231 | 0.0000% | 5.48% | 35.84% | 10.40% | 5.48% | 8.72% | 7.55% + 2.98% | - |
| LCV_diesel_N1-II_Euro-4 | LCV diesel N1-II Euro-4 | 222240 | 3.3960% | 6.21% | 26.10% | 4.20% | 6.21% | 10.08% | 7.58% + 2.98% | - |
| LCV_diesel_N1-II_Euro-4_(DPF) | LCV diesel N1-II Euro-4 (DPF) | 222241 | 0.0000% | 6.21% | 26.10% | 4.20% | 6.21% | 10.08% | 16.66% + 2.98% | - |
| LCV_diesel_N1-II_Euro-5 | LCV diesel N1-II Euro-5 | 222250 | 4.5286% | 5.40% | 106.64% | 13.87% | 5.40% | 16.92% | 26.02% + 2.98% | - |
| LCV_diesel_N1-II_Euro-6ab | LCV diesel N1-II Euro-6ab | 222261 | 3.4881% | 5.75% | 13.24% | 18.31% | 5.75% | 26.61% | 25.03% + 3.00% | - |
| LCV_diesel_N1-II_Euro-6c | LCV diesel N1-II Euro-6c | 222265 | 0.0005% | 5.78% | 13.68% | 22.82% | 5.78% | 27.25% | 86.28% + 3.00% | - |
| LCV_diesel_N1-II_Euro-6d-temp | LCV diesel N1-II Euro-6d-temp | 222266 | 0.8063% | 5.81% | 36.22% | 40.38% | 5.81% | 26.86% | 103.82% + 3.01% | - |
| LCV_diesel_N1-II_Euro-6d | LCV diesel N1-II Euro-6d | 222267 | 1.0097% | 5.85% | 36.22% | 40.48% | 5.85% | 26.50% | 105.71% + 3.01% | - |
| LCV_diesel_N1-III_convlt_1986 | LCV diesel N1-III conv< 1986 | 222300 | 0.0000% | 7.83% | 11.24% | 5.60% | 7.83% | 8.88% | 6.77% + 2.90% | - |
| LCV_diesel_N1-III_Euro-1 | LCV diesel N1-III Euro-1 | 222310 | 0.5585% | 8.19% | 11.86% | 5.67% | 8.19% | 8.96% | 10.79% + 2.90% | - |
| LCV_diesel_N1-III_XXIII | LCV diesel N1-III XXIII | 222311 | 0.6569% | 7.83% | 11.24% | 5.60% | 7.83% | 8.88% | 6.77% + 2.90% | - |
| LCV_diesel_N1-III_Euro-2 | LCV diesel N1-III Euro-2 | 222320 | 1.0800% | 5.78% | 12.42% | 5.61% | 5.78% | 9.71% | 7.77% + 2.99% | - |
| LCV_diesel_N1-III_Euro-3 | LCV diesel N1-III Euro-3 | 222330 | 3.8286% | 6.39% | 37.58% | 10.44% | 6.39% | 9.24% | 7.68% + 2.99% | - |
| LCV_diesel_N1-III_Euro-3_(DPF) | LCV diesel N1-III Euro-3 (DPF) | 222331 | 0.0000% | 6.39% | 37.58% | 10.44% | 6.39% | 9.24% | 7.68% + 2.99% | - |
| LCV_diesel_N1-III_Euro-4 | LCV diesel N1-III Euro-4 | 222340 | 11.8729% | 6.68% | 26.43% | 4.17% | 6.68% | 10.66% | 7.95% + 3.00% | - |
| LCV_diesel_N1-III_Euro-4_(DPF) | LCV diesel N1-III Euro-4 (DPF) | 222341 | 0.0000% | 6.68% | 26.43% | 4.17% | 6.68% | 10.66% | 17.16% + 3.00% | - |
| LCV_diesel_N1-III_Euro-5 | LCV diesel N1-III Euro-5 | 222350 | 20.0352% | 5.94% | 30.24% | 12.64% | 5.94% | 17.65% | 12.82% + 3.00% | - |
| LCV_diesel_N1-III_Euro-6ab | LCV diesel N1-III Euro-6ab | 222361 | 21.9621% | 6.39% | 32.94% | 37.98% | 6.39% | 15.68% | 20.77% + 3.03% | - |
| LCV_diesel_N1-III_Euro-6c | LCV diesel N1-III Euro-6c | 222365 | 1.2773% | 6.40% | 14.36% | 24.60% | 6.40% | 15.94% | 93.73% + 3.03% | - |
| LCV_diesel_N1-III_Euro-6d-temp | LCV diesel N1-III Euro-6d-temp | 222366 | 7.4060% | 6.43% | 36.31% | 41.80% | 6.43% | 25.54% | 112.46% + 3.03% | - |
| LCV_diesel_N1-III_Euro-6d | LCV diesel N1-III Euro-6d | 222367 | 7.8094% | 6.48% | 36.23% | 41.51% | 6.48% | 25.44% | 111.51% + 3.03% | - |
| LCV_2S_EE | LCV 2S EE | 230017 | 0.0000% | 6.51% | 35.01% | 30.26% | 6.51% | 33.93% | 47.95% + 2.97% | - |
| LCV_CNG_petrol_M+N1-I_Euro-2_(CNG) | LCV CNG/petrol M+N1-I Euro-2_(CNG) | 233121 | 0.0000% | 7.05% | 46.23% | 26.30% | 7.05% | 13.03% | 46.23% + 2.96% | - |
| LCV_CNG_petrol_M+N1-I_Euro-2_(P) | LCV CNG/petrol M+N1-I Euro-2_(P) | 233122 | 0.0000% | 7.05% | 46.23% | 26.30% | 7.05% | 13.03% | 46.23% + 2.96% | - |
| LCV_CNG_petrol_M+N1-I_Euro-3_(CNG) | LCV CNG/petrol M+N1-I Euro-3_(CNG) | 233131 | 0.0000% | 6.88% | 50.34% | 30.24% | 6.88% | 12.41% | 49.40% + 2.98% | - |
| LCV_CNG_petrol_M+N1-I_Euro-3_(P) | LCV CNG/petrol M+N1-I Euro-3_(P) | 233132 | 0.0000% | 6.88% | 50.34% | 30.24% | 6.88% | 12.41% | 49.40% + 2.98% | - |
| LCV_CNG_petrol_M+N1-I_Euro-4_(CNG) | LCV CNG/petrol M+N1-I Euro-4_(CNG) | 233141 | 0.0009% | 6.79% | 53.40% | 34.90% | 6.79% | 7.63% | 47.14% + 3.00% | - |
| LCV_CNG_petrol_M+N1-I_Euro-4_(P) | LCV CNG/petrol M+N1-I Euro-4_(P) | 233142 | 0.0000% | 6.79% | 53.40% | 34.90% | 6.79% | 7.63% | 47.14% + 3.00% | - |
| LCV_CNG_petrol_M+N1-I_Euro-5_(CNG) | LCV CNG/petrol M+N1-I Euro-5_(CNG) | 233151 | 0.0013% | 6.87% | 58.23% | 38.52% | 6.87% | 7.12% | 51.57% + 3.03% | - |
| LCV_CNG_petrol_M+N1-I_Euro-5_(P) | LCV CNG/petrol M+N1-I Euro-5_(P) | 233152 | 0.0001% | 6.87% | 58.23% | 38.52% | 6.87% | 7.12% | 51.57% + 3.03% | - |
| LCV_CNG_petrol_M+N1-I_Euro-6_(CNG) | LCV CNG/petrol M+N1-I Euro-6_(CNG) | 233161 | 0.0026% | 6.63% | 56.99% | 37.77% | 6.63% | 6.80% | 50.42% + 3.02% | - |
| LCV_CNG_petrol_M+N1-I_Euro-6_(P) | LCV CNG/petrol M+N1-I Euro-6_(P) | 233162 | 0.0001% | 6.63% | 56.99% | 37.77% | 6.63% | 6.80% | 50.42% + 3.02% | - |
| LCV_CNG_petrol_N1-II_Euro-2_(CNG) | LCV CNG/petrol N1-II Euro-2_(CNG) | 233221 | 0.0000% | 5.30% | 44.14% | 24.13% | 5.30% | 12.66% | 44.13% + 2.92% | - |
| LCV_CNG_petrol_N1-II_Euro-2_(P) | LCV CNG/petrol N1-II Euro-2_(P) | 233222 | 0.0000% | 5.30% | 44.14% | 24.13% | 5.30% | 12.66% | 44.13% + 2.92% | - |
| LCV_CNG_petrol_N1-II_Euro-3_(CNG) | LCV CNG/petrol N1-II Euro-3_(CNG) | 233231 | 0.0000% | 5.00% | 53.38% | 29.79% | 5.00% | 13.26% | 47.97% + 2.98% | - |
| LCV_CNG_petrol_N1-II_Euro-3_(P) | LCV CNG/petrol N1-II Euro-3_(P) | 233232 | 0.0000% | 5.00% | 53.38% | 29.79% | 5.00% | 13.26% | 47.97% + 2.98% | - |
| LCV_CNG_petrol_N1-II_Euro-4_(CNG) | LCV CNG/petrol N1-II Euro-4_(CNG) | 233241 | 0.0851% | 5.34% | 49.52% | 32.11% | 5.34% | 7.27% | 50.72% + 2.94% | - |
| LCV_CNG_petrol_N1-II_Euro-4_(P) | LCV CNG/petrol N1-II Euro-4_(P) | 233242 | 0.0045% | 5.34% | 49.52% | 32.11% | 5.34% | 7.27% | 50.72% + 2.94% | - |
| LCV_CNG_petrol_N1-II_Euro-5_(CNG) | LCV CNG/petrol N1-II Euro-5_(CNG) | 233251 | 0.0197% | 5.37% | 50.96% | 33.15% | 5.37% | 6.30% | 47.36% + 3.00% | - |
| LCV_CNG_petrol_N1-II_Euro-5_(P) | LCV CNG/petrol N1-II Euro-5_(P) | 233252 | 0.0010% | 5.37% | 50.96% | 33.15% | 5.37% | 6.30% | 47.36% + 3.00% | - |
| LCV_CNG_petrol_N1-II_Euro-6_(CNG) | LCV CNG/petrol N1-II Euro-6_(CNG) | 233261 | 0.0554% | 5.48% | 57.10% | 36.68% | 5.48% | 6.53% | 48.92% + 3.00% | - |
| LCV_CNG_petrol_N1-II_Euro-6_(P) | LCV CNG/petrol N1-II Euro-6_(P) | 233262 | 0.0029% | 5.48% | 57.10% | 36.68% | 5.48% | 6.53% | 48.92% + 3.00% | - |
| LCV_CNG_petrol_N1-III_Euro-2_(CNG) | LCV CNG/petrol N1-III Euro-2_(CNG) | 233321 | 0.0000% | 5.75% | 46.88% | 25.71% | 5.75% | 12.40% | 47.10% + 2.95% | - |
| LCV_CNG_petrol_N1-III_Euro-2_(P) | LCV CNG/petrol N1-III Euro-2_(P) | 233322 | 0.0000% | 5.75% | 46.88% | 25.71% | 5.75% | 12.40% | 47.10% + 2.95% | - |
| LCV_CNG_petrol_N1-III_Euro-3_(CNG) | LCV CNG/petrol N1-III Euro-3_(CNG) | 233331 | 0.0000% | 5.70% | 52.34% | 29.83% | 5.70% | 12.76% | 48.04% + 2.98% | - |
| LCV_CNG_petrol_N1-III_Euro-3_(P) | LCV CNG/petrol N1-III Euro-3_(P) | 233332 | 0.0000% | 5.70% | 52.34% | 29.83% | 5.70% | 12.76% | 48.04% + 2.98% | - |
| LCV_CNG_petrol_N1-III_Euro-4_(CNG) | LCV CNG/petrol N1-III Euro-4_(CNG) | 233341 | 0.0734% | 5.97% | 52.32% | 33.35% | 5.97% | 7.49% | 46.53% + 2.98% | - |
| LCV_CNG_petrol_N1-III_Euro-4_(P) | LCV CNG/petrol N1-III Euro-4_(P) | 233342 | 0.0039% | 5.97% | 52.32% | 33.35% | 5.97% | 7.49% | 46.53% + 2.98% | - |
| LCV_CNG_petrol_N1-III_Euro-5_(CNG) | LCV CNG/petrol N1-III Euro-5_(CNG) | 233351 | 0.0666% | 5.88% | 54.74% | 35.47% | 5.88% | 6.48% | 48.09% + 3.00% | - |
| LCV_CNG_petrol_N1-III_Euro-5_(P) | LCV CNG/petrol N1-III Euro-5_(P) | 233352 | 0.0035% | 5.88% | 54.74% | 35.47% | 5.88% | 6.48% | 48.09% + 3.00% | - |
| LCV_CNG_petrol_N1-III_Euro-6_(CNG) | LCV CNG/petrol N1-III Euro-6_(CNG) | 233361 | 0.1046% | 5.94% | 55.77% | 36.18% | 5.94% | 6.58% | 48.70% + 3.00% | - |
| LCV_CNG_petrol_N1-III_Euro-6_(P) | LCV CNG/petrol N1-III Euro-6_(P) | 233362 | 0.0055% | 5.94% | 55.77% | 36.18% | 5.94% | 6.58% | 48.70% + 3.00% | - |
| LCV_FFV_M+N1-I_Euro-3__(E85) | LCV FFV M+N1-I Euro-3 _(E85) | 242131 | 0.0000% | 6.88% | 50.34% | 30.24% | 6.88% | 12.41% | 49.40% + 2.98% | - |
| LCV_FFV_M+N1-I_Euro-3__(P) | LCV FFV M+N1-I Euro-3 _(P) | 242132 | 0.0000% | 6.88% | 50.34% | 30.24% | 6.88% | 12.41% | 49.40% + 2.98% | - |
| LCV_FFV_M+N1-I_Euro-4__(E85) | LCV FFV M+N1-I Euro-4 _(E85) | 242141 | 0.0000% | 6.79% | 53.40% | 34.90% | 6.79% | 7.63% | 47.14% + 3.00% | - |
| LCV_FFV_M+N1-I_Euro-4__(P) | LCV FFV M+N1-I Euro-4 _(P) | 242142 | 0.0000% | 6.79% | 53.40% | 34.90% | 6.79% | 7.63% | 47.14% + 3.00% | - |
| LCV_FFV_M+N1-I_Euro-5__(E85) | LCV FFV M+N1-I Euro-5 _(E85) | 242151 | 0.0000% | 6.87% | 58.23% | 38.52% | 6.87% | 7.12% | 51.57% + 3.03% | - |
| LCV_FFV_M+N1-I_Euro-5__(P) | LCV FFV M+N1-I Euro-5 _(P) | 242152 | 0.0000% | 6.87% | 58.23% | 38.52% | 6.87% | 7.12% | 51.57% + 3.03% | - |
| LCV_FFV_M+N1-I_Euro-6__(E85) | LCV FFV M+N1-I Euro-6 _(E85) | 242161 | 0.0000% | 6.56% | 56.74% | 37.47% | 6.56% | 6.74% | 50.30% + 3.01% | - |
| LCV_FFV_M+N1-I_Euro-6__(P) | LCV FFV M+N1-I Euro-6 _(P) | 242162 | 0.0000% | 6.56% | 56.74% | 37.47% | 6.56% | 6.74% | 50.30% + 3.01% | - |
| LCV_FFV_N1-II_Euro-3__(E85) | LCV FFV N1-II Euro-3 _(E85) | 242231 | 0.0000% | 5.00% | 53.38% | 29.79% | 5.00% | 13.26% | 47.97% + 2.98% | - |
| LCV_FFV_N1-II_Euro-3__(P) | LCV FFV N1-II Euro-3 _(P) | 242232 | 0.0000% | 5.00% | 53.38% | 29.79% | 5.00% | 13.26% | 47.97% + 2.98% | - |
| LCV_FFV_N1-II_Euro-4__(E85) | LCV FFV N1-II Euro-4 _(E85) | 242241 | 0.0000% | 5.34% | 49.52% | 32.11% | 5.34% | 7.27% | 50.72% + 2.94% | - |
| LCV_FFV_N1-II_Euro-4__(P) | LCV FFV N1-II Euro-4 _(P) | 242242 | 0.0000% | 5.34% | 49.52% | 32.11% | 5.34% | 7.27% | 50.72% + 2.94% | - |
| LCV_FFV_N1-II_Euro-5__(E85) | LCV FFV N1-II Euro-5 _(E85) | 242251 | 0.0000% | 5.37% | 50.96% | 33.15% | 5.37% | 6.30% | 47.36% + 3.00% | - |
| LCV_FFV_N1-II_Euro-5__(P) | LCV FFV N1-II Euro-5 _(P) | 242252 | 0.0000% | 5.37% | 50.96% | 33.15% | 5.37% | 6.30% | 47.36% + 3.00% | - |
| LCV_FFV_N1-II_Euro-6__(E85) | LCV FFV N1-II Euro-6 _(E85) | 242261 | 0.0000% | 5.40% | 56.51% | 36.37% | 5.40% | 6.50% | 48.40% + 3.00% | - |
| LCV_FFV_N1-II_Euro-6__(P) | LCV FFV N1-II Euro-6 _(P) | 242262 | 0.0000% | 5.40% | 56.51% | 36.37% | 5.40% | 6.50% | 48.40% + 3.00% | - |
| LCV_FFV_N1-III_Euro-3__(E85) | LCV FFV N1-III Euro-3 _(E85) | 242331 | 0.0000% | 5.70% | 52.34% | 29.83% | 5.70% | 12.76% | 48.04% + 2.98% | - |
| LCV_FFV_N1-III_Euro-3__(P) | LCV FFV N1-III Euro-3 _(P) | 242332 | 0.0000% | 5.70% | 52.34% | 29.83% | 5.70% | 12.76% | 48.04% + 2.98% | - |
| LCV_FFV_N1-III_Euro-4__(E85) | LCV FFV N1-III Euro-4 _(E85) | 242341 | 0.0000% | 5.97% | 52.32% | 33.35% | 5.97% | 7.49% | 46.53% + 2.98% | - |
| LCV_FFV_N1-III_Euro-4__(P) | LCV FFV N1-III Euro-4 _(P) | 242342 | 0.0000% | 5.97% | 52.32% | 33.35% | 5.97% | 7.49% | 46.53% + 2.98% | - |
| LCV_FFV_N1-III_Euro-5__(E85) | LCV FFV N1-III Euro-5 _(E85) | 242351 | 0.0000% | 5.88% | 54.74% | 35.47% | 5.88% | 6.48% | 48.09% + 3.00% | - |
| LCV_FFV_N1-III_Euro-5__(P) | LCV FFV N1-III Euro-5 _(P) | 242352 | 0.0000% | 5.88% | 54.74% | 35.47% | 5.88% | 6.48% | 48.09% + 3.00% | - |
| LCV_FFV_N1-III_Euro-6__(E85) | LCV FFV N1-III Euro-6 _(E85) | 242361 | 0.0000% | 5.93% | 55.02% | 35.66% | 5.93% | 6.52% | 48.29% + 2.99% | - |
| LCV_FFV_N1-III_Euro-6__(P) | LCV FFV N1-III Euro-6 _(P) | 242362 | 0.0000% | 5.93% | 55.02% | 35.66% | 5.93% | 6.52% | 48.29% + 2.99% | - |
| LCV_BEV_M+N1-I | LCV BEV M+N1-I | 253101 | 0.0533% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.09% | 5.02% |
| LCV_BEV_N1-II | LCV BEV N1-II | 253201 | 0.4551% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.07% | 5.06% |
| LCV_BEV_N1-IIII | LCV BEV N1-IIII | 253301 | 0.9939% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.03% | 5.85% |
| LCV_PHEV_petrol_M+N1-I_Euro-5_(El) | LCV PHEV petrol M+N1-I Euro-5_(El) | 254151 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.04% | 6.07% |
| LCV_PHEV_petrol_M+N1-I_Euro-5_(P) | LCV PHEV petrol M+N1-I Euro-5_(P) | 254152 | 0.0000% | 10.86% | 70.79% | 48.96% | 10.86% | 7.58% | 61.78% + 3.04% | - |
| LCV_PHEV_petrol_M+N1-I_Euro-6_(El) | LCV PHEV petrol M+N1-I Euro-6_(El) | 254161 | 0.0001% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.03% | 5.45% |
| LCV_PHEV_petrol_M+N1-I_Euro-6_(P) | LCV PHEV petrol M+N1-I Euro-6_(P) | 254162 | 0.0001% | 15.47% | 70.59% | 49.32% | 15.47% | 7.31% | 60.95% + 3.03% | - |
| LCV_PHEV_petrol_N1-II_Euro-5_(El) | LCV PHEV petrol N1-II Euro-5_(El) | 254251 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.93% | 3.62% |
| LCV_PHEV_petrol_N1-II_Euro-5_(P) | LCV PHEV petrol N1-II Euro-5_(P) | 254252 | 0.0000% | 6.74% | 63.61% | 44.29% | 6.74% | 6.54% | 57.90% + 2.93% | - |
| LCV_PHEV_petrol_N1-II_Euro-6_(El) | LCV PHEV petrol N1-II Euro-6_(El) | 254261 | 0.0045% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.95% | 3.88% |
| LCV_PHEV_petrol_N1-II_Euro-6_(P) | LCV PHEV petrol N1-II Euro-6_(P) | 254262 | 0.0040% | 13.75% | 70.54% | 48.63% | 13.75% | 6.80% | 60.53% + 2.95% | - |
| LCV_PHEV_petrol_N1-III_Euro-5_(El) | LCV PHEV petrol N1-III Euro-5_(El) | 254351 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.97% | 5.48% |
| LCV_PHEV_petrol_N1-III_Euro-5_(P) | LCV PHEV petrol N1-III Euro-5_(P) | 254352 | 0.0000% | 7.26% | 67.66% | 46.70% | 7.26% | 6.81% | 59.03% + 2.97% | - |
| LCV_PHEV_petrol_N1-III_Euro-6_(El) | LCV PHEV petrol N1-III Euro-6_(El) | 254361 | 0.0280% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.97% | 5.42% |
| LCV_PHEV_petrol_N1-III_Euro-6_(P) | LCV PHEV petrol N1-III Euro-6_(P) | 254362 | 0.0250% | 14.58% | 69.06% | 47.93% | 14.58% | 6.93% | 60.12% + 2.97% | - |
| LCV_PHEV_diesel_M+N1-I_Euro-5_(El) | LCV PHEV diesel M+N1-I Euro-5_(El) | 255151 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 3.00% | 6.83% |
| LCV_PHEV_diesel_M+N1-I_Euro-5_(D) | LCV PHEV diesel M+N1-I Euro-5_(D) | 255152 | 0.0000% | 9.77% | 56.35% | 11.36% | 9.77% | 14.31% | 17.47% + 3.00% | - |
| LCV_PHEV_diesel_M+N1-I_Euro-6_(El) | LCV PHEV diesel M+N1-I Euro-6_(El) | 255161 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.99% | 6.18% |
| LCV_PHEV_diesel_M+N1-I_Euro-6_(D) | LCV PHEV diesel M+N1-I Euro-6_(D) | 255162 | 0.0000% | 7.87% | 34.74% | 40.49% | 7.87% | 30.77% | 116.64% + 2.99% | - |
| LCV_PHEV_diesel_N1-II_Euro-5_(El) | LCV PHEV diesel N1-II Euro-5_(El) | 255251 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.93% | 6.08% |
| LCV_PHEV_diesel_N1-II_Euro-5_(D) | LCV PHEV diesel N1-II Euro-5_(D) | 255252 | 0.0000% | 6.01% | 106.64% | 12.89% | 6.01% | 17.25% | 22.31% + 2.93% | - |
| LCV_PHEV_diesel_N1-II_Euro-6_(El) | LCV PHEV diesel N1-II Euro-6_(El) | 255261 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.94% | 7.07% |
| LCV_PHEV_diesel_N1-II_Euro-6_(D) | LCV PHEV diesel N1-II Euro-6_(D) | 255262 | 0.0000% | 6.25% | 35.18% | 40.47% | 6.25% | 31.01% | 109.53% + 2.94% | - |
| LCV_PHEV_diesel_N1-III_Euro-5_(El) | LCV PHEV diesel N1-III Euro-5_(El) | 255351 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.96% | 7.80% |
| LCV_PHEV_diesel_N1-III_Euro-5_(D) | LCV PHEV diesel N1-III Euro-5_(D) | 255352 | 0.0000% | 6.48% | 28.98% | 12.40% | 6.48% | 23.38% | 12.12% + 2.96% | - |
| LCV_PHEV_diesel_N1-III_Euro-6_(El) | LCV PHEV diesel N1-III Euro-6_(El) | 255361 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 2.97% | 7.09% |
| LCV_PHEV_diesel_N1-III_Euro-6_(D) | LCV PHEV diesel N1-III Euro-6_(D) | 255362 | 0.0000% | 6.34% | 35.38% | 41.79% | 6.34% | 29.13% | 117.33% + 2.97% | - |
| LCV_FuelCell_N1-III | LCV FuelCell N1-III | 281301 | 0.0000% | 0.00% | 0.00% | 0.00% | 5.85% | 0.00% | 0.00% + 3.03% | - |

### Coaches
| SUMO emission class | HBEFA subsegment | HBEFA subsegment ID | fleet share 2022 | error CO2 | error CO | error HC | error FC | error NOx | error PM | error FC_MJ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Coach_Std_le18t_50ties | Coach Std <=18t 50ties | 626105 | 0.0000% | - | 12.27% | 15.60% | 20.02% | 21.28% | 11.69% | - |
| Coach_Std_le18t_60ties | Coach Std <=18t 60ties | 626106 | 0.0000% | - | 12.27% | 15.60% | 20.02% | 21.28% | 11.69% | - |
| Coach_Std_le18t_70ties | Coach Std <=18t 70ties | 626107 | 0.0000% | - | 12.27% | 15.60% | 20.02% | 21.28% | 11.69% | - |
| Coach_Std_le18t_80ties | Coach Std <=18t 80ties | 626108 | 0.7483% | - | 12.27% | 15.60% | 20.02% | 21.28% | 11.69% | - |
| Coach_Std_le18t_Euro-I | Coach Std <=18t Euro-I | 626110 | 0.1373% | - | 17.11% | 8.60% | 20.17% | 19.33% | 13.39% | - |
| Coach_Std_le18t_Euro-II | Coach Std <=18t Euro-II | 626120 | 0.8803% | - | 15.73% | 8.40% | 20.36% | 17.57% | 13.96% | - |
| Coach_Std_le18t_Euro-III | Coach Std <=18t Euro-III | 626130 | 2.7014% | - | 15.72% | 10.27% | 20.70% | 17.45% | 10.38% | - |
| Coach_Std_le18t_Euro-IV_EGR | Coach Std <=18t Euro-IV EGR | 626141 | 0.4671% | - | 15.13% | 18.24% | 21.75% | 21.11% | 19.31% | - |
| Coach_Std_le18t_Euro-IV_SCR | Coach Std <=18t Euro-IV SCR | 626142 | 1.4013% | - | 30.42% | 14.75% | 21.70% | 19.59% | 23.24% | - |
| Coach_Std_le18t_Euro-IV_SCRs | Coach Std <=18t Euro-IV SCR* | 626143 | 0.0000% | - | 30.42% | 14.75% | 21.70% | 18.20% | 23.24% | - |
| Coach_Std_le18t_Euro-V_EGR | Coach Std <=18t Euro-V EGR | 626151 | 2.4364% | - | 32.39% | 9.63% | 22.09% | 17.56% | 16.84% | - |
| Coach_Std_le18t_Euro-V_SCR | Coach Std <=18t Euro-V SCR | 626152 | 7.3092% | - | 29.58% | 16.74% | 22.01% | 23.41% | 23.41% | - |
| Coach_Std_le18t_Euro-V_SCRs | Coach Std <=18t Euro-V SCR* | 626153 | 0.0000% | - | 29.58% | 16.74% | 22.01% | 18.45% | 23.41% | - |
| Coach_Std_le18t_Euro-VI_A-C | Coach Std <=18t Euro-VI A-C | 626164 | 15.8400% | - | 16.06% | 18.59% | 22.24% | 28.42% | 23.70% | - |
| Coach_Std_le18t_Euro-VIs_A-C | Coach Std <=18t Euro-VI* A-C | 626165 | 0.0000% | - | 16.06% | 18.59% | 22.24% | 24.36% | 23.70% | - |
| Coach_Std_le18t_Euro-VI_D-E | Coach Std <=18t Euro-VI D-E | 626166 | 11.5902% | - | 77.58% | 4.86% | 22.53% | 44.94% | 16.77% | - |
| Coach_Std_le18t_Euro-VIs_D-E | Coach Std <=18t Euro-VI* D-E | 626167 | 0.0000% | - | 77.58% | 4.86% | 22.53% | 21.19% | 16.77% | - |
| Coach_3-Axes_gt18t_50ties | Coach 3-Axes >18t 50ties | 626205 | 0.0000% | - | 11.63% | 14.98% | 21.74% | 23.08% | 12.50% | - |
| Coach_3-Axes_gt18t_60ties | Coach 3-Axes >18t 60ties | 626206 | 0.0000% | - | 11.63% | 14.98% | 21.74% | 23.08% | 12.50% | - |
| Coach_3-Axes_gt18t_70ties | Coach 3-Axes >18t 70ties | 626207 | 0.0000% | - | 11.63% | 14.98% | 21.74% | 23.08% | 12.50% | - |
| Coach_3-Axes_gt18t_80ties | Coach 3-Axes >18t 80ties | 626208 | 0.0481% | - | 11.63% | 14.98% | 21.74% | 23.08% | 12.50% | - |
| Coach_3-Axes_gt18t_Euro-I | Coach 3-Axes >18t Euro-I | 626210 | 0.0223% | - | 17.54% | 8.74% | 22.06% | 21.42% | 14.84% | - |
| Coach_3-Axes_gt18t_Euro-II | Coach 3-Axes >18t Euro-II | 626220 | 0.2175% | - | 18.40% | 8.85% | 22.50% | 19.63% | 15.51% | - |
| Coach_3-Axes_gt18t_Euro-III | Coach 3-Axes >18t Euro-III | 626230 | 1.8179% | - | 19.45% | 9.50% | 23.16% | 20.78% | 9.62% | - |
| Coach_3-Axes_gt18t_Euro-IV_EGR | Coach 3-Axes >18t Euro-IV EGR | 626241 | 0.4848% | - | 17.35% | 19.78% | 23.85% | 24.97% | 19.93% | - |
| Coach_3-Axes_gt18t_Euro-IV_SCR | Coach 3-Axes >18t Euro-IV SCR | 626242 | 1.4545% | - | 32.60% | 16.65% | 23.67% | 18.73% | 23.81% | - |
| Coach_3-Axes_gt18t_Euro-IV_SCRs | Coach 3-Axes >18t Euro-IV SCR* | 626243 | 0.0000% | - | 32.60% | 16.65% | 23.67% | 20.04% | 23.81% | - |
| Coach_3-Axes_gt18t_Euro-V_EGR | Coach 3-Axes >18t Euro-V EGR | 626251 | 3.8027% | - | 31.19% | 9.70% | 24.21% | 20.77% | 18.49% | - |
| Coach_3-Axes_gt18t_Euro-V_SCR | Coach 3-Axes >18t Euro-V SCR | 626252 | 11.4081% | - | 32.55% | 19.19% | 24.16% | 22.99% | 24.63% | - |
| Coach_3-Axes_gt18t_Euro-V_SCRs | Coach 3-Axes >18t Euro-V SCR* | 626253 | 0.0000% | - | 32.55% | 19.19% | 24.16% | 20.23% | 24.63% | - |
| Coach_3-Axes_gt18t_Euro-VI_A-C | Coach 3-Axes >18t Euro-VI A-C | 626264 | 27.1243% | - | 17.91% | 22.58% | 24.24% | 29.23% | 24.07% | - |
| Coach_3-Axes_gt18t_Euro-VIs_A-C | Coach 3-Axes >18t Euro-VI* A-C | 626265 | 0.0000% | - | 17.91% | 22.58% | 24.24% | 25.77% | 24.07% | - |
| Coach_3-Axes_gt18t_Euro-VI_D-E | Coach 3-Axes >18t Euro-VI D-E | 626266 | 9.8550% | - | 74.91% | 5.52% | 24.63% | 44.66% | 19.54% | - |
| Coach_3-Axes_gt18t_Euro-VIs_D-E | Coach 3-Axes >18t Euro-VI* D-E | 626267 | 0.0000% | - | 74.91% | 5.52% | 24.63% | 23.09% | 19.54% | - |
| Coach_Midi_le15t_50ties | Coach Midi <=15t 50ties | 626305 | 0.0000% | - | 15.52% | 18.63% | 13.67% | 11.65% | 11.36% | - |
| Coach_Midi_le15t_60ties | Coach Midi <=15t 60ties | 626306 | 0.0000% | - | 15.52% | 18.63% | 13.67% | 11.65% | 11.36% | - |
| Coach_Midi_le15t_70ties | Coach Midi <=15t 70ties | 626307 | 0.0000% | - | 15.52% | 18.63% | 13.67% | 11.65% | 11.36% | - |
| Coach_Midi_le15t_80ties | Coach Midi <=15t 80ties | 626308 | 0.0000% | - | 15.52% | 18.63% | 13.67% | 11.65% | 11.36% | - |
| Coach_Midi_le15t_Euro-I | Coach Midi <=15t Euro-I | 626310 | 0.0000% | - | 17.25% | 10.06% | 14.24% | 14.28% | 10.42% | - |
| Coach_Midi_le15t_Euro-II | Coach Midi <=15t Euro-II | 626320 | 0.0000% | - | 22.91% | 8.27% | 14.93% | 12.50% | 13.53% | - |
| Coach_Midi_le15t_Euro-III | Coach Midi <=15t Euro-III | 626330 | 0.0000% | - | 20.69% | 9.33% | 15.31% | 12.40% | 10.83% | - |
| Coach_Midi_le15t_Euro-IV_EGR | Coach Midi <=15t Euro-IV EGR | 626341 | 0.0000% | - | 20.37% | 23.57% | 16.01% | 15.54% | 24.94% | - |
| Coach_Midi_le15t_Euro-IV_SCR | Coach Midi <=15t Euro-IV SCR | 626342 | 0.0000% | - | 35.90% | 16.93% | 16.11% | 12.74% | 25.41% | - |
| Coach_Midi_le15t_Euro-IV_SCRs | Coach Midi <=15t Euro-IV SCR* | 626343 | 0.0000% | - | 35.90% | 16.93% | 16.11% | 13.16% | 25.41% | - |
| Coach_Midi_le15t_Euro-V_EGR | Coach Midi <=15t Euro-V EGR | 626351 | 0.0000% | - | 37.39% | 8.15% | 16.65% | 19.39% | 19.40% | - |
| Coach_Midi_le15t_Euro-V_SCR | Coach Midi <=15t Euro-V SCR | 626352 | 0.0000% | - | 38.89% | 16.78% | 16.84% | 19.35% | 29.54% | - |
| Coach_Midi_le15t_Euro-V_SCRs | Coach Midi <=15t Euro-V SCR* | 626353 | 0.0000% | - | 38.89% | 16.78% | 16.84% | 14.77% | 29.54% | - |
| Coach_Midi_le15t_Euro-VI_A-C | Coach Midi <=15t Euro-VI A-C | 626364 | 0.0000% | - | 15.59% | 27.08% | 16.88% | 34.42% | 20.89% | - |
| Coach_Midi_le15t_Euro-VIs_A-C | Coach Midi <=15t Euro-VI* A-C | 626365 | 0.0000% | - | 15.59% | 27.08% | 16.88% | 17.81% | 20.89% | - |
| Coach_Midi_le15t_Euro-VI_D-E | Coach Midi <=15t Euro-VI D-E | 626366 | 0.0000% | - | 92.12% | 7.55% | 17.21% | 17.70% | 19.63% | - |
| Coach_Midi_le15t_Euro-VIs_D-E | Coach Midi <=15t Euro-VI* D-E | 626367 | 0.0000% | - | 92.12% | 7.55% | 17.21% | 20.66% | 19.63% | - |
| Coach_Std_le18t_CNG_Euro-I | Coach Std <=18t CNG Euro-I | 636110 | 0.0000% | - | 17.11% | 8.60% | 20.17% | 19.33% | 13.39% | - |
| Coach_Std_le18t_CNG_Euro-II | Coach Std <=18t CNG Euro-II | 636120 | 0.0000% | - | 15.73% | 8.40% | 20.36% | 17.57% | 13.96% | - |
| Coach_Std_le18t_CNG_Euro-III | Coach Std <=18t CNG Euro-III | 636130 | 0.0000% | - | 15.72% | 10.27% | 20.70% | 17.45% | 10.38% | - |
| Coach_Std_le18t_CNG_Euro-IV | Coach Std <=18t CNG Euro-IV | 636140 | 0.0000% | - | 32.39% | 9.63% | 21.75% | 17.56% | 19.31% | - |
| Coach_Std_le18t_CNG_Euro-V | Coach Std <=18t CNG Euro-V | 636150 | 0.0000% | - | 22.01% | 22.01% | 22.01% | 22.01% | 22.01% | - |
| Coach_Std_le18t_CNG_Euro-VI | Coach Std <=18t CNG Euro-VI | 636160 | 0.0074% | - | 22.24% | 22.24% | 22.24% | 22.24% | 22.24% | - |
| Coach_3-Axes_gt18t_CNG_Euro-I | Coach 3-Axes >18t CNG Euro-I | 636210 | 0.0000% | - | 17.54% | 8.74% | 22.06% | 21.42% | 14.84% | - |
| Coach_3-Axes_gt18t_CNG_Euro-II | Coach 3-Axes >18t CNG Euro-II | 636220 | 0.0000% | - | 18.40% | 8.85% | 22.50% | 19.63% | 15.51% | - |
| Coach_3-Axes_gt18t_CNG_Euro-III | Coach 3-Axes >18t CNG Euro-III | 636230 | 0.0000% | - | 19.45% | 9.50% | 23.16% | 20.78% | 9.62% | - |
| Coach_3-Axes_gt18t_CNG_Euro-IV | Coach 3-Axes >18t CNG Euro-IV | 636240 | 0.0000% | - | 31.19% | 9.70% | 23.85% | 20.77% | 19.93% | - |
| Coach_3-Axes_gt18t_CNG_Euro-V | Coach 3-Axes >18t CNG Euro-V | 636250 | 0.0000% | - | 24.16% | 24.16% | 24.16% | 24.16% | 24.16% | - |
| Coach_3-Axes_gt18t_CNG_Euro-VI | Coach 3-Axes >18t CNG Euro-VI | 636260 | 0.0055% | - | 24.24% | 24.24% | 24.24% | 24.24% | 24.24% | - |
| Coach_Midi_le15t_CNG_Euro-I | Coach Midi <=15t CNG Euro-I | 636310 | 0.0000% | - | 17.25% | 10.06% | 14.24% | 14.28% | 10.42% | - |
| Coach_Midi_le15t_CNG_Euro-II | Coach Midi <=15t CNG Euro-II | 636320 | 0.0000% | - | 22.91% | 8.27% | 14.93% | 12.50% | 13.53% | - |
| Coach_Midi_le15t_CNG_Euro-III | Coach Midi <=15t CNG Euro-III | 636330 | 0.0000% | - | 20.69% | 9.33% | 15.31% | 12.40% | 10.83% | - |
| Coach_Midi_le15t_CNG_Euro-IV | Coach Midi <=15t CNG Euro-IV | 636340 | 0.0000% | - | 37.39% | 8.15% | 16.01% | 19.39% | 24.94% | - |
| Coach_Midi_le15t_CNG_Euro-V | Coach Midi <=15t CNG Euro-V | 636350 | 0.0000% | - | 16.84% | 16.84% | 16.84% | 16.84% | 16.84% | - |
| Coach_Midi_le15t_CNG_Euro-VI | Coach Midi <=15t CNG Euro-VI | 636360 | 0.0000% | - | 14.17% | 14.17% | 14.17% | 14.17% | 14.17% | - |
| Coach_BEV_Std_le18t | Coach BEV Std <=18t | 656100 | 0.0899% | - | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 6.13% |
| Coach_BEV_3-Axes_gt18t | Coach BEV 3-Axes >18t | 656200 | 0.1506% | - | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 6.57% |
| Coach_BEV_Midi | Coach BEV Midi | 656300 | 0.0000% | - | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 3.91% |
| Coach_FuelCell_Std_le18t | Coach FuelCell Std <=18t | 686101 | 0.0000% | - | 0.00% | 0.00% | 6.13% | 0.00% | 0.00% | - |
| Coach_FuelCell_Std_3-Axes_gt18t | Coach FuelCell Std 3-Axes >18t | 686200 | 0.0000% | - | 0.00% | 0.00% | 6.57% | 0.00% | 0.00% | - |
| Coach_FuelCell_Std_Midi | Coach FuelCell Std Midi | 686300 | 0.0000% | - | 0.00% | 0.00% | 3.91% | 0.00% | 0.00% | - |
| Coach_Std_le18t_EE | Coach Std <=18t EE | 696909 | 0.0000% | - | 12.27% | 15.60% | 20.02% | 21.28% | 11.69% | - |

### City Bus
| SUMO emission class | HBEFA subsegment | HBEFA subsegment ID | fleet share 2022 | error CO2 | error CO | error HC | error FC | error NOx | error PM | error FC_MJ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| UBus_Midi_P_le15t_Euro-IV | UBus Midi P <=15t Euro-IV | 717140 | 0.0000% | 18.23% | 17.77% | 15.02% | 18.23% | 21.40% | 18.52% + 15.91% | - |
| UBus_Midi_P_le15t_Euro-V | UBus Midi P <=15t Euro-V | 717150 | 0.0000% | 18.88% | 44.99% | 8.75% | 18.88% | 18.80% | 13.10% + 15.76% | - |
| UBus_Midi_P_le15t_Euro-VI | UBus Midi P <=15t Euro-VI | 717161 | 0.0000% | 19.30% | 22.57% | 15.43% | 19.30% | 36.65% | 38.60% + 15.91% | - |
| UBus_Midi_le15t_50ties | UBus Midi <=15t 50ties | 727105 | 0.0000% | 15.37% | 11.90% | 18.26% | 15.37% | 13.80% | 9.69% + 14.90% | - |
| UBus_Midi_le15t_60ties | UBus Midi <=15t 60ties | 727106 | 0.0000% | 15.37% | 11.90% | 18.26% | 15.37% | 13.80% | 9.69% + 14.90% | - |
| UBus_Midi_le15t_70ties | UBus Midi <=15t 70ties | 727107 | 0.0000% | 15.37% | 11.90% | 18.26% | 15.37% | 13.80% | 9.69% + 14.90% | - |
| UBus_Midi_le15t_80ties | UBus Midi <=15t 80ties | 727108 | 0.0011% | 15.37% | 11.90% | 18.26% | 15.37% | 13.80% | 9.69% + 14.90% | - |
| UBus_Midi_le15t_Euro-I | UBus Midi <=15t Euro-I | 727110 | 0.0107% | 15.83% | 17.99% | 9.31% | 15.83% | 16.85% | 10.11% + 15.32% | - |
| UBus_Midi_le15t_Euro-I_(DPF) | UBus Midi <=15t Euro-I (DPF) | 727111 | 0.0000% | 15.83% | 17.99% | 9.31% | 15.83% | 16.85% | 10.11% + 15.32% | - |
| UBus_Midi_le15t_Euro-II | UBus Midi <=15t Euro-II | 727120 | 0.0160% | 16.59% | 26.51% | 10.66% | 16.59% | 14.60% | 12.51% + 15.89% | - |
| UBus_Midi_le15t_Euro-II_(DPF) | UBus Midi <=15t Euro-II (DPF) | 727121 | 0.0126% | 16.59% | 26.51% | 10.66% | 16.59% | 14.60% | 12.51% + 15.89% | - |
| UBus_Midi_le15t_Euro-III | UBus Midi <=15t Euro-III | 727130 | 0.0387% | 17.15% | 24.86% | 9.43% | 17.15% | 17.39% | 8.08% + 16.09% | - |
| UBus_Midi_le15t_Euro-III_(DPF) | UBus Midi <=15t Euro-III (DPF) | 727131 | 0.1525% | 17.15% | 24.86% | 9.43% | 17.15% | 17.39% | 8.08% + 16.09% | - |
| UBus_Midi_le15t_Euro-IV_EGR | UBus Midi <=15t Euro-IV EGR | 727141 | 0.0115% | 18.23% | 17.77% | 15.02% | 18.23% | 21.40% | 18.52% + 15.91% | - |
| UBus_Midi_le15t_Euro-IV_SCR | UBus Midi <=15t Euro-IV SCR | 727142 | 0.0344% | 18.11% | 34.16% | 12.61% | 18.11% | 16.45% | 21.88% + 15.83% | - |
| UBus_Midi_le15t_Euro-IV_SCRs | UBus Midi <=15t Euro-IV SCR* | 727143 | 0.0000% | 18.11% | 34.16% | 12.61% | 18.11% | 15.01% | 21.88% + 15.83% | - |
| UBus_Midi_le15t_Euro-IV_EGR_(DPF) | UBus Midi <=15t Euro-IV EGR (DPF) | 727144 | 0.0458% | 18.23% | 17.77% | 15.02% | 18.23% | 21.40% | 18.52% + 15.91% | - |
| UBus_Midi_le15t_Euro-IV_SCR_(DPF) | UBus Midi <=15t Euro-IV SCR (DPF) | 727145 | 0.1375% | 18.11% | 34.16% | 12.61% | 18.11% | 16.45% | 21.88% + 15.83% | - |
| UBus_Midi_le15t_Euro-IV_SCRs_(DPF) | UBus Midi <=15t Euro-IV SCR* (DPF) | 727146 | 0.0000% | 18.11% | 34.16% | 12.61% | 18.11% | 15.01% | 21.88% + 15.83% | - |
| UBus_Midi_le15t_Euro-V_EGR | UBus Midi <=15t Euro-V EGR | 727151 | 0.0373% | 18.88% | 44.99% | 8.75% | 18.88% | 18.80% | 13.10% + 15.76% | - |
| UBus_Midi_le15t_Euro-V_SCR | UBus Midi <=15t Euro-V SCR | 727152 | 0.1120% | 18.82% | 31.85% | 14.04% | 18.82% | 23.04% | 22.06% + 15.70% | - |
| UBus_Midi_le15t_Euro-V_SCRs | UBus Midi <=15t Euro-V SCR* | 727153 | 0.0000% | 18.82% | 31.85% | 14.04% | 18.82% | 15.88% | 22.06% + 15.70% | - |
| UBus_Midi_le15t_Euro-V_EGR_(DPF) | UBus Midi <=15t Euro-V EGR (DPF) | 727154 | 0.2740% | 18.88% | 44.99% | 8.75% | 18.88% | 18.80% | 13.10% + 15.76% | - |
| UBus_Midi_le15t_Euro-V_SCR_(DPF) | UBus Midi <=15t Euro-V SCR (DPF) | 727155 | 0.8220% | 18.82% | 31.85% | 14.04% | 18.82% | 23.04% | 22.06% + 15.70% | - |
| UBus_Midi_le15t_Euro-V_SCRs_(DPF) | UBus Midi <=15t Euro-V SCR* (DPF) | 727156 | 0.0000% | 18.82% | 31.85% | 14.04% | 18.82% | 15.88% | 22.06% + 15.70% | - |
| UBus_Midi_le15t_Euro-VI_A-C | UBus Midi <=15t Euro-VI A-C | 727164 | 1.7321% | 19.30% | 22.57% | 15.43% | 19.30% | 36.65% | 38.60% + 15.91% | - |
| UBus_Midi_le15t_Euro-VIs_A-C | UBus Midi <=15t Euro-VI* A-C | 727165 | 0.0000% | 19.30% | 22.57% | 15.43% | 19.30% | 21.86% | 38.60% + 15.91% | - |
| UBus_Midi_le15t_Euro-VI_D-E | UBus Midi <=15t Euro-VI D-E | 727166 | 0.6137% | 19.56% | 77.58% | 7.63% | 19.56% | 19.13% | 17.80% + 15.84% | - |
| UBus_Midi_le15t_Euro-VIs_D-E | UBus Midi <=15t Euro-VI* D-E | 727167 | 0.0000% | 19.56% | 77.58% | 7.63% | 19.56% | 20.55% | 17.80% + 15.84% | - |
| UBus_Std_gt15-18t_50ties | UBus Std >15-18t 50ties | 727205 | 0.0000% | 16.59% | 17.14% | 25.04% | 16.59% | 19.06% | 14.98% + 14.32% | - |
| UBus_Std_gt15-18t_60ties | UBus Std >15-18t 60ties | 727206 | 0.0000% | 16.59% | 17.14% | 25.04% | 16.59% | 19.06% | 14.98% + 14.32% | - |
| UBus_Std_gt15-18t_70ties | UBus Std >15-18t 70ties | 727207 | 0.0000% | 16.59% | 17.14% | 25.04% | 16.59% | 19.06% | 14.98% + 14.32% | - |
| UBus_Std_gt15-18t_80ties | UBus Std >15-18t 80ties | 727208 | 0.0007% | 16.59% | 17.14% | 25.04% | 16.59% | 19.06% | 14.98% + 14.32% | - |
| UBus_Std_gt15-18t_Euro-I | UBus Std >15-18t Euro-I | 727210 | 0.0096% | 16.76% | 12.00% | 11.40% | 16.76% | 21.83% | 10.75% + 14.31% | - |
| UBus_Std_gt15-18t_Euro-I_(DPF) | UBus Std >15-18t Euro-I (DPF) | 727211 | 0.0000% | 16.76% | 12.00% | 11.40% | 16.76% | 21.83% | 10.75% + 14.31% | - |
| UBus_Std_gt15-18t_Euro-II | UBus Std >15-18t Euro-II | 727220 | 0.3251% | 17.08% | 14.53% | 8.66% | 17.08% | 19.05% | 13.38% + 14.37% | - |
| UBus_Std_gt15-18t_Euro-II_(DPF) | UBus Std >15-18t Euro-II (DPF) | 727221 | 0.4839% | 17.08% | 14.53% | 8.66% | 17.08% | 19.05% | 13.38% + 14.37% | - |
| UBus_Std_gt15-18t_Euro-III | UBus Std >15-18t Euro-III | 727230 | 0.9061% | 17.60% | 22.35% | 13.12% | 17.60% | 20.23% | 10.19% + 14.45% | - |
| UBus_Std_gt15-18t_Euro-III_(DPF) | UBus Std >15-18t Euro-III (DPF) | 727231 | 3.5473% | 17.60% | 22.35% | 13.12% | 17.60% | 20.23% | 10.19% + 14.45% | - |
| UBus_Std_gt15-18t_Euro-IV_EGR | UBus Std >15-18t Euro-IV EGR | 727241 | 0.0209% | 18.46% | 20.88% | 22.73% | 18.46% | 17.86% | 23.55% + 14.39% | - |
| UBus_Std_gt15-18t_Euro-IV_SCR | UBus Std >15-18t Euro-IV SCR | 727242 | 0.0626% | 18.57% | 29.94% | 9.05% | 18.57% | 19.35% | 18.76% + 14.70% | - |
| UBus_Std_gt15-18t_Euro-IV_SCRs | UBus Std >15-18t Euro-IV SCR* | 727243 | 0.0000% | 18.57% | 29.94% | 9.05% | 18.57% | 20.08% | 18.76% + 14.70% | - |
| UBus_Std_gt15-18t_Euro-IV_EGR_(DPF) | UBus Std >15-18t Euro-IV EGR (DPF) | 727244 | 0.0835% | 18.46% | 20.88% | 22.73% | 18.46% | 17.86% | 23.55% + 14.39% | - |
| UBus_Std_gt15-18t_Euro-IV_SCR_(DPF) | UBus Std >15-18t Euro-IV SCR (DPF) | 727245 | 0.2506% | 18.57% | 29.94% | 9.05% | 18.57% | 19.35% | 18.76% + 14.70% | - |
| UBus_Std_gt15-18t_Euro-IV_SCRs_(DPF) | UBus Std >15-18t Euro-IV SCR* (DPF) | 727246 | 0.0000% | 18.57% | 29.94% | 9.05% | 18.57% | 20.08% | 18.76% + 14.70% | - |
| UBus_Std_gt15-18t_Euro-V_EGR | UBus Std >15-18t Euro-V EGR | 727251 | 0.1243% | 19.55% | 26.32% | 10.50% | 19.55% | 19.49% | 23.66% + 14.58% | - |
| UBus_Std_gt15-18t_Euro-V_SCR | UBus Std >15-18t Euro-V SCR | 727252 | 0.3728% | 19.59% | 37.08% | 6.88% | 19.59% | 24.02% | 19.90% + 14.85% | - |
| UBus_Std_gt15-18t_Euro-V_SCRs | UBus Std >15-18t Euro-V SCR* | 727253 | 0.0000% | 19.59% | 37.08% | 6.88% | 19.59% | 20.10% | 19.90% + 14.85% | - |
| UBus_Std_gt15-18t_Euro-V_EGR_(DPF) | UBus Std >15-18t Euro-V EGR (DPF) | 727254 | 0.6185% | 19.55% | 26.32% | 10.50% | 19.55% | 19.49% | 23.66% + 14.58% | - |
| UBus_Std_gt15-18t_Euro-V_SCR_(DPF) | UBus Std >15-18t Euro-V SCR (DPF) | 727255 | 1.8556% | 19.59% | 37.08% | 6.88% | 19.59% | 24.02% | 19.90% + 14.85% | - |
| UBus_Std_gt15-18t_Euro-V_SCRs_(DPF) | UBus Std >15-18t Euro-V SCR* (DPF) | 727256 | 0.0000% | 19.59% | 37.08% | 6.88% | 19.59% | 20.10% | 19.90% + 14.85% | - |
| UBus_Std_gt15-18t_Euro-VI_A-C | UBus Std >15-18t Euro-VI A-C | 727264 | 4.2706% | 20.65% | 19.16% | 39.46% | 20.65% | 50.99% | 67.20% + 14.92% | - |
| UBus_Std_gt15-18t_Euro-VIs_A-C | UBus Std >15-18t Euro-VI* A-C | 727265 | 0.0000% | 20.65% | 19.16% | 39.46% | 20.65% | 29.04% | 67.20% + 14.92% | - |
| UBus_Std_gt15-18t_Euro-VI_D-E | UBus Std >15-18t Euro-VI D-E | 727266 | 1.1004% | 20.39% | 103.96% | 27.78% | 20.39% | 67.23% | 14.72% + 14.45% | - |
| UBus_Std_gt15-18t_Euro-VIs_D-E | UBus Std >15-18t Euro-VI* D-E | 727267 | 0.0000% | 20.39% | 103.96% | 27.78% | 20.39% | 26.46% | 14.72% + 14.45% | - |
| UBus_Artic_gt18t_50ties | UBus Artic >18t 50ties | 727305 | 0.0000% | 20.45% | 16.42% | 18.87% | 20.45% | 25.39% | 13.73% + 15.26% | - |
| UBus_Artic_gt18t_60ties | UBus Artic >18t 60ties | 727306 | 0.0000% | 20.45% | 16.42% | 18.87% | 20.45% | 25.39% | 13.73% + 15.26% | - |
| UBus_Artic_gt18t_70ties | UBus Artic >18t 70ties | 727307 | 0.0000% | 20.45% | 16.42% | 18.87% | 20.45% | 25.39% | 13.73% + 15.26% | - |
| UBus_Artic_gt18t_80ties | UBus Artic >18t 80ties | 727308 | 0.0002% | 20.45% | 16.42% | 18.87% | 20.45% | 25.39% | 13.73% + 15.26% | - |
| UBus_Artic_gt18t_Euro-I | UBus Artic >18t Euro-I | 727310 | 0.0033% | 20.68% | 16.46% | 16.55% | 20.68% | 27.13% | 11.58% + 14.82% | - |
| UBus_Artic_gt18t_Euro-I_(DPF) | UBus Artic >18t Euro-I (DPF) | 727311 | 0.0000% | 20.68% | 16.46% | 16.55% | 20.68% | 27.13% | 11.58% + 14.82% | - |
| UBus_Artic_gt18t_Euro-II | UBus Artic >18t Euro-II | 727320 | 0.1277% | 20.75% | 14.43% | 13.46% | 20.75% | 23.76% | 15.67% + 14.64% | - |
| UBus_Artic_gt18t_Euro-II_(DPF) | UBus Artic >18t Euro-II (DPF) | 727321 | 0.1978% | 20.75% | 14.43% | 13.46% | 20.75% | 23.76% | 15.67% + 14.64% | - |
| UBus_Artic_gt18t_Euro-III | UBus Artic >18t Euro-III | 727330 | 0.8338% | 20.74% | 24.85% | 14.06% | 20.74% | 25.51% | 10.59% + 15.43% | - |
| UBus_Artic_gt18t_Euro-III_(DPF) | UBus Artic >18t Euro-III (DPF) | 727331 | 3.3046% | 20.74% | 24.85% | 14.06% | 20.74% | 25.51% | 10.59% + 15.43% | - |
| UBus_Artic_gt18t_Euro-IV_EGR | UBus Artic >18t Euro-IV EGR | 727341 | 0.1916% | 22.16% | 18.81% | 24.13% | 22.16% | 25.90% | 23.69% + 15.25% | - |
| UBus_Artic_gt18t_Euro-IV_SCR | UBus Artic >18t Euro-IV SCR | 727342 | 0.5749% | 21.77% | 36.20% | 10.60% | 21.77% | 23.08% | 30.70% + 15.25% | - |
| UBus_Artic_gt18t_Euro-IV_SCRs | UBus Artic >18t Euro-IV SCR* | 727343 | 0.0000% | 21.77% | 36.20% | 10.60% | 21.77% | 24.53% | 30.70% + 15.25% | - |
| UBus_Artic_gt18t_Euro-IV_EGR_(DPF) | UBus Artic >18t Euro-IV EGR (DPF) | 727344 | 0.7666% | 22.16% | 18.81% | 24.13% | 22.16% | 25.90% | 23.69% + 15.25% | - |
| UBus_Artic_gt18t_Euro-IV_SCR_(DPF) | UBus Artic >18t Euro-IV SCR (DPF) | 727345 | 2.2998% | 21.77% | 36.20% | 10.60% | 21.77% | 23.08% | 30.70% + 15.25% | - |
| UBus_Artic_gt18t_Euro-IV_SCRs_(DPF) | UBus Artic >18t Euro-IV SCR* (DPF) | 727346 | 0.0000% | 21.77% | 36.20% | 10.60% | 21.77% | 24.53% | 30.70% + 15.25% | - |
| UBus_Artic_gt18t_Euro-V_EGR | UBus Artic >18t Euro-V EGR | 727351 | 1.0172% | 22.62% | 32.18% | 11.35% | 22.62% | 19.14% | 22.37% + 15.26% | - |
| UBus_Artic_gt18t_Euro-V_SCR | UBus Artic >18t Euro-V SCR | 727352 | 3.0516% | 22.45% | 37.77% | 7.66% | 22.45% | 25.77% | 23.24% + 15.23% | - |
| UBus_Artic_gt18t_Euro-V_SCRs | UBus Artic >18t Euro-V SCR* | 727353 | 0.0000% | 22.45% | 37.77% | 7.66% | 22.45% | 24.93% | 23.24% + 15.23% | - |
| UBus_Artic_gt18t_Euro-V_EGR_(DPF) | UBus Artic >18t Euro-V EGR (DPF) | 727354 | 5.0614% | 22.62% | 32.18% | 11.35% | 22.62% | 19.14% | 22.37% + 15.26% | - |
| UBus_Artic_gt18t_Euro-V_SCR_(DPF) | UBus Artic >18t Euro-V SCR (DPF) | 727355 | 15.1843% | 22.45% | 37.77% | 7.66% | 22.45% | 25.77% | 23.24% + 15.23% | - |
| UBus_Artic_gt18t_Euro-V_SCRs_(DPF) | UBus Artic >18t Euro-V SCR* (DPF) | 727356 | 0.0000% | 22.45% | 37.77% | 7.66% | 22.45% | 24.93% | 23.24% + 15.23% | - |
| UBus_Artic_gt18t_Euro-VI_A-C | UBus Artic >18t Euro-VI A-C | 727364 | 29.2032% | 24.08% | 24.84% | 32.76% | 24.08% | 49.23% | 72.27% + 15.42% | - |
| UBus_Artic_gt18t_Euro-VIs_A-C | UBus Artic >18t Euro-VI* A-C | 727365 | 0.0000% | 24.08% | 24.84% | 32.76% | 24.08% | 26.39% | 72.27% + 15.42% | - |
| UBus_Artic_gt18t_Euro-VI_D-E | UBus Artic >18t Euro-VI D-E | 727366 | 11.0033% | 36.95% | 82.21% | 17.22% | 36.95% | 70.18% | 16.85% + 15.29% | - |
| UBus_Artic_gt18t_Euro-VIs_D-E | UBus Artic >18t Euro-VI* D-E | 727367 | 0.0000% | 36.95% | 82.21% | 17.22% | 36.95% | 24.47% | 16.85% + 15.29% | - |
| UBus_Std_le18t_EE | UBus Std <=18t EE | 727709 | 0.0000% | 16.59% | 23.77% | 33.83% | 16.59% | 19.06% | 14.98% + 14.32% | - |
| UBus_Artic_gt18t_EE | UBus Artic >18t EE | 727809 | 0.0000% | 20.45% | 16.42% | 18.87% | 20.45% | 25.39% | 13.73% + 15.26% | - |
| UBus_Midi_lt15t_HEV_Euro-IV | UBus Midi <15t HEV Euro-IV | 728140 | 0.0000% | 15.53% | 17.77% | 15.02% | 15.53% | 21.40% | 18.52% + 15.96% | - |
| UBus_Midi_lt15t_HEV_Euro-V | UBus Midi <15t HEV Euro-V | 728150 | 0.0000% | 15.53% | 36.93% | 14.11% | 15.53% | 23.64% | 26.87% + 15.96% | - |
| UBus_Midi_lt15t_HEV_Euro-VI_A-C | UBus Midi <15t HEV Euro-VI A-C | 728164 | 0.0000% | 16.51% | 29.18% | 15.28% | 16.51% | 60.61% | 45.64% + 15.95% | - |
| UBus_Midi_lt15t_HEV_Euro-VI_D-E | UBus Midi <15t HEV Euro-VI D-E | 728166 | 0.0799% | 16.51% | 29.18% | 15.28% | 16.51% | 60.61% | 45.64% + 15.95% | - |
| UBus_Std_gt15-18t_HEV_Euro-IV | UBus Std >15-18t HEV Euro-IV | 728240 | 0.0000% | 23.43% | 17.77% | 15.02% | 23.43% | 21.40% | 18.52% + 16.63% | - |
| UBus_Std_gt15-18t_HEV_Euro-V | UBus Std >15-18t HEV Euro-V | 728250 | 0.0166% | 23.43% | 36.34% | 8.72% | 23.43% | 24.97% | 19.04% + 16.63% | - |
| UBus_Std_gt15-18t_HEV_Euro-VI_A-C | UBus Std >15-18t HEV Euro-VI A-C | 728264 | 0.0370% | 24.24% | 17.55% | 32.61% | 24.24% | 98.29% | 62.21% + 16.58% | - |
| UBus_Std_gt15-18t_HEV_Euro-VI_D-E | UBus Std >15-18t HEV Euro-VI D-E | 728266 | 0.1537% | 24.24% | 17.55% | 32.61% | 24.24% | 98.29% | 62.21% + 16.58% | - |
| UBus_Artic_gt18t_HEV_Euro-IV | UBus Artic >18t HEV Euro-IV | 728340 | 0.0000% | 29.32% | 18.81% | 24.13% | 29.32% | 25.90% | 23.69% + 17.82% | - |
| UBus_Artic_gt18t_HEV_Euro-V | UBus Artic >18t HEV Euro-V | 728350 | 0.2581% | 29.32% | 34.56% | 9.27% | 29.32% | 29.10% | 15.29% + 17.82% | - |
| UBus_Artic_gt18t_HEV_Euro-VI_A-C | UBus Artic >18t HEV Euro-VI A-C | 728364 | 1.1563% | 28.83% | 21.51% | 27.91% | 28.83% | 76.19% | 66.81% + 17.69% | - |
| UBus_Artic_gt18t_HEV_Euro-VI_D-E | UBus Artic >18t HEV Euro-VI D-E | 728366 | 3.7297% | 28.83% | 21.51% | 27.91% | 28.83% | 76.19% | 66.81% + 17.69% | - |
| UBus_Midi_lt15t_CNG_Euro-II | UBus Midi <15t CNG Euro-II | 737120 | 0.0000% | 16.59% | 26.51% | 10.66% | 16.59% | 14.60% | 12.51% + 15.89% | - |
| UBus_Midi_lt15t_CNG_Euro-III | UBus Midi <15t CNG Euro-III | 737130 | 0.0000% | 17.15% | 24.86% | 9.43% | 17.15% | 17.39% | 8.08% + 16.09% | - |
| UBus_Midi_lt15t_CNG_Euro-IV | UBus Midi <15t CNG Euro-IV | 737140 | 0.0000% | 18.23% | 17.77% | 15.02% | 18.23% | 21.40% | 18.52% + 15.91% | - |
| UBus_Midi_lt15t_CNG_Euro-V | UBus Midi <15t CNG Euro-V | 737150 | 0.0136% | 15.76% | 15.76% | 15.76% | 15.76% | 15.76% | 15.76% + 15.20% | - |
| UBus_Midi_lt15t_CNG_Euro-VI | UBus Midi <15t CNG Euro-VI | 737160 | 0.0039% | 15.94% | 15.94% | 15.94% | 15.94% | 15.94% | 15.94% + 16.06% | - |
| UBus_Std_gt15-18t_CNG_Euro-II | UBus Std >15-18t CNG Euro-II | 737220 | 0.0000% | 17.08% | 14.53% | 8.66% | 17.08% | 19.05% | 13.38% + 14.37% | - |
| UBus_Std_gt15-18t_CNG_Euro-III | UBus Std >15-18t CNG Euro-III | 737230 | 0.0000% | 17.60% | 22.35% | 13.12% | 17.60% | 20.23% | 10.19% + 14.45% | - |
| UBus_Std_gt15-18t_CNG_Euro-IV | UBus Std >15-18t CNG Euro-IV | 737240 | 0.0318% | 18.46% | 20.88% | 22.73% | 18.46% | 17.86% | 23.55% + 14.39% | - |
| UBus_Std_gt15-18t_CNG_Euro-V | UBus Std >15-18t CNG Euro-V | 737250 | 0.1168% | 19.53% | 19.53% | 19.53% | 19.53% | 19.53% | 19.53% + 14.88% | - |
| UBus_Std_gt15-18t_CNG_Euro-VI | UBus Std >15-18t CNG Euro-VI | 737260 | 0.0132% | 20.68% | 20.68% | 20.68% | 20.68% | 20.68% | 20.68% + 14.94% | - |
| UBus_Artic_gt18t_CNG_Euro-II | UBus Artic >18t CNG Euro-II | 737320 | 0.0000% | 20.75% | 14.43% | 13.46% | 20.75% | 23.76% | 15.67% + 14.64% | - |
| UBus_Artic_gt18t_CNG_Euro-III | UBus Artic >18t CNG Euro-III | 737330 | 0.0000% | 20.74% | 24.85% | 14.06% | 20.74% | 25.51% | 10.59% + 15.43% | - |
| UBus_Artic_gt18t_CNG_Euro-IV | UBus Artic >18t CNG Euro-IV | 737340 | 0.0436% | 22.16% | 18.81% | 24.13% | 22.16% | 25.90% | 23.69% + 15.25% | - |
| UBus_Artic_gt18t_CNG_Euro-V | UBus Artic >18t CNG Euro-V | 737350 | 0.8462% | 22.43% | 22.43% | 22.43% | 22.43% | 22.43% | 22.43% + 15.25% | - |
| UBus_Artic_gt18t_CNG_Euro-VI | UBus Artic >18t CNG Euro-VI | 737360 | 0.6258% | 24.11% | 24.11% | 24.11% | 24.11% | 24.11% | 24.11% + 15.44% | - |
| UBus_Midi_lt15t_Ethanol_Euro-II | UBus Midi <15t Ethanol Euro-II | 787120 | 0.0000% | 16.59% | 26.51% | 10.66% | 16.59% | 14.60% | 12.51% + 15.89% | - |
| UBus_Midi_lt15t_Ethanol_Euro-III | UBus Midi <15t Ethanol Euro-III | 787130 | 0.0000% | 17.15% | 24.86% | 9.43% | 17.15% | 17.39% | 8.08% + 16.09% | - |
| UBus_Midi_lt15t_Ethanol_Euro-IV | UBus Midi <15t Ethanol Euro-IV | 787140 | 0.0000% | 18.23% | 17.77% | 15.02% | 18.23% | 21.40% | 18.52% + 15.91% | - |
| UBus_Midi_lt15t_Ethanol_Euro-V | UBus Midi <15t Ethanol Euro-V | 787150 | 0.0000% | 18.88% | 44.99% | 8.75% | 18.88% | 18.80% | 13.10% + 15.76% | - |
| UBus_Midi_lt15t_Ethanol_Euro-VI | UBus Midi <15t Ethanol Euro-VI | 787160 | 0.0000% | 19.30% | 22.57% | 15.43% | 19.30% | 36.65% | 38.60% + 15.91% | - |
| UBus_Std_gt15-18t_Ethanol_Euro-II | UBus Std >15-18t Ethanol Euro-II | 787220 | 0.0000% | 17.08% | 14.53% | 8.66% | 17.08% | 19.05% | 13.38% + 14.37% | - |
| UBus_Std_gt15-18t_Ethanol_Euro-III | UBus Std >15-18t Ethanol Euro-III | 787230 | 0.0000% | 17.60% | 22.35% | 13.12% | 17.60% | 20.23% | 10.19% + 14.45% | - |
| UBus_Std_gt15-18t_Ethanol_Euro-IV | UBus Std >15-18t Ethanol Euro-IV | 787240 | 0.0000% | 18.46% | 20.88% | 22.73% | 18.46% | 17.86% | 23.55% + 14.39% | - |
| UBus_Std_gt15-18t_Ethanol_Euro-V | UBus Std >15-18t Ethanol Euro-V | 787250 | 0.0000% | 19.55% | 26.32% | 10.50% | 19.55% | 19.49% | 23.66% + 14.58% | - |
| UBus_Std_gt15-18t_Ethanol_Euro-VI | UBus Std >15-18t Ethanol Euro-VI | 787260 | 0.0000% | 20.65% | 19.16% | 39.46% | 20.65% | 50.99% | 67.20% + 14.92% | - |
| UBus_Artic_gt18t_Ethanol_Euro-II | UBus Artic >18t Ethanol Euro-II | 787320 | 0.0000% | 20.75% | 14.43% | 13.46% | 20.75% | 23.76% | 15.67% + 14.64% | - |
| UBus_Artic_gt18t_Ethanol_Euro-III | UBus Artic >18t Ethanol Euro-III | 787330 | 0.0000% | 20.74% | 24.85% | 14.06% | 20.74% | 25.51% | 10.59% + 15.43% | - |
| UBus_Artic_gt18t_Ethanol_Euro-IV | UBus Artic >18t Ethanol Euro-IV | 787340 | 0.0000% | 22.16% | 18.81% | 24.13% | 22.16% | 25.90% | 23.69% + 15.25% | - |
| UBus_Artic_gt18t_Ethanol_Euro-V | UBus Artic >18t Ethanol Euro-V | 787350 | 0.0000% | 22.62% | 32.18% | 11.35% | 22.62% | 19.14% | 22.37% + 15.26% | - |
| UBus_Artic_gt18t_Ethanol_Euro-VI | UBus Artic >18t Ethanol Euro-VI | 787360 | 0.0000% | 24.08% | 24.84% | 32.76% | 24.08% | 49.23% | 72.27% + 15.42% | - |
| UBus_FuelCell_Midi_lt15t | UBus FuelCell Midi <15t | 788301 | 0.0000% | 0.00% | 0.00% | 0.00% | 5.56% | 0.00% | 0.00% + 16.71% | - |
| UBus_FuelCell_Std_gt15-18t | UBus FuelCell Std >15-18t | 788302 | 0.0000% | 0.00% | 0.00% | 0.00% | 9.03% | 0.00% | 0.00% + 16.50% | - |
| UBus_FuelCell_Artic_gt18t | UBus FuelCell Artic >18t | 788303 | 0.0000% | 0.00% | 0.00% | 0.00% | 12.36% | 0.00% | 0.00% + 15.22% | - |
| UBus_Electric_Midi_lt15t | UBus Electric Midi <15t | 788401 | 0.0903% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 16.71% | 5.56% |
| UBus_Electric_Std_gt15-18t | UBus Electric Std >15-18t | 788402 | 0.1274% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 16.50% | 9.03% |
| UBus_Electric_Artic_gt18t | UBus Electric Artic >18t | 788403 | 1.7500% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 15.22% | 12.36% |
| UBus_Midi_lt15t_LNG_Euro-IV | UBus Midi <15t LNG Euro-IV | 796140 | 0.0000% | 18.23% | 17.77% | 15.02% | 18.23% | 21.40% | 18.52% + 15.91% | - |
| UBus_Midi_lt15t_LNG_Euro-V | UBus Midi <15t LNG Euro-V | 796150 | 0.0000% | 15.76% | 15.76% | 15.76% | 15.76% | 15.76% | 15.76% + 15.20% | - |
| UBus_Midi_lt15t_LNG_Euro-VI | UBus Midi <15t LNG Euro-VI | 796160 | 0.0000% | 15.94% | 15.94% | 15.94% | 15.94% | 15.94% | 15.94% + 16.06% | - |
| UBus_Std_gt15-18t_LNG_Euro-IV | UBus Std >15-18t LNG Euro-IV | 796240 | 0.0000% | 18.46% | 20.88% | 22.73% | 18.46% | 17.86% | 23.55% + 14.39% | - |
| UBus_Std_gt15-18t_LNG_Euro-V | UBus Std >15-18t LNG Euro-V | 796250 | 0.0000% | 19.53% | 19.53% | 19.53% | 19.53% | 19.53% | 19.53% + 14.88% | - |
| UBus_Std_gt15-18t_LNG_Euro-VI | UBus Std >15-18t LNG Euro-VI | 796260 | 0.0000% | 20.68% | 20.68% | 20.68% | 20.68% | 20.68% | 20.68% + 14.94% | - |
| UBus_Artic_gt18t_LNG_Euro-IV | UBus Artic >18t LNG Euro-IV | 796340 | 0.0000% | 22.16% | 18.81% | 24.13% | 22.16% | 25.90% | 23.69% + 15.25% | - |
| UBus_Artic_gt18t_LNG_Euro-V | UBus Artic >18t LNG Euro-V | 796350 | 0.0000% | 22.43% | 22.43% | 22.43% | 22.43% | 22.43% | 22.43% + 15.25% | - |
| UBus_Artic_gt18t_LNG_Euro-VI | UBus Artic >18t LNG Euro-VI | 796360 | 0.0000% | 24.11% | 24.11% | 24.11% | 24.11% | 24.11% | 24.11% + 15.44% | - |

### Motor Cycles
| SUMO emission class | HBEFA subsegment | HBEFA subsegment ID | fleet share 2022 | error CO2 | error CO | error HC | error FC | error NOx | error PM | error FC_MJ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| MC_4S_le250cc_preEuro | MC 4S <=250cc preEuro | 919200 | 4.9814% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 12.83% + 6.21% | - |
| MC_4S_le250cc_Euro-1 | MC 4S <=250cc Euro-1 | 919210 | 2.2953% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 12.83% + 6.21% | - |
| MC_4S_le250cc_Euro-2 | MC 4S <=250cc Euro-2 | 919220 | 1.4972% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 12.83% + 6.21% | - |
| MC_4S_le250cc_Euro-3 | MC 4S <=250cc Euro-3 | 919230 | 4.2638% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 12.83% + 6.21% | - |
| MC_4S_le250cc_Euro-4 | MC 4S <=250cc Euro-4 | 919240 | 2.1464% | 2.42% | 16.72% | 16.83% | 2.42% | 28.22% | 17.67% + 6.23% | - |
| MC_4S_le250cc_Euro-5 | MC 4S <=250cc Euro-5 | 919250 | 0.6659% | 2.42% | 16.72% | 16.83% | 2.42% | 28.22% | 17.67% + 6.23% | - |
| MC_4S_le250cc_Euro-6 | MC 4S <=250cc Euro-6 | 919260 | 0.0000% | 2.42% | 16.72% | 16.83% | 2.42% | 28.22% | 17.67% + 6.23% | - |
| MC_4S_gt250cc_preEuro | MC 4S >250cc preEuro | 919600 | 15.4257% | 1.29% | 20.71% | 41.28% | 1.29% | 8.13% | 20.61% + 6.23% | - |
| MC_4S_gt250cc_Euro-1 | MC 4S >250cc Euro-1 | 919610 | 7.9680% | 1.29% | 20.71% | 41.28% | 1.29% | 8.13% | 20.61% + 6.23% | - |
| MC_4S_gt250cc_Euro-2 | MC 4S >250cc Euro-2 | 919620 | 4.0568% | 1.29% | 20.71% | 41.28% | 1.29% | 8.13% | 20.61% + 6.23% | - |
| MC_4S_gt250cc_Euro-3 | MC 4S >250cc Euro-3 | 919630 | 13.6459% | 1.29% | 20.71% | 41.28% | 1.29% | 8.13% | 20.61% + 6.23% | - |
| MC_4S_gt250cc_Euro-4 | MC 4S >250cc Euro-4 | 919640 | 6.4921% | 1.19% | 58.46% | 10.82% | 1.19% | 18.49% | 33.23% + 6.23% | - |
| MC_4S_gt250cc_Euro-5 | MC 4S >250cc Euro-5 | 919650 | 2.4263% | 1.19% | 58.46% | 10.82% | 1.19% | 18.49% | 33.23% + 6.23% | - |
| MC_4S_gt250cc_Euro-6 | MC 4S >250cc Euro-6 | 919660 | 0.0000% | 1.19% | 58.46% | 10.82% | 1.19% | 18.49% | 33.23% + 6.23% | - |
| Moped_le50cc_(vlt30)_without_cat | Moped <=50cc (v<30) without cat | 939000 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_(vlt30)_with_cat___FAV4 | Moped <=50cc (v<30) with cat / FAV4 | 939001 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_(vlt30)_EU2 | Moped <=50cc (v<30) EU2 | 939002 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_(vlt30)_EU3 | Moped <=50cc (v<30) EU3 | 939003 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_(vlt30)_EU4 | Moped <=50cc (v<30) EU4 | 939004 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_(vlt30)_EU5 | Moped <=50cc (v<30) EU5 | 939005 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_preEuro | Moped <=50cc preEuro | 939100 | 9.2119% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_Euro-1 | Moped <=50cc Euro-1 | 939110 | 6.3885% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_Euro-2 | Moped <=50cc Euro-2 | 939120 | 11.6556% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_Euro-3 | Moped <=50cc Euro-3 | 939130 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_Euro-4 | Moped <=50cc Euro-4 | 939140 | 2.0340% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| Moped_le50cc_Euro-5 | Moped <=50cc Euro-5 | 939150 | 0.9979% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | - |
| MC_2S_le250cc_preEuro | MC 2S <=250cc preEuro | 939600 | 2.4022% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 0.00% + 6.21% | - |
| MC_2S_le250cc_Euro-1 | MC 2S <=250cc Euro-1 | 939610 | 0.3401% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 0.00% + 6.21% | - |
| MC_2S_le250cc_Euro-2 | MC 2S <=250cc Euro-2 | 939620 | 0.2602% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 0.00% + 6.21% | - |
| MC_2S_le250cc_Euro-3 | MC 2S <=250cc Euro-3 | 939630 | 0.1556% | 2.97% | 13.73% | 12.06% | 2.97% | 16.67% | 0.00% + 6.21% | - |
| MC_2S_le250cc_Euro-4 | MC 2S <=250cc Euro-4 | 939640 | 0.0004% | 2.42% | 16.72% | 16.83% | 2.42% | 28.22% | 0.00% + 6.23% | - |
| MC_2S_le250cc_Euro-5 | MC 2S <=250cc Euro-5 | 939650 | 0.0000% | 2.42% | 16.72% | 16.83% | 2.42% | 28.22% | 0.00% + 6.23% | - |
| MC_2S_le250cc_Euro-6 | MC 2S <=250cc Euro-6 | 939660 | 0.0000% | 2.42% | 16.72% | 16.83% | 2.42% | 28.22% | 0.00% + 6.23% | - |
| eBike | eBike | 951201 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 29.71% | 0.00% |
| eScooter | eScooter | 951401 | 0.4225% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 9.05% | 2.93% |
| MC_BEV | MC BEV | 951501 | 0.2665% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 6.23% | 1.40% |

### Heavy Goods Vehicles
| SUMO emission class | HBEFA subsegment | HBEFA subsegment ID | fleet share 2022 | error CO2 | error CO | error HC | error FC | error NOx | error PM | error FC_MJ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| RT_petrol | RT petrol | 1413000 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 0.00% + 12.29% | - |
| RT_le7.5t_50ties | RT <=7.5t 50ties | 1423105 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| RT_le7.5t_60ties | RT <=7.5t 60ties | 1423106 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| RT_le7.5t_70ties | RT <=7.5t 70ties | 1423107 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| RT_le7.5t_80ties | RT <=7.5t 80ties | 1423108 | 1.8326% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| RT_le7_5t_EE | RT <=7,5t EE | 1423109 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| RT_le7.5t_Euro-I | RT <=7.5t Euro-I | 1423110 | 0.8918% | 6.09% | 15.91% | 11.06% | 6.09% | 5.74% | 11.33% + 12.07% | - |
| RT_le7.5t_Euro-II | RT <=7.5t Euro-II | 1423120 | 1.9741% | 6.25% | 14.81% | 10.50% | 6.25% | 5.62% | 7.69% + 11.92% | - |
| RT_le7.5t_Euro-III | RT <=7.5t Euro-III | 1423130 | 2.8229% | 6.76% | 17.84% | 11.41% | 6.76% | 5.83% | 8.04% + 12.06% | - |
| RT_le7.5t_Euro-IV_EGR | RT <=7.5t Euro-IV EGR | 1423141 | 0.4427% | 5.95% | 9.05% | 15.18% | 5.95% | 8.10% | 17.28% + 12.13% | - |
| RT_le7.5t_Euro-IV_SCR | RT <=7.5t Euro-IV SCR | 1423142 | 1.3281% | 5.73% | 27.28% | 7.50% | 5.73% | 16.99% | 22.54% + 12.13% | - |
| RT_le7.5t_Euro-IV_SCRs | RT <=7.5t Euro-IV SCR* | 1423143 | 0.0000% | 5.73% | 27.28% | 7.50% | 5.73% | 4.05% | 22.54% + 12.13% | - |
| RT_le7.5t_Euro-V_EGR | RT <=7.5t Euro-V EGR | 1423151 | 1.5690% | 6.16% | 25.79% | 10.57% | 6.16% | 17.74% | 11.17% + 12.07% | - |
| RT_le7.5t_Euro-V_SCR | RT <=7.5t Euro-V SCR | 1423152 | 4.7071% | 6.16% | 28.39% | 6.08% | 6.16% | 25.73% | 23.64% + 12.19% | - |
| RT_le7.5t_Euro-V_SCRs | RT <=7.5t Euro-V SCR* | 1423153 | 0.0000% | 6.16% | 28.39% | 6.08% | 6.16% | 5.02% | 23.64% + 12.19% | - |
| RT_le7.5t_Euro-VI_A-C | RT <=7.5t Euro-VI A-C | 1423164 | 5.0816% | 6.17% | 13.08% | 8.86% | 6.17% | 37.86% | 24.34% + 12.09% | - |
| RT_le7.5t_Euro-VIs_A-C | RT <=7.5t Euro-VI* A-C | 1423165 | 0.0000% | 6.17% | 13.08% | 8.86% | 6.17% | 16.43% | 24.34% + 12.09% | - |
| RT_le7.5t_Euro-VI_D-E | RT <=7.5t Euro-VI D-E | 1423166 | 2.8776% | 7.27% | 75.44% | 4.73% | 7.27% | 16.53% | 8.71% + 12.08% | - |
| RT_le7.5t_Euro-VIs_D-E | RT <=7.5t Euro-VI* D-E | 1423167 | 0.0000% | 7.27% | 75.44% | 4.73% | 7.27% | 12.47% | 8.71% + 12.08% | - |
| RT_gt7_5-12t_50ties | RT >7,5-12t 50ties | 1423205 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| RT_gt7_5-12t_60ties | RT >7,5-12t 60ties | 1423206 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| RT_gt7_5-12t_70ties | RT >7,5-12t 70ties | 1423207 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| RT_gt7_5-12t_80ties | RT >7,5-12t 80ties | 1423208 | 0.1692% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| RT_gt7_5-12t_EE | RT >7,5-12t EE | 1423209 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| RT_gt7_5-12t_Euro-I | RT >7,5-12t Euro-I | 1423210 | 0.0732% | 14.05% | 13.26% | 11.92% | 14.05% | 15.62% | 9.77% + 12.78% | - |
| RT_gt7_5-12t_Euro-II | RT >7,5-12t Euro-II | 1423220 | 0.1488% | 14.28% | 26.36% | 12.31% | 14.28% | 13.16% | 13.25% + 12.52% | - |
| RT_gt7_5-12t_Euro-III | RT >7,5-12t Euro-III | 1423230 | 0.3581% | 14.87% | 23.58% | 15.56% | 14.87% | 17.16% | 7.26% + 12.54% | - |
| RT_gt7_5-12t_Euro-IV_EGR | RT >7,5-12t Euro-IV EGR | 1423241 | 0.0757% | 15.59% | 16.09% | 16.63% | 15.59% | 20.04% | 16.41% + 12.53% | - |
| RT_gt7_5-12t_Euro-IV_SCR | RT >7,5-12t Euro-IV SCR | 1423242 | 0.2270% | 15.49% | 25.86% | 7.89% | 15.49% | 14.59% | 18.35% + 12.56% | - |
| RT_gt7_5-12t_Euro-IV_SCRs | RT >7,5-12t Euro-IV SCR* | 1423243 | 0.0000% | 15.49% | 25.86% | 7.89% | 15.49% | 12.93% | 18.35% + 12.56% | - |
| RT_gt7_5-12t_Euro-V_EGR | RT >7,5-12t Euro-V EGR | 1423251 | 0.4069% | 16.25% | 29.83% | 10.17% | 16.25% | 13.73% | 20.16% + 12.54% | - |
| RT_gt7_5-12t_Euro-V_SCR | RT >7,5-12t Euro-V SCR | 1423252 | 1.2207% | 16.20% | 25.23% | 10.70% | 16.20% | 23.27% | 18.67% + 12.56% | - |
| RT_gt7_5-12t_Euro-V_SCRs | RT >7,5-12t Euro-V SCR* | 1423253 | 0.0000% | 16.20% | 25.23% | 10.70% | 16.20% | 13.29% | 18.67% + 12.56% | - |
| RT_gt7_5-12t_Euro-VI_A-C | RT >7,5-12t Euro-VI A-C | 1423264 | 1.5883% | 16.45% | 11.46% | 25.47% | 16.45% | 43.26% | 64.88% + 12.34% | - |
| RT_gt7_5-12t_Euro-VIs_A-C | RT >7,5-12t Euro-VI* A-C | 1423265 | 0.0000% | 16.45% | 11.46% | 25.47% | 16.45% | 16.55% | 64.88% + 12.34% | - |
| RT_gt7_5-12t_Euro-VI_D-E | RT >7,5-12t Euro-VI D-E | 1423266 | 0.8723% | 16.94% | 100.53% | 7.91% | 16.94% | 46.30% | 11.96% + 12.32% | - |
| RT_gt7_5-12t_Euro-VIs_D-E | RT >7,5-12t Euro-VI* D-E | 1423267 | 0.0000% | 16.94% | 100.53% | 7.91% | 16.94% | 15.93% | 11.96% + 12.32% | - |
| RT_gt12-14t_50ties | RT >12-14t 50ties | 1423305 | 0.0000% | 17.14% | 17.99% | 20.60% | 17.14% | 15.30% | 15.53% + 13.69% | - |
| RT_gt12-14t_60ties | RT >12-14t 60ties | 1423306 | 0.0000% | 17.14% | 17.99% | 20.60% | 17.14% | 15.30% | 15.53% + 13.69% | - |
| RT_gt12-14t_70ties | RT >12-14t 70ties | 1423307 | 0.0000% | 17.14% | 17.99% | 20.60% | 17.14% | 15.30% | 15.53% + 13.69% | - |
| RT_gt12-14t_80ties | RT >12-14t 80ties | 1423308 | 0.0348% | 17.14% | 17.99% | 20.60% | 17.14% | 15.30% | 15.53% + 13.69% | - |
| RT_gt12-14t_Euro-I | RT >12-14t Euro-I | 1423310 | 0.0154% | 17.63% | 21.94% | 12.00% | 17.63% | 17.44% | 14.11% + 13.44% | - |
| RT_gt12-14t_Euro-II | RT >12-14t Euro-II | 1423320 | 0.0192% | 18.05% | 31.47% | 11.59% | 18.05% | 15.41% | 16.73% + 12.93% | - |
| RT_gt12-14t_Euro-III | RT >12-14t Euro-III | 1423330 | 0.0261% | 18.46% | 24.36% | 10.50% | 18.46% | 17.61% | 9.98% + 12.82% | - |
| RT_gt12-14t_Euro-IV_EGR | RT >12-14t Euro-IV EGR | 1423341 | 0.0042% | 19.32% | 17.41% | 21.51% | 19.32% | 17.70% | 22.58% + 12.71% | - |
| RT_gt12-14t_Euro-IV_SCR | RT >12-14t Euro-IV SCR | 1423342 | 0.0125% | 19.24% | 29.65% | 16.51% | 19.24% | 14.38% | 22.41% + 12.70% | - |
| RT_gt12-14t_Euro-IV_SCRs | RT >12-14t Euro-IV SCR* | 1423343 | 0.0000% | 19.24% | 29.65% | 16.51% | 19.24% | 15.42% | 22.41% + 12.70% | - |
| RT_gt12-14t_Euro-V_EGR | RT >12-14t Euro-V EGR | 1423351 | 0.0223% | 19.22% | 31.90% | 8.52% | 19.22% | 21.91% | 17.53% + 12.51% | - |
| RT_gt12-14t_Euro-V_SCR | RT >12-14t Euro-V SCR | 1423352 | 0.0670% | 19.26% | 31.17% | 17.82% | 19.26% | 21.90% | 23.23% + 12.51% | - |
| RT_gt12-14t_Euro-V_SCRs | RT >12-14t Euro-V SCR* | 1423353 | 0.0000% | 19.26% | 31.17% | 17.82% | 19.26% | 15.51% | 23.23% + 12.51% | - |
| RT_gt12-14t_Euro-VI_A-C | RT >12-14t Euro-VI A-C | 1423364 | 0.2169% | 19.55% | 15.31% | 20.53% | 19.55% | 36.56% | 21.72% + 12.54% | - |
| RT_gt12-14t_Euro-VIs_A-C | RT >12-14t Euro-VI* A-C | 1423365 | 0.0000% | 19.55% | 15.31% | 20.53% | 19.55% | 22.91% | 21.72% + 12.54% | - |
| RT_gt12-14t_Euro-VI_D-E | RT >12-14t Euro-VI D-E | 1423366 | 0.1105% | 19.97% | 67.51% | 5.21% | 19.97% | 32.60% | 17.01% + 12.54% | - |
| RT_gt12-14t_Euro-VIs_D-E | RT >12-14t Euro-VI* D-E | 1423367 | 0.0000% | 19.97% | 67.51% | 5.21% | 19.97% | 23.92% | 17.01% + 12.54% | - |
| RT_gt14-20t_50ties | RT >14-20t 50ties | 1423405 | 0.0000% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| RT_gt14-20t_60ties | RT >14-20t 60ties | 1423406 | 0.0000% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| RT_gt14-20t_70ties | RT >14-20t 70ties | 1423407 | 0.0000% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| RT_gt14-20t_80ties | RT >14-20t 80ties | 1423408 | 0.0817% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| RT_gt14-20t_Euro-I | RT >14-20t Euro-I | 1423410 | 0.0548% | 19.35% | 18.99% | 8.93% | 19.35% | 19.58% | 13.34% + 14.26% | - |
| RT_gt14-20t_Euro-II | RT >14-20t Euro-II | 1423420 | 0.1543% | 20.81% | 27.36% | 10.70% | 20.81% | 18.36% | 17.35% + 13.79% | - |
| RT_gt14-20t_Euro-III | RT >14-20t Euro-III | 1423430 | 0.3121% | 21.43% | 22.18% | 9.44% | 21.43% | 17.58% | 12.94% + 13.06% | - |
| RT_gt14-20t_Euro-IV_EGR | RT >14-20t Euro-IV EGR | 1423441 | 0.0477% | 21.80% | 14.45% | 16.77% | 21.80% | 16.01% | 26.19% + 12.98% | - |
| RT_gt14-20t_Euro-IV_SCR | RT >14-20t Euro-IV SCR | 1423442 | 0.1431% | 21.84% | 29.90% | 14.36% | 21.84% | 15.54% | 23.24% + 13.20% | - |
| RT_gt14-20t_Euro-IV_SCRs | RT >14-20t Euro-IV SCR* | 1423443 | 0.0000% | 21.84% | 29.90% | 14.36% | 21.84% | 18.56% | 23.24% + 13.20% | - |
| RT_gt14-20t_Euro-V_EGR | RT >14-20t Euro-V EGR | 1423451 | 0.2629% | 21.42% | 33.94% | 9.15% | 21.42% | 16.86% | 18.32% + 12.61% | - |
| RT_gt14-20t_Euro-V_SCR | RT >14-20t Euro-V SCR | 1423452 | 0.7888% | 21.40% | 27.31% | 17.45% | 21.40% | 23.75% | 21.19% + 12.59% | - |
| RT_gt14-20t_Euro-V_SCRs | RT >14-20t Euro-V SCR* | 1423453 | 0.0000% | 21.40% | 27.31% | 17.45% | 21.40% | 17.80% | 21.19% + 12.59% | - |
| RT_gt14-20t_Euro-VI_A-C | RT >14-20t Euro-VI A-C | 1423464 | 1.3424% | 20.51% | 21.14% | 9.26% | 20.51% | 49.98% | 29.41% + 12.32% | - |
| RT_gt14-20t_Euro-VIs_A-C | RT >14-20t Euro-VI* A-C | 1423465 | 0.0000% | 20.51% | 21.14% | 9.26% | 20.51% | 26.40% | 29.41% + 12.32% | - |
| RT_gt14-20t_Euro-VI_D-E | RT >14-20t Euro-VI D-E | 1423466 | 0.8307% | 21.31% | 95.85% | 6.71% | 21.31% | 32.27% | 17.39% + 12.21% | - |
| RT_gt14-20t_Euro-VIs_D-E | RT >14-20t Euro-VI* D-E | 1423467 | 0.0000% | 21.31% | 95.85% | 6.71% | 21.31% | 20.79% | 17.39% + 12.21% | - |
| RT_gt20-26t_50ties | RT >20-26t 50ties | 1423505 | 0.0000% | 23.14% | 14.96% | 10.12% | 23.14% | 24.16% | 12.63% + 12.78% | - |
| RT_gt20-26t_60ties | RT >20-26t 60ties | 1423506 | 0.0000% | 23.14% | 14.96% | 10.12% | 23.14% | 24.16% | 12.63% + 12.78% | - |
| RT_gt20-26t_70ties | RT >20-26t 70ties | 1423507 | 0.0000% | 23.14% | 14.96% | 10.12% | 23.14% | 24.16% | 12.63% + 12.78% | - |
| RT_gt20-26t_80ties | RT >20-26t 80ties | 1423508 | 0.0163% | 23.14% | 14.96% | 10.12% | 23.14% | 24.16% | 12.63% + 12.78% | - |
| RT_gt20-26t_Euro-I | RT >20-26t Euro-I | 1423510 | 0.0133% | 23.45% | 24.11% | 8.86% | 23.45% | 22.85% | 13.53% + 12.82% | - |
| RT_gt20-26t_Euro-II | RT >20-26t Euro-II | 1423520 | 0.0629% | 23.48% | 23.93% | 9.59% | 23.48% | 20.21% | 16.36% + 12.54% | - |
| RT_gt20-26t_Euro-III | RT >20-26t Euro-III | 1423530 | 0.1875% | 23.71% | 21.95% | 9.03% | 23.71% | 19.54% | 13.29% + 12.56% | - |
| RT_gt20-26t_Euro-IV_EGR | RT >20-26t Euro-IV EGR | 1423541 | 0.0273% | 24.44% | 16.68% | 23.77% | 24.44% | 18.55% | 28.45% + 12.54% | - |
| RT_gt20-26t_Euro-IV_SCR | RT >20-26t Euro-IV SCR | 1423542 | 0.0818% | 24.39% | 37.32% | 18.11% | 24.39% | 20.16% | 29.80% + 12.59% | - |
| RT_gt20-26t_Euro-IV_SCRs | RT >20-26t Euro-IV SCR* | 1423543 | 0.0000% | 24.39% | 37.32% | 18.11% | 24.39% | 20.23% | 29.80% + 12.59% | - |
| RT_gt20-26t_Euro-V_EGR | RT >20-26t Euro-V EGR | 1423551 | 0.2420% | 24.77% | 26.25% | 10.49% | 24.77% | 18.48% | 18.22% + 12.53% | - |
| RT_gt20-26t_Euro-V_SCR | RT >20-26t Euro-V SCR | 1423552 | 0.7260% | 24.64% | 35.16% | 20.26% | 24.64% | 23.72% | 27.46% + 12.45% | - |
| RT_gt20-26t_Euro-V_SCRs | RT >20-26t Euro-V SCR* | 1423553 | 0.0000% | 24.64% | 35.16% | 20.26% | 24.64% | 19.99% | 27.46% + 12.45% | - |
| RT_gt20-26t_Euro-VI_A-C | RT >20-26t Euro-VI A-C | 1423564 | 1.5238% | 24.64% | 14.09% | 28.28% | 24.64% | 39.67% | 21.96% + 12.45% | - |
| RT_gt20-26t_Euro-VIs_A-C | RT >20-26t Euro-VI* A-C | 1423565 | 0.0000% | 24.64% | 14.09% | 28.28% | 24.64% | 24.67% | 21.96% + 12.45% | - |
| RT_gt20-26t_Euro-VI_D-E | RT >20-26t Euro-VI D-E | 1423566 | 1.0220% | 24.53% | 82.15% | 5.05% | 24.53% | 55.36% | 22.28% + 12.43% | - |
| RT_gt20-26t_Euro-VIs_D-E | RT >20-26t Euro-VI* D-E | 1423567 | 0.0000% | 24.53% | 82.15% | 5.05% | 24.53% | 23.96% | 22.28% + 12.43% | - |
| RT_gt26-28t_50ties | RT >26-28t 50ties | 1423605 | 0.0000% | 22.29% | 20.19% | 19.05% | 22.29% | 22.62% | 18.44% + 13.65% | - |
| RT_gt26-28t_60ties | RT >26-28t 60ties | 1423606 | 0.0000% | 22.29% | 20.19% | 19.05% | 22.29% | 22.62% | 18.44% + 13.65% | - |
| RT_gt26-28t_70ties | RT >26-28t 70ties | 1423607 | 0.0000% | 22.29% | 20.19% | 19.05% | 22.29% | 22.62% | 18.44% + 13.65% | - |
| RT_gt26-28t_80ties | RT >26-28t 80ties | 1423608 | 0.0002% | 22.29% | 20.19% | 19.05% | 22.29% | 22.62% | 18.44% + 13.65% | - |
| RT_gt26-28t_Euro-I | RT >26-28t Euro-I | 1423610 | 0.0001% | 22.89% | 26.57% | 11.74% | 22.89% | 25.00% | 14.90% + 12.94% | - |
| RT_gt26-28t_Euro-II | RT >26-28t Euro-II | 1423620 | 0.0008% | 22.83% | 34.91% | 8.07% | 22.83% | 22.24% | 22.75% + 13.44% | - |
| RT_gt26-28t_Euro-III | RT >26-28t Euro-III | 1423630 | 0.0081% | 22.95% | 24.61% | 8.79% | 22.95% | 23.29% | 9.25% + 12.75% | - |
| RT_gt26-28t_Euro-IV_EGR | RT >26-28t Euro-IV EGR | 1423641 | 0.0055% | 23.53% | 14.34% | 16.25% | 23.53% | 20.77% | 19.20% + 12.73% | - |
| RT_gt26-28t_Euro-IV_SCR | RT >26-28t Euro-IV SCR | 1423642 | 0.0166% | 23.50% | 25.67% | 9.91% | 23.50% | 16.04% | 24.35% + 12.78% | - |
| RT_gt26-28t_Euro-IV_SCRs | RT >26-28t Euro-IV SCR* | 1423643 | 0.0000% | 23.50% | 25.67% | 9.91% | 23.50% | 21.65% | 24.35% + 12.78% | - |
| RT_gt26-28t_Euro-V_EGR | RT >26-28t Euro-V EGR | 1423651 | 0.0763% | 24.09% | 33.20% | 10.19% | 24.09% | 15.56% | 19.93% + 12.78% | - |
| RT_gt26-28t_Euro-V_SCR | RT >26-28t Euro-V SCR | 1423652 | 0.2288% | 24.07% | 22.42% | 9.94% | 24.07% | 21.21% | 22.24% + 12.83% | - |
| RT_gt26-28t_Euro-V_SCRs | RT >26-28t Euro-V SCR* | 1423653 | 0.0000% | 24.07% | 22.42% | 9.94% | 24.07% | 22.73% | 22.24% + 12.83% | - |
| RT_gt26-28t_Euro-VI_A-C | RT >26-28t Euro-VI A-C | 1423664 | 0.7191% | 23.83% | 11.94% | 24.25% | 23.83% | 52.62% | 48.24% + 12.57% | - |
| RT_gt26-28t_Euro-VIs_A-C | RT >26-28t Euro-VI* A-C | 1423665 | 0.0000% | 23.83% | 11.94% | 24.25% | 23.83% | 25.30% | 48.24% + 12.57% | - |
| RT_gt26-28t_Euro-VI_D-E | RT >26-28t Euro-VI D-E | 1423666 | 0.4587% | 24.01% | 73.10% | 9.71% | 24.01% | 71.63% | 18.34% + 12.57% | - |
| RT_gt26-28t_Euro-VIs_D-E | RT >26-28t Euro-VI* D-E | 1423667 | 0.0000% | 24.01% | 73.10% | 9.71% | 24.01% | 18.54% | 18.34% + 12.57% | - |
| RT_gt28-32t_50ties | RT >28-32t 50ties | 1423705 | 0.0000% | 23.11% | 25.03% | 13.67% | 23.11% | 25.06% | 15.40% + 13.67% | - |
| RT_gt28-32t_60ties | RT >28-32t 60ties | 1423706 | 0.0000% | 23.11% | 25.03% | 13.67% | 23.11% | 25.06% | 15.40% + 13.67% | - |
| RT_gt28-32t_70ties | RT >28-32t 70ties | 1423707 | 0.0000% | 23.11% | 25.03% | 13.67% | 23.11% | 25.06% | 15.40% + 13.67% | - |
| RT_gt28-32t_80ties | RT >28-32t 80ties | 1423708 | 0.0047% | 23.11% | 25.03% | 13.67% | 23.11% | 25.06% | 15.40% + 13.67% | - |
| RT_gt28-32t_Euro-I | RT >28-32t Euro-I | 1423710 | 0.0033% | 23.74% | 20.51% | 10.98% | 23.74% | 25.82% | 13.04% + 13.42% | - |
| RT_gt28-32t_Euro-II | RT >28-32t Euro-II | 1423720 | 0.0307% | 23.68% | 35.69% | 8.97% | 23.68% | 23.12% | 22.94% + 13.35% | - |
| RT_gt28-32t_Euro-III | RT >28-32t Euro-III | 1423730 | 0.0883% | 24.23% | 24.89% | 9.50% | 24.23% | 25.18% | 8.42% + 13.02% | - |
| RT_gt28-32t_Euro-IV_EGR | RT >28-32t Euro-IV EGR | 1423741 | 0.0106% | 24.77% | 20.73% | 19.03% | 24.77% | 27.71% | 19.02% + 13.28% | - |
| RT_gt28-32t_Euro-IV_SCR | RT >28-32t Euro-IV SCR | 1423742 | 0.0319% | 24.65% | 29.82% | 12.63% | 24.65% | 16.06% | 21.02% + 13.53% | - |
| RT_gt28-32t_Euro-IV_SCRs | RT >28-32t Euro-IV SCR* | 1423743 | 0.0000% | 24.65% | 29.82% | 12.63% | 24.65% | 22.55% | 21.02% + 13.53% | - |
| RT_gt28-32t_Euro-V_EGR | RT >28-32t Euro-V EGR | 1423751 | 0.0801% | 25.04% | 33.12% | 10.41% | 25.04% | 16.23% | 21.28% + 12.74% | - |
| RT_gt28-32t_Euro-V_SCR | RT >28-32t Euro-V SCR | 1423752 | 0.2402% | 25.04% | 22.79% | 10.70% | 25.04% | 21.33% | 22.74% + 12.79% | - |
| RT_gt28-32t_Euro-V_SCRs | RT >28-32t Euro-V SCR* | 1423753 | 0.0000% | 25.04% | 22.79% | 10.70% | 25.04% | 23.70% | 22.74% + 12.79% | - |
| RT_gt28-32t_Euro-VI_A-C | RT >28-32t Euro-VI A-C | 1423764 | 0.7075% | 24.89% | 12.46% | 18.57% | 24.89% | 47.88% | 54.57% + 12.76% | - |
| RT_gt28-32t_Euro-VIs_A-C | RT >28-32t Euro-VI* A-C | 1423765 | 0.0000% | 24.89% | 12.46% | 18.57% | 24.89% | 31.99% | 54.57% + 12.76% | - |
| RT_gt28-32t_Euro-VI_D-E | RT >28-32t Euro-VI D-E | 1423766 | 0.3436% | 25.25% | 88.86% | 11.99% | 25.25% | 64.70% | 21.19% + 12.78% | - |
| RT_gt28-32t_Euro-VIs_D-E | RT >28-32t Euro-VI* D-E | 1423767 | 0.0000% | 25.25% | 88.86% | 11.99% | 25.25% | 20.07% | 21.19% + 12.78% | - |
| RT_gt32t_50ties | RT >32t 50ties | 1423805 | 0.0000% | 23.78% | 24.35% | 13.04% | 23.78% | 25.69% | 14.84% + 13.66% | - |
| RT_gt32t_60ties | RT >32t 60ties | 1423806 | 0.0000% | 23.78% | 24.35% | 13.04% | 23.78% | 25.69% | 14.84% + 13.66% | - |
| RT_gt32t_70ties | RT >32t 70ties | 1423807 | 0.0000% | 23.78% | 24.35% | 13.04% | 23.78% | 25.69% | 14.84% + 13.66% | - |
| RT_gt32t_80ties | RT >32t 80ties | 1423808 | 0.0116% | 23.78% | 24.35% | 13.04% | 23.78% | 25.69% | 14.84% + 13.66% | - |
| RT_gt32t_Euro-I | RT >32t Euro-I | 1423810 | 0.0040% | 24.92% | 26.07% | 13.46% | 24.92% | 26.86% | 15.22% + 13.01% | - |
| RT_gt32t_Euro-II | RT >32t Euro-II | 1423820 | 0.0131% | 24.34% | 35.68% | 9.02% | 24.34% | 23.81% | 23.45% + 13.44% | - |
| RT_gt32t_Euro-III | RT >32t Euro-III | 1423830 | 0.0565% | 24.64% | 30.64% | 10.25% | 24.64% | 26.09% | 10.13% + 13.68% | - |
| RT_gt32t_Euro-IV_EGR | RT >32t Euro-IV EGR | 1423841 | 0.0208% | 25.39% | 22.77% | 17.03% | 25.39% | 26.84% | 18.13% + 13.49% | - |
| RT_gt32t_Euro-IV_SCR | RT >32t Euro-IV SCR | 1423842 | 0.0625% | 25.05% | 27.29% | 12.76% | 25.05% | 14.85% | 17.98% + 13.59% | - |
| RT_gt32t_Euro-IV_SCRs | RT >32t Euro-IV SCR* | 1423843 | 0.0000% | 25.05% | 27.29% | 12.76% | 25.05% | 22.64% | 17.98% + 13.59% | - |
| RT_gt32t_Euro-V_EGR | RT >32t Euro-V EGR | 1423851 | 0.1461% | 25.68% | 24.20% | 10.94% | 25.68% | 21.67% | 20.95% + 12.86% | - |
| RT_gt32t_Euro-V_SCR | RT >32t Euro-V SCR | 1423852 | 0.4382% | 25.53% | 31.95% | 13.28% | 25.53% | 20.33% | 23.14% + 13.16% | - |
| RT_gt32t_Euro-V_SCRs | RT >32t Euro-V SCR* | 1423853 | 0.0000% | 25.53% | 31.95% | 13.28% | 25.53% | 22.64% | 23.14% + 13.16% | - |
| RT_gt32t_Euro-VI_A-C | RT >32t Euro-VI A-C | 1423864 | 1.0194% | 25.98% | 13.79% | 17.34% | 25.98% | 42.34% | 48.55% + 13.13% | - |
| RT_gt32t_Euro-VIs_A-C | RT >32t Euro-VI* A-C | 1423865 | 0.0000% | 25.98% | 13.79% | 17.34% | 25.98% | 30.21% | 48.55% + 13.13% | - |
| RT_gt32t_Euro-VI_D-E | RT >32t Euro-VI D-E | 1423866 | 0.6295% | 25.97% | 91.61% | 11.40% | 25.97% | 60.02% | 18.27% + 12.79% | - |
| RT_gt32t_Euro-VIs_D-E | RT >32t Euro-VI* D-E | 1423867 | 0.0000% | 25.97% | 91.61% | 11.40% | 25.97% | 26.29% | 18.27% + 12.79% | - |
| TT_AT_le7_5t_50ties | TT/AT <=7,5t 50ties | 1425005 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| TT_AT_le7_5t_60ties | TT/AT <=7,5t 60ties | 1425006 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| TT_AT_le7_5t_70ties | TT/AT <=7,5t 70ties | 1425007 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| TT_AT_le7_5t_80ties | TT/AT <=7,5t 80ties | 1425008 | 0.0000% | 5.03% | 12.38% | 15.59% | 5.03% | 3.57% | 9.32% + 12.29% | - |
| TT_AT_le7_5t_Euro-I | TT/AT <=7,5t Euro-I | 1425010 | 0.0000% | 6.09% | 15.91% | 11.06% | 6.09% | 5.74% | 11.33% + 12.07% | - |
| TT_AT_le7_5t_Euro-II | TT/AT <=7,5t Euro-II | 1425020 | 0.0000% | 6.25% | 14.81% | 10.50% | 6.25% | 5.62% | 7.69% + 11.92% | - |
| TT_AT_le7_5t_Euro-III | TT/AT <=7,5t Euro-III | 1425030 | 0.0000% | 6.76% | 17.84% | 11.41% | 6.76% | 5.83% | 8.04% + 12.06% | - |
| TT_AT_le7_5t_Euro-IV_EGR | TT/AT <=7,5t Euro-IV EGR | 1425041 | 0.0000% | 5.95% | 9.05% | 15.18% | 5.95% | 8.10% | 17.28% + 12.13% | - |
| TT_AT_le7_5t_Euro-IV_SCR | TT/AT <=7,5t Euro-IV SCR | 1425042 | 0.0000% | 5.73% | 27.28% | 7.50% | 5.73% | 16.99% | 22.54% + 12.13% | - |
| TT_AT_le7_5t_Euro-IV_SCRs | TT/AT <=7,5t Euro-IV SCR* | 1425043 | 0.0000% | 5.73% | 27.28% | 7.50% | 5.73% | 4.05% | 22.54% + 12.13% | - |
| TT_AT_le7_5t_Euro-V_EGR | TT/AT <=7,5t Euro-V EGR | 1425051 | 0.0000% | 6.16% | 25.79% | 10.57% | 6.16% | 17.74% | 11.17% + 12.07% | - |
| TT_AT_le7_5t_Euro-V_SCR | TT/AT <=7,5t Euro-V SCR | 1425052 | 0.0000% | 6.16% | 28.39% | 6.08% | 6.16% | 25.73% | 23.64% + 12.19% | - |
| TT_AT_le7_5t_Euro-V_SCRs | TT/AT <=7,5t Euro-V SCR* | 1425053 | 0.0000% | 6.16% | 28.39% | 6.08% | 6.16% | 5.02% | 23.64% + 12.19% | - |
| TT_AT_le7_5t_Euro-VI_A-C | TT/AT <=7,5t Euro-VI A-C | 1425064 | 0.0000% | 6.17% | 13.08% | 8.86% | 6.17% | 37.86% | 24.34% + 12.09% | - |
| TT_AT_le7_5t_Euro-VIs_A-C | TT/AT <=7,5t Euro-VI* A-C | 1425065 | 0.0000% | 6.17% | 13.08% | 8.86% | 6.17% | 16.43% | 24.34% + 12.09% | - |
| TT_AT_le7_5t_Euro-VI_D-E | TT/AT <=7,5t Euro-VI D-E | 1425066 | 0.0000% | 7.27% | 75.44% | 4.73% | 7.27% | 16.53% | 8.71% + 12.08% | - |
| TT_AT_le7_5t_Euro-VIs_D-E | TT/AT <=7,5t Euro-VI* D-E | 1425067 | 0.0000% | 7.27% | 75.44% | 4.73% | 7.27% | 12.47% | 8.71% + 12.08% | - |
| TT_AT_gt20-28t_50ties | TT/AT >20-28t 50ties | 1425105 | 0.0000% | 19.35% | 17.48% | 18.12% | 19.35% | 17.33% | 18.30% + 12.35% | - |
| TT_AT_gt20-28t_60ties | TT/AT >20-28t 60ties | 1425106 | 0.0000% | 19.35% | 17.48% | 18.12% | 19.35% | 17.33% | 18.30% + 12.35% | - |
| TT_AT_gt20-28t_70ties | TT/AT >20-28t 70ties | 1425107 | 0.0000% | 19.35% | 17.48% | 18.12% | 19.35% | 17.33% | 18.30% + 12.35% | - |
| TT_AT_gt20-28t_80ties | TT/AT >20-28t 80ties | 1425108 | 0.5787% | 19.35% | 17.48% | 18.12% | 19.35% | 17.33% | 18.30% + 12.35% | - |
| TT_AT_lt28t_EE | TT/AT <28t EE | 1425109 | 0.0000% | 19.35% | 17.48% | 18.12% | 19.35% | 17.33% | 18.30% + 12.35% | - |
| TT_AT_gt20-28t_Euro-I | TT/AT >20-28t Euro-I | 1425110 | 0.2649% | 19.57% | 17.06% | 9.01% | 19.57% | 18.02% | 14.57% + 12.37% | - |
| TT_AT_gt20-28t_Euro-II | TT/AT >20-28t Euro-II | 1425120 | 0.6196% | 19.57% | 20.58% | 9.96% | 19.57% | 15.59% | 13.90% + 12.15% | - |
| TT_AT_gt20-28t_Euro-III | TT/AT >20-28t Euro-III | 1425130 | 0.9991% | 19.12% | 18.57% | 9.14% | 19.12% | 14.66% | 7.18% + 12.06% | - |
| TT_AT_gt20-28t_Euro-IV_EGR | TT/AT >20-28t Euro-IV EGR | 1425141 | 0.1630% | 19.98% | 12.15% | 19.97% | 19.98% | 20.34% | 27.01% + 12.08% | - |
| TT_AT_gt20-28t_Euro-IV_SCR | TT/AT >20-28t Euro-IV SCR | 1425142 | 0.4890% | 19.90% | 36.04% | 12.43% | 19.90% | 22.25% | 28.15% + 12.23% | - |
| TT_AT_gt20-28t_Euro-IV_SCRs | TT/AT >20-28t Euro-IV SCR* | 1425143 | 0.0000% | 19.90% | 36.04% | 12.43% | 19.90% | 16.34% | 28.15% + 12.23% | - |
| TT_AT_gt20-28t_Euro-V_EGR | TT/AT >20-28t Euro-V EGR | 1425151 | 0.6915% | 20.65% | 34.18% | 7.91% | 20.65% | 17.67% | 15.32% + 12.23% | - |
| TT_AT_gt20-28t_Euro-V_SCR | TT/AT >20-28t Euro-V SCR | 1425152 | 2.0745% | 20.66% | 30.43% | 13.77% | 20.66% | 25.75% | 24.33% + 12.39% | - |
| TT_AT_gt20-28t_Euro-V_SCRs | TT/AT >20-28t Euro-V SCR* | 1425153 | 0.0000% | 20.66% | 30.43% | 13.77% | 20.66% | 17.06% | 24.33% + 12.39% | - |
| TT_AT_gt20-28t_Euro-VI_A-C | TT/AT >20-28t Euro-VI A-C | 1425164 | 2.6355% | 20.67% | 19.12% | 9.32% | 20.67% | 52.59% | 28.72% + 12.29% | - |
| TT_AT_gt20-28t_Euro-VIs_A-C | TT/AT >20-28t Euro-VI* A-C | 1425165 | 0.0000% | 20.67% | 19.12% | 9.32% | 20.67% | 25.17% | 28.72% + 12.29% | - |
| TT_AT_gt20-28t_Euro-VI_D-E | TT/AT >20-28t Euro-VI D-E | 1425166 | 1.4048% | 20.48% | 82.91% | 6.43% | 20.48% | 46.21% | 12.18% + 12.17% | - |
| TT_AT_gt20-28t_Euro-VIs_D-E | TT/AT >20-28t Euro-VI* D-E | 1425167 | 0.0000% | 20.48% | 82.91% | 6.43% | 20.48% | 19.86% | 12.18% + 12.17% | - |
| TT_AT_gt28-34t_50ties | TT/AT >28-34t 50ties | 1425205 | 0.0000% | 25.95% | 22.48% | 11.83% | 25.95% | 24.65% | 22.18% + 13.00% | - |
| TT_AT_gt28-34t_60ties | TT/AT >28-34t 60ties | 1425206 | 0.0000% | 25.95% | 22.48% | 11.83% | 25.95% | 24.65% | 22.18% + 13.00% | - |
| TT_AT_gt28-34t_70ties | TT/AT >28-34t 70ties | 1425207 | 0.0000% | 25.95% | 22.48% | 11.83% | 25.95% | 24.65% | 22.18% + 13.00% | - |
| TT_AT_gt28-34t_80ties | TT/AT >28-34t 80ties | 1425208 | 0.0508% | 25.95% | 22.48% | 11.83% | 25.95% | 24.65% | 22.18% + 13.00% | - |
| TT_AT_28-34t_EE | TT/AT 28-34t EE | 1425209 | 0.0000% | 25.95% | 22.48% | 11.83% | 25.95% | 24.65% | 22.18% + 13.00% | - |
| TT_AT_gt28-34t_Euro-I | TT/AT >28-34t Euro-I | 1425210 | 0.0371% | 26.03% | 25.86% | 12.58% | 26.03% | 25.46% | 17.21% + 12.81% | - |
| TT_AT_gt28-34t_Euro-II | TT/AT >28-34t Euro-II | 1425220 | 0.0649% | 26.68% | 23.60% | 13.08% | 26.68% | 23.05% | 17.91% + 12.88% | - |
| TT_AT_gt28-34t_Euro-III | TT/AT >28-34t Euro-III | 1425230 | 0.1125% | 26.15% | 22.54% | 10.44% | 26.15% | 21.52% | 16.13% + 12.87% | - |
| TT_AT_gt28-34t_Euro-IV_EGR | TT/AT >28-34t Euro-IV EGR | 1425241 | 0.0145% | 27.22% | 23.50% | 27.83% | 27.22% | 21.14% | 26.39% + 13.08% | - |
| TT_AT_gt28-34t_Euro-IV_SCR | TT/AT >28-34t Euro-IV SCR | 1425242 | 0.0436% | 27.07% | 39.99% | 23.66% | 27.07% | 20.99% | 30.96% + 12.97% | - |
| TT_AT_gt28-34t_Euro-IV_SCRs | TT/AT >28-34t Euro-IV SCR* | 1425243 | 0.0000% | 27.07% | 39.99% | 23.66% | 27.07% | 23.05% | 30.96% + 12.97% | - |
| TT_AT_gt28-34t_Euro-V_EGR | TT/AT >28-34t Euro-V EGR | 1425251 | 0.0851% | 27.27% | 44.98% | 12.50% | 27.27% | 21.35% | 24.24% + 12.82% | - |
| TT_AT_gt28-34t_Euro-V_SCR | TT/AT >28-34t Euro-V SCR | 1425252 | 0.2552% | 27.29% | 43.46% | 21.57% | 27.29% | 22.91% | 34.41% + 12.71% | - |
| TT_AT_gt28-34t_Euro-V_SCRs | TT/AT >28-34t Euro-V SCR* | 1425253 | 0.0000% | 27.29% | 43.46% | 21.57% | 27.29% | 23.25% | 34.41% + 12.71% | - |
| TT_AT_gt28-34t_Euro-VI_A-C | TT/AT >28-34t Euro-VI A-C | 1425264 | 0.6096% | 26.95% | 15.38% | 26.61% | 26.95% | 37.87% | 22.97% + 12.84% | - |
| TT_AT_gt28-34t_Euro-VIs_A-C | TT/AT >28-34t Euro-VI* A-C | 1425265 | 0.0000% | 26.95% | 15.38% | 26.61% | 26.95% | 27.22% | 22.97% + 12.84% | - |
| TT_AT_gt28-34t_Euro-VI_D-E | TT/AT >28-34t Euro-VI D-E | 1425266 | 0.3059% | 27.14% | 94.77% | 7.05% | 27.14% | 39.67% | 25.68% + 12.78% | - |
| TT_AT_gt28-34t_Euro-VIs_D-E | TT/AT >28-34t Euro-VI* D-E | 1425267 | 0.0000% | 27.14% | 94.77% | 7.05% | 27.14% | 23.42% | 25.68% + 12.78% | - |
| TT_AT_gt34-40t_50ties | TT/AT >34-40t 50ties | 1425305 | 0.0000% | 26.65% | 23.40% | 11.62% | 26.65% | 27.44% | 18.16% + 14.02% | - |
| TT_AT_gt34-40t_60ties | TT/AT >34-40t 60ties | 1425306 | 0.0000% | 26.65% | 23.40% | 11.62% | 26.65% | 27.44% | 18.16% + 14.02% | - |
| TT_AT_gt34-40t_70ties | TT/AT >34-40t 70ties | 1425307 | 0.0000% | 26.65% | 23.40% | 11.62% | 26.65% | 27.44% | 18.16% + 14.02% | - |
| TT_AT_gt34-40t_80ties | TT/AT >34-40t 80ties | 1425308 | 0.1187% | 26.65% | 23.40% | 11.62% | 26.65% | 27.44% | 18.16% + 14.02% | - |
| TT_AT_gt34-40t_Euro-I | TT/AT >34-40t Euro-I | 1425310 | 0.1226% | 26.87% | 25.28% | 12.09% | 26.87% | 26.46% | 17.57% + 13.99% | - |
| TT_AT_gt34-40t_Euro-II | TT/AT >34-40t Euro-II | 1425320 | 0.3800% | 27.20% | 28.71% | 13.24% | 27.20% | 24.67% | 20.11% + 13.35% | - |
| TT_AT_gt34-40t_Euro-III | TT/AT >34-40t Euro-III | 1425330 | 1.0111% | 27.31% | 27.29% | 8.75% | 27.31% | 25.73% | 12.41% + 13.20% | - |
| TT_AT_gt34-40t_Euro-IV_EGR | TT/AT >34-40t Euro-IV EGR | 1425341 | 0.0889% | 27.97% | 20.93% | 22.75% | 27.97% | 25.62% | 20.98% + 13.12% | - |
| TT_AT_gt34-40t_Euro-IV_SCR | TT/AT >34-40t Euro-IV SCR | 1425342 | 0.2666% | 28.24% | 24.92% | 21.70% | 28.24% | 23.50% | 19.46% + 13.16% | - |
| TT_AT_gt34-40t_Euro-IV_SCRs | TT/AT >34-40t Euro-IV SCR* | 1425343 | 0.0000% | 28.24% | 24.92% | 21.70% | 28.24% | 24.64% | 19.46% + 13.16% | - |
| TT_AT_gt34-40t_Euro-V_EGR | TT/AT >34-40t Euro-V EGR | 1425351 | 1.5393% | 28.75% | 29.64% | 13.10% | 28.75% | 23.67% | 19.92% + 13.06% | - |
| TT_AT_gt34-40t_Euro-V_SCR | TT/AT >34-40t Euro-V SCR | 1425352 | 4.6179% | 28.93% | 36.41% | 24.50% | 28.93% | 22.89% | 26.05% + 13.12% | - |
| TT_AT_gt34-40t_Euro-V_SCRs | TT/AT >34-40t Euro-V SCR* | 1425353 | 0.0000% | 28.93% | 36.41% | 24.50% | 28.93% | 24.23% | 26.05% + 13.12% | - |
| TT_AT_gt34-40t_Euro-VI_A-C | TT/AT >34-40t Euro-VI A-C | 1425364 | 21.5803% | 28.97% | 18.58% | 32.00% | 28.97% | 34.01% | 23.38% + 13.12% | - |
| TT_AT_gt34-40t_Euro-VIs_A-C | TT/AT >34-40t Euro-VI* A-C | 1425365 | 0.0000% | 28.97% | 18.58% | 32.00% | 28.97% | 29.57% | 23.38% + 13.12% | - |
| TT_AT_gt34-40t_Euro-VI_D-E | TT/AT >34-40t Euro-VI D-E | 1425366 | 14.9930% | 28.82% | 89.90% | 7.04% | 28.82% | 38.67% | 23.92% + 13.07% | - |
| TT_AT_gt34-40t_Euro-VIs_D-E | TT/AT >34-40t Euro-VI* D-E | 1425367 | 0.0000% | 28.82% | 89.90% | 7.04% | 28.82% | 30.37% | 23.92% + 13.07% | - |
| TT_AT_gt40-50t_50ties | TT/AT >40-50t 50ties | 1425405 | 0.0000% | 26.91% | 18.10% | 11.11% | 26.91% | 27.80% | 16.35% + 13.58% | - |
| TT_AT_gt40-50t_60ties | TT/AT >40-50t 60ties | 1425406 | 0.0000% | 26.91% | 18.10% | 11.11% | 26.91% | 27.80% | 16.35% + 13.58% | - |
| TT_AT_gt40-50t_70ties | TT/AT >40-50t 70ties | 1425407 | 0.0000% | 26.91% | 18.10% | 11.11% | 26.91% | 27.80% | 16.35% + 13.58% | - |
| TT_AT_gt40-50t_80ties | TT/AT >40-50t 80ties | 1425408 | 0.0000% | 26.91% | 18.10% | 11.11% | 26.91% | 27.80% | 16.35% + 13.58% | - |
| TT_AT_gt40-50t_Euro-I | TT/AT >40-50t Euro-I | 1425410 | 0.0000% | 26.87% | 27.30% | 12.92% | 26.87% | 26.43% | 17.48% + 14.00% | - |
| TT_AT_gt40-50t_Euro-II | TT/AT >40-50t Euro-II | 1425420 | 0.0000% | 27.66% | 25.77% | 12.59% | 27.66% | 24.75% | 20.12% + 12.96% | - |
| TT_AT_gt40-50t_Euro-III | TT/AT >40-50t Euro-III | 1425430 | 0.0000% | 27.35% | 27.28% | 8.77% | 27.35% | 25.70% | 12.45% + 13.19% | - |
| TT_AT_gt40-50t_Euro-IV_EGR | TT/AT >40-50t Euro-IV EGR | 1425441 | 0.0000% | 27.98% | 20.95% | 22.73% | 27.98% | 25.60% | 21.02% + 13.12% | - |
| TT_AT_gt40-50t_Euro-IV_SCR | TT/AT >40-50t Euro-IV SCR | 1425442 | 0.0000% | 28.26% | 25.27% | 21.74% | 28.26% | 23.74% | 19.68% + 13.16% | - |
| TT_AT_gt40-50t_Euro-IV_SCRs | TT/AT >40-50t Euro-IV SCR* | 1425443 | 0.0000% | 28.26% | 25.27% | 21.74% | 28.26% | 24.66% | 19.68% + 13.16% | - |
| TT_AT_gt40-50t_Euro-V_EGR | TT/AT >40-50t Euro-V EGR | 1425451 | 0.0000% | 28.77% | 29.68% | 13.14% | 28.77% | 23.71% | 19.85% + 13.06% | - |
| TT_AT_gt40-50t_Euro-V_SCR | TT/AT >40-50t Euro-V SCR | 1425452 | 0.0000% | 28.97% | 36.16% | 24.42% | 28.97% | 23.09% | 25.88% + 13.11% | - |
| TT_AT_gt40-50t_Euro-V_SCRs | TT/AT >40-50t Euro-V SCR* | 1425453 | 0.0000% | 28.97% | 36.16% | 24.42% | 28.97% | 24.27% | 25.88% + 13.11% | - |
| TT_AT_gt40-50t_Euro-VI_A-C | TT/AT >40-50t Euro-VI A-C | 1425464 | 0.0000% | 28.98% | 18.62% | 32.00% | 28.98% | 33.58% | 23.40% + 13.12% | - |
| TT_AT_gt40-50t_Euro-VIs_A-C | TT/AT >40-50t Euro-VI* A-C | 1425465 | 0.0000% | 28.98% | 18.62% | 32.00% | 28.98% | 29.58% | 23.40% + 13.12% | - |
| TT_AT_gt40-50t_Euro-VI_D-E | TT/AT >40-50t Euro-VI D-E | 1425466 | 0.0000% | 28.82% | 89.66% | 7.04% | 28.82% | 39.28% | 24.06% + 13.07% | - |
| TT_AT_gt40-50t_Euro-VIs_D-E | TT/AT >40-50t Euro-VI* D-E | 1425467 | 0.0000% | 28.82% | 89.66% | 7.04% | 28.82% | 30.18% | 24.06% + 13.07% | - |
| TT_AT_gt50-60t_50ties | TT/AT >50-60t 50ties | 1425505 | 0.0000% | 27.17% | 23.98% | 11.23% | 27.17% | 27.97% | 18.17% + 14.27% | - |
| TT_AT_gt50-60t_60ties | TT/AT >50-60t 60ties | 1425506 | 0.0000% | 27.17% | 23.98% | 11.23% | 27.17% | 27.97% | 18.17% + 14.27% | - |
| TT_AT_gt50-60t_70ties | TT/AT >50-60t 70ties | 1425507 | 0.0000% | 27.17% | 23.98% | 11.23% | 27.17% | 27.97% | 18.17% + 14.27% | - |
| TT_AT_gt50-60t_80ties | TT/AT >50-60t 80ties | 1425508 | 0.0000% | 27.17% | 23.98% | 11.23% | 27.17% | 27.97% | 18.17% + 14.27% | - |
| TT_AT_gt50-60t_Euro-I | TT/AT >50-60t Euro-I | 1425510 | 0.0000% | 28.53% | 22.91% | 12.76% | 28.53% | 27.73% | 19.61% + 13.14% | - |
| TT_AT_gt50-60t_Euro-II | TT/AT >50-60t Euro-II | 1425520 | 0.0000% | 27.56% | 34.04% | 12.83% | 27.56% | 25.33% | 22.15% + 14.01% | - |
| TT_AT_gt50-60t_Euro-III | TT/AT >50-60t Euro-III | 1425530 | 0.0000% | 28.75% | 26.44% | 11.72% | 28.75% | 25.32% | 16.66% + 13.02% | - |
| TT_AT_gt50-60t_Euro-IV_EGR | TT/AT >50-60t Euro-IV EGR | 1425541 | 0.0000% | 28.52% | 23.06% | 23.94% | 28.52% | 27.39% | 21.10% + 13.26% | - |
| TT_AT_gt50-60t_Euro-IV_SCR | TT/AT >50-60t Euro-IV SCR | 1425542 | 0.0000% | 29.22% | 40.12% | 23.43% | 29.22% | 25.10% | 30.26% + 13.01% | - |
| TT_AT_gt50-60t_Euro-IV_SCRs | TT/AT >50-60t Euro-IV SCR* | 1425543 | 0.0000% | 29.22% | 40.12% | 23.43% | 29.22% | 25.63% | 30.26% + 13.01% | - |
| TT_AT_gt50-60t_Euro-V_EGR | TT/AT >50-60t Euro-V EGR | 1425551 | 0.0000% | 29.59% | 42.41% | 14.67% | 29.59% | 24.86% | 24.71% + 13.02% | - |
| TT_AT_gt50-60t_Euro-V_SCR | TT/AT >50-60t Euro-V SCR | 1425552 | 0.0000% | 29.65% | 35.03% | 21.76% | 29.65% | 24.41% | 27.67% + 13.05% | - |
| TT_AT_gt50-60t_Euro-V_SCRs | TT/AT >50-60t Euro-V SCR* | 1425553 | 0.0000% | 29.65% | 35.03% | 21.76% | 29.65% | 25.77% | 27.67% + 13.05% | - |
| TT_AT_gt50-60t_Euro-VI_A-C | TT/AT >50-60t Euro-VI A-C | 1425564 | 0.0000% | 29.44% | 19.23% | 21.61% | 29.44% | 35.02% | 31.29% + 13.03% | - |
| TT_AT_gt50-60t_Euro-VIs_A-C | TT/AT >50-60t Euro-VI* A-C | 1425565 | 0.0000% | 29.44% | 19.23% | 21.61% | 29.44% | 31.06% | 31.29% + 13.03% | - |
| TT_AT_gt50-60t_Euro-VI_D-E | TT/AT >50-60t Euro-VI D-E | 1425566 | 0.0000% | 29.79% | 78.77% | 8.70% | 29.79% | 39.02% | 25.79% + 13.04% | - |
| TT_AT_gt50-60t_Euro-VIs_D-E | TT/AT >50-60t Euro-VI* D-E | 1425567 | 0.0000% | 29.79% | 78.77% | 8.70% | 29.79% | 26.27% | 25.79% + 13.04% | - |
| TT_AT_gt60t_50ties | TT/AT >60t 50ties | 1425605 | 0.0000% | 24.85% | 26.93% | 10.38% | 24.85% | 25.64% | 16.56% + 14.30% | - |
| TT_AT_gt60t_60ties | TT/AT >60t 60ties | 1425606 | 0.0000% | 24.85% | 26.93% | 10.38% | 24.85% | 25.64% | 16.56% + 14.30% | - |
| TT_AT_gt60t_70ties | TT/AT >60t 70ties | 1425607 | 0.0000% | 24.85% | 26.93% | 10.38% | 24.85% | 25.64% | 16.56% + 14.30% | - |
| TT_AT_gt60t_80ties | TT/AT >60t 80ties | 1425608 | 0.0000% | 24.85% | 26.93% | 10.38% | 24.85% | 25.64% | 16.56% + 14.30% | - |
| TT_AT_gt60t_Euro-I | TT/AT >60t Euro-I | 1425610 | 0.0000% | 23.57% | 27.61% | 11.17% | 23.57% | 24.41% | 14.78% + 14.65% | - |
| TT_AT_gt60t_Euro-II | TT/AT >60t Euro-II | 1425620 | 0.0000% | 25.57% | 36.02% | 13.75% | 25.57% | 24.55% | 22.03% + 13.97% | - |
| TT_AT_gt60t_Euro-III | TT/AT >60t Euro-III | 1425630 | 0.0000% | 26.95% | 25.86% | 11.96% | 26.95% | 26.20% | 14.67% + 14.60% | - |
| TT_AT_gt60t_Euro-IV_EGR | TT/AT >60t Euro-IV EGR | 1425641 | 0.0000% | 26.92% | 22.82% | 22.33% | 26.92% | 26.31% | 18.13% + 14.51% | - |
| TT_AT_gt60t_Euro-IV_SCR | TT/AT >60t Euro-IV SCR | 1425642 | 0.0000% | 26.94% | 30.85% | 17.99% | 26.94% | 22.61% | 25.08% + 15.13% | - |
| TT_AT_gt60t_Euro-IV_SCRs | TT/AT >60t Euro-IV SCR* | 1425643 | 0.0000% | 26.94% | 30.85% | 17.99% | 26.94% | 24.93% | 25.08% + 15.13% | - |
| TT_AT_gt60t_Euro-V_EGR | TT/AT >60t Euro-V EGR | 1425651 | 0.0000% | 27.20% | 37.86% | 14.43% | 27.20% | 23.70% | 28.44% + 14.93% | - |
| TT_AT_gt60t_Euro-V_SCR | TT/AT >60t Euro-V SCR | 1425652 | 0.0000% | 27.29% | 30.08% | 19.39% | 27.29% | 23.70% | 24.13% + 14.87% | - |
| TT_AT_gt60t_Euro-V_SCRs | TT/AT >60t Euro-V SCR* | 1425653 | 0.0000% | 27.29% | 30.08% | 19.39% | 27.29% | 24.79% | 24.13% + 14.87% | - |
| TT_AT_gt60t_Euro-VI_A-C | TT/AT >60t Euro-VI A-C | 1425664 | 0.0000% | 26.61% | 20.84% | 16.02% | 26.61% | 33.66% | 34.45% + 15.00% | - |
| TT_AT_gt60t_Euro-VIs_A-C | TT/AT >60t Euro-VI* A-C | 1425665 | 0.0000% | 26.61% | 20.84% | 16.02% | 26.61% | 30.63% | 34.45% + 15.00% | - |
| TT_AT_gt60t_Euro-VI_D-E | TT/AT >60t Euro-VI D-E | 1425666 | 0.0000% | 27.78% | 77.44% | 11.59% | 27.78% | 37.56% | 24.38% + 14.86% | - |
| TT_AT_gt60t_Euro-VIs_D-E | TT/AT >60t Euro-VI* D-E | 1425667 | 0.0000% | 27.78% | 77.44% | 11.59% | 27.78% | 24.45% | 24.38% + 14.86% | - |
| TT_AT_gt14-20t_50ties | TT/AT >14-20t 50ties | 1425805 | 0.0000% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| TT_AT_gt14-20t_60ties | TT/AT >14-20t 60ties | 1425806 | 0.0000% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| TT_AT_gt14-20t_70ties | TT/AT >14-20t 70ties | 1425807 | 0.0000% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| TT_AT_gt14-20t_80ties | TT/AT >14-20t 80ties | 1425808 | 0.0000% | 19.83% | 18.45% | 14.10% | 19.83% | 19.01% | 16.77% + 13.85% | - |
| TT_AT_gt14-20t_Euro-I | TT/AT >14-20t Euro-I | 1425810 | 0.0000% | 19.35% | 18.99% | 8.93% | 19.35% | 19.58% | 13.34% + 14.26% | - |
| TT_AT_gt14-20t_Euro-II | TT/AT >14-20t Euro-II | 1425820 | 0.0000% | 20.81% | 27.36% | 10.70% | 20.81% | 18.36% | 17.35% + 13.79% | - |
| TT_AT_gt14-20t_Euro-III | TT/AT >14-20t Euro-III | 1425830 | 0.0000% | 21.43% | 22.18% | 9.44% | 21.43% | 17.58% | 12.94% + 13.06% | - |
| TT_AT_gt14-20t_Euro-IV_EGR | TT/AT >14-20t Euro-IV EGR | 1425841 | 0.0000% | 21.80% | 14.45% | 16.77% | 21.80% | 16.01% | 26.19% + 12.98% | - |
| TT_AT_gt14-20t_Euro-IV_SCR | TT/AT >14-20t Euro-IV SCR | 1425842 | 0.0000% | 21.84% | 29.90% | 14.36% | 21.84% | 15.54% | 23.24% + 13.20% | - |
| TT_AT_gt14-20t_Euro-IV_SCRs | TT/AT >14-20t Euro-IV SCR* | 1425843 | 0.0000% | 21.84% | 29.90% | 14.36% | 21.84% | 18.56% | 23.24% + 13.20% | - |
| TT_AT_gt14-20t_Euro-V_EGR | TT/AT >14-20t Euro-V EGR | 1425851 | 0.0000% | 21.42% | 33.94% | 9.15% | 21.42% | 16.86% | 18.32% + 12.61% | - |
| TT_AT_gt14-20t_Euro-V_SCR | TT/AT >14-20t Euro-V SCR | 1425852 | 0.0000% | 21.40% | 27.31% | 17.45% | 21.40% | 23.75% | 21.19% + 12.59% | - |
| TT_AT_gt14-20t_Euro-V_SCRs | TT/AT >14-20t Euro-V SCR* | 1425853 | 0.0000% | 21.40% | 27.31% | 17.45% | 21.40% | 17.80% | 21.19% + 12.59% | - |
| TT_AT_gt14-20t_Euro-VI_A-C | TT/AT >14-20t Euro-VI A-C | 1425864 | 0.0000% | 20.51% | 21.14% | 9.26% | 20.51% | 49.98% | 29.41% + 12.32% | - |
| TT_AT_gt14-20t_Euro-VIs_A-C | TT/AT >14-20t Euro-VI* A-C | 1425865 | 0.0000% | 19.55% | 15.31% | 20.53% | 19.55% | 22.91% | 21.72% + 12.54% | - |
| TT_AT_gt14-20t_Euro-VI_D-E | TT/AT >14-20t Euro-VI D-E | 1425866 | 0.0000% | 21.31% | 95.85% | 6.71% | 21.31% | 32.27% | 17.39% + 12.21% | - |
| TT_AT_gt14-20t_Euro-VIs_D-E | TT/AT >14-20t Euro-VI* D-E | 1425867 | 0.0000% | 19.97% | 67.51% | 5.21% | 19.97% | 23.92% | 17.01% + 12.54% | - |
| TT_AT_gt7_5-14t_50ties | TT/AT >7,5-14t 50ties | 1425905 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| TT_AT_gt7_5-14t_60ties | TT/AT >7,5-14t 60ties | 1425906 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| TT_AT_gt7_5-14t_70ties | TT/AT >7,5-14t 70ties | 1425907 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| TT_AT_gt7_5-14t_80ties | TT/AT >7,5-14t 80ties | 1425908 | 0.0000% | 13.44% | 12.70% | 24.82% | 13.44% | 12.16% | 12.34% + 12.65% | - |
| TT_AT_gt7_5-14t_Euro-I | TT/AT >7,5-14t Euro-I | 1425910 | 0.0000% | 14.05% | 13.26% | 11.92% | 14.05% | 15.62% | 9.77% + 12.78% | - |
| TT_AT_gt7_5-14t_Euro-II | TT/AT >7,5-14t Euro-II | 1425920 | 0.0000% | 14.28% | 26.36% | 12.31% | 14.28% | 13.16% | 13.25% + 12.52% | - |
| TT_AT_gt7_5-14t_Euro-III | TT/AT >7,5-14t Euro-III | 1425930 | 0.0000% | 14.87% | 23.58% | 15.56% | 14.87% | 17.16% | 7.26% + 12.54% | - |
| TT_AT_gt7_5-14t_Euro-IV_EGR | TT/AT >7,5-14t Euro-IV EGR | 1425941 | 0.0000% | 15.59% | 16.09% | 16.63% | 15.59% | 20.04% | 16.41% + 12.53% | - |
| TT_AT_gt7_5-14t_Euro-IV_SCR | TT/AT >7,5-14t Euro-IV SCR | 1425942 | 0.0000% | 15.49% | 25.86% | 7.89% | 15.49% | 14.59% | 18.35% + 12.56% | - |
| TT_AT_gt7_5-14t_Euro-IV_SCRs | TT/AT >7,5-14t Euro-IV SCR* | 1425943 | 0.0000% | 15.49% | 25.86% | 7.89% | 15.49% | 12.93% | 18.35% + 12.56% | - |
| TT_AT_gt7_5-14t_Euro-V_EGR | TT/AT >7,5-14t Euro-V EGR | 1425951 | 0.0000% | 16.25% | 29.83% | 10.17% | 16.25% | 13.73% | 20.16% + 12.54% | - |
| TT_AT_gt7_5-14t_Euro-V_SCR | TT/AT >7,5-14t Euro-V SCR | 1425952 | 0.0000% | 16.20% | 25.23% | 10.70% | 16.20% | 23.27% | 18.67% + 12.56% | - |
| TT_AT_gt7_5-14t_Euro-V_SCRs | TT/AT >7,5-14t Euro-V SCR* | 1425953 | 0.0000% | 16.20% | 25.23% | 10.70% | 16.20% | 13.29% | 18.67% + 12.56% | - |
| TT_AT_gt7_5-14t_Euro-VI_A-C | TT/AT >7,5-14t Euro-VI A-C | 1425964 | 0.0000% | 16.45% | 11.46% | 25.47% | 16.45% | 43.26% | 64.88% + 12.34% | - |
| TT_AT_gt7_5-14t_Euro-VIs_A-C | TT/AT >7,5-14t Euro-VI* A-C | 1425965 | 0.0000% | 16.45% | 11.46% | 25.47% | 16.45% | 16.55% | 64.88% + 12.34% | - |
| TT_AT_gt7_5-14t_Euro-VI_D-E | TT/AT >7,5-14t Euro-VI D-E | 1425966 | 0.0000% | 16.94% | 100.53% | 7.91% | 16.94% | 46.30% | 11.96% + 12.32% | - |
| TT_AT_gt7_5-14t_Euro-VIs_D-E | TT/AT >7,5-14t Euro-VI* D-E | 1425967 | 0.0000% | 16.94% | 100.53% | 7.91% | 16.94% | 15.93% | 11.96% + 12.32% | - |
| HGV_CNG_le7_5t_Euro-IV | HGV CNG <=7,5t Euro-IV | 1433140 | 0.0000% | 5.95% | 9.05% | 15.18% | 5.95% | 8.10% | 17.28% + 12.13% | - |
| HGV_CNG_le7_5t_Euro-V | HGV CNG <=7,5t Euro-V | 1433150 | 0.0285% | 6.16% | 6.16% | 6.16% | 6.16% | 6.16% | 6.16% + 12.19% | - |
| HGV_CNG_le7_5t_Euro-VI | HGV CNG <=7,5t Euro-VI | 1433160 | 0.0310% | 6.17% | 6.17% | 6.17% | 6.17% | 6.17% | 6.17% + 12.09% | - |
| HGV_CNG_gt7_5-12t_Euro-IV | HGV CNG >7,5-12t Euro-IV | 1433240 | 0.0000% | 15.59% | 16.09% | 16.63% | 15.59% | 20.04% | 16.41% + 12.53% | - |
| HGV_CNG_gt7_5-12t_Euro-V | HGV CNG >7,5-12t Euro-V | 1433250 | 0.0002% | 16.20% | 16.20% | 16.20% | 16.20% | 16.20% | 16.20% + 12.56% | - |
| HGV_CNG_gt7_5-12t_Euro-VI | HGV CNG >7,5-12t Euro-VI | 1433260 | 0.0004% | 16.45% | 16.45% | 16.45% | 16.45% | 16.45% | 16.45% + 12.34% | - |
| HGV_CNG_gt12t_Euro-IV | HGV CNG >12t Euro-IV | 1434140 | 0.0000% | 21.80% | 14.45% | 16.77% | 21.80% | 16.01% | 26.19% + 12.98% | - |
| HGV_CNG_gt12t_Euro-V | HGV CNG >12t Euro-V | 1434150 | 0.0037% | 21.40% | 21.40% | 21.40% | 21.40% | 21.40% | 21.40% + 12.59% | - |
| HGV_CNG_gt12t_Euro-VI | HGV CNG >12t Euro-VI | 1434160 | 0.0357% | 20.51% | 20.51% | 20.51% | 20.51% | 20.51% | 20.51% + 12.32% | - |
| TT_AT_CNG_Euro-IV | TT/AT CNG Euro-IV | 1435940 | 0.0000% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% + 13.12% | - |
| TT_AT_CNG_Euro-V | TT/AT CNG Euro-V | 1435950 | 0.0102% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% + 13.12% | - |
| TT_AT_CNG_Euro-VI | TT/AT CNG Euro-VI | 1435960 | 0.0361% | 28.97% | 28.97% | 28.97% | 28.97% | 28.97% | 28.97% + 13.12% | - |
| HGV_LNG_le7_5t_Euro-IV | HGV LNG <=7,5t Euro-IV | 1445140 | 0.0000% | 5.95% | 9.05% | 15.18% | 5.95% | 8.10% | 17.28% + 12.13% | - |
| HGV_LNG_le7_5t_Euro-V | HGV LNG <=7,5t Euro-V | 1445150 | 0.0000% | 6.16% | 6.16% | 6.16% | 6.16% | 6.16% | 6.16% + 12.19% | - |
| HGV_LNG_le7_5t_Euro-VI | HGV LNG <=7,5t Euro-VI | 1445160 | 0.0000% | 6.17% | 6.17% | 6.17% | 6.17% | 6.17% | 6.17% + 12.09% | - |
| HGV_LNG_gt7_5-12t_Euro-IV | HGV LNG >7,5-12t Euro-IV | 1445240 | 0.0000% | 15.59% | 16.09% | 16.63% | 15.59% | 20.04% | 16.41% + 12.53% | - |
| HGV_LNG_gt7_5-12t_Euro-V | HGV LNG >7,5-12t Euro-V | 1445250 | 0.0000% | 16.20% | 16.20% | 16.20% | 16.20% | 16.20% | 16.20% + 12.56% | - |
| HGV_LNG_gt7_5-12t_Euro-VI | HGV LNG >7,5-12t Euro-VI | 1445260 | 0.0000% | 16.45% | 16.45% | 16.45% | 16.45% | 16.45% | 16.45% + 12.34% | - |
| HGV_LNG_gt12t_Euro-IV | HGV LNG >12t Euro-IV | 1445340 | 0.0000% | 21.80% | 14.45% | 16.77% | 21.80% | 16.01% | 26.19% + 12.98% | - |
| HGV_LNG_gt12t_Euro-V | HGV LNG >12t Euro-V | 1445350 | 0.0000% | 21.40% | 21.40% | 21.40% | 21.40% | 21.40% | 21.40% + 12.59% | - |
| HGV_LNG_gt12t_Euro-VI | HGV LNG >12t Euro-VI | 1445360 | 0.0000% | 20.51% | 20.51% | 20.51% | 20.51% | 20.51% | 20.51% + 12.32% | - |
| TT_AT_LNG_Euro-IV | TT/AT LNG Euro-IV | 1445940 | 0.0000% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% + 13.12% | - |
| TT_AT_LNG_Euro-V | TT/AT LNG Euro-V | 1445950 | 0.0000% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% | 28.93% + 13.12% | - |
| TT_AT_LNG_Euro-VI_(CI) | TT/AT LNG Euro-VI (CI) | 1445960 | 0.4200% | 28.97% | 18.58% | 29.52% | 28.97% | 34.01% | 23.38% + 13.12% | - |
| TT_AT_BEV | TT/AT BEV | 1453900 | 0.0949% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 14.84% | 8.46% |
| RigidTruck_BEV_le7.5t | RigidTruck BEV <=7.5t | 1456000 | 0.4336% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 12.10% | 2.57% |
| RigidTruck_BEV_gt7.5-12t | RigidTruck BEV >7.5-12t | 1456100 | 0.0620% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 12.97% | 5.38% |
| RigidTruck_BEV_gt12t | RigidTruck BEV >12t | 1456200 | 0.0505% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 13.71% | 6.76% |
| RigidTruck_PHEV_le7_5t_Euro-VI-(El) | RigidTruck PHEV <=7,5t Euro-VI-(El) | 1473161 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 12.10% | 2.57% |
| RigidTruck_PHEV_le7_5t_Euro-VI-(D) | RigidTruck PHEV <=7,5t Euro-VI-(D) | 1473162 | 0.0000% | 12.37% | 13.08% | 8.86% | 12.37% | 37.86% | 24.34% + 12.09% | - |
| RigidTruck_PHEV_gt7_5-12t_Euro-VI-(El) | RigidTruck PHEV >7,5-12t Euro-VI-(El) | 1473261 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 12.97% | 5.38% |
| RigidTruck_PHEV_gt7_5-12t_Euro-VI-(D) | RigidTruck PHEV >7,5-12t Euro-VI-(D) | 1473262 | 0.0000% | 18.62% | 11.46% | 25.47% | 18.62% | 43.26% | 64.88% + 12.34% | - |
| RigidTruck_PHEV_gt12t_Euro-VI-(El) | RigidTruck PHEV >12t Euro-VI-(El) | 1474061 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 13.71% | 6.76% |
| RigidTruck_PHEV_gt12t_Euro-VI-(D) | RigidTruck PHEV >12t Euro-VI-(D) | 1474062 | 0.0000% | 23.09% | 23.48% | 8.57% | 23.09% | 51.83% | 32.91% + 12.64% | - |
| TT_AT_PHEV_Euro-VI-(El) | TT/AT PHEV Euro-VI-(El) | 1475961 | 0.0000% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% + 14.84% | 8.46% |
| TT_AT_PHEV_Euro-VI-(D) | TT/AT PHEV Euro-VI-(D) | 1475962 | 0.0000% | 30.46% | 18.58% | 32.00% | 30.46% | 34.01% | 23.38% + 13.12% | - |
| RigidTruck_FCEV_le7_5t | RigidTruck FCEV <=7,5t | 1483116 | 0.0000% | 0.00% | 0.00% | 0.00% | 2.57% | 0.00% | 0.00% + 12.10% | - |
| RigidTruck_FCEV_gt7_5-12t | RigidTruck FCEV >7,5-12t | 1483216 | 0.0000% | 0.00% | 0.00% | 0.00% | 5.38% | 0.00% | 0.00% + 12.97% | - |
| RigidTruck_FCEV_gt12t | RigidTruck FCEV >12t | 1484016 | 0.0000% | 0.00% | 0.00% | 0.00% | 6.76% | 0.00% | 0.00% + 13.71% | - |
| TT_AT_FCEV | TT/AT FCEV | 1485916 | 0.0000% | 0.00% | 0.00% | 0.00% | 8.46% | 0.00% | 0.00% + 14.84% | - |
