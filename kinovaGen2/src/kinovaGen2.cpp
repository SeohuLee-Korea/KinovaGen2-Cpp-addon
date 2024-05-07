#include "kinovaGen2.h"

kinovaGen2::kinovaGen2() {
	commandLayerHandle = LoadLibrary(L"CommandLayerWindows.dll");

	initAPI = (int(*)()) GetProcAddress(commandLayerHandle, "InitAPI");
	getDevices = (int(*)(KinovaDevice[MAX_KINOVA_DEVICE], int&)) GetProcAddress(commandLayerHandle, "GetDevices");
	setActiveDevice = (int(*)(KinovaDevice)) GetProcAddress(commandLayerHandle, "SetActiveDevice");
	moveHome = (int(*)()) GetProcAddress(commandLayerHandle, "MoveHome");
	initFingers = (int(*)()) GetProcAddress(commandLayerHandle, "InitFingers");
	getCartesianCommand = (int(*)(CartesianPosition&)) GetProcAddress(commandLayerHandle, "GetCartesianCommand");
	getAngularCommand = (int(*)(AngularPosition&)) GetProcAddress(commandLayerHandle, "GetAngularCommand");
	sendBasicTrajectory = (int(*)(TrajectoryPoint)) GetProcAddress(commandLayerHandle, "SendBasicTrajectory");
	closeAPI = (int(*)()) GetProcAddress(commandLayerHandle, "CloseAPI");

	if (!initAPI || !getDevices || !setActiveDevice || !moveHome || !initFingers || !getCartesianCommand || !getAngularCommand || !sendBasicTrajectory || !closeAPI) {
		cerr << "Error: Fail to load Kinova library." << endl;
		return;
	}
}
kinovaGen2::~kinovaGen2() {
	(*closeAPI)();
	FreeLibrary(commandLayerHandle);
}

void kinovaGen2::initKinova() {
	int initResult = initAPI();
	numDevice = getDevices(deviceList, initResult);
	cout << "The number of connected devices: " << numDevice << endl;

	if (numDevice == 0) {
		cerr << "Error: No kinova device connected." << endl;
		return;
	}

	for (int i = 0; i < numDevice; i++) {
		cout << "Device #" << i << " found. USB serial number: " << "(" << deviceList[i].SerialNumber << ")" << endl;
		setActiveDevice(deviceList[i]); // Set device activation
		moveHome(); // Move device home
		initFingers(); // Initiate fingers
		cout << "Device #" << i << "initialization complete." << endl;
	}
}

void kinovaGen2::moveInitPosition(int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]);
	moveHome();
	cout << "Robot arm moves to initial position." << endl;
}

void kinovaGen2::moveInitFingerPosition(int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]);
	moveAbsoluteFingerPosition(0, 0, 0, _idxDevice);
	cout << "Robot fingers moves to initial position." << endl;
}

void kinovaGen2::moveAbsoluteCartesianPosition(float _x, float _y, float _z, float _thetaRadX, float _thetaRadY, float _thetaRadZ, int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]); // Set device activation with this index

	TrajectoryPoint targetPosition;
	targetPosition.InitStruct();

	// Set cartesian position mode
	targetPosition.Position.Type = CARTESIAN_POSITION; 

	// Get and set current finger position to maintain fingers during arm movement
	float* currentFingerPosition = new float[numFinger];
	currentFingerPosition = getCurrentFingerPosition();

	targetPosition.Position.Fingers.Finger1 = currentFingerPosition[rawFinger1];
	targetPosition.Position.Fingers.Finger2 = currentFingerPosition[rawFinger2];
	targetPosition.Position.Fingers.Finger3 = currentFingerPosition[rawFinger3];

	// Set new position
	targetPosition.Position.CartesianPosition.X = _x;
	targetPosition.Position.CartesianPosition.Y = _y;
	targetPosition.Position.CartesianPosition.Z = _z;
	targetPosition.Position.CartesianPosition.ThetaX = _thetaRadX;
	targetPosition.Position.CartesianPosition.ThetaY = _thetaRadY;
	targetPosition.Position.CartesianPosition.ThetaZ = _thetaRadZ;

	// Send target position to the robot
	sendBasicTrajectory(targetPosition);

	// Free memory
	delete[] currentFingerPosition;

	cout << "Robot arm moves (Absolute cartesian position)." << endl;
}

void kinovaGen2::moveAbsoluteAngularPosition(float _deg1, float _deg2, float _deg3, float _deg4, float _deg5, float _deg6, int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]); // Set device activation with this index

	TrajectoryPoint targetPosition;
	targetPosition.InitStruct();

	// Set angular position mode
	targetPosition.Position.Type = ANGULAR_POSITION; // Set angular position mode

	// Get and set current finger position to maintain fingers during arm movement
	float* currentFingerPosition = new float[numFinger];
	currentFingerPosition = getCurrentFingerPosition();

	targetPosition.Position.Fingers.Finger1 = currentFingerPosition[rawFinger1];
	targetPosition.Position.Fingers.Finger2 = currentFingerPosition[rawFinger2];
	targetPosition.Position.Fingers.Finger3 = currentFingerPosition[rawFinger3];

	// Set new position
	targetPosition.Position.Actuators.Actuator1 = _deg1;
	targetPosition.Position.Actuators.Actuator2 = _deg2;
	targetPosition.Position.Actuators.Actuator3 = _deg3;
	targetPosition.Position.Actuators.Actuator4 = _deg4;
	targetPosition.Position.Actuators.Actuator5 = _deg5;
	targetPosition.Position.Actuators.Actuator6 = _deg6;

	// Send target position to the robot
	sendBasicTrajectory(targetPosition);

	// Free memory
	delete[] currentFingerPosition;

	cout << "Robot arm moves (Absolute angular position)." << endl;
}

void kinovaGen2::moveRelativeCartesianPosition(float _x, float _y, float _z, float _thetaRadX, float _thetaRadY, float _thetaRadZ, int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]); // Set device activation with this index

	TrajectoryPoint targetPosition;
	targetPosition.InitStruct();

	// Set cartesian position mode
	targetPosition.Position.Type = CARTESIAN_POSITION;

	// Get and set current finger position to maintain fingers during arm movement
	float* currentFingerPosition = new float[numFinger];
	currentFingerPosition = getCurrentFingerPosition();

	targetPosition.Position.Fingers.Finger1 = currentFingerPosition[rawFinger1];
	targetPosition.Position.Fingers.Finger2 = currentFingerPosition[rawFinger2];
	targetPosition.Position.Fingers.Finger3 = currentFingerPosition[rawFinger3];

	// Get current cartesian position for relative movement
	float* currentCartesianPosition = new float[numActuator];
	currentCartesianPosition = getCurrentCartesianPosition();

	// Set new position
	targetPosition.Position.CartesianPosition.X = currentCartesianPosition[x] + _x;
	targetPosition.Position.CartesianPosition.Y = currentCartesianPosition[y] + _y;
	targetPosition.Position.CartesianPosition.Z = currentCartesianPosition[z] + _z;
	targetPosition.Position.CartesianPosition.ThetaX = currentCartesianPosition[thetaRadX] + _thetaRadX;
	targetPosition.Position.CartesianPosition.ThetaY = currentCartesianPosition[thetaRadY] + _thetaRadY;
	targetPosition.Position.CartesianPosition.ThetaZ = currentCartesianPosition[thetaRadZ] + _thetaRadZ;

	// Send target position to the robot
	sendBasicTrajectory(targetPosition);

	// Free memory
	delete[] currentFingerPosition;
	delete[] currentCartesianPosition;

	cout << "Robot arm moves (Relative cartesian position)." << endl;
}

void kinovaGen2::moveRelativeAngularPosition(float _deg1, float _deg2, float _deg3, float _deg4, float _deg5, float _deg6, int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]); // Set device activation with this index

	TrajectoryPoint targetPosition;
	targetPosition.InitStruct();

	// Set angular position mode
	targetPosition.Position.Type = ANGULAR_POSITION; // Set angular position mode

	// Get and set current finger position to maintain fingers during arm movement
	float* currentFingerPosition = new float[numFinger];
	currentFingerPosition = getCurrentFingerPosition();

	targetPosition.Position.Fingers.Finger1 = currentFingerPosition[rawFinger1];
	targetPosition.Position.Fingers.Finger2 = currentFingerPosition[rawFinger2];
	targetPosition.Position.Fingers.Finger3 = currentFingerPosition[rawFinger3];

	// Get current angular position for relative movement
	float* currentAngularPosition = new float[numActuator];
	currentAngularPosition = getCurrentAngularPosition();

	// Set new position
	targetPosition.Position.Actuators.Actuator1 = currentAngularPosition[deg1] + _deg1;
	targetPosition.Position.Actuators.Actuator2 = currentAngularPosition[deg2] + _deg2;
	targetPosition.Position.Actuators.Actuator3 = currentAngularPosition[deg3] + _deg3;
	targetPosition.Position.Actuators.Actuator4 = currentAngularPosition[deg4] + _deg4;
	targetPosition.Position.Actuators.Actuator5 = currentAngularPosition[deg5] + _deg5;
	targetPosition.Position.Actuators.Actuator6 = currentAngularPosition[deg6] + _deg6;

	// Send target position to the robot
	sendBasicTrajectory(targetPosition);

	// Free memory
	delete[] currentFingerPosition;
	delete[] currentAngularPosition;

	cout << "Robot arm moves (Relative Angular position)." << endl;
}

void kinovaGen2::moveAbsoluteFingerPosition(float _rawFinger1, float _rawFinger2, float _rawFinger3, int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]); // Set device activation with this index

	TrajectoryPoint targetPosition;
	targetPosition.InitStruct();

	// Note: Although cartesian position is used here, fnger position is not affected by POSITION_TYPE.  
	targetPosition.Position.Type = CARTESIAN_POSITION;

	// Get and set current cartesian position to maintain arm during finger movement
	float* currentCartesianPosition = new float[numActuator];
	currentCartesianPosition = getCurrentCartesianPosition();

	targetPosition.Position.CartesianPosition.X = currentCartesianPosition[x];
	targetPosition.Position.CartesianPosition.Y = currentCartesianPosition[y];
	targetPosition.Position.CartesianPosition.Z = currentCartesianPosition[z];
	targetPosition.Position.CartesianPosition.ThetaX = currentCartesianPosition[thetaRadX];
	targetPosition.Position.CartesianPosition.ThetaY = currentCartesianPosition[thetaRadY];
	targetPosition.Position.CartesianPosition.ThetaZ = currentCartesianPosition[thetaRadZ];


	// Set new finger position
	targetPosition.Position.Fingers.Finger1 = _rawFinger1;
	targetPosition.Position.Fingers.Finger2 = _rawFinger2;
	targetPosition.Position.Fingers.Finger3 = _rawFinger3;

	// Send target position to the robot
	sendBasicTrajectory(targetPosition);

	// Free memory
	delete[] currentCartesianPosition;

	cout << "Robot fingers moves to the specific position." << endl;
	
}

void kinovaGen2::moveRelativeFingerPosition(float _rawFinger1, float _rawFinger2, float _rawFinger3, int _idxDevice) {
	setActiveDevice(deviceList[_idxDevice]); // Set device activation with this index

	TrajectoryPoint targetPosition;
	targetPosition.InitStruct();

	// Note: Although cartesian position is used here, fnger position is not affected by POSITION_TYPE.  
	targetPosition.Position.Type = CARTESIAN_POSITION;

	// Get and set current cartesian position to maintain arm during finger movement
	float* currentCartesianPosition = new float[numActuator];
	currentCartesianPosition = getCurrentCartesianPosition();

	targetPosition.Position.CartesianPosition.X = currentCartesianPosition[x];
	targetPosition.Position.CartesianPosition.Y = currentCartesianPosition[y];
	targetPosition.Position.CartesianPosition.Z = currentCartesianPosition[z];
	targetPosition.Position.CartesianPosition.ThetaX = currentCartesianPosition[thetaRadX];
	targetPosition.Position.CartesianPosition.ThetaY = currentCartesianPosition[thetaRadY];
	targetPosition.Position.CartesianPosition.ThetaZ = currentCartesianPosition[thetaRadZ];

	// Get current finger position for relative movement
	float* currentFingerPosition = new float[numFinger];
	currentFingerPosition = getCurrentFingerPosition();

	// Set new finger position
	targetPosition.Position.Fingers.Finger1 = currentFingerPosition[rawFinger1] + _rawFinger1;
	targetPosition.Position.Fingers.Finger2 = currentFingerPosition[rawFinger2] + _rawFinger2;
	targetPosition.Position.Fingers.Finger3 = currentFingerPosition[rawFinger3] + _rawFinger3;

	// Send target position to the robot
	sendBasicTrajectory(targetPosition);

	// Free memory
	delete[] currentCartesianPosition;
	delete[] currentFingerPosition;

	cout << "Robot fingers moves to the specific position." << endl;
}



//------------------------------------------------------------------------------------
float* kinovaGen2::getCurrentCartesianPosition() {
	CartesianPosition currentCartesianPosition;
	getCartesianCommand(currentCartesianPosition);

	float* flt_currentCartesianPosition = new float[numActuator];
	flt_currentCartesianPosition[x] = currentCartesianPosition.Coordinates.X;
	flt_currentCartesianPosition[y] = currentCartesianPosition.Coordinates.Y;
	flt_currentCartesianPosition[z] = currentCartesianPosition.Coordinates.Z;
	flt_currentCartesianPosition[thetaRadX] = currentCartesianPosition.Coordinates.ThetaX;
	flt_currentCartesianPosition[thetaRadY] = currentCartesianPosition.Coordinates.ThetaY;
	flt_currentCartesianPosition[thetaRadZ] = currentCartesianPosition.Coordinates.ThetaZ;

	return flt_currentCartesianPosition;
}

float* kinovaGen2::getCurrentAngularPosition() {
	AngularPosition currentAngularPosition;
	getAngularCommand(currentAngularPosition);

	float* flt_currentAngularPosition = new float[numActuator];
	flt_currentAngularPosition[deg1] = currentAngularPosition.Actuators.Actuator1;
	flt_currentAngularPosition[deg2] = currentAngularPosition.Actuators.Actuator2;
	flt_currentAngularPosition[deg3] = currentAngularPosition.Actuators.Actuator3;
	flt_currentAngularPosition[deg4] = currentAngularPosition.Actuators.Actuator4;
	flt_currentAngularPosition[deg5] = currentAngularPosition.Actuators.Actuator5;
	flt_currentAngularPosition[deg6] = currentAngularPosition.Actuators.Actuator6;

	return flt_currentAngularPosition;
}

float* kinovaGen2::getCurrentFingerPosition() {
	// Finger position is the same in cartesian or angular position in Kinova API.
	// Cartesian position is used here, but angular position can also be used
	CartesianPosition currentCartesianPosition;
	getCartesianCommand(currentCartesianPosition);

	float* flt_currentFingerPosition = new float[numFinger];
	flt_currentFingerPosition[rawFinger1] = currentCartesianPosition.Fingers.Finger1;
	flt_currentFingerPosition[rawFinger2] = currentCartesianPosition.Fingers.Finger2;
	flt_currentFingerPosition[rawFinger3] = currentCartesianPosition.Fingers.Finger3;

	return flt_currentFingerPosition;
}