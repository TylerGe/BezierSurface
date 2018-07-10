#pragma once
#ifndef _MLBEZIERH_
#define _MLBEZIERH_
#include "../inc/common.h"
#include <vector>
#include <memory>

#define MOUSE_SELECT_SENSITIVENESS 5e-2
constexpr  uint32_t controlPointsNums = 25;
class mlBezier
{
public:
	mlBezier();
	~mlBezier();


	glm::vec3 mlEvalBezierCurve(const glm::vec3 *P, const float &t);
	glm::vec3 mlEvalBezierPatch(const glm::vec3 *controlPoints, const float &u, const float &v);
	glm::vec3 derivBezier(const glm::vec3 *P, const float &t);
	glm::vec3 dUBezier(const glm::vec3 *controlPoints, const float &u, const float &v);
	glm::vec3 dVBezier(const glm::vec3 *controlPoints, const float &u, const float &v);

	void mlCreateBeizermesh();
	void mlTriangularization();

	int getSelectedControlPointIndice(int posX, int posY);
	void getRayScreenToSpace(int posX, int posY, glm::vec3 &outRay_o, glm::vec3 &outRay_d);
	double distancePointToRay(glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 p);
	void updateControlPointPosition(int posX, int posY, int selectedInd);
public:

	uint32_t divs;
	std::vector<glm::vec3> P;
	std::vector<glm::vec3> N;
	std::vector<glm::vec2> st;

	std::vector<int> indicesofControlpoints;
	std::vector<int> indicesofP;
public:

	glm::vec3 controlPoints[controlPointsNums]
		=
	{
        {-3.0,-3.0,1.0},
        {-2.0,-3.0,2.0},
        {-0.5,-3.0,0.0},
        {0.5,-3.0,1.0},
        {2.0,-3.0,-1.0},
//        {3.0,-3.0,1.0},
        
        {-3.0,-2.0,0.0},
		{ -2.0, -2.0,  1.0 },
		{ -0.5, -2.0,  0.0 },
		{ 0.5, -2.0, -2.0 },
		{ 2.0, -2.0,  1.0 },
//        {3.0,-2.0,1.0},

        {-3.0,-0.5,2.5},
		{ -2.0, -0.5,  2.0 },
		{ -0.5, -0.5,  1.5 },
		{ 0.5, -0.5,  0.0 },
		{ 2.0, -0.5, -1.0 },
//        {3.0,-0.5,1.0},

        {-3.0,0.5,1.0},
		{ -2.0,  0.5,  2.0 },
		{ -0.5,  0.5,  1.0 },
		{ 0.5,  0.5, -1.0 },
		{ 2.0,  0.5,  1.0 },
//        {3.0,0.5,0.0},

        {-3.0,2.0,0.0},
		{ -2.0,  2.0, -1.0 },
		{ -0.5,  2.0, -1.0 },
		{ 0.5,  2.0,  0.0 },
		{ 2.0,  2.0, -0.5 },
//        {3.0,2.0,-1.0},
        
//        {-3.0,3.0,-1.0},
//        {-2.0,3.0,0.0},
//        {-0.5,3.0,1.0},
//        {0.5,3.0,0.0},
//        {2.0,3.0,0.5},
//        {3.0,3.0,2.0}
        
	};
};

mlBezier::mlBezier()
{
}

mlBezier::~mlBezier()
{
}

glm::vec3 mlBezier::mlEvalBezierCurve(const glm::vec3 * P, const float & t)
{
    if(!(0<=t&&t<=1)){
        return glm::vec3(0.0);
    }
    
    glm::vec3 P0=P[0];
    glm::vec3 P1=P[1];
    glm::vec3 P2=P[2];
    glm::vec3 P3=P[3];
    glm::vec3 P4=P[4];
    
    glm::vec3 P01=(1-t)*P0+t*P1;
    glm::vec3 P12=(1-t)*P1+t*P2;
    glm::vec3 P23=(1-t)*P2+t*P3;
    glm::vec3 P34=(1-t)*P3+t*P4;
    
    glm::vec3 P0112=(1-t)*P01+t*P12;
    glm::vec3 P1223=(1-t)*P12+t*P23;
    glm::vec3 P2334=(1-t)*P23+t*P34;
    
    
    glm::vec3 P01121223=(1-t)*P0112+t*P1223;
    glm::vec3 P12232334=(1-t)*P1223+t*P2334;
    
	return (1-t)*P01121223+t*P12232334;
}

glm::vec3 mlBezier::mlEvalBezierPatch(const glm::vec3 * controlPoints, const float & u, const float & v)
{
    glm::vec3 Pu[5];
    for(int i=0;i<5;i++){
        glm::vec3 curveP[5];
        curveP[0]=controlPoints[i*5];
        curveP[1]=controlPoints[i*5+1];
        curveP[2]=controlPoints[i*5+2];
        curveP[3]=controlPoints[i*5+3];
        curveP[4]=controlPoints[i*5+4];
        Pu[i]=mlEvalBezierCurve(curveP, u);
    }
    
	return mlEvalBezierCurve(Pu, v);
}

inline glm::vec3 mlBezier::derivBezier(const glm::vec3 * P, const float & t)
{
	return glm::vec3(0.0);
}

glm::vec3 mlBezier::dUBezier(const glm::vec3 * controlPoints, const float & u, const float & v)
{
    glm::vec3 Pu[4];
    for(int i=0;i<4;i++){
        glm::vec3 curveP[4];
        curveP[0]=controlPoints[i*4];
        curveP[1]=controlPoints[i*4+1];
        curveP[2]=controlPoints[i*4+2];
        curveP[3]=controlPoints[i*4+3];
        Pu[i]=mlEvalBezierCurve(curveP, u);
    }
    
    glm::vec3 P0=Pu[0];
    glm::vec3 P1=Pu[1];
    glm::vec3 P2=Pu[2];
    glm::vec3 P3=Pu[3];
    
    glm::vec3 P01=(1-v)*P0+v*P1;
    glm::vec3 P12=(1-v)*P1+v*P2;
    glm::vec3 P23=(1-v)*P2+v*P3;
    
    glm::vec3 P0112=(1-v)*P01+v*P12;
    glm::vec3 P1223=(1-v)*P12+v*P23;
    
    glm::vec3 du=P1223-P0112;
	return du;
}

glm::vec3 mlBezier::dVBezier(const glm::vec3 * controlPoints, const float & u, const float & v)
{
    glm::vec3 Pv[4];
    for(int i=0;i<4;i++){
        glm::vec3 curveP[4];
        curveP[0]=controlPoints[i];
        curveP[1]=controlPoints[i+4];
        curveP[2]=controlPoints[i+4*2];
        curveP[3]=controlPoints[i+4*3];
        Pv[i]=mlEvalBezierCurve(curveP, v);
    }
    
    glm::vec3 P0=Pv[0];
    glm::vec3 P1=Pv[1];
    glm::vec3 P2=Pv[2];
    glm::vec3 P3=Pv[3];
    
    glm::vec3 P01=(1-u)*P0+u*P1;
    glm::vec3 P12=(1-u)*P1+u*P2;
    glm::vec3 P23=(1-u)*P2+u*P3;
    
    glm::vec3 P0112=(1-u)*P01+u*P12;
    glm::vec3 P1223=(1-u)*P12+u*P23;
    
    glm::vec3 dv=P1223-P0112;
    return dv;

}

void mlBezier::mlCreateBeizermesh()
{

}

void mlBezier::mlTriangularization()
{
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			int ind = j * 5 + i;
			indicesofControlpoints.push_back(ind);
			indicesofControlpoints.push_back(ind + 1);

			indicesofControlpoints.push_back(ind + 6);
			indicesofControlpoints.push_back(ind + 5);
		}
	}

}




// you will fill the four function to edit control points
int mlBezier::getSelectedControlPointIndice(int posX, int posY)
{
	return 0;
}

void mlBezier::getRayScreenToSpace(int posX, int posY, glm::vec3 &outRay_o, glm::vec3 &outRay_d)
{
	
}

double mlBezier::distancePointToRay(glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 p)
{
	return 0;
}

void mlBezier::updateControlPointPosition(int posX, int posY, int selectedInd)
{
}

#endif // !_MLBEZIERH_
