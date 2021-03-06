/*
 * trakstar.cpp
 *
 *  Created on: Nov 05, 2013
 *      Author: Bjorn Blissing
 */

#include "trakstar.h"
#include <iostream>

TrakStar::TrakStar() : m_sensor(0), m_transmitter(0), m_initialized(false), m_numberOfSensors(0), m_numberOfTransmitters(0)
{
	initialize();
}

TrakStar::~TrakStar()
{
	shutdownTransmittor();
	delete[] m_transmitter;
	delete[] m_sensor;
};

int TrakStar::shutdownTransmittor()
{
	int id = -1;
	return SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
}

int TrakStar::initialize()
{
	int errorCode = 0;
	// Try to Initialize BIRD System
	errorCode = InitializeBIRDSystem();

	if (errorCode != BIRD_ERROR_SUCCESS) {
		std::cerr << "Error: Unable to initialize BIRD System." << std::endl;
		return errorCode;
	}

	// Get system configuration
	errorCode = GetBIRDSystemConfiguration(&m_system);

	if (errorCode != BIRD_ERROR_SUCCESS) {
		std::cerr << "Error: Unable to get system configuration of BIRD System." << std::endl;
		return errorCode;
	}

	// Get number of sensors and try to get sensor information
	m_numberOfSensors = static_cast<unsigned short>(m_system.numberSensors);
	m_sensor = new SENSOR_CONFIGURATION[m_numberOfSensors];

	for (USHORT i = 0; i < m_system.numberSensors; ++i)	{
		errorCode = GetSensorConfiguration(i, (m_sensor+i));

		if (errorCode != BIRD_ERROR_SUCCESS) {
			std::cerr << "Error: Unable to get sensor configuration for sensor id: " << i << std::endl;
			return errorCode;
		}
	}

	// Set system settings
	double pl = 50.0; // 50 Hz power line
	errorCode = SetSystemParameter(POWER_LINE_FREQUENCY, &pl, sizeof(pl));

	if (errorCode != BIRD_ERROR_SUCCESS) {
		std::cerr << "Error: Unable to get set system configuration: Power Line." << std::endl;
		return errorCode;
	}

	BOOL metric = true; // metric reporting enabled
	errorCode = SetSystemParameter(METRIC, &metric, sizeof(metric));

	if (errorCode != BIRD_ERROR_SUCCESS) {
		std::cerr << "Error: Unable to get set system configuration: Metric mode." << std::endl;
		return errorCode;
	}

	// Get transmitter configuration
	m_numberOfTransmitters = static_cast<unsigned short>(m_system.numberTransmitters);
	m_transmitter = new TRANSMITTER_CONFIGURATION[m_numberOfTransmitters];

	for (USHORT i = 0; i < m_system.numberTransmitters; ++i) {
		errorCode = GetTransmitterConfiguration(i, (m_transmitter+i));

		if (errorCode != BIRD_ERROR_SUCCESS) {
			std::cerr << "Error: Unable to get transmitter configuration for transmitter id: " << i << std::endl;
			return errorCode;
		}
	}

	// Search for first active transmitter and turn it on
	for (USHORT id = 0; id < m_system.numberTransmitters; ++id) {
		if ((m_transmitter+id)->attached)	{
			// Transmitter selection is a system function.
			// Using the SELECT_TRANSMITTER parameter we send the id of the
			// transmitter that we want to run with the SetSystemParameter() call
			errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));

			if (errorCode != BIRD_ERROR_SUCCESS) {
				std::cerr << "Error: Unable turn on transmitter id: " << id << std::endl;
				return errorCode;
			}

			break;
		}
	}

	m_initialized = true;
	return 0;
}

void TrakStar::getPosition(unsigned short sensorId, double& x, double& y, double& z)
{
	DOUBLE_POSITION_ANGLES_RECORD record;
	int errorCode = GetAsynchronousRecord(sensorId, &record, sizeof(record));

	if (errorCode != BIRD_ERROR_SUCCESS) {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	} else {
		x = record.x;
		y = record.y;
		z = record.z;
	}
}

void TrakStar::getOrientation(unsigned short sensorId, double& azimuth, double& elevation, double& roll)
{
	DOUBLE_POSITION_ANGLES_RECORD record;
	int errorCode = GetAsynchronousRecord(sensorId, &record, sizeof(record));

	if (errorCode != BIRD_ERROR_SUCCESS) {
		azimuth = 0.0;
		elevation = 0.0;
		roll = 0.0;
	} else {
		azimuth = record.a;
		elevation = record.e;
		roll = record.r;
	}
}

void TrakStar::getPositionAndOrientation(unsigned short sensorId, double& x, double& y, double& z, double& azimuth, double& elevation, double& roll)
{
	DOUBLE_POSITION_ANGLES_RECORD record;
	int errorCode = GetAsynchronousRecord(sensorId, &record, sizeof(record));

	if (errorCode != BIRD_ERROR_SUCCESS) {
		x = 0.0;
		y = 0.0;
		z = 0.0;
		azimuth = 0.0;
		elevation = 0.0;
		roll = 0.0;
	} else {
		x = record.x;
		y = record.y;
		z = record.z;
		azimuth = record.a;
		elevation = record.e;
		roll = record.r;
	}
}

void TrakStar::getPositionOrientationTime(unsigned short sensorId, double& x, double& y, double& z, double& azimuth, double& elevation, double& roll, double& time)
{
	DOUBLE_POSITION_ANGLES_TIME_STAMP_RECORD record;
	int errorCode = GetAsynchronousRecord(sensorId, &record, sizeof(record));

	if (errorCode != BIRD_ERROR_SUCCESS) {
		x = 0.0;
		y = 0.0;
		z = 0.0;
		azimuth = 0.0;
		elevation = 0.0;
		roll = 0.0;
		time = 0.0;
	} else {
		x = record.x;
		y = record.y;
		z = record.z;
		azimuth = record.a;
		elevation = record.e;
		roll = record.r;
		time = record.time;
	}
}

void TrakStar::getMeasurementRate(int& measurementRate)
{
	SYSTEM_CONFIGURATION systemConfiguration;
	int errorCode = GetBIRDSystemConfiguration(&systemConfiguration);

	if (errorCode != BIRD_ERROR_SUCCESS) {
		measurementRate = 0;
	} else {
		measurementRate = static_cast<int>(systemConfiguration.measurementRate);
	}
}