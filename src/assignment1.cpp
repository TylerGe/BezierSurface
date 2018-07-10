#include "../bezier/mlbezier.h"
#include "../inc/shader_m.h"
#include "../3rdparty/stb_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <math.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLfloat rotate;

glm::vec3 cameraPos   = glm::vec3(0.0f, 1.0f,  7.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  800.0 / 2.0;
float fov   =  45.0f;

bool dragFlag = false;
float deltaTime = 0.0f; //
float lastFrame = 0.0f; //
mlBezier mlbezier;

const unsigned int SCR_WINDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void drawControlPoint(mlBezier &mlbezier)
{
	glBegin(GL_QUADS);
	for (int i = 0; i < (int)mlbezier.indicesofControlpoints.size() / 4; i++)
	{
		glColor3f(0, 1, 0);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].z
		);
	}
	glEnd();
}
glm::vec3 NormalBezier(mlBezier &mlbezier, const float & u, const float & v)
{
    glm::vec3 du=mlbezier.dUBezier(mlbezier.controlPoints, u, v);
    glm::vec3 dv=mlbezier.dVBezier(mlbezier.controlPoints, u, v);
    glm::vec3 Norm=glm::cross(du, dv);
    Norm/=(Norm[0]*Norm[0]+Norm[1]*Norm[1]+Norm[2]*Norm[2]);
//    printf("%f,%f,%f\n",Norm[0],Norm[1],Norm[2]);
    return Norm;
}
void drawBezierSurface(mlBezier &mlbezier)
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, 1);
    glm::vec3 P1,P2,P3,Norm1,Norm2,Norm3;
    GLfloat texp1[2];
    GLfloat texp2[2];
    GLfloat texp3[2];
    
    glm::vec3 Point[101][101];
    for(int k=0;k<101;k++){
        for(int t=0;t<101;t++){
            Point[k][t]=mlbezier.mlEvalBezierPatch(mlbezier.controlPoints, k/100.0, t/100.0);
        }
    }
    for(int j=0;j<100;j++){
        
        P1=Point[0][1+j];
        Norm1=NormalBezier(mlbezier, 0, 0.01+j/100.0);
        P2=Point[0][j];
        Norm2=NormalBezier(mlbezier, 0,j/100.0);
        P3=Point[1][1+j];
        Norm3=NormalBezier(mlbezier, 0.01,0.01+j/100.0);
        texp1[0]=0;
        texp1[1]=0.01+j/100.0;
        texp2[0]=0;
        texp2[1]=j/100.0;
        texp3[0]=0.01;
        texp3[1]=0.01+j/100.0;
        glBegin(GL_TRIANGLES);
//        glColor3f(0.6, 0.2, 0.3);
        glTexCoord2fv(texp1);
        glNormal3f(Norm1[0], Norm1[1], Norm1[2]);
        glVertex3f(P1[0], P1[1], P1[2]);
        glTexCoord2fv(texp2);
        glNormal3f(Norm2[0], Norm2[1], Norm2[2]);
        glVertex3f(P2[0], P2[1], P2[2]);
        glTexCoord2fv(texp3);
        glNormal3f(Norm3[0], Norm3[1], Norm3[2]);
        glVertex3f(P3[0], P3[1], P3[2]);
        glEnd();
        
        for(int i=1;i<200;i++)
        {
            
            texp1[0]=texp2[0];
            texp1[1]=texp2[1];
            P1=P2;
            Norm1=Norm2;
            texp2[0]=texp3[0];
            texp2[1]=texp3[1];
            P2=P3;
            Norm2=Norm3;
            texp3[0]=(i+2-i%2)/200.0;
//            printf("help:%f",texp3[0]);
            texp3[1]=((i+1)%2+j)/100.0;
            P3=Point[(i+2-i%2)/2][((i+1)%2+j)];
//            P3=mlbezier.mlEvalBezierPatch(mlbezier.controlPoints,(i+1)/200.0 ,((i+1)%2+j)/100.0);
            Norm3=NormalBezier(mlbezier, (i+2-i%2)/200.0 ,((i+1)%2+j)/100.0);
            glBegin(GL_TRIANGLES);
//            glColor3f(0.6, 0.2, 0.3);
            glTexCoord2fv(texp1);
            glNormal3f(Norm1[0], Norm1[1], Norm1[2]);
            glVertex3f(P1[0], P1[1], P1[2]);
            glTexCoord2fv(texp2);
            glNormal3f(Norm2[0], Norm2[1], Norm2[2]);
            glVertex3f(P2[0], P2[1], P2[2]);
            glTexCoord2fv(texp3);
            glNormal3f(Norm3[0], Norm3[1], Norm3[2]);
            glVertex3f(P3[0], P3[1], P3[2]);
            glEnd();
        }
    }
    glFlush();
    glDisable(GL_TEXTURE_2D);
}
void ChangePmv()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPos[0],cameraPos[1],cameraPos[2] ,
              cameraPos[0] + cameraFront[0], cameraPos[1] + cameraFront[1], cameraPos[2] + cameraFront[2],
              cameraUp[0],cameraUp[1],cameraUp[2]);
}

void initPMV()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, SCR_WINDTH / SCR_HEIGHT, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt
	(
		0, 0, -7.5,
		0, 0, 0,
		0, 1, 0
	);
}

void initTexture(unsigned int &texture)
{
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
//    unsigned char *data = stbi_load(("resource/textures/container.jpg"), &width, &height, &nrChannels, 0);
    unsigned char *data = stbi_load(("/Users/Buddha/container.jpg"), &width, &height, &nrChannels, 0);
    printf("char:%d,%d,%d\n",width,height,nrChannels);
    //bind it to texture
    if(data==NULL){
        printf("Error\n");
    }
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // sample: specify texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // set the active texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}


void AddLight(mlBezier &mlbezier)
{
    glDepthFunc(GL_LESS);
//    glEnable(GL_DEPTH_TEST);
    GLfloat sun_light_position[] = { 0.0f, 6.0f, 0.0f, 1.0f };
    GLfloat sun_light_ambient[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat sun_light_diffuse[] = { 1.0f, 2.0f, 1.0f, 1.0f };
    GLfloat sun_light_specular[] = { 1.0f, 2.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //material
    GLfloat earth_mat_ambient[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    GLfloat earth_mat_diffuse[] = { 0.0f, 0.0f, 0.5f, 1.0f };
    GLfloat earth_mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat earth_mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat earth_mat_shininess = 70.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
    //glEnable(GL_DEPTH_TEST);
//    glEnable(GL_MULTISAMPLE);

}
int main()
{
	glfwInit();

	GLFWwindow * window = glfwCreateWindow(SCR_WINDTH, SCR_HEIGHT, "hello", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
	glewExperimental = GL_TRUE;
	glewInit();



	//mlBezier mlbezier;
	mlbezier.divs = 25;
	mlbezier.mlCreateBeizermesh();
	mlbezier.mlTriangularization();



	unsigned int texture = 1;
	initTexture(texture);

    
    glEnable(GL_AUTO_NORMAL);
    AddLight(mlbezier);
	initPMV();

	while (!glfwWindowShouldClose(window))
	{
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        ChangePmv();
        
		glPointSize(5);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawControlPoint(mlbezier);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        drawBezierSurface(mlbezier);
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
    
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

