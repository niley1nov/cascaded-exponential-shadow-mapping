#include"Renderer.h"
#include<iostream>
#include"Shader_s.h"
#include"VertexBuffer.h"
#include"VertexBufferLayout.h"
#include"VertexArray.h"
#include"Texture.h"
#include"Camera.h"
#include"FrameBuffer.h"
#include"UniformBuffer.h"
#include"imgui\imgui.h"
#include"imgui\imgui_impl_glfw_gl3.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float lastX;
float lastY;
float lastTime = 0.0f, deltaTime = 0.0f;
bool firstTime = true;
const float _NEAR = 0.1f;
const float _FAR = 22.0f;

void framebuffer_size_callback(GLFWwindow*, int, int);

struct uniBuf
{
	glm::mat4 projView;
	glm::mat4 lightSpaceMat[3];
	glm::vec3 m_viewPos;
	float m_artifact;
	glm::vec3 m_ratio;
	float m_cascade;
	glm::vec3 m_gCascadeEndClipSpace;
	float pad3;
	glm::vec3 dirColor;
	float pad4;
};

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
	if (!glfwInit())
	{
		std::cout << "glfw can't be initialized" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "AlooMatar", NULL, NULL);
	if (!window)
	{
		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glCullFace(GL_BACK));
	GLCall(glFrontFace(GL_CCW));
	GLCall(glEnable(GL_MULTISAMPLE));
	{
		glm::vec3 cubePositions[] = {
			glm::vec3(3.0f,  -2.5f,  1.0f),
			glm::vec3(3.0f,  -2.5f, -5.0f),
			glm::vec3(-3.0f, -2.5f, 1.0f),
			glm::vec3(-3.0f, -2.5f, -5.0f),
			glm::vec3(0.8f, -1.5f, -4.0f),
			glm::vec3(2.0f, 0.0f, -2.0f),
			glm::vec3(-2.0f, 0.0f, -2.0f),
			glm::vec3(0.0f, 0.0f, -4.0f),
			glm::vec3(0.0f,  0.0f, 0.0f),
			glm::vec3(0.0f,  2.0f, -2.0f)
		};
		glm::mat4 *boxModelMats;
		boxModelMats = new glm::mat4[10];
		for (unsigned int i = 0; i < 10; ++i)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			boxModelMats[i] = model;
		}

		glm::vec3 dirLight[5] = {
			glm::vec3(2.0f, -4.0f, 1.0f),
			glm::vec3(0.2f),
			glm::vec3(0.6f),
			glm::vec3(1.0f)
		};

		FrameBuffer fb(SCR_WIDTH, SCR_HEIGHT,4,0);
		FrameBuffer screenFB(SCR_WIDTH, SCR_HEIGHT, 0);

		VertexArray planeVA;
		VertexBuffer planeVB("planeVertices");
		{
			VertexBufferLayout planeVL;
			planeVL.push<float>(2);
			planeVA.AddBuffNLaySub(planeVB, planeVL);
			planeVB.Unbind();
			planeVA.Unbind();
		}

		VertexArray quadVA;
		VertexBuffer quadVB("quadVertices");
		{
			VertexBufferLayout quadVL;
			quadVL.push<float>(2);
			quadVL.push<float>(2);
			quadVA.AddBuffNLaySub(quadVB, quadVL);
			quadVB.Unbind();
			quadVA.Unbind();
		}

		VertexArray va;
		VertexBuffer vb("vertices");
		VertexBuffer InstanceVB(boxModelMats, 10 * sizeof(glm::mat4));
		{
			VertexBufferLayout vl;
			VertexBufferLayout Ivl;
			vl.push<float>(3);
			vl.push<float>(3);
			vl.push<float>(2);
			for (unsigned int i = 0; i < 4; ++i)
				Ivl.push<float>(4);
			va.AddBuffNLaySub(vb, vl);
			va.AddBuffNLay(InstanceVB, Ivl);
			vb.Unbind();
			InstanceVB.Unbind();
			va.Unbind();
		}

		VertexArray shadBoxVA;
		VertexBuffer shadBoxVB("ShadVertices");
		VertexBuffer InstanceShadBoxVB(boxModelMats, 10 * sizeof(glm::mat4));
		{
			VertexBufferLayout ShadBoxVL;
			VertexBufferLayout IShadBoxVL;
			ShadBoxVL.push<float>(3);
			for (unsigned int i = 0; i < 4; ++i)
				IShadBoxVL.push<float>(4);
			shadBoxVA.AddBuffNLaySub(shadBoxVB, ShadBoxVL);
			shadBoxVA.AddBuffNLay(InstanceShadBoxVB, IShadBoxVL);
			shadBoxVB.Unbind();
			InstanceShadBoxVB.Unbind();
			shadBoxVA.Unbind();
		}

		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024,NUM_CASCADE = 3;

		float cascadEnds[NUM_CASCADE + 1] = { _NEAR,6.0f,12.0f,_FAR };

		CascadeFBO casFB(SHADOW_WIDTH, SHADOW_HEIGHT, NUM_CASCADE, cascadEnds, (float)SCR_HEIGHT / SCR_WIDTH);
		CascadeFBO casSoftFB(SHADOW_WIDTH, SHADOW_HEIGHT, NUM_CASCADE, cascadEnds, (float)SCR_HEIGHT / SCR_WIDTH);
		CascadeFBO casBoxFB(SHADOW_WIDTH, SHADOW_HEIGHT, NUM_CASCADE, cascadEnds, (float)SCR_HEIGHT / SCR_WIDTH);

		Renderer renderer;
		
		Shader boxShader;
		boxShader.push(GL_VERTEX_SHADER, "extras/shader/box1.vs");
		boxShader.push(GL_FRAGMENT_SHADER, "extras/shader/box1.fs");
		boxShader.buildShader();

		Shader quadShader;
		quadShader.push(GL_VERTEX_SHADER, "extras/shader/quadShader.vs");
		quadShader.push(GL_FRAGMENT_SHADER, "extras/shader/quadShader.fs");
		quadShader.buildShader();

		Shader boxDepthShader;
		boxDepthShader.push(GL_VERTEX_SHADER, "extras/shader/depth.vs");
		boxDepthShader.push(GL_FRAGMENT_SHADER, "extras/shader/depth.fs");
		boxDepthShader.buildShader();

		Shader planeDepthShader;
		planeDepthShader.push(GL_VERTEX_SHADER, "extras/shader/planeShad.vs");
		planeDepthShader.push(GL_FRAGMENT_SHADER, "extras/shader/planeShad.fs");
		planeDepthShader.buildShader();

		Shader sQuadShader;
		sQuadShader.push(GL_VERTEX_SHADER, "extras/shader/Squad.vs");
		sQuadShader.push(GL_FRAGMENT_SHADER, "extras/shader/Squad.fs");
		sQuadShader.buildShader();

		Shader BlurTexShader;
		BlurTexShader.push(GL_VERTEX_SHADER, "extras/shader/blurTex.vs");
		BlurTexShader.push(GL_FRAGMENT_SHADER, "extras/shader/blurTex.fs");
		BlurTexShader.buildShader();

		Shader planeShader;
		planeShader.push(GL_VERTEX_SHADER, "extras/shader/plane.vs");
		planeShader.push(GL_FRAGMENT_SHADER, "extras/shader/plane.fs");
		planeShader.buildShader();
		
		boxShader.use();

		Texture t1("extras/textures/ran2.jpg", GL_REPEAT);
		boxShader.setInt("material.diffuse", 0);
		boxShader.setFloat("material.shininess", 128.0f);

		boxShader.dirStruct_set(dirLight);

		planeShader.use();

		planeShader.setInt("material.diffuse", 0);
		planeShader.setFloat("material.shininess", 128.0f);

		planeShader.dirStruct_set(dirLight);

		UniformBuffer uboShad;
		uboShad.push(1, sizeof(glm::mat4));
		uboShad.BuildBuffer(0, GL_DYNAMIC_DRAW);

		uniBuf buf1;
		buf1.m_artifact = 0.0f;
		buf1.m_cascade = 0.0f;
		buf1.pad3 = 0.0f;
		buf1.pad4 = 0.0f;

		UniformBuffer uboOrg;
		uboOrg.push(1, sizeof(uniBuf));
		uboOrg.BuildBuffer(1, GL_DYNAMIC_DRAW);
		
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, _NEAR, _FAR);

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImVec2 Pos = ImVec2(0.0f, 0.0f);
		ImVec2 Size = ImVec2(350.0f, 200.0f);
		ImVec4 SunPos = ImVec4(dirLight[0].x, dirLight[0].y, dirLight[0].z, 0.0f);
		ImVec4 DColor(1.0f, 1.0f, 1.0f, 1.0f); 
		float sunX = 2.0f/4.0f, sunZ = 1.0f/4.0f;
		glm::vec3 dirColor(DColor.x, DColor.y, DColor.z);
		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = float(glfwGetTime());
			deltaTime = currentFrame - lastTime;
			lastTime = currentFrame;

			ImGui_ImplGlfwGL3_NewFrame();

			processInput(window);
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glCullFace(GL_BACK);
			
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 vView[3];
			glm::vec3 LitDir = glm::normalize(glm::vec3(SunPos.x, SunPos.y, SunPos.z));
			glm::vec3 right = glm::normalize(glm::cross(LitDir, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 up = glm::normalize(glm::cross(right, LitDir));
			glm::mat4 testLitView[1];
			testLitView[0] = glm::lookAt(glm::vec3(-SunPos.x, -SunPos.y, -SunPos.z), glm::vec3(0.0f), up);

			casFB.CalcOrtProjs(view, testLitView, 45.0f);
			for (unsigned int i = 0; i < 3; ++i)
			{
				vView[i] = glm::lookAt(casFB.GetModlCent(i), casFB.GetModlCent(i) +LitDir * 0.2f, up);
			}

			glm::mat4 modl;
			modl = glm::translate(modl, glm::vec3(0.0f, -3.0f, -2.0f));
			modl = glm::scale(modl, glm::vec3(10.0f));
			modl = glm::rotate(modl, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			casFB.Bind();
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
			{
				casFB.BindForWriting(i);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

				glm::mat4 projView = casFB.GetProjMat(vView[i],i) * vView[i];

				uboShad.Bind();
				uboShad.fillData(0, glm::value_ptr(projView));
				uboShad.Unbind();

				planeDepthShader.use();

				planeDepthShader.setMat4("model", glm::value_ptr(modl));
				renderer.Draw(GL_TRIANGLES, planeVA, planeDepthShader, 6);
				
				boxDepthShader.use();
				boxDepthShader.setVec2("m_stencil", glm::vec2(1.0f, 1.0f));
				renderer.DrawInstanced(GL_TRIANGLES, shadBoxVA, boxDepthShader, 36, 10);
			
				casBoxFB.BindForWriting(i);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				boxDepthShader.use();
				boxDepthShader.setVec2("m_stencil", glm::vec2(0.0f, 0.0f));
				renderer.DrawInstanced(GL_TRIANGLES, shadBoxVA, boxDepthShader, 36, 10);
			}

			casBoxFB.BindForReading(0);
			BlurTexShader.use();
			glDisable(GL_DEPTH_TEST);
			static float blur[3] = { 1.0f,2.0f,3.0f };
			BlurTexShader.setVec2("xy", glm::vec2(0.0f, 1.0f));
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
			{
				casSoftFB.BindForWriting(i);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				BlurTexShader.setFloat("m_cascNum", blur[i]);
				BlurTexShader.setInt("Texture1", i);
				renderer.Draw(GL_TRIANGLES, quadVA, BlurTexShader, 6);
			}
			casSoftFB.BindForReading(0);
			BlurTexShader.setVec2("xy", glm::vec2(1.0f, 0.0f));
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
			{
				casBoxFB.BindForWriting(i);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				BlurTexShader.setFloat("m_cascNum", blur[i]);
				BlurTexShader.setInt("Texture1", i);
				renderer.Draw(GL_TRIANGLES, quadVA, BlurTexShader, 6);
			}

			casFB.BindForReading(0);
			BlurTexShader.use();
			BlurTexShader.setVec2("xy", glm::vec2(0.0f, 1.0f));
			glDisable(GL_DEPTH_TEST);
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
			{
				casSoftFB.BindForWriting(i);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				BlurTexShader.setFloat("m_cascNum", blur[i]);
				BlurTexShader.setInt("Texture1", i);
				renderer.Draw(GL_TRIANGLES, quadVA, BlurTexShader, 6);
			}
			casSoftFB.BindForReading(0);
			BlurTexShader.setVec2("xy", glm::vec2(1.0f, 0.0f));
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
			{
				casFB.BindForWriting(i);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				BlurTexShader.setFloat("m_cascNum", blur[i]);
				BlurTexShader.setInt("Texture1", i);
				renderer.Draw(GL_TRIANGLES, quadVA, BlurTexShader, 6);
			}
			glEnable(GL_DEPTH_TEST);

			dirColor = glm::vec3(DColor.x, DColor.y, DColor.z);

			glm::vec3 cameraPos = camera.GetPosition();

			buf1.projView = projection * view;
			buf1.lightSpaceMat[0] = casFB.GetProjMat(vView[0], 0)*vView[0];
			buf1.lightSpaceMat[1] = casFB.GetProjMat(vView[1], 1)*vView[1];
			buf1.lightSpaceMat[2] = casFB.GetProjMat(vView[2], 2)*vView[2];
			buf1.m_viewPos = cameraPos;
			buf1.dirColor = dirColor;
			buf1.m_ratio = glm::vec3( casFB.GetRatio(vView[0], 0), casFB.GetRatio(vView[1], 1), casFB.GetRatio(vView[2], 2) );
			buf1.m_gCascadeEndClipSpace = glm::vec3((projection*glm::vec4(0.0f, 0.0f, -cascadEnds[1], 1.0f)).z ,
													(projection*glm::vec4(0.0f, 0.0f, -cascadEnds[2], 1.0f)).z ,
													(projection*glm::vec4(0.0f, 0.0f, -cascadEnds[3], 1.0f)).z);

			uboOrg.Bind();							
			uboOrg.fillData(0, &buf1);
			uboOrg.Unbind();

			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			
			fb.Bind();

			boxShader.use();
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			static float backColor = 1.0f;
			glClearColor(backColor, backColor, backColor, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			t1.Bind(0, GL_TEXTURE_2D);

			casFB.BindForReading(2);
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
				boxShader.setInt("shadMap[" + std::to_string(i) + "]", 2 + i);

			renderer.DrawInstanced(GL_TRIANGLES, va, boxShader, 36, 10);

			planeShader.use();
			casFB.BindForReading(2);
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
				planeShader.setInt("shadMap[" + std::to_string(i) + "]", 2 + i);
			casBoxFB.BindForReading(5);
			for (unsigned int i = 0; i < NUM_CASCADE; ++i)
				planeShader.setInt("logMap[" + std::to_string(i) + "]", 5 + i);

			planeShader.setMat4("model", glm::value_ptr(modl));

			renderer.Draw(GL_TRIANGLES, planeVA, planeShader, 6);
	
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenFB.GetID());
			glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			
			fb.Unbind();
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glDisable(GL_DEPTH_TEST);
			glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
			quadShader.use();
			screenFB.BindTex(0);
			quadShader.setInt("shadMap", 0);
			renderer.Draw(GL_TRIANGLES, quadVA, quadShader, 6);

			//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			//glDisable(GL_DEPTH_TEST);
			//glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
			//sQuadShader.use();
			//casFB.BindForReading(5);
			//sQuadShader.setInt("Texture1", 5);
			//renderer.Draw(GL_TRIANGLES, quadVA, sQuadShader, 6);

			if (Renderer::status)
			{
				static int counter = 0;
				ImGui::SetWindowPos(Pos);
				ImGui::SetWindowSize(Size);
				ImGui::Text("PAUSE MENU");                           // Display some text (you can use a format string too)
				ImGui::SliderFloat("backColor", &backColor, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
				ImGui::SliderFloat("SunX", &sunX, -1.0f, 1.0f);
				ImGui::SliderFloat("SunZ", &sunZ, -1.0f, 1.0f);
				ImGui::ColorEdit3("light color", (float *)&DColor); // Edit 3 floats representing a color
				if (ImGui::Button("artifacts"))
				{
					if (buf1.m_artifact == 0.0f)
						buf1.m_artifact = 1.0f;
					else
						buf1.m_artifact = 0.0f;
				}
				ImGui::SameLine();
				if (ImGui::Button("cascade"))
				{
					if (buf1.m_cascade == 0.0f)
						buf1.m_cascade = 1.0f;
					else
						buf1.m_cascade = 0.0f;
				}
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			if (Renderer::status)
			{
				glm::vec3 SunPosTemp = glm::vec3(sunX*4.0f, SunPos.y, sunZ*4.0f);
				if (SunPosTemp.x >= 0.0f)
					SunPosTemp.x = glm::clamp(SunPosTemp.x, 0.01f, 4.0f);
				else if (SunPosTemp.x < 0.0f)
					SunPosTemp.x = glm::clamp(SunPosTemp.x, -4.0f, -0.01f);
				if (SunPosTemp.z >= 0.0f)
					SunPosTemp.z = glm::clamp(SunPosTemp.z, 0.01f, 4.0f);
				else if (SunPosTemp.z < 0.0f)
					SunPosTemp.z = glm::clamp(SunPosTemp.z, -4.0f, -0.01f);
				SunPos = ImVec4(SunPosTemp.x, SunPosTemp.y, SunPosTemp.z, 1.0f);
				boxShader.use();
				boxShader.setVec3("dirLit.direction", SunPosTemp);
				planeShader.use();
				planeShader.setVec3("dirLit.direction", SunPosTemp);
			}
			glfwPollEvents();
		}
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::WUP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::WDOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && Renderer::tilde == false)
	{
		//std::cout << "pressed" << std::endl;
		Renderer::PauseMenu(window);
	}
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE && Renderer::tilde == true)
	{
		//std::cout << "released" << std::endl;
		Renderer::tilde = false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	GLCall(glViewport(0, 0, width, height));
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstTime)
	{
		lastX = xPos;
		lastY = yPos;
		firstTime = false;
	}
	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;
	camera.ProcessMouseMovement(xOffset, yOffset);
}

void mouse_pause(GLFWwindow* window, double xPos, double yPos)
{
	firstTime = true;
}

void scroll_callback(GLFWwindow*, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset,false);
}
