//
// Created by Chillstep on 2024/2/6.
//

#include "SkyBoxShader.h"
SkyBoxShader::~SkyBoxShader()
{

}
Matrix<4, 1, float> SkyBoxShader::vertex(int iface, int nthvert)
{
	Varying_uv[nthvert] = model->getuv(iface, nthvert);

	Matrix<4, 1, float> gl_vertex = Matrix<4, 1, float>::Embed(model->getvert(iface, nthvert));
	Varying_tri[nthvert] = Mat4x1::Proj(gl_vertex, true);
	gl_vertex = camera.ViewMatrix * gl_vertex;
	gl_vertex = camera.ViewportMatrix * camera.ProjectionMatrix * gl_vertex;
	Varying_normal[nthvert] = Matrix<4, 1, float>::Proj(
		Uniform_MIT * Mat4x1::Embed(model->getNormal(iface, nthvert))
	);
	return gl_vertex;
}
bool SkyBoxShader::fragment(Vec3f bar, TGAColor& color)
{
	Vec2f uv = ChillMathUtility::TriangleBarycentricInterp(Varying_uv, bar);
	EFaceOrientation CurrentFaceOri = EFaceOrientation::Unknown;
	if(std::all_of(Varying_tri.begin(), Varying_tri.end(), [](const Vec3f& In){return In.x > 0;}))
	{
		CurrentFaceOri = EFaceOrientation::Left;
	}
	else if(std::all_of(Varying_tri.begin(), Varying_tri.end(), [](const Vec3f& In){return In.y > 0;}))
	{
		CurrentFaceOri = EFaceOrientation::Top;
	}
	else if(std::all_of(Varying_tri.begin(), Varying_tri.end(), [](const Vec3f& In){return In.z > 0;}))
	{
		CurrentFaceOri = EFaceOrientation::Front;
	}
	else if(std::all_of(Varying_tri.begin(), Varying_tri.end(), [](const Vec3f& In){return In.x < 0;}))
	{
		CurrentFaceOri = EFaceOrientation::Right;
	}
	else if(std::all_of(Varying_tri.begin(), Varying_tri.end(), [](const Vec3f& In){return In.y < 0;}))
	{
		CurrentFaceOri = EFaceOrientation::Bottom;
	}
	else if(std::all_of(Varying_tri.begin(), Varying_tri.end(), [](const Vec3f& In){return In.z < 0;}))
	{
		CurrentFaceOri = EFaceOrientation::Back;
	}
	TGAColor c = model->diffuseSkyBox(uv, CurrentFaceOri);
	color = c;

	return false;
}
