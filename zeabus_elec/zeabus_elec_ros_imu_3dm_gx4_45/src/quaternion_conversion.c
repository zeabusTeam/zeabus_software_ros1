static void toQuaternion(Quaterniond& q, double pitch, double roll, double yaw)
{
	double t0 = std::cos(yaw * 0.5);
	double t1 = std::sin(yaw * 0.5);
	double t2 = std::cos(roll * 0.5);
	double t3 = std::sin(roll * 0.5);
	double t4 = std::cos(pitch * 0.5);
	double t5 = std::sin(pitch * 0.5);

	q.w() = t0 * t2 * t4 + t1 * t3 * t5;
	q.x() = t0 * t3 * t4 - t1 * t2 * t5;
	q.y() = t0 * t2 * t5 + t1 * t3 * t4;
	q.z() = t1 * t2 * t4 - t0 * t3 * t5;
	return q;
}

static void toEulerianAngle(const Quaterniond& q, double& roll, double& pitch, double& yaw)
{
	double ysqr = q.y() * q.y();

	// roll (x-axis rotation)
	double t0 = +2.0 * (q.w() * q.x() + q.y() * q.z());
	double t1 = +1.0 - 2.0 * (q.x() * q.x() + ysqr);
	roll = std::atan2(t0, t1);

	// pitch (y-axis rotation)
	double t2 = +2.0 * (q.w() * q.y() - q.z() * q.x());
	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;
	pitch = std::asin(t2);

	// yaw (z-axis rotation)
	double t3 = +2.0 * (q.w() * q.z() + q.x() * q.y());
	double t4 = +1.0 - 2.0 * (ysqr + q.z() * q.z());  
	yaw = std::atan2(t3, t4);
}