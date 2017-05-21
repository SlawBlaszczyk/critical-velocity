#ifndef ORBITCAM_H
#define ORBITCAM_H

#define NOMINMAX

#include "../QGraphicsD3D/Camera.h"

class Game;
class Vehicle;

class OrbitCam : public Camera
{
public:
//protected:
	Vehicle* targetVehicle;		//camera's target vehicle
	double targetHeight;		//height over the target where the camera will be looking at

	double orbitDistance;		//distance from camera to target projected on 'ground' plane
	double orbitHeight;			
	double orbitRotation;

public:
	OrbitCam(Game* eng, Vehicle* tgtvec);

	void MoveFB(FLOAT dist);
	void MoveLR(FLOAT dist);

	void Update();
	D3DXVECTOR3 CalculateTarget();
	void CalculatePosition();
	void UpdateOrbitRotation();

};

#endif