#ifndef UKF_H
#define UKF_H

#include "measurement_package.h"
#include "Eigen/Dense"
#include <vector>
#include <string>
#include <fstream>
#include "tools.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

enum runtask
{
    RADAR,
    LIDAR,
    CAMERA,
    RADARANDLIDAR
};

class UKF {
public:

  // initially set to false, set to true in first call of ProcessMeasurement
  bool is_initialized_;

  // if this is false, laser measurements will be ignored (except for init)
  bool use_laser_;

  // if this is false, radar measurements will be ignored (except for init)
  bool use_radar_;

  // time when the state is true, in us
  long long time_us_;

  // Process noise standard deviation longitudinal acceleration in m/s^2
  double std_a_;

  // Process noise standard deviation yaw acceleration in rad/s^2
  double std_yawdd_;

  // Laser measurement noise standard deviation position1 in m
  double std_laspx_;

  // Laser measurement noise standard deviation position2 in m
  double std_laspy_;

  // Camera measurement noise standard deviation position1 in m
  double std_Caspx_;

  // Camera measurement noise standard deviation position2 in m
  double std_Caspy_;

  // Radar measurement noise standard deviation radius in m
  double std_radr_;

  // Radar measurement noise standard deviation angle in rad
  double std_radphi_;

  // Radar measurement noise standard deviation radius change in m/s
  double std_radrd_ ;

  // State dimension
  int n_x_;

  // Augmented state dimension, 增广到7位向量
  int n_aug_;

  // Number of sigma points
  int n_aug_sigma_;

  // Sigma point spreading parameter
  double lambda_;

  // the current NIS for radar
  double NIS_radar_;

  // the current NIS for laser
  double NIS_laser_;

  // the current NIS for Camera
  double NIS_Caser_;

  // Weights of sigma points
  VectorXd weights_;

  // state vector: [pos1 pos2 vel_abs yaw_angle yaw_rate] in SI units and rad
  // 原始的状态向量，分别是px,py 位置，v速度（矢量）、航向角，角速度,7X1
  // modify it to 7 dimensions [pos1 pos2 rbx, rby,vel_abs yaw_angle yaw_rate] 
  // pos1 pos2 仍然作为雷达和镭射激光的位置，同时作为camera 的左上点，扩展rbx rby作为右下数据
  VectorXd x_;

  // state covariance matrix,5X5
  MatrixXd P_;

  // predicted sigma points matrix
  // 根据Xsig_aug_更新之后，为5X15的矩阵
  MatrixXd Xsig_pred_;

  // augmented sigma points matrix
  // 这个值算出来之后，要代入运动模型当中，更新新的sigma点， 7X15的矩阵
  MatrixXd Xsig_aug_;

  runtask m_run;

  /**
   * Constructor
   */
  UKF();

  /**
   * Destructor
   */
  virtual ~UKF();

  /**
   * ProcessMeasurement
   * @param meas_package The latest measurement data of either radar or laser
   */
  void ProcessMeasurement(MeasurementPackage meas_package);

  /**
   * Initialize the state, covariance matrix and timestamp using a radar measurement
   * @param meas_package The first measurement
   */
  void Initialization(MeasurementPackage meas_package);

  /**
   * Prediction Predicts sigma points, the state, and the state covariance
   * matrix
   * @param delta_t Time between k and k+1 in s
   */
  void Prediction(double delta_t);

  /**
   * Updates the state and the state covariance matrix using a laser measurement
   * @param meas_package The measurement at k+1
   */
  void UpdateLidar(MeasurementPackage meas_package);


  /**
  * Updates the state and the state covariance matrix using a Camera measurement
  * @param meas_package The measurement at k+1
  */
  void UpdateCamera(MeasurementPackage meas_package);

  /**
   * Predict either lidar or radar measurement with given Sigma predictions
   * @param n_z The measurement dimension
   * @param Zsig The matrix for sigma points in measurement space
   * @param z_pred The predicted measurement mean
   * @param S The measurement covariance matrix
   * @param R The measurement noise covariance matrix
   */
  void PredictMeasurement(int n_z, const MatrixXd &Zsig, VectorXd &z_pred, MatrixXd &S, MatrixXd &R) ;

  /**
   * Updates the state and the state covariance matrix using a radar measurement
   * @param meas_package The measurement at k+1
   */
  void UpdateRadar(MeasurementPackage meas_package);

  /**
   * Updates the state with either lidar or radar measurement
   * @param z The measurement at k+1
   * @param z_pred The predictionof measurement at k+1
   * @param S The measurement covariance matrix
   * @param Zsig The matrix for sigma points in measurement space
   */
  void UpdateState(const VectorXd &z, const VectorXd &z_pred, const MatrixXd &S, const MatrixXd &Zsig);

  /**
   * Calculate augmented sigma points: Xsig_agu_
   */
  void AugmentSigmaPoints();


  /**
   * Predict the sigma points: Xsig_pred_
   * @param delta_t Time between k and k+1 in s
   */
  void PredictSigmaPoints(double delta_t);

  /**
   * Predict Mean and Covariance of the predicted state: x_ and P_
   */
  void PredictMeanAndCovariance();
};

#endif /* UKF_H */
