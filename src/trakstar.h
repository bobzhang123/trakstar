/*
 * trakstar.h
 *
 *  Created on: Nov 05, 2013
 *      Author: Bjorn Blissing
 */
#ifndef _TRAKSTAR_H_
#define _TRAKSTAR_H_

#include <ATC3DG.h>

class TrakStar {
	public:
		static TrakStar& instance() {
			static TrakStar instance;
			return instance;
		}

		void getPosition(unsigned short sensorId, double& x, double& y, double& z);
		void getOrientation(unsigned short sensorId, double& azimuth, double& elevation, double& roll);
		unsigned short numberOfSensors() const { return m_numberOfSensors; }
	protected:
		TrakStar();
		~TrakStar();
		TrakStar(TrakStar const&); // No copy constructor allowed for singleton
		TrakStar& operator=(TrakStar const&); // No assignment operator allowed for singleton
		int initialize();
		int shutdownTransmittor();
	private:
		SYSTEM_CONFIGURATION m_system;
		SENSOR_CONFIGURATION* m_sensor;
		TRANSMITTER_CONFIGURATION* m_transmitter;
		bool m_initialized;
		unsigned short m_numberOfSensors;
		unsigned short m_numberOfTransmitters;

};

#endif