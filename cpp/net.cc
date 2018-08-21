#include <Eigen/Dense>

#include "matrix.h"
#include "board.h"
#include "net.h"

using namespace std;
using namespace Eigen;

matrix<float> fc0_w, fc0_b, fc1_w, fc1_b, fc2_w, fc2_b, theta;

void net_config(){
	fc0_w.load("fc0_w.dat");
	fc0_b.load("fc0_b.dat");
	fc1_w.load("fc1_w.dat");
	fc1_b.load("fc1_b.dat");
	fc2_w.load("fc2_w.dat");
	fc2_b.load("fc2_b.dat");
	theta.load("theta.dat"); 	
};

val_type board::score_net(cbool color)const{
	matrix<int> brd_mat = this->to_mat();
	matrix<float> brd_mat_f = brd_mat;
	auto data = Map<Matrix<float, 1, 128>>(brd_mat_f.table);
	auto fc0_w_m = Map<Matrix<float, 128, 64>>(fc0_w.table);
	auto fc0_b_m = Map<Matrix<float, 1, 64>>(fc0_b.table);
	auto fc1_w_m = Map<Matrix<float, 64, 32>>(fc1_w.table);
	auto fc1_b_m = Map<Matrix<float, 1, 32>>(fc1_b.table);
	auto fc2_w_m = Map<Matrix<float, 32, 16>>(fc2_w.table);
	auto fc2_b_m = Map<Matrix<float, 1, 16>>(fc2_b.table);
	auto theta_m = Map<Matrix<float, 16, 1>>(theta.table);
	val_type result;

	#define sigmoid(m) (((m).array().tanh() + 1.0f) / 2.0f).matrix()

	result = (sigmoid(sigmoid(sigmoid(data * fc0_w_m + fc0_b_m) * fc1_w_m + fc1_b_m) * fc2_w_m + fc2_b_m) * theta_m)[0];
	return result;
}
