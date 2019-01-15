# Homework 6

## about Order
> the order of command "CIRGate -fanout" is match to refrence program before
> * a06fb79 - HW6: change parse flow, now we maintain io list at the same time <WildfootW>
> the difference is cause by now the gate's relationship(include _o_gate_list) is create while a gate is being create.
> By comparison, the reference code(and my code before a06fb79) maintain the _o_gate_list after all of the circuit have been read in.
> **if you want to match the order of ref program, just checkout 5fc0712(the version before a06fb79)**
code is checked back to origin code. we will used new code on final

## CIRRead
1. 讀入header
2. resize list 把所有pointer都指向 _dummy_udf_gate
3. 讀入各種閥的資料，並紀錄input gate(用**CirGate)
4. 完成output的relation ship
5. 把udf_gate各自填上

