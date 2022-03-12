# laplacelib
A small library to simulate laplace-transfer-systems. Suitable to analyze complete controler
It simply calulates the derivatives of input and output and calculates direct-form 1

-First the individual components are initialized
-Then the components get interconnected
-In a loop all components are called with magic()

Take care.
-The code is increddibly slow
-The code is quite questionable in a few places
-Its not really usefull outside of my own education

There is a Makefile with lib creation and example

The example simulates a PT4 system controlled to 1 with a PID Controler with output limiter

At the moment the graph is a simple command line plot. And I guess it will stay that way
