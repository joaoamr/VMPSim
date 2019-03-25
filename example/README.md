# Input Examples

This folder contains the input examples for the programs. The program reads two files in its same folder:

## vms

This files contains the VMs of the system with the communication flow. The first line indicates the number of VMs in the system and the 
following lines the resource requirements and communication patterns of each VMs node.
At each resource line, the first integer number indicates a dimension of the VM, the second must be 1 (not used yet) and the last another VM
dimension followed by a float number that indicates the flow rate between the referenced VM of the line a the nth VM referenced by the nth colunm
For example, the file above represents a system with 4 VMs.

```
4
2 1 28 0.0 1.0 0.0 0.0
4 1 38 1.0 0.0 0.0 0.0
8 1 30 0.0 0.0 0.0 1.5
6 1 28 0.0 0.0 1.5 0.0
```

In this example we have 4 VMs, where VM-0 and VM-1 performs 1.0 of communication rate and VM-2 and VM-3 perform 1.5 of commmuncation rate.

## pms

This files stores the datacenter system configuration. The fist line is composed by an integer, that indicates the number of PMs, a string
that indicates the datacenter topology and an integer that indicates the parameter *k* of topology properties.
The following lines indicates the resource availabily of each PM. The first integer indicates a dimension, the second must be 1 (not used yet)
and the last another dimension of the PM.
For example, the file above represents a datacenter with 4 PMs in a star topology:

```
4 Star 48
16 1 1500 
32 1 4000 
64 1 3000 
128 1 4000 
```

In this example we have 4 PMs connected in a 48-Star topology. The program even supports *Fat-tree* instead of *Star*. However, be aware that
weather the number of PMs matches with *k* parameter.

# Running the program
You must specify the placement algorithm at the program call. For exemple:

```
./VMPSim [algorithm] [optional]
```

The parameter *[algorithm]* can be *ff* for *First-Fit, *ffd* for *First-Fit Decreasing*, *rr* for *Round-Robin* or *klvmp* for *KLVMP*.
The parameter *[optional]* can be *-v* for sorting PMs in decreasing order by dimensions product or *-nv* for sorting PMs in decreasing order by dimensions 
product and exchange the their position in the datacenter so that the greater VMs be as near as possible one from another.

# Output

The program output two files:

## placement

This file indicates the placement solution, for exemple:

```
0 0 1 2
```

In this case, the VM-0 is placed in PM-0, VM-1 in PM-0, VM-2 in PM-1 and VM-3 in PM-2.

## performance.txt

Informations about the objectives values and the time it took to solve the problem.
