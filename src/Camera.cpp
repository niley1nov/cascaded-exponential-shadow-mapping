#include"Camera.h"

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_Pitch))*cos(glm::radians(m_Yaw));
	front.y = sin(glm::radians(m_Pitch));
	front.z = cos(glm::radians(m_Pitch))*sin(glm::radians(m_Yaw));
	m_Front = glm::normalize(front);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

Camera::Camera(const glm::vec3& position, const glm::vec3& WorldUp, const float& yaw, const float& pitch)
	:MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), m_Zoom(ZOOM), m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	m_Position = position;
	m_WorldUp = WorldUp;
	m_Yaw = yaw;
	m_Pitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}
glm::mat4 Camera::GetLitViewMatrix(glm::vec3 frontPos)
{
	return glm::lookAt(m_Position, frontPos, m_Up);
}
glm::vec3 Camera::GetPosition()
{
	return m_Position;
}
glm::vec3 Camera::GetUp()
{
	return m_Up;
}
glm::vec3 Camera::GetRight()
{
	return m_Right;
}
glm::vec3 Camera::GetFront()
{
	return m_Front;
}
float Camera::GetZoom()
{
	return m_Zoom;
}
void Camera::ProcessKeyboard(const camera_movement& direction, const float& deltaTime)
{
	if (!pause)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == camera_movement::FORWARD)
			m_Position += m_Front * velocity;
		else if (direction == camera_movement::BACKWARD)
			m_Position -= m_Front * velocity;
		else if (direction == camera_movement::RIGHT)
			m_Position += m_Right * velocity;
		else if (direction == camera_movement::LEFT)
			m_Position -= m_Right * velocity;
		else if (direction == camera_movement::WUP)
			m_Position += m_WorldUp * velocity;
		else if (direction == camera_movement::WDOWN)
			m_Position -= m_WorldUp * velocity;
	}
}
void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool ConstrainPitch)
{
	xOffset *= MouseSensitivity;
	yOffset *= MouseSensitivity;

	m_Yaw += xOffset;
	m_Pitch += yOffset;

	if (ConstrainPitch)
	{
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		else if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}
	updateCameraVectors();
}
void Camera::ProcessMouseScroll(float yOffset, bool doo)
{
	if (!doo)
		return;
	if (m_Zoom >= 1.0f && m_Zoom <= 45.0f)
		m_Zoom -= yOffset;
	if (m_Zoom <= 1.0f)
		m_Zoom = 1.0f;
	else if (m_Zoom >= 45.0f)
		m_Zoom = 45.0f;
}

bool Camera::pause = false;