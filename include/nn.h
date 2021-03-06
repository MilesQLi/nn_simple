
#ifndef NN_H_
#define NN_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <Eigen/Core>
#include <cassert>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include "types.h"

#ifndef SQR
#define SQR(X) ((X)*(X))
#endif

namespace neural {

  struct nn_layer 
  {
    int input_dim;
    int output_dim;
    enum ACT_TYPE activation_type;
    
    //static boost::mt19937 rng; // We could set a seed here
  
    double decay;
    bool bias_supression;

    // parameters
    // NOTE that weights and weight derivatives can be shared
    // in this case  one must ensure that the updates are only applied once
    // this is done via the is_shared_copy variable
    Eigen::VectorXd *bias;
    Eigen::MatrixXd *weights;

    Eigen::VectorXd *bias_deriv;
    Eigen::MatrixXd *weights_deriv;
    bool is_shared_copy;
    bool is_trans;
    int shared_with;

    // temporary for holding the activation 
    //Eigen::VectorXd actiation;

    nn_layer(int idim, int odim, enum ACT_TYPE type, Eigen::VectorXd *b, Eigen::VectorXd *bd, Eigen::MatrixXd *w, Eigen::MatrixXd *wd, bool shared=false, bool trans=false, const int shared_with =-1);
    // for initializing parameters
    struct nn_layer &
    reset(int idim, int odim, enum ACT_TYPE type);

    struct nn_layer &
    initRandom(boost::mt19937 &rng);
    struct nn_layer &
    setDecay(double _decay);
    void
    clearGrad();

    // actvation
    void
    forward_pass(const Eigen::VectorXd &x, Eigen::VectorXd &y);
    void
    backward_pass(Eigen::VectorXd &err, const Eigen::VectorXd &x, const Eigen::VectorXd &activation);
    void 
    calc_derivative(Eigen::VectorXd &err, const Eigen::VectorXd &x, const Eigen::VectorXd &activation, const Eigen::VectorXd dEdxi);

    void
    update(const double lrate);
    void
    update(const std::pair<Eigen::MatrixXd, Eigen::VectorXd> &delta);

    bool
    to_stream(std::ofstream &out);
    bool
    from_stream(std::ifstream &in);
      
  };

  struct nn
  {
    enum LOSS_TYPE ltype;
    std::vector<nn_layer> layers;
    std::vector<Eigen::VectorXd> activations;

    std::vector<Eigen::VectorXd*> biases;
    std::vector<Eigen::VectorXd*> biases_deriv;
    std::vector<Eigen::MatrixXd*> weights;
    std::vector<Eigen::MatrixXd*> weights_deriv;

    nn();
    nn(std::vector<int> &dimensions, std::vector<enum ACT_TYPE> &acttype, std::vector<int> &shared);
    ~nn();
    struct nn &
    initRandom(int seed = -1);
    struct nn &
    setLoss(enum LOSS_TYPE type);
    struct nn &
    setDecay(double d);
    void
    clearGrad();

    // activation
    void
    forward_pass(const Eigen::VectorXd &x, Eigen::VectorXd &y);
    void
    backward_pass(const Eigen::VectorXd &desired_y);
    
    void 
    update(const double lrate);
    void
    update(const std::vector<std::pair<Eigen::MatrixXd, Eigen::VectorXd> > &deltas);

    bool 
    to_file(const std::string &fname);
    bool 
    from_file(const std::string &fname);
    bool
    to_stream(std::ofstream &out);
    bool
    from_stream(std::ifstream &in);
  };

  // from a neural network point of view a binary svm is nothing
  // but a simple one layer nn with only one neuron trained using a hinge loss.
  // NOTE: when training an svm this way we directly optimize the primal
  //       and all performance guarantees are lost. 
  //       In practice this should however not matter most of the time
  struct svm  : nn
  {
    svm(int input_dim, enum LOSS_TYPE l, double lambda = 1e-8);
    int 
    predict(const Eigen::VectorXd &x);
  };

}


#endif
