#include "OrbitCam.h"
#include "Vehicle.h"
#include "QMath.h"
#include "Frame.h"
#include "TimeManager.h"
#include <cmath>

OrbitCam::OrbitCam(Game* eng, Vehicle* tgtvec) : Camera(eng)
{
	targetVehicle = tgtvec;
	targetHeight = 1.0f;
	
	orbitDistance = 20.0f;
	orbitHeight = 1.5f;
	orbitRotation = 0.0f;
}

void OrbitCam::MoveFB(FLOAT dist)
{

}

void OrbitCam::MoveLR(FLOAT dist)
{

}

void OrbitCam::CalculatePosition()
{
	////get a vector with camera distance and height values
	//D3DXVECTOR3 tmp ((float)orbitDistance, (float)orbitHeight, 0);

	////rotate vector by orbital rotation
	//D3DXMATRIX rot;
	//D3DXVECTOR4 out;
	//D3DXMatrixRotationY(&rot, (float)orbitRotation);
	//D3DXVec3Transform(&out, &tmp, &rot);

	//	//add vehicle position
	//NxVec3 vpos = targetVehicle->actor->getGlobalPosition();
	//Position = D3DXVECTOR3(vpos.x + out.x, vpos.y + out.y, vpos.z + out.z);

	/////////////////////////////////////////////////////////////////////
	//ANOTHER

	//NxVec3 velo = targetVehicle->actor->getLinearVelocity();
	//velo.y = 0.0f;
	//velo.normalize();

	////subtract from vehicle position
	//NxVec3 vpos = targetVehicle->actor->getGlobalPosition();
	//vpos = vpos - (velo * orbitDistance);
	//
	//vpos.y += orbitHeight;
	//
	//Position = D3DXVECTOR3(vpos.x, vpos.y, vpos.z);

	//NEW STYLE CALCULATION
	D3DXVECTOR3 front (1, 0, 0);

	//get car transform matrix
	//D3DXMATRIX rot = targetVehicle->Frames[CAR_BODY]->TransformMatrix;
	////cut translation parameters to get rotation only matrix
	//rot._14 = 0.0f;
	//rot._24 = 0.0f;
	//rot._34 = 0.0f;
	D3DXMATRIX rot, rot2;
	NxMat33 rotx = targetVehicle->actor->getGlobalOrientation();

	//Full orientation copy
	NxF32 srcmat [3][3];
	rotx.getColumnMajor(srcmat);

	rot._11 = srcmat[0][0];
	rot._12 = srcmat[0][1];
	rot._13 = srcmat[0][2];
	rot._14 = 0;

	rot._21 = srcmat[1][0];
	rot._22 = srcmat[1][1];
	rot._23 = srcmat[1][2];
	rot._24 = 0;

	rot._31 = srcmat[2][0];
	rot._32 = srcmat[2][1];
	rot._33 = srcmat[2][2];
	rot._34 = 0;

	rot._41 = 0;
	rot._42 = 0;
	rot._43 = 0;
	rot._44 = 0;

	//transform front vector through rotation matrix
	D3DXVECTOR4 vehicle_front4;
	//D3DXVec3Transform(&vehicle_front4, &front, &rot);


	//unrotated vector
	D3DXVec3Transform(&vehicle_front4, &front, &rot);
	D3DXVECTOR3 vehicle_front_clear (vehicle_front4.x, 0, vehicle_front4.z);
	D3DXVec3Normalize(&vehicle_front_clear, &vehicle_front_clear);

	//get angle between rotated and front
	float a = acos(D3DXVec3Dot(&front, &vehicle_front_clear));

	//modify orbit rotation angle
	if (a > 0.05f )
	{
		orbitRotation -= 0.4f * engine->tman->GetDeltaTime();// * sin(a);
	}
	else if (a < -0.05f )
	{
		orbitRotation += 0.4f * engine->tman->GetDeltaTime();// * sin(a);
	}

	//transform by orbit rotate matrix
	D3DXMatrixRotationY(&rot2, orbitRotation);
	D3DXMatrixMultiply(&rot2, &rot, &rot2);
	//vehicle_front4.x = front.x;
	//vehicle_front4.y = front.y;
	//vehicle_front4.z = front.z;
	D3DXVec3Transform(&vehicle_front4, &front, &rot2);

	//cut y coordinate and normalize
	D3DXVECTOR3 vehicle_front_rot (vehicle_front4.x, 0, vehicle_front4.z);
	D3DXVec3Normalize(&vehicle_front_rot, &vehicle_front_rot);

	//add to vehicle position
	/*NxVec3 nxvf (vehicle_front_rot.x, vehicle_front_rot.y, vehicle_front_rot.z); */
	NxVec3 nxvf (vehicle_front_clear.x, vehicle_front_clear.y, vehicle_front_clear.z); 
	NxVec3 vpos = targetVehicle->actor->getGlobalPosition();
	vpos = vpos + (nxvf * orbitDistance);


	vpos.y += orbitHeight;
	
	Position = D3DXVECTOR3(vpos.x, vpos.y, vpos.z);
}

D3DXVECTOR3 OrbitCam::CalculateTarget()
{
	CalculatePosition();

	//subtract camera position vector from vehicle position + height, and normalize

	//vehicle position + height
	D3DXVECTOR3 vph (targetVehicle->actor->getGlobalPosition().x, 
		targetVehicle->actor->getGlobalPosition().y + (float)targetHeight,
		targetVehicle->actor->getGlobalPosition().z);

	//subtract from position, getting unnormalized target vec
	//D3DXVECTOR3 tv = Position - vph;
	D3DXVECTOR3 tv = vph - Position;

	//normaliza target vector
	D3DXVec3Normalize(&tv, &tv);
	Target = tv;

	return Target;
}

void OrbitCam::UpdateOrbitRotation()
{
	//get the y rotation angle of the car
	float car_angle;

	NxQuat carq;
	NxMat33 mat = targetVehicle->actor->getGlobalOrientation();
	mat.toQuat(carq);

	//carq.

	carq = targetVehicle->actor->getGlobalOrientationQuat();
	carq.getAngleAxis(car_angle, NxVec3(0, 1, 0));
	car_angle = QMath::DegToRad(car_angle);
	car_angle *= (-1.0f);

	orbitRotation = car_angle;

	////rotate the camera according tho car angle
	//float a = car_angle - orbitRotation;
	//if (a > 0.05f )
	//{
	//	orbitRotation += 0.4f * engine->tman->GetDeltaTime() * sin(a);
	//}
	//else if (a < -0.05f )
	//{
	//	orbitRotation -= 0.4f * engine->tman->GetDeltaTime() * sin(a);
	//}

		//get the y rotation angle of the car


	//float car_angle;

	//NxQuat carq;
	//NxMat33 mat = targetVehicle->actor->getGlobalOrientation();
	//mat.toQuat(carq);

	////carq.

	////carq = targetVehicle->actor->getGlobalOrientationQuat();
	//carq.getAngleAxis(car_angle, NxVec3(0, 1, 0));
	//car_angle = QMath::DegToRad(car_angle);
	////car_angle *= (-1.0f);

	////rotate the camera according tho car angle
	//float a = car_angle - orbitRotation;
	//if (a > 0.02f )
	//{
	//	orbitRotation += 2.0f * engine->tman->GetDeltaTime() * sin(a);
	//}
	//else if (a < -0.02f )
	//{
	//	orbitRotation -= 2.0f * engine->tman->GetDeltaTime() * sin(a);
	//}
}