#pragma once

#include "CommandLayer.h"
#include <iostream>
#include <iomanip>

using namespace std;

class kinovaGen2 {
public:
	kinovaGen2();
	~kinovaGen2();

	/* Setters */
	// Initialize all devices
	void initKinova();
	
	/* Runners */
	// Move device to initial position
	void moveInitPosition(int _idxDevice = 0);

	// Move finger to initial position
	void moveInitFingerPosition(int _idxDevice = 0);

	// Move device (Absolute Cartesian position)
	void moveAbsoluteCartesianPosition(float _x, float _y, float _z, float _thetaRadX, float _thetaRadY, float _thetaRadZ, int _idxDevice = 0);

	// Move device (Absolute Angular Position)
	void moveAbsoluteAngularPosition(float _deg1, float _deg2, float _deg3, float _deg4, float _deg5, float _deg6, int _idxDevice = 0);

	// Move device (Relative Cartesian position)
	void moveRelativeCartesianPosition(float _x, float _y, float _z, float _thetaRadX, float _thetaRadY, float _thetaRadZ, int _idxDevice = 0);

	// Move device (Relative angular position)
	void moveRelativeAngularPosition(float _deg1, float _deg2, float _deg3, float _deg4, float _deg5, float _deg6, int _idxDevice = 0);

	// Move finger (Absolute position)
	void moveAbsoluteFingerPosition(float _rawFinger1, float _rawFinger2, float _rawFinger3, int _idxDevice = 0);

	// Move finger (Relative positiob)
	void moveRelativeFingerPosition(float _rawFinger1, float _rawFinger2, float _rawFinger3, int _idxDevice = 0);

protected: // Functions
	int(*initAPI)();
	int(*closeAPI)();
	int(*sendBasicTrajectory)(TrajectoryPoint command);
	int(*getDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int& result);
	int(*setActiveDevice)(KinovaDevice device);
	int(*moveHome)();
	int(*initFingers)();
	int(*getAngularCommand)(AngularPosition&);
	int(*getCartesianCommand)(CartesianPosition&);

	float* getCurrentCartesianPosition();
	float* getCurrentAngularPosition();
	float* getCurrentFingerPosition();

protected:// Member variables
	int numDevice = 0;
	AngularPosition currentPosition;
	KinovaDevice deviceList[MAX_KINOVA_DEVICE];

	enum cartesianComponent { x, y, z, thetaRadX, thetaRadY, thetaRadZ };
	enum angularComponent { deg1, deg2, deg3, deg4, deg5, deg6 };
	enum fingerComponent { rawFinger1, rawFinger2, rawFinger3 };

	static const int numActuator = 6;
	static const int numFinger = 3;

protected: // Task handle
	HINSTANCE commandLayerHandle;
};