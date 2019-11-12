#include <iostream>
#include "MicroQiskitCpp.h"

int main () {

  // initialize a circuit with two qubits
  QuantumCircuit qc;
  qc.set_registers(2);  
  // add the gates to create a Bell pair
  qc.h(0);
  qc.cx(0,1);

  // initialize another circuit with two qubits and two output bits
  QuantumCircuit meas;
  meas.set_registers(2,2);
  // add the measurements
  meas.measure(0,0);
  meas.measure(1,1);

  // add the measurement circuit to the end of the original circuit
  qc.add(meas);

  // simulate the circuit
  Simulator result (qc);

  // we can use the result to see what the Qiskit syntax would be
  cout << "\nThis circuit would be expressed in Qiskit as:" << endl;
  cout << result.get_qiskit() << endl;

  // and also to get the counts
  std::map<std::string, int> counts = result.get_counts();
  cout << "Results are:" << endl;
  cout << "Counts for 00 = " << counts["00"] << "\n";
  cout << "Counts for 01 = " << counts["01"] << "\n";
  cout << "Counts for 10 = " << counts["10"] << "\n";
  cout << "Counts for 11 = " << counts["11"] << "\n";

  // we can also get the statevector (as a vector<vector<double>>)
  // and the memory (vector<string>)

  return 0;
}