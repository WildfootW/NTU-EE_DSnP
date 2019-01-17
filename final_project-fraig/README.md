# data structure
## class CirGate
base class to represent a logic gate
data members:
* 'symbolic_name': gate symbol
* '_i_gate_list', '_o_gate_list': input and output list (vector<RelatedGate>)
* '_variable_id': store variable id
* '_line_no': gate declare in which line in aag file
* 'visited': for various dfs functions (multable)
* 'visited_ref': for various dfs functoins (static)

inheritance by following class
* class CONSTGate
* class AIGGate
* class PIGate
* class POGate
* class UNDEFGate

## class RelatedGate
store the one way edge between two gates
data members:
* 'value': (size_t) it save the pointer to other CirGate and store whether it is inverted in it's LSB

## class CirMgr
meneger a circuit of CirGate, and provide a interface can be called by CirCmd
data members:
* 'comments': store circuit comments
* '_gate_list': (vector<CirGate*>) store all the gates in circuit
* '_header_M,I,L,O,A': store the number like aag header represent
* '_pi_list', '_po_list': (vector<unsigned>) PI/PO gates' variable id
* '_simLog': ofstream*

# workflow
## CIRRead
1. read in aag file header and resize _gate_list for gates
2. fill up the list with UNDEFGate
3. read in gates in aag file. connect the gates.
4. new a correct type gate and copy the i/o list
5. replace the RelatedGate from other connected gate with new gate
6. delete the origin UNDEF gate
7. place the new gate in the _gate_list

## CIRSweep
1. find the gate that is not using.
2. new a UNDEFGate and modify the connection from other gates to the new gate
3. delete the origin gate
4. place the new gate in the _gate_list
5. go back to 1

## CIROptimize
1. get the dfs list
2. find aig gate in the list
3. get the input RelatedGate as ia, ib.
4. optimize the aig gate with following situation, replace relationship in "the aig gate"s output gate with new relation

* two input equal and both are (not) inverted => new relation is input gate, (not) inverted.
* two input equal and have difference invert status => new relation is constant false
* one gate is constant 0 => new relation is constant 0
* one gate is constant 1 => new relation is another input gate, whether another input gate is inverted

5. replace the relationship in the output gates's _i_gate_list with new relationship ([note] if origin relationship is inverted, reverse the new_relaton's inverted status)
6. create new UNDEF gate
7. replace the relationship in the input gate's _o_gate_list with new UDF gate
8. delete origin gate and replace it's place in _gate_list with new UDF gate
