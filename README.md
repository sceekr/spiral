# SPIRAL

**SPIRAL** (**SP**ICE-based **IR**radiation **A**nalysis for **L**ayouts) is a software suite for simulating and analyzing radiation effects on integrated circuits, from physical layout to circuit-level behavior using particle-matter interactions and SPICE simulations.

## Features

- 📐 **GDSII and OASIS Based Layout Parsing**  
  Extracts physical and volumetric information from IC layouts.
  *Based on gdstk.*

- ☢️ **Particle-Matter Interaction Modeling**  
  Simulates ionizing radiation interactions within semiconductor materials.
  *Based on the Geant4 platform.*

- ⚡ **SPICE-Based Simulation Engine**  
  Injects radiation-induced effects (e.g., current responses) into SPICE simulations.
  Results are parsed so as to benchmark the IC's robusteness to SEEs.

- 📊 **Post-Processing and Visualization**  
  Provides analysis of common figures of merit (e.g., soft error rates, critical nodes, cross-sections and sensitivity maps).

## Use Cases

- RHA (Radiation Hardness Assurance) for aerospace and defense ICs
- Design verification under high-energy particle exposure
- Layout-level identification of sensitive areas
- Research and development of hardened-by-design techniques
- Reduce RH IC design costs by reducing the amount of beam time required in the process

## Getting Started

### Prerequisites

- blahblah
- bleh

### Building

```bash
git clone --recurse-submodules https://github.com/sceeker/spiral.git
cd spiral
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Basic usage

```bash
./spiral --help
```

```bash
./spiral --gds path/to/layout.gds --netlist path/to/circuit.sp --chier-dans-le-cul
```

## License

LGPL v3

## Authors

- Sceeker
- Contributions welcome!