#include <iostream>
#include "Wave.h"
//#include "camera.h"
static float PI = 3.14;

Wave::Wave() {}

Wave::Wave(float amplitude_, float steepness_, float wave_lenght_, float velocity_) {
	amplitude = amplitude_;
	steepness = steepness_;
	velocity = velocity_;
	frequencyVal = frequency(wave_lenght_);
	fiVal = fi();
	init_direction();
}

Wave::Wave(float amplitude_, float wave_lenght_, float velocity_) {
	steepness = 1.5;
	amplitude = amplitude_;
	velocity = velocity_;
	frequencyVal = frequency(wave_lenght_);
	fiVal = fi();
	init_direction();
}

void Wave::init_direction() {

	dir_x = 1.;
	dir_y = 1.;
	d_dir_x = 0.;
	d_dir_y = 0.;
}

void Wave::set_direction_vector(float x, float y, float d_x, float d_y) {
	d_dir_x = x;
	d_dir_y = y;
	dir_x = d_x;
	dir_y = d_y;
}

float Wave::f(float x, float y, float t) {
	return amplitude*pow((cos((S(x, y, t))) + 1.) / 2., steepness);
}

float Wave::d_dx(float x, float y, float t) {
	float s = S(x, y, t);
	return 1. / 2. * steepness*frequencyVal*amplitude*cos(s)*pow((cos(s) + 1.) / 2., steepness - 1.)*direction(x, y).first;
}

float Wave::d_dy(float x, float y, float t) {
	float s = S(x, y, t);
	return 1. / 2. * steepness*frequencyVal*amplitude*cos(s)*pow((cos(s) + 1.) / 2., steepness - 1.)*direction(x, y).second;
}

float Wave::frequency(float wave_length) {
	return 2. * PI / wave_length;
}

float Wave::scalar(std::pair<float, float> position, std::pair<float, float> direction) {
	return position.first*direction.first + position.second*direction.second;
}

float Wave::S(float x, float y, float t) {
	return scalar(direction(x, y), position(x, y))*frequencyVal + t*fiVal;
}

float Wave::fi() {
	return velocity * frequencyVal;
}

std::pair<float, float> Wave::direction(float x, float y) {
	return std::pair<float, float>(d_dir_x + dir_x * x, d_dir_y + dir_y * y);
//	return std::pair<float, float >((d_dir_x + dir_x * x - x) / abs((d_dir_x + dir_x * x - x)), (d_dir_y + dir_y * y)/ abs(d_dir_y + dir_y * y));
}

std::pair<float, float> Wave::position(float x, float y) {
	return std::pair<float, float>(x, y);
}

void Wave::setAmplitude(float amplitude) {
	amplitude = amplitude;
}

void Wave::setSteepness(float steepness) {
	steepness = steepness;
}

void Wave::setVelocity(float velocity) {
	velocity = velocity;
}
