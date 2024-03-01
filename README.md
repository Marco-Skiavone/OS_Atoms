# OS_Atoms
This is the project part for the OS course, year 2023-2024.

For info about the project, please check out the [assignment](20231222_Progetto_SO_DEFINITIVO_2023_24.pdf) (it is written in Italian). 

> This is made just for fun.

---
### Parameters:
> For now, i will **exlude** the 30-mark assignment part from my analysis.
- `ENERGY_DEMAND` : energy taken by the master every 1 sec
- `N_ATOMI_INIT` : initial atoms generated number  
- `N_ATOM_MAX` : max atomic number possible
- `MIN_N_ATOMICO` : minimum atomic number possible under which an atom becomes slag (english for 'scoria')
- `STEP_ATTIVATORE` : time step for activator to broadcast a signal
- `STEP_ALIMENTAZIONE` : time step (nanosec) for creating *N_NUOVI_ATOMI* new atoms
- `N_NUOVI_ATOMI` : new atoms created by power_supplier
- `SIM_DURATION` :max duration time (sec) of the simulation
- `ENERGY_EXPLODE_THRESHOLD` : energy limit over which the simulation ends in the ***explode*** termination case
---
### Termination cases:
- timeout
- explode
- blackout
- meltdown
---
### Other important details:
- Energy liberation is calculated as follow: `E(a, b) = a*b - max(a, b)`
    where **a** & **b** are the atomic numbers of the 2 atoms generated after the nuclear fission of the parent. 
