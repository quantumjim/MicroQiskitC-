/*
Comments in this file are mostly there to point out differences with the Python version of MicroQiskit, or to flag up jobs still to be done.

For comments on what everything is supposed to do, see the Python version of MicroQiskit.
https://github.com/quantumjim/MicroQiskit/blob/master/microqiskit.py
https://github.com/qiskit-community/MicroQiskit
*/

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <complex>  
#include <iostream>
#include <bitset>
#include <ctime>
#include <map>
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define ERROR(MESSAGE) my_error_handler(__FILE__, __LINE__, MESSAGE)

using namespace std;

void my_error_handler(const char* file, int line, const char* message) {
      cout << RED << message << RESET << endl;
      cout << RED << "Please review: "<< file << " line: " << line << RESET << endl;
      abort();
    } // TO DO: maybe __FILE__ and __LINE__ won't be very useful. consider removing it and making it just simply print the error message.

const int N_QUBITS_MAX = 20; // limit on qubit number
// TO DO: Remove this! It is only used in conversions to bit strings, because it doesn't like variables.



class QuantumCircuit {

  public:

    int nQubits, nBits;
    vector<vector<string>> data;

    // TO DO: Could this be done by a constructor, and still be consistent with the usage of QuantumCircuit objects in Simulator?
    void set_registers (int n, int m = 0) {
      nQubits = n;
      nBits = m;
      if(!(nQubits==nBits || nBits==0))
      {
        ERROR("Only the cases nQubits=nBits and nBits=0 are allowed in MicroQiskit");
      }
    }

    void add (QuantumCircuit qc2) {

      nBits = max(nBits,qc2.nBits);
      
      for (int g=0; g<qc2.data.size(); g++){ 
        data.push_back( qc2.data[g] );
      }
      // TO DO: It is only possible to add circuits with equal nQubits in MicroQiskit, and qc2.nBits cannot be non-zero if qc.nBits is.
      // Abort and explain if the user provides other inputs.
    }

    // TO DO: Add initialize function

    void x (int q) {
      vector<string> gate;
      gate.push_back("x");
      gate.push_back(to_string(q));
      data.push_back(gate);
    }
    void rx (double theta, int q) {
      vector<string> gate;
      gate.push_back("rx");
      gate.push_back(to_string(theta));
      gate.push_back(to_string(q));
      data.push_back(gate);
    }
    void h (int q) {
      vector<string> gate;
      gate.push_back("h");
      gate.push_back(to_string(q));
      data.push_back(gate);
    }
    void cx (int s, int t) { 
      vector<string> gate;
      gate.push_back("cx");
      gate.push_back(to_string(s));
      gate.push_back(to_string(t));
      data.push_back(gate);
    }
    void measure (int q, int b) {
      vector<string> gate;
      gate.push_back("m");
      gate.push_back(to_string(b));
      gate.push_back(to_string(q));
      data.push_back(gate);
      // TO DO: It is only possible to add measure gates of the form measure(j,j) in MicroQiskit.
      // Abort and explain if the user provides other inputs.
    }
    void rz (double theta, int q) {
      h(q);
      rx(theta,q);
      h(q);
    }
    void ry (double theta, int q) {
      rx(M_PI/2,q);
      h(q);
      rx(theta,q);
      h(q);
      rx(-M_PI/2,q);
    }
    void z ( int q) {
      rz(M_PI,q);
    }
    void y ( int q) {
      z(q);
      x(q);
    }

};

class Simulator {
  // Contains methods required to simulate a circuit and provide the desired outputs.

  vector<vector<double>> simulate (QuantumCircuit qc) {

    vector<vector<double>> ket;

    for (int j=0; j<pow(2,qc.nQubits); j++){
      vector<double> e;
      for (int k=0; k<=2; k++){
        e.push_back(0.0);
      }
      ket.push_back(e);
    }
    ket[0][0] = 1.0;


    for (int g=0; g<qc.data.size(); g++){

      if ( (qc.data[g][0]=="x") or (qc.data[g][0]=="rx") or (qc.data[g][0]=="h") ) {

        int q;
        q = stoi( qc.data[g][qc.data[g].size()-1] );

        for (int i0=0; i0<pow(2,q); i0++){
          for (int i1=0; i1<pow(2,qc.nQubits-q-1); i1++){
            int b0,b1;
            b0 = i0 + int(pow(2,q+1)) * i1;
            b1 = b0 + int(pow(2,q));

            vector<double> e0, e1;
            e0 = ket[b0];
            e1 = ket[b1];

            if (qc.data[g][0]=="x"){
              ket[b0] = e1;
              ket[b1] = e0;
            } else if (qc.data[g][0]=="rx"){
              double theta = stof( qc.data[g][1] );
              ket[b0][0] = e0[0]*cos(theta/2)+e1[1]*sin(theta/2);
              ket[b0][1] = e0[1]*cos(theta/2)-e1[0]*sin(theta/2);
              ket[b1][0] = e1[0]*cos(theta/2)+e0[1]*sin(theta/2);
              ket[b1][1] = e1[1]*cos(theta/2)-e0[0]*sin(theta/2);
            } else if (qc.data[g][0]=="h"){
              for (int k=0; k<=2; k++){
                ket[b0][k] = (e0[k] + e1[k])/sqrt(2);
                ket[b1][k] = (e0[k] - e1[k])/sqrt(2);
              }
            }

          }
        }

      } else if (qc.data[g][0]=="cx") {
        int s,t,l,h;
        s = stoi( qc.data[g][qc.data[g].size()-2] );
        t = stoi( qc.data[g][qc.data[g].size()-1] );
        if (s>t){
          h = s;
          l = t;
        } else {
          h = t;
          l = s;
        }

        for (int i0=0; i0<pow(2,l); i0++){
          for (int i1=0; i1<pow(2,h-l-1); i1++){
            for (int i2=0; i2<pow(2,qc.nQubits-h-1); i2++){

              int b0,b1;
              b0 = i0 + pow(2,l+1)*i1 + pow(2,h+1)*i2 + pow(2,s);
              b1 = b0 + pow(2,t);

              vector<double> e0, e1;
              e0 = ket[b0];
              e1 = ket[b1];

              ket[b0] = e1;
              ket[b1] = e0;

            }
          }
        }
        
      }

    }

    return ket;

  }

  vector<double> get_probs (QuantumCircuit qc) {

      // TO DO: For get_counts and get_memory (both of which call this function) the circuit should have a full set of measure gates.
      // Abort and explain if the user does not provide this input.

    vector<vector<double>> ket;
    ket = simulate(qc);

    vector<double> probs;
    for (int j=0; j<ket.size(); j++){

      probs.push_back( pow(ket[j][0],2) + pow(ket[j][1],2) );

    }

    return probs;

  }

  public:

    QuantumCircuit qc;
    int shots;

    Simulator (QuantumCircuit qc_in, int shots_in = 1024) {
      srand((unsigned)time(0));
      qc = qc_in;
      shots = shots_in;
    }

    vector<std::complex<double>> get_statevector () {
    
      vector<vector<double>> ket;
      ket = simulate(qc);

      vector<std::complex<double>> complex_ket;
      for (int j=0; j<ket.size(); j++){

        std::complex<double> e (ket[j][0],ket[j][1]);
        complex_ket.push_back( e );

      }

      return complex_ket;

    }

    vector<string> get_memory () {

      vector<double> probs;
      probs = get_probs(qc);

      vector<string> memory;

      for (int s=0; s<shots; s++){

        double cumu = 0;
        bool un = true;
        double r = double(rand())/RAND_MAX;

        for (int j=0; j<probs.size();j++){
          cumu += probs[j];
          if ((r<cumu) && un){
            std::string long_out = std::bitset<N_QUBITS_MAX>(j).to_string();
            std::string out = long_out.substr (N_QUBITS_MAX-qc.nQubits,N_QUBITS_MAX);
            memory.push_back( out );
            un = false;
          }
        }

      }

      return memory;

    }

    std::map<std::string, int> get_counts () {

      std::map<std::string, int> counts;

      vector<string> memory = get_memory();

      for (int s=0; s<shots; s++){
        counts[memory[s]] += 1;
      }

      return counts;

    }

    string get_qiskit () {

        string qiskitPy;

        if (qc.nBits==0){
          qiskitPy += "qc = QuantumCircuit("+to_string(qc.nQubits)+")\n";
        } else {
          qiskitPy += "qc = QuantumCircuit("+to_string(qc.nQubits)+","+to_string(qc.nBits)+")\n";
        }

        for (int g=0; g<qc.data.size(); g++){
            if (qc.data[g][0]=="x"){
              qiskitPy += "qc.x("+qc.data[g][1]+")\n";
            } else if (qc.data[g][0]=="rx") {
              qiskitPy += "qc.rx("+qc.data[g][1]+","+qc.data[g][2]+")\n";
            } else if (qc.data[g][0]=="h") {
              qiskitPy += "qc.h("+qc.data[g][1]+")\n";
            } else if (qc.data[g][0]=="cx") {
              qiskitPy += "qc.cx("+qc.data[g][1]+","+qc.data[g][2]+")\n";
            } else if (qc.data[g][0]=="m") {
              qiskitPy += "qc.measure("+qc.data[g][1]+","+qc.data[g][2]+")\n";
            }
        }

        return qiskitPy;

    }

};




