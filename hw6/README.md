# Homework 6

## about Order
the order of command "CIRGate -fanout" is match to refrence program before
```
* a06fb79 - HW6: change parse flow, now we maintain io list at the same time <WildfootW>
```
the difference is cause by now the gate's relationship(include _o_gate_list) is create while a gate is being create.
By comparison, the reference code(and my code before a06fb79) maintain the _o_gate_list after all of the circuit have been read in.
**if you want to match the order of ref program, just checkout 5fc0712(the version before a06fb79)**
