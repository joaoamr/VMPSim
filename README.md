# VMPSim
2D Virtual Machine Placement solve tool that leverages the network traffic flow between VMs nodes.

This a tool for solvind a 2D Virtual Machine Placent network-aware problem. It receives the PMs list with the network topology and VMs with
its communication mesh, solve the problem and display two objectives values:

1. Actives PMs: 
The number of system PMs that host at least 1 PM.

2. Network Cost:
The total sum of VMs pair flow times the cost between them.

The tool has the following algorithms implemented:

1 - First-Fit

2 - Firs-Fit Decreasing

3 - Best-Fit Decreasing

4 - Round-Robin

5 - KLVMP (Kernighan-and-Lin's based heuristic for Virtual Machine Placement)
