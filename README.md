# Fluid Simulation in C++ with SFML

This repository contains a C++ program for simulating fluid dynamics using the Simple and Fast Multimedia Library (SFML). The program implements the Navier-Stokes equations and allows for interactive manipulation of the fluid behavior.

## Features

- Fluid simulation with diffusion and advection
- Visualization of the fluid dynamics in a graphical window
- User interaction to add sources and reset the simulation

## Prerequisites

Before running the program, ensure you have the following prerequisites installed:

- C++ compiler
- SFML library

## Getting Started

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/SJuanOG/box-fluid-simulation.git
   ```

2. Compile the code using your C++ compiler. Make sure to link the SFML libraries correctly. For example, using g++:

   ```bash
   g++ Fluid.cpp -o fluid_simulation -lsfml-graphics -lsfml-window -lsfml-system
   ```

3. Run the compiled executable:

   ```bash
   ./fluid_simulation
   ```

Or simply execute the file *execution.sh*.

## Usage

- Left-click to add fluid sources.
- Right-click to reset the simulation.
- Press `Esc` to close the simulation window.

## Configuration

You can adjust the simulation parameters in the `main` function of `Fluid.cpp`, such as the grid size, diffusion, viscosity, and framerate.


## Acknowledgments

This fluid simulation is based on the Navier-Stokes equations and is intended for educational and recreational purposes. The code is inspired by various fluid simulation tutorials and resources available online. Mainly by the paper `Real-Time Fluid Dynamics for Games` from Jos Stam.
