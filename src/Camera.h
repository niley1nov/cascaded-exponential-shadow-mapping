#ifndef CAMERA_H
#define CAMERA_H

//#include"Renderer.h"
#include"glm\glm.hpp"
#include"glm\gtc\matrix_transform.hpp"
#include"glm\gtc\type_ptr.hpp"
#include<vector>

enum class camera_movement
{
	FORWARD,BACKWARD,RIGHT,LEFT,WUP,WDOWN
};

const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ZOOM = 45.0f;

class Camera
{
private:
	glm::vec3 m_Position, m_Up, m_WorldUp, m_Front, m_Right;
	float m_Zoom, m_Pitch, m_Yaw, MovementSpeed, MouseSensitivity;
	void updateCameraVectors();
protected:
	static bool pause;
public:
	Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& WorldUp = glm::vec3(0.0f, 1.0f, 0.0f), const float& yaw = YAW, const float& pitch = PITCH);
	glm::mat4 GetViewMatrix();
	glm::mat4 GetLitViewMatrix(glm::vec3 frontPos);
	glm::vec3 GetPosition();
	glm::vec3 GetUp();
	glm::vec3 GetRight();
	glm::vec3 GetFront();
	float GetZoom();
	void ProcessKeyboard(const camera_movement& direction, const float& deltaTime);
	void ProcessMouseMovement(float xOffset, float yOffset, bool ConstrainPitch = true);
	void ProcessMouseScroll(float yOffset, bool doo);
};



#endif