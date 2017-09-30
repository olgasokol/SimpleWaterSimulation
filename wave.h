#pragma once
class Wave {
	float amplitude;
	float steepness;
	float velocity;
	float frequencyVal;
	float fiVal;

	float dir_x, dir_y;
	float d_dir_x, d_dir_y;

	float frequency(float wave_length);
	float scalar(std::pair<float, float> position, std::pair<float, float> direction);
	float S(float x, float y, float t);
	float fi();
	std::pair<float, float> direction(float x, float y);
	std::pair<float, float> position(float x, float y);
	void init_direction();

public:
	Wave();
	Wave(float amplitude_, float steepness_, float wave_lenght_, float velocity_);
	Wave(float amplitude_, float wave_lenght_, float velocity_);
	void set_direction_vector(float x, float y, float d_x, float d_y);
	float f(float x, float y, float t);
	float d_dx(float x, float y, float t);
	float d_dy(float x, float y, float t);
	void setAmplitude(float amplitude);
	void setSteepness(float steepness);
	void setVelocity(float velocity);
};
